// Copyright (C)2022 - Eduard Heidt
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

#include "../squares-and-circles-api.h"
#include "misc/nes_noise.hxx"
#include "stmlib/dsp/filter.h"

static NESNoise _nes_noise;
static stmlib::DCBlocker _dc_blocker;
static const uint8_t max_level = 15;
static uint8_t gain = max_level;
static float buffer[FRAME_BUFFER_SIZE];

DSP_SETUP
void setup()
{
    dsp_param_u8("Level", &gain, 0, max_level);
    dsp_param_u8("@Period", &_nes_noise.period_index, 0, 15);
    dsp_param_u8("ModeBit", &_nes_noise.mode_bit, 0, 1);

    dsp_frame_f(OUTPUT_L, buffer);
    _nes_noise.init(1);
    _dc_blocker.Init(0.999f);
};

DSP_PROCESS
void process()
{
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        buffer[i] = _nes_noise.generateSample(1.f / max_level) * gain;
    }

    _dc_blocker.Process(&buffer[0], FRAME_BUFFER_SIZE);
}

GFX_DISPLAY
void display()
{
    gfx_draw_rect(75, 37, 47, 14);

    for (size_t bit = 0; bit < 15; bit++)
    {
        if ((_nes_noise.shift_reg & (1 << bit)) != 0)
            gfx_fill_rect(76 + (bit * 3), 38, 3, 12);
    }
}