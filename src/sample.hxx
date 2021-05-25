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
#include "braids/envelope.h"

// TODO: use plaits envelope...

template <typename T, uint16_t len, uint16_t sample_rate = 48000>
struct Sample : public machine::Engine
{
    float i = 0;
    const float default_inc = 1.0f / len * (sample_rate / 48000.0f);
    float inc = default_inc;
    float start = 0;
    float end = 1;

    const T *data = nullptr;
    uint16_t params_[4] = {INT16_MAX, INT16_MAX, 0, UINT16_MAX};
    float buffer[machine::FRAME_BUFFER_SIZE];

    inline float At(const uint8_t *table, float pos)
    {
        const int shift = (sizeof(int16_t) * 8 - sizeof(uint8_t) * 8);
        float min = std::min(start, end);
        float max = std::max(start, end);
        if (pos < min || pos > max)
            return 0;

        int p = pos * (len - 2);
        float pd = (pos * (len - 1)) - p;
        float a = (1 - pd) * (table[p] - 127) / 128 + pd * (table[p + 1] - 127) / 128;

        return a;
    }

    inline float At(const int16_t *table, float pos)
    {
        float min = std::min(start, end);
        float max = std::max(start, end);
        if (pos < min || pos > max)
            return 0;

        int p = pos * (len - 1);
        float pd = (pos * (len - 1)) - p;
        float a = (1.0 - pd) * table[p] / INT16_MAX + pd * table[p + 1] / INT16_MAX;
        return a;
    }

public:
    bool loop = false;

    Sample(const uint8_t *sample) : data((const T *)sample)
    {
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

        while (size--)
        {
            *p++ = At(data, i);
            i += inc;
        }

        float s = std::min(start, end);
        float e = std::max(start, end);
        if (loop && (i < s || i > e))
            inc = -inc;

        *out = buffer;
    }

    void SetParams(const uint16_t *params) override
    {
        for (int i = 0; i < 4; i++)
            params_[i] = params[i];

        float pitch_fine = 0;
        float pitch_coarse = params_[0];
        pitch_coarse /= UINT16_MAX;
        pitch_coarse -= 0.5f;
        inc = default_inc + (default_inc * pitch_fine * 0.5f) + (default_inc * pitch_coarse * 2.f);
        start = params_[2];
        start /= UINT16_MAX;
        end = params_[3];
        end /= UINT16_MAX;
        if (start > end)
            inc = -inc;
    }

    const char **GetParams(uint16_t *values) override
    {
        static const char *param_names[] = {"Pitch", "", "Start", "End", nullptr};

        for (int i = 0; i < 4; i++)
        {
            values[i] = params_[i];
        }

        return param_names;
    }
};
