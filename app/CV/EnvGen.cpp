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

// ENGINE_NAME:CV/EnvGen_AD;CV/EnvGen_ADSR
// build_flags: -fno-inline -mfloat-abi=hard -mfpu=fpv5-d16

#include "../squares-and-circles-api.h"
#include "lib/peaks/modulations/multistage_envelope.h"

#include "lib/peaks/modulations/multistage_envelope.cc"
#include "lib/peaks/resources.cc"

peaks::MultistageEnvelope _processor;

int32_t _attack = 0;
int32_t _decay = UINT16_MAX / 2;
int32_t _sustain = UINT16_MAX / 2;
int32_t _release = UINT16_MAX / 2;
float scale = 0.5f;
float offset = 0;
int32_t dummy = 0;

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

    if (!strcmp(engine::name(), "CV/EnvGen_ADSR"))
    {
        engine::addParam("Attack", &_attack, 0, UINT16_MAX);
        engine::addParam("Decay", &_decay, 0, UINT16_MAX);
        engine::addParam("Sustain", &_sustain, 0, UINT16_MAX);
        engine::addParam("Release", &_release, 0, UINT16_MAX);
        engine::addParam("AttVer", &scale, -1, 1);
        engine::addParam("Offset", &offset, -1, 1);
    }
    else
    {
        engine::addParam("Attack", &_attack, 0, UINT16_MAX);
        engine::addParam("Decay", &_decay, 0, UINT16_MAX);
        engine::addParam("AttVer", &scale, -1, 1);
        engine::addParam("Offset", &offset, -1, 1);
        // AD - Mode
        _sustain = -1;
        _release = -1;
    }

    engine::setMode(ENGINE_MODE_COMPACT | ENGINE_MODE_CV_OUT);
}

void engine::process()
{
    peaks::GateFlags flags[FRAME_BUFFER_SIZE];

    uint16_t params[4];
    params[0] = _attack;
    params[1] = _decay;
    if (_sustain >= 0)
    {
        params[2] = _sustain;
        params[3] = _release;
        _processor.Configure(params, peaks::CONTROL_MODE_FULL);
    }
    else
    {
        // AD - Mode
        _processor.Configure(params, peaks::CONTROL_MODE_HALF);
    }

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

    if ((engine::t() % 50) == 0)
    {
        int8_t v = buffer[0] * scale / (INT16_MAX / 16);
        _scope.push(v);
    }

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        buffer[i] >>= 1;
        buffer[i] *= scale;
        buffer[i] += (offset * PITCH_PER_OCTAVE * 5);
    }
}

void engine::draw()
{
    if (_sustain < 0)
    {
        if (scale > 0)
            _scope.draw(0, 58);
        else
            _scope.draw(0, 58 - 14);
    }
}