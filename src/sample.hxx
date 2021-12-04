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
    const char *param_names[5] = {"Pitch", "", "Start", "End", nullptr};

    float i = 0;
    float start = 1;
    float end = 1;

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
    int sample_count = 1;
    float default_inc;
    float inc;

    uint16_t params_[4] = {INT16_MAX, 0, 0, UINT16_MAX};
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

    SampleEngine(const sample_spec<T> *samples, int select, int count) : ptr(samples), sample_count(count)
    {
        params_[1] = select;
    }

    void Process(const machine::ControlFrame &frame, float **out, float **aux) override
    {
        auto p = buffer;
        auto size = machine::FRAME_BUFFER_SIZE;

        if (frame.trigger)
        {
            SetParams(params_);
            i = start;
        }

        auto &smpl = ptr[params_[1]];
        param_names[1] = smpl.name;

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

    void OnEncoder(uint8_t param_index, int16_t inc, bool pressed) override
    {
        if (param_index == 1)
        {
            auto select = this->params_[1];
            if (inc > 0 && select < (sample_count - 1))
                select++;
            else if (inc < 0 && select > 0)
                select--;

            this->params_[1] = select;
            auto &spec = ptr[params_[1]];
            this->default_inc = 1.0f / spec.len * (spec.sample_rate / 48000.0f);
            this->inc = default_inc;
        }
        else
        {
            Engine::OnEncoder(param_index, inc, pressed);
        }
    }

    void SetParams(const uint16_t *params) override
    {
        for (int i = 0; i < 4; i++)
            params_[i] = params[i];

        auto &spec = ptr[params_[1]];
        this->default_inc = 1.0f / spec.len * (spec.sample_rate / 48000.0f);

        float pitch_fine = 0;
        float pitch_coarse = params_[0];
        pitch_coarse /= UINT16_MAX;
        pitch_coarse -= 0.5f;

        this->inc = default_inc + (default_inc * pitch_fine * 0.5f) + (default_inc * pitch_coarse * 2.f);
        this->start = (float)params_[2] / UINT16_MAX;
        this->end = (float)params_[3] / UINT16_MAX;
    }

    const char **GetParams(uint16_t *values) override
    {
        for (int i = 0; i < 4; i++)
        {
            values[i] = params_[i];
        }

        return param_names;
    }
};
