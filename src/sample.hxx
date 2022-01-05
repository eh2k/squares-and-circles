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

#pragma once
#include "machine.h"
#include "stmlib/dsp/dsp.h"

template <typename T>
struct SampleEngine : public machine::Engine
{
    float i = 1; //0=plays the sample on init, 1=plays the sample next trig
    float start = 1;
    float end = 1;

    uint16_t selection = 0;
    float pitch_coarse = 0;

    template <typename U>
    struct sample_spec
    {
        const char *name;
        const U *data;
        int len;
        int sample_rate;
        int addr_shift;
    };

    const sample_spec<T> *ptr;
    float default_inc;
    float inc;

    float buffer[machine::FRAME_BUFFER_SIZE];

    inline float get_float(const sample_spec<uint8_t> &smpl, int index)
    {
        if (index < smpl.len && index >= 0)
            return ((float)smpl.data[index << smpl.addr_shift] - 127) / 128;
        else
            return 0;
    }

    inline float get_float(const sample_spec<int16_t> &smpl, int index)
    {

        if (index < smpl.len && index >= 0)
            return (float)smpl.data[index << smpl.addr_shift] / INT16_MAX;
        else
            return 0;
    }

    inline float Interpolate(const sample_spec<T> &table, float index)
    {
        index *= table.len;
        MAKE_INTEGRAL_FRACTIONAL(index)
        float a = get_float(table, index_integral);
        float b = get_float(table, index_integral + 1);
        return a + (b - a) * index_fractional;
    }

    inline float InterpolateHermite(const sample_spec<T> &table, float index)
    {
        index *= table.len;
        MAKE_INTEGRAL_FRACTIONAL(index)
        const float xm1 = get_float(table, index_integral - 1);
        const float x0 = get_float(table, index_integral + 0);
        const float x1 = get_float(table, index_integral + 1);
        const float x2 = get_float(table, index_integral + 2);
        const float c = (x1 - xm1) * 0.5f;
        const float v = x0 - x1;
        const float w = c + v;
        const float a = w + v + (x2 - x0) * 0.5f;
        const float b_neg = w + a;
        const float f = index_fractional;
        return (((a * f) - b_neg) * f + c) * f + x0;
    }

public:
    bool loop = false;

    SampleEngine(const sample_spec<T> *samples, int select, int count) : ptr(samples)
    {
        param[0].init("Pitch", &pitch_coarse, pitch_coarse, -.5f, .5f);
        param[1].init("Sample", &selection, select, 0, count - 1);
        param[2].init("Start", &start, 0);
        param[3].init("End", &end, 1);
    }

    void Process(const machine::ControlFrame &frame, float **out, float **aux) override
    {
        auto &smpl = ptr[selection];

        this->default_inc = 1.0f / smpl.len * (smpl.sample_rate / (float)machine::SAMPLE_RATE);

        float pitch_fine = 0;
        this->inc = (this->inc < 0 ? -1.f : 1.f) *
                    (default_inc + (default_inc * pitch_fine * 0.5f) + (default_inc * pitch_coarse * 2.f));

        auto p = buffer;
        auto size = machine::FRAME_BUFFER_SIZE;

        if (frame.trigger)
        {
            i = start;
        }

        float s = std::min(start, end);
        float e = std::max(start, end);

        while (size--)
        {
            *p++ = s <= i && i < e ? InterpolateHermite(smpl, i) : 0;
            i += this->start < this->end ? inc : -inc;
        }

        if (loop)
        {

            if (i < s || i > e)
                inc = -inc;
        }

        *out = buffer;
    }

    void OnDisplay(uint8_t *buffer) override
    {
        auto &smpl = ptr[selection];
        param[1].name = smpl.name;

        gfx::drawEngine(buffer, this);
    }
};
