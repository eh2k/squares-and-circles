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

// build_flags: -fno-inline -mfloat-abi=hard -mfpu=fpv5-d16 -ffast-math

#include "../squares-and-circles-api.h"
#include "lib/peaks/drums/snare_drum.h"

peaks::SnareDrum _processor;

int32_t _freq = UINT16_MAX / 2;
int32_t _snappy = UINT16_MAX / 2;
int32_t _tone = UINT16_MAX / 2;
int32_t _decay = UINT16_MAX / 2;

peaks::GateFlags flags[FRAME_BUFFER_SIZE];

void engine::setup()
{
    _processor.Init();
    std::fill(&flags[0], &flags[FRAME_BUFFER_SIZE], peaks::GATE_FLAG_LOW);

    engine::addParam("Pitch", &_freq, 0, UINT16_MAX);
    engine::addParam("Snappy", &_snappy, 0, UINT16_MAX);
    engine::addParam("Tone", &_tone, 0, UINT16_MAX);
    engine::addParam("Decay", &_decay, 0, UINT16_MAX);
}

void engine::process()
{
    _processor.set_frequency(_freq - 32768);
    _processor.set_tone(_tone);
    _processor.set_snappy(_snappy);
    _processor.set_decay(_decay);

    if (engine::trig())
    {
        flags[0] = peaks::GATE_FLAG_RISING;
        std::fill(&flags[1], &flags[FRAME_BUFFER_SIZE], peaks::GATE_FLAG_HIGH);
    }
    else if (engine::gate())
    {
        std::fill(&flags[0], &flags[FRAME_BUFFER_SIZE], peaks::GATE_FLAG_HIGH);
    }
    else
    {
        flags[0] = flags[0] == peaks::GATE_FLAG_HIGH ? peaks::GATE_FLAG_FALLING : peaks::GATE_FLAG_LOW;
        std::fill(&flags[1], &flags[FRAME_BUFFER_SIZE], peaks::GATE_FLAG_LOW);
    }

    _processor.Process(flags, engine::outputBuffer_i16<0>(), FRAME_BUFFER_SIZE);
}

#include "lib/peaks/drums/snare_drum.cc"
#include "lib/stmlib/utils/random.cc"
#include "lib/peaks/resources.cc"
