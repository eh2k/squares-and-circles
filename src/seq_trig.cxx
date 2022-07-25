
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

using namespace machine;

class TrigSequencer : public Engine
{
    uint8_t pattern[16] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
    uint8_t len = 16;
    uint8_t edit_pos = 0;
    uint8_t edit_note = 0;

    // https://xbm.jazzychad.net/
    const uint8_t xmb_note_6x8[8] = {0xc8, 0xd8, 0xe8, 0xd8, 0xe8, 0xce, 0xcf, 0xc6};
    const uint8_t xmb_rest_6x8[8] = {0xd6, 0xde, 0xd0, 0xd6, 0xde, 0xd0, 0xd0, 0xd0};

    const uint8_t xmb_arrow_up_5x6[6] = {0xe4, 0xee, 0xff, 0xe4, 0xe4, 0xe4};
    const uint8_t xmb_arrow_dn_5x6[6] = {0xe4, 0xe4, 0xe4, 0xf5, 0xee, 0xe4};
    const uint8_t xmb_circle_7x7[7] = {0x9c, 0xa2, 0xc1, 0xc1, 0xc1, 0xa2, 0x9c};
    const uint8_t xmb_filled_circle_7x7[7] = {0x80, 0x88, 0x9c, 0xbe, 0x9c, 0x88, 0x80};

public:
    TrigSequencer() : Engine(OUT_EQ_VOLT | SEQUENCER_ENGINE)
    {
    }

    void init() override
    {
        param[0].init(".pos", &edit_pos, 0, 0, len - 1);
        param[0].value_changed = [&]()
        {
            edit_pos %= len;
            edit_note = (pattern[edit_pos]) ? 1 : 0;
        };
        param[0].value_changed();
        param[2].init(".len", &len, len, 1, 16);
        param[2].value_changed = [&]()
        {
            param[0].init(".pos", &edit_pos, edit_pos, 0, len - 1);
        };

        param[1].init(".note", &edit_note, 1, 0, 1);
        param[1].value_changed = [&]()
        {
            // 0 = 000 REST
            // 1 = 001 NOTE
            // 2 = 011 NOTE + S
            // 3 = 101 NOTE + A
            // 4 = 111 NOTE + S + A

            if (edit_note == 0)
                pattern[edit_pos] = 0;
            else if (edit_note == 1)
                pattern[edit_pos] = UINT8_MAX;
        };

        // param[3].init(".shift", &seq_shift, 128, 0, 255);
    }

    void eeprom(std::function<void(void *, size_t)> read_write) override
    {
        read_write(&pattern[0], len);
    }

    uint8_t seq_pos = UINT8_MAX;
    uint8_t seq_shift = 0;

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
                if (seq_pos == UINT8_MAX)
                    seq_pos = (((frame.clock) / 6) % len);
                else
                    ++seq_pos %= len;
            }

            gate_until_t = frame.t;

            if ((pattern[seq_pos]))
                gate_until_t = frame.t + 2; // 10ms trigger
        }

        int16_t trig = 0;

        if (frame.t < gate_until_t)
            trig = INT16_MAX;

        of.push<int16_t>(&trig, 1);
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
                }

                if (pattern[i])
                {
                    gfx::drawXbm(display, x, y, 7, 7, xmb_filled_circle_7x7);
                }

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

        y = 13 + 4;

        if (param[1].flags & machine::Parameter::IS_SELECTED)
        {
            gfx::drawString(display, 66, y, ">");
        }

        if (pattern[edit_pos] == 0)
            gfx::drawXbm(display, 64 + 10, y - 1, 6, 8, xmb_rest_6x8);
        else
            gfx::drawXbm(display, 64 + 10, y - 1, 6, 8, xmb_note_6x8);
    }
};

void init_trig_sequencer()
{
    machine::add<TrigSequencer>("SEQ", "TrigSequencer");
}

MACHINE_INIT(init_trig_sequencer);