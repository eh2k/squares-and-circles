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

#define private public
#include "lib/peaks/modulations/multistage_envelope.h"
#include "lib/stmlib/utils/dsp.h"
#include <algorithm>

#ifndef PEAKS_RESOURCES_H_
#include "lib/peaks/modulations/multistage_envelope.cc"
#include "lib/peaks/resources.cc"
#endif

peaks::MultistageEnvelope _processor;

int32_t _attack = 0;
int32_t _decay = UINT16_MAX / 2;
int32_t _sustain = UINT16_MAX / 2;
int32_t _release = UINT16_MAX / 2;
float scale = 1.0f;
float offset = 0;
float scale2 = 0;
float offset2 = 0;
int32_t output = 0;
int16_t* out_cv = nullptr;

void engine::setup()
{
    _processor.Init();

    if (!strcmp(engine::name(), "CV/EnvGen_ADSR"))
    {
        engine::addParam("Attack", &_attack, 0, UINT16_MAX);
        engine::addParam("Decay", &_decay, 0, UINT16_MAX);
        engine::addParam("Sustain", &_sustain, 0, UINT16_MAX);
        engine::addParam("Release", &_release, 0, UINT16_MAX);
        engine::addParam("@Scale\n%+.1f", &scale, -1, 1);
        engine::addParam("@Offset\n%+.1fV", &offset, -5, 5);
    }
    else
    {
        engine::addParam("Attack", &_attack, 0, UINT16_MAX);
        engine::addParam("Decay", &_decay, 0, UINT16_MAX);
        engine::addParam("@Scale\n%+.1f", &scale, -2, 2);
        engine::addParam("@Offset\n%+.1fV", &offset, -5, 5);
        // AD - Mode
        _sustain = -1;
        _release = -1;
    }

    out_cv = engine::outputBuffer_i16(OUT_CV);
    engine::setMode(ENGINE_MODE_COMPACT);
}

void engine::process()
{
    peaks::GateFlags flags[FRAME_BUFFER_SIZE];
    uint16_t params[4];
    scale2 = scale;
    offset2 = offset;

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

    _processor.Process(flags, out_cv, FRAME_BUFFER_SIZE);

    output = out_cv[0];

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        out_cv[i] *= scale / 4.f;
        out_cv[i] += (offset * PITCH_PER_OCTAVE);
    }
}

void draw_envelope(int base_line = 58, int height = 16)
{
    float sum_time = 0;
    for (size_t segment = 0; segment < _processor.num_segments_; segment++)
        sum_time += _processor.time_[segment];

    float x = 0;
    for (size_t segment = 0; segment < _processor.segment_; segment++)
        x += _processor.time_[segment];

    x += (float)_processor.phase_ / UINT32_MAX * _processor.time_[_processor.segment_];
    x *= (128.f / sum_time);

    int last_y = 0;
    for (int i = 0; i < 127; i++)
    {
        int segment = 0;
        uint32_t phase = 0;
        float j = sum_time * (i + 1) / 128.f;
        for (segment = 0; segment < _processor.num_segments_; segment++)
        {
            if (j <= _processor.time_[segment])
            {
                phase = j / _processor.time_[segment] * UINT32_MAX;
                break;
            }

            j -= _processor.time_[segment];
        }

        int32_t start = _processor.level_[segment];
        int32_t end = _processor.level_[segment + 1];

        const uint16_t *lutt[] = {peaks::lut_env_linear, peaks::lut_env_expo, peaks::lut_env_quartic};
        uint16_t t = stmlib::Interpolate824(lutt[_processor.shape_[segment]], phase);

        int16_t y = start + ((end - start) * (t >> 1) >> 15);
        y = ceil((float)y / INT16_MAX * height);
        int16_t y2 = ceil(1 + (float)output / INT16_MAX * height);

        gfx::drawLine(x, base_line, x, base_line - height);
        gfx::drawLine(i, base_line - last_y, i + 1, base_line - y);

        last_y = y;
    }
}

void engine::draw()
{
    if (_processor.num_segments_ <= 2)
    {
        int gnd = 58;
        if (scale2 < 0)
            gnd -= 16;

        CONSTRAIN(scale2, -1, 1);

        draw_envelope(gnd, 16 * scale2);
        for (int i = 0; i < 127; i += 2)
            gfx::setPixel(i, gnd);
    }
}

void engine::screensaver()
{
    gfx::clearRect(0, 0, 128, 64);
    for (int i = 0; i < 127; i += 2)
        gfx::setPixel(i, 32);

    draw_envelope(32 - (offset2 / 5 * 32), 32 * scale2);
}