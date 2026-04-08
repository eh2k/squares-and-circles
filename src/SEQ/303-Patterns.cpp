
// Copyright (C)2021 - E.Heidt
//
// Author: E.Heidt (eh2k@gmx.de)
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

// xbuild_flags: -fno-inline -mfloat-abi=hard -mfpu=fpv5-d16

#include "../squares-and-circles-api.h"
#include "../lib/stmlib/utils/dsp.h"
#include "../lib/misc/noise.hxx"
#include "../lib/misc/patterns_303.h"
#include <algorithm>

uint8_t seq_pos = UINT8_MAX;

int32_t pattern_select = 2;
int32_t pattern_shift = 0;
int32_t pattern_len = 16;
int32_t gate_len = 3; // https://www.firstpr.com.au/rwi/dfish/303-slide.html

int16_t *out_gate;
int16_t *out_voct;
int16_t *out_accent;
int16_t *out_slide;

// https://xbm.jazzychad.net/
const uint8_t xmb_note_6x8[8] = {0xc8, 0xd8, 0xe8, 0xd8, 0xe8, 0xce, 0xcf, 0xc6};
const uint8_t xmb_rest_6x8[8] = {0xd6, 0xde, 0xd0, 0xd6, 0xde, 0xd0, 0xd0, 0xd0};

const uint8_t xmb_arrow_up_5x6[6] = {0xe4, 0xee, 0xff, 0xe4, 0xe4, 0xe4};
const uint8_t xmb_arrow_dn_5x6[6] = {0xe4, 0xe4, 0xe4, 0xf5, 0xee, 0xe4};
const uint8_t xmb_circle_7x7[7] = {0x9c, 0xa2, 0xc1, 0xc1, 0xc1, 0xa2, 0x9c};
const uint8_t xmb_filled_circle_7x7[7] = {0x80, 0x88, 0x9c, 0xbe, 0x9c, 0x88, 0x80};

uint8_t pattern[16];

int32_t last_val[4] = {-1, -1, -1, -1};

inline int32_t note_to_voct(uint8_t note)
{
    int32_t tmp = ((int32_t)(note & NOTE_MASK) - C4);
    tmp *= (PITCH_PER_OCTAVE / 12);
    return tmp;
}

inline uint8_t voct_to_note(int32_t voct)
{
    return C4 + voct / (PITCH_PER_OCTAVE / 12);
}

inline uint8_t &pattern_at(int pos)
{
    pos %= pattern_len;
    pos += 16 - engine::getParamValueModulated_i32(&pattern_shift);
    return pattern[pos % LEN_OF(pattern)];
}

void engine::setup()
{
    engine::addParam("#PAT", &pattern_select, 0, LEN_OF(x0xb0x_patterns) - 1);
    engine::addParam("#SHFT", &pattern_shift, -16, 16);
    engine::addParam("#LEN", &pattern_len, 0, 16);
    engine::addParam("#GATE", &gate_len, 1, 6);

    out_gate = engine::outputBuffer_i16(OUT_GATE);
    out_accent = engine::outputBuffer_i16(OUT_ACCENT);
    out_slide = engine::outputBuffer_i16("SLIDE");
    out_voct = engine::outputBuffer_i16(OUT_VOCT);
}

uint32_t phase_;
uint32_t phase_inc = 0;

uint32_t t_ = 0;
uint32_t gate_until_t = 0;
int32_t last_pattern_select = -1;
uint8_t curr_note, prev_note = 0;

int16_t _gate = 0;
int16_t _acc = 0;
int16_t _slide = 0;
int32_t _voct = 0.f;

void engine::process()
{
    if(pattern_len == 0)
    {
        std::fill_n(out_gate, FRAME_BUFFER_SIZE, 0);
        std::fill_n(out_voct, FRAME_BUFFER_SIZE, 0);
        std::fill_n(out_accent, FRAME_BUFFER_SIZE, 0);
        std::fill_n(out_slide, FRAME_BUFFER_SIZE, 0);
        return;
    }

    if (pattern_shift < 0)
        pattern_shift += 16;
    else if (pattern_shift >= 16)
        pattern_shift -= 16;

    if (pattern_select != last_pattern_select)
    {
        last_pattern_select = pattern_select;
        memcpy(pattern, x0xb0x_patterns[pattern_select], sizeof(pattern));
        size_t l = 0;
        for (l = 0; l < LEN_OF(pattern); l++)
            if (pattern[l] == 0xff)
                break;

        for (size_t i = l; i < LEN_OF(pattern); i++)
            pattern[i] = pattern[i - l];
    }

    if (engine_sync::stepChanged())
    {
        seq_pos = engine_sync::step() % pattern_len;

        prev_note = curr_note;
        curr_note = pattern_at(seq_pos);

        phase_ = UINT32_MAX;

        float n = 1 + (engine::t() - t_);
        gate_until_t = n * (float)gate_len / 6;
        t_ = engine::t();
    }

    if (prev_note & SLIDE)
    {
        auto cv = engine::qz_process(note_to_voct(curr_note));
        float delta = (cv - _voct);
        CONSTRAIN(delta, -PITCH_PER_OCTAVE, PITCH_PER_OCTAVE);
        _voct += delta / 100;

        _slide = 5 * PITCH_PER_OCTAVE;
    }
    else
    {
        _slide = 0;
        _voct = engine::qz_process(note_to_voct(curr_note));
    }

    if (((curr_note & NOTE_MASK) && gate_until_t > 0) || (curr_note & SLIDE))
        _gate = 5 * PITCH_PER_OCTAVE;
    else
        _gate = 0;

    if (curr_note & ACCENT)
        _acc = 5 * PITCH_PER_OCTAVE;
    else
        _acc = 0;

    if (gate_until_t > 0)
        --gate_until_t;

    auto voct = engine::cv_i32() + _voct + (2 * PITCH_PER_OCTAVE);

    std::fill_n(out_gate, FRAME_BUFFER_SIZE, _gate);
    std::fill_n(out_voct, FRAME_BUFFER_SIZE, voct);
    std::fill_n(out_accent, FRAME_BUFFER_SIZE, _acc);
    std::fill_n(out_slide, FRAME_BUFFER_SIZE, (curr_note & SLIDE) ? (5 * PITCH_PER_OCTAVE) : 0);
}

char tmp[16];
void engine::draw()
{
    uint8_t cur_pos = engine_sync::ppn() ? seq_pos : -1;

    auto len_mod = engine::getParamValueModulated_i32(&pattern_len);
    for (int i = 0, r = 0; r < 4 && i < len_mod; r++)
    {
        for (int k = 0; k < 8 && i < len_mod; k++)
        {
            int x = 24 + (k * 10) + (k < 1 ? 1 : 1);
            int y = 0 + 13 + (r * 10) + (len_mod <= 8 ? 5 : 0);

            int r = 4;
            if (pattern_at((len_mod + i - 1) % len_mod) & SLIDE)
                gfx::drawCircle(x + r, y + r, r);
            else if (pattern_at(i) & NOTE_MASK)
            {
                gfx::drawCircle(x + r, y + r, r);
                if (i != cur_pos)
                    gfx::fillCircle(x + r, y + r, r);
            }
            else
                gfx::fillRect(x + 1, y + r - 1, r + r - 1, 3);

            if (i == cur_pos)
            {
                gfx::drawCircle(x + r, y + r, r - 1);
            }

            i++;
        }
    }

    int y = 35 + 8;

    gfx::drawPiano(24, y, (float)_voct / PITCH_PER_OCTAVE);

    y -= 1;

    if (curr_note == 0)
        gfx::drawXbm(62 + 10, y - 1, 6, 8, xmb_rest_6x8);
    else
        gfx::drawXbm(62 + 10, y - 1, 6, 8, xmb_note_6x8);

    if (curr_note & SLIDE)
        gfx::drawString(62 + 16, y, "-S");

    if (curr_note & ACCENT)
        gfx::drawString(62 + 16, y, "  -A");
}

void engine::screensaver()
{
    gfx::clearRect(0, 0, 128, 64);
    engine::draw();
}