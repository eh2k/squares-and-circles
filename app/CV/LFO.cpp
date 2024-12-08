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

// build_flags: -fno-inline -mfloat-abi=hard -mfpu=fpv5-d16

#include "../squares-and-circles-api.h"
#include "lib/peaks/modulations/lfo.h"

#include "lib/peaks/modulations/lfo.cc"
#include "lib/peaks/resources.cc"
#include "lib/stmlib/utils/random.cc"

peaks::Lfo _processor;

int32_t shape = peaks::LFO_SHAPE_SINE;
int32_t rate = -INT16_MIN;
int32_t waveform = -INT16_MIN;
float scale = 0.5f;
float offset = 0;

const char *shape_names[] = {"SIN", "TRI", "SQR", "STEPS", "NOISE"};

struct
{
    int8_t scope[128] = {};
    int i = 0;

    void draw(int x0, int y)
    {
        for (int x = x0; x < 127; x++)
        {
            if (x % 3 == 0)
                gfx::setPixel(x, y);
            gfx::drawLine(x, y - scope[(i + x) % 128], x + 1, y - scope[(1 + i + x) % 128]);
        }
    }

    void push(int y)
    {
        scope[i++ % 128] = y;
        if (i > 127)
            i = 0;
    }
} _scope;

void engine::setup()
{
    _processor.Init();
    _processor.set_level(UINT16_MAX);
    _processor.set_rate(rate);
    _processor.set_parameter((waveform - 32768));
    _processor.set_reset_phase(-INT16_MIN - 32768);

    engine::addParam("Freq.", &rate, 0, UINT16_MAX);
    engine::addParam("Shape", &shape, 0, peaks::LFO_SHAPE_LAST - 1, shape_names);
    engine::addParam("Scale", &scale, -0.5f, 0.5f);
    engine::addParam("Wavefrm", &waveform, 0, UINT16_MAX);
    engine::addParam("Offset", &offset, -1, 1);

    engine::setMode(ENGINE_MODE_COMPACT | ENGINE_MODE_CV_OUT);
}

void engine::process()
{
    peaks::GateFlags flags[FRAME_BUFFER_SIZE];

    _processor.set_shape((peaks::LfoShape)shape);
    _processor.set_rate(rate);
    _processor.set_parameter((waveform - 32768));

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

    int16_t *buffer = engine::outputBuffer_i16<0>();
    _processor.Process(flags, buffer, FRAME_BUFFER_SIZE);

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        buffer[i] >>= 1;
        buffer[i] *= scale;
        buffer[i] += (offset * PITCH_PER_OCTAVE * 5);
    }

    if ((engine::t() % 50) == 0)
        _scope.push(buffer[0] >> 10);
}

void engine::draw()
{
    _scope.draw(64, 51);
}