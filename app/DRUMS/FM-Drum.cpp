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

#include "../squares-and-circles-api.h"
#include "peaks/drums/fm_drum.h"
#include "peaks/drums/fm_drum.cc"
#include "resources/peaks_lut_osc.hpp"
#include "resources/peaks_lut_env.hpp"
#include "stmlib/utils/random.cc"

static peaks::FmDrum _processor;

static int32_t _freq = UINT16_MAX / 2;
static int32_t _punch = UINT16_MAX / 2;
static int32_t _tone = UINT16_MAX / 2;
static int32_t _decay = UINT16_MAX / 2;

peaks::GateFlags flags[FRAME_BUFFER_SIZE];

void engine::setup()
{
    _processor.Init();
    std::fill(&flags[0], &flags[FRAME_BUFFER_SIZE], peaks::GATE_FLAG_LOW);

    engine::addParam("Freq.", &_freq, 0, UINT16_MAX);
    engine::addParam("Noise", &_punch, 0, UINT16_MAX);
    engine::addParam("FM", &_tone, 0, UINT16_MAX);
    engine::addParam("Decay", &_decay, 0, UINT16_MAX);
}

void engine::process()
{
    int32_t freq = (_freq) + (engine::cv_i32() / PITCH_PER_OCTAVE * INT16_MAX / 3); // CV or Midi Pitch ?!
    freq += (-2 * INT16_MAX / 3);
    CONSTRAIN(freq, 0, UINT16_MAX); // BUG: lower values?

    _processor.set_frequency(freq);
    _processor.set_fm_amount(((uint16_t)(_tone) >> 2) * 3);
    _processor.set_decay(_decay);
    _processor.set_noise(_punch);

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