
// Copyright (C)2021 - Eduard Heidt
//
// Author: Eduard Heidt (eh2k@gmx.de)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//

#include "machine.h"
#include "stmlib/utils/dsp.h"
#include "peaks/resources.h"

#define REST 0x0

/* between 0x0 and 0xA, the VCO voltage pins, so these notes arent really
 * 'effective' in that they all sound the same.
 */

// lowest octave
#define C1 0x0B
#define C1_SHARP 0x0C
#define D1 0x0D
#define D1_SHARP 0x0E
#define E1 0x0F
#define F1 0x10
#define F1_SHARP 0x11
#define G1 0x12
#define G1_SHARP 0x13

// middle octave
#define A1 0x14
#define A1_SHARP 0x15
#define B1 0x16
#define C2 0x17
#define C2_SHARP 0x18
#define D2 0x19
#define D2_SHARP 0x1A
#define E2 0x1B
#define F2 0x1C
#define F2_SHARP 0x1D
#define G2 0x1E
#define G2_SHARP 0x1F

// high octave
#define A2 0x20
#define A2_SHARP 0x21
#define B2 0x22
#define C3 0x23
#define C3_SHARP 0x24
#define D3 0x25
#define D3_SHARP 0x26
#define E3 0x27
#define F3 0x28
#define F3_SHARP 0x29
#define G3 0x2A
#define G3_SHARP 0x2B

#define A3 0x2C
#define A3_SHARP 0x2D
#define B3 0x2E
#define C4 0x2F
#define C4_SHARP 0x30
#define D4 0x31
#define D4_SHARP 0x32
#define E4 0x33
#define F4 0x34
#define F4_SHARP 0x35
#define G4 0x36
#define G4_SHARP 0x37

#define A4 0x38
#define A4_SHARP 0x39
#define B4 0x3A
#define C5 0x3B
#define C5_SHARP 0x3C
#define D5 0x3D
#define D5_SHARP 0x3E
// no more notes!

#define NOTE_MASK 0x3F
#define SLIDE 0x80
#define ACCENT 0x40

using namespace machine;

class ACIDSequencer : public Engine
{
    //"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"

    uint8_t pattern[16] = {A2_SHARP, F4, D3 | SLIDE, C5, A3_SHARP | SLIDE | ACCENT, G3, F2_SHARP | SLIDE | ACCENT, G3_SHARP,
                           A4_SHARP | SLIDE, A2, C4 | SLIDE, D4 | SLIDE | ACCENT, F3, D3 | SLIDE, F2 | SLIDE, D3_SHARP | SLIDE};

    uint8_t len = 16;
    uint8_t edit_pos = 0;
    uint8_t edit_cv = DEFAULT_NOTE;
    uint8_t edit_note = REST;

    // https://xbm.jazzychad.net/
    const uint8_t xmb_note_6x8[8] = {0xc8, 0xd8, 0xe8, 0xd8, 0xe8, 0xce, 0xcf, 0xc6};
    const uint8_t xmb_rest_6x8[8] = {0xd6, 0xde, 0xd0, 0xd6, 0xde, 0xd0, 0xd0, 0xd0};

    const uint8_t xmb_arrow_up_5x6[6] = {0xe4, 0xee, 0xff, 0xe4, 0xe4, 0xe4};
    const uint8_t xmb_arrow_dn_5x6[6] = {0xe4, 0xe4, 0xe4, 0xf5, 0xee, 0xe4};
    const uint8_t xmb_circle_7x7[7] = {0x9c, 0xa2, 0xc1, 0xc1, 0xc1, 0xa2, 0x9c};
    const uint8_t xmb_filled_circle_7x7[7] = {0x80, 0x88, 0x9c, 0xbe, 0x9c, 0x88, 0x80};

public:
    ACIDSequencer() : Engine(OUT_EQ_VOLT | VOCT_INPUT | SEQUENCER_ENGINE)
    {
    }

    void init() override
    {
        param[0].init(".pos", &edit_pos, 0, 0, len - 1);
        param[0].value_changed = [&]()
        {
            edit_pos %= len;
            edit_cv = DEFAULT_NOTE + (pattern[edit_pos] & NOTE_MASK) - C4;
            edit_note = (pattern[edit_pos] & NOTE_MASK) ? 1 : 0;
            if ((pattern[edit_pos] & (SLIDE | ACCENT)) == (SLIDE | ACCENT))
                edit_note = 4;
            else if (pattern[edit_pos] & SLIDE)
                edit_note = 2;
            else if (pattern[edit_pos] & ACCENT)
                edit_note = 3;
        };
        param[0].value_changed();

        param[1].init_v_oct(">cv", &edit_cv);
        param[1].value_changed = [&]()
        {
            pattern[edit_pos] &= (SLIDE | ACCENT);
            pattern[edit_pos] |= (C4 + edit_cv - DEFAULT_NOTE);
        };

        param[2].init(".len", &len, len, 1, 16);
        param[2].value_changed = [&]()
        {
            param[0].init(".pos", &edit_pos, edit_pos, 0, len - 1);
        };

        param[3].init(".note", &edit_note, 1, 0, 4);
        param[3].value_changed = [&]()
        {
            // 0 = 000 REST
            // 1 = 001 NOTE
            // 2 = 011 NOTE + S
            // 3 = 101 NOTE + A
            // 4 = 111 NOTE + S + A

            if (edit_note == 0)
                pattern[edit_pos] = 0;
            else if (edit_note == 1)
                pattern[edit_pos] = (C4 + edit_cv - DEFAULT_NOTE);
            else if (edit_note == 2)
                pattern[edit_pos] = (C4 + edit_cv - DEFAULT_NOTE) | SLIDE;
            else if (edit_note == 3)
                pattern[edit_pos] = (C4 + edit_cv - DEFAULT_NOTE) | ACCENT;
            else
                pattern[edit_pos] = (C4 + edit_cv - DEFAULT_NOTE) | SLIDE | ACCENT;
        };

        param[4].init(".shift", &seq_shift, 128, 0, 255);
    }

    void eeprom(std::function<void(void *, size_t)> read_write) override
    {
        read_write(&pattern[0], len);
    }

    inline int32_t seq_cv(int step)
    {
        int32_t tmp = (int32_t)(pattern[step % len] & NOTE_MASK) - C4;
        return tmp * (machine::PITCH_PER_OCTAVE / 12);
    }

    uint32_t phase_;
    uint32_t phase_inc = 0;

    uint8_t seq_pos = -1;
    uint8_t seq_shift = 0;

    uint32_t t = 0;
    uint32_t gate_until_t = 0;

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        if ((frame.clock % 6) == 1)
        {
            if (frame.clock == CLOCK_RESET)
            {
                seq_pos = 0;
            }
            else
            {
                if (seq_pos >= len)
                    seq_pos = (((frame.clock) / 6) % len);
                else
                    ++seq_pos %= len;
            }

            phase_ = UINT32_MAX;

            uint32_t n = 1 + (frame.t - t) / 2;

            if (pattern[(seq_pos - 1) % len] & SLIDE)
            {
                phase_ = 0;
                phase_inc = UINT32_MAX / n;
            }

            gate_until_t = frame.t + n;
            t = frame.t;
        }

        int32_t cv = frame.cv_voltage_;

        if (phase_ < (UINT32_MAX - phase_inc))
        {
            uint16_t e = stmlib::Interpolate824(peaks::lut_env_expo, phase_);
            phase_ += phase_inc;
            auto last_cv = seq_cv(seq_pos - 1);
            cv += last_cv + ((seq_cv(seq_pos) - last_cv) * e) / UINT16_MAX;
        }
        else
        {
            cv += seq_cv(seq_pos);
        }

        int16_t gate = 0;
        int16_t acc = 0;

        if (((pattern[seq_pos] && NOTE_MASK) && frame.t < gate_until_t) || (pattern[seq_pos] & SLIDE))
            gate = INT16_MAX;

        if (((pattern[seq_pos] && NOTE_MASK) && frame.t < gate_until_t) && (pattern[seq_pos] & ACCENT))
            acc = INT16_MAX;

        of.push<int32_t>(&cv, 1);
        of.push<int16_t>(&gate, 1);
        of.push<int16_t>(&acc, 1);
    }

    uint8_t _blink = 0;
    void onDisplay(uint8_t *display) override
    {
        _blink += 32;

        gfx::drawEngine(display, this);

        int r = 0;
        int y = 0;
        for (int i = 0; r < 4 && i < len; r++)
        {
            for (int k = 0; k < 8 && i < len; k++)
            {
                int x = (k * 8) + (k < 1 ? 1 : 1);
                y = 13 + (r * 8);

                if ((param[0].flags & machine::Parameter::IS_SELECTED) && edit_pos == i)
                    gfx::drawRect(display, x - 1, y - 1, 9, 9);
                else
                {
                    gfx::drawXbm(display, x, y, 7, 7, xmb_circle_7x7);
                    if (pattern[i] & SLIDE)
                    {
                        gfx::drawLine(display, x + 7, y + 2, x + 7, y + 4);
                    }
                }

                // if (pattern[i])
                // {
                //     gfx::drawXbm(display, x, y, 7, 7, xmb_filled_circle_7x7);
                // }

                if (i == seq_pos)
                {
                    gfx::drawRect(display, x + 1, y + 1, 5, 5);
                }

                i++;
            }
        }

        y += 12;

        char tmp[16];
        {
            if (param[2].flags & machine::Parameter::IS_SELECTED)
                sprintf(tmp, ">Len: %d", len);
            else
                sprintf(tmp, " Len: %d", len);

            gfx::drawString(display, 1, y, tmp, 0);
        }

        {
            if (param[4].flags & machine::Parameter::IS_SELECTED)
                sprintf(tmp, ">Shift: %+d", -128 + seq_shift);
            else
                sprintf(tmp, " Shift: %+d", -128 + seq_shift);

            gfx::drawString(display, 1, y + 8, tmp, 0);
        }

        y = 13 + 16;

        if (param[3].flags & machine::Parameter::IS_SELECTED)
        {
            gfx::drawString(display, 66, y, ">");
        }

        if (pattern[edit_pos] == 0)
            gfx::drawXbm(display, 64 + 10, y - 1, 6, 8, xmb_rest_6x8);
        else
            gfx::drawXbm(display, 64 + 10, y - 1, 6, 8, xmb_note_6x8);

        if (pattern[edit_pos] & SLIDE)
            gfx::drawString(display, 64 + 16, y, "-S");

        if (pattern[edit_pos] & ACCENT)
            gfx::drawString(display, 64 + 16, y, "  -A");
    }
};

void init_acid_sequencer()
{
    machine::add<ACIDSequencer>("SEQ", "ACIDSequencer");
}

MACHINE_INIT(init_acid_sequencer);