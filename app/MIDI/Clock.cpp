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

// build_flags: -fno-inline -mfloat-abi=soft -mfpu=fpv5-d16

#include "../squares-and-circles-api.h"
#include <algorithm>

static int32_t bpm = 120;
static int32_t ppqn = 0;
static const char *ppqn_names[]{
    "4ppqn", "8ppqn", "24ppqn"};
static int32_t offset = 0;
static int32_t impulse = 1;
static int count_down = 0;
static uint32_t next = UINT32_MAX;
static uint32_t last_t = 0;

void engine::setup()
{
    // 24, 8 or 4 ppqn
    bpm = 120; // machine::midi_bpm() / 100;
    engine::addParam("@BPM\n%d", &bpm, 60, 250);
    engine::addParam("@ppqn", &ppqn, 0, 2, ppqn_names);
    engine::addParam("@Impulse\n%dms", &impulse, 1, 5);
    engine::addParam("@Offset\n%dms", &offset, 0, 127);
    engine::setMode(ENGINE_MODE_CV_OUT);
}

uint8_t last_clk = 0;
uint8_t clk_change(uint8_t clk)
{
    if (last_clk != clk)
    {
        last_clk = clk;
        return last_clk;
    }
    else
        return 0;
}

void engine::process()
{
    machine::clk_bpm(bpm * 100);
    uint8_t clk = clk_change(engine::clock());

    if (clk)
        last_t = engine::t();

    uint32_t div = ppqn == 0 ? 6 : (ppqn == 1 ? 3 : 1);

    if ((clk % div) == 1 || (ppqn > 1 && clk))
    {
        next = engine::t() + (offset * 2);
    }

    if (engine::t() == next)
    {
        next = UINT32_MAX;
        count_down = (impulse * 2);
    }

    int16_t a = count_down-- > 0 ? (5 * PITCH_PER_OCTAVE) : 0;
    std::fill_n(engine::outputBuffer_i16<0>(), FRAME_BUFFER_SIZE, a);
    std::fill_n(engine::outputBuffer_i16<1>(), FRAME_BUFFER_SIZE, a);
}

void engine::draw()
{
}
