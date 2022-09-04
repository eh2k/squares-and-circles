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
#include "stmlib/dsp/units.h"

struct SampleEngine : public machine::Engine
{
    float i = 1; // 0=plays the sample on init, 1=plays the sample next trig
    float start = 1;
    float end = 1;

    uint16_t selection = 0;
    float pitch_coarse = 0;

    struct sample_spec
    {
        const char *name;
        int len;
        int sample_rate;
        const void *data;
        int addr_shift;
        virtual float get_float(int index) const = 0;
    };

    const sample_spec *ptr = nullptr;
    float default_inc;
    float inc;

    float buffer[machine::FRAME_BUFFER_SIZE];

    inline float Interpolate(const sample_spec &table, float index)
    {
        index *= table.len;
        MAKE_INTEGRAL_FRACTIONAL(index)
        float a = table.get_float(index_integral);
        float b = table.get_float(index_integral + 1);
        return a + (b - a) * index_fractional;
    }

    inline float InterpolateHermite(const sample_spec &table, float index)
    {
        index *= table.len;
        MAKE_INTEGRAL_FRACTIONAL(index)
        const float xm1 = table.get_float(index_integral - 1);
        const float x0 = table.get_float(index_integral + 0);
        const float x1 = table.get_float(index_integral + 1);
        const float x2 = table.get_float(index_integral + 2);
        const float c = (x1 - xm1) * 0.5f;
        const float v = x0 - x1;
        const float w = c + v;
        const float a = w + v + (x2 - x0) * 0.5f;
        const float b_neg = w + a;
        const float f = index_fractional;
        return (((a * f) - b_neg) * f + c) * f + x0;
    }

public:
    int loop = 0; // 1 = knight rider, 2 infinite

    uint8_t rtrg = 0;
    uint8_t rtim = 0;
    uint8_t rtrg2 = 0;
    uint32_t trg_next = 0;

    SampleEngine(const sample_spec *samples, int select, int count) : machine::Engine()
    {
        ptr = samples;
        param[0].init("Pitch", &pitch_coarse, pitch_coarse, -.5f, .5f);
        param[1].init("Sample", &selection, select, 0, count - 1);
        param[2].init("Start", &start, 0);
        param[3].init("End", &end, 1);
        // param[4].init("RTRG", &rtrg, 0, 0, 16);
        // param[5].init("RTIM", &rtim, 2, 1, 64);
        // param[5].print_value = [&](char *tmp)
        // {
        //     sprintf(tmp, "%c%d/32", param[5].flags & machine::Parameter::IS_SELECTED ? '>' : ' ', rtim);
        // };
    }

    void process(const machine::ControlFrame &frame, machine::OutputFrame &of) override
    {
        auto &smpl = ptr[selection];

        this->default_inc = 1.0f / smpl.len * (smpl.sample_rate / (float)machine::SAMPLE_RATE);

        this->default_inc *= stmlib::SemitonesToRatio(frame.cv_voltage() * 12);

        float pitch_fine = 0;
        this->inc = (this->inc < 0 ? -1.f : 1.f) *
                    (default_inc + (default_inc * pitch_fine * 0.5f) + (default_inc * pitch_coarse * 2.f));

        auto p = buffer;
        auto size = machine::FRAME_BUFFER_SIZE;

        if (frame.trigger)
        {
            i = start;
        }

        // if (frame.trigger || (rtrg2 > 0 && trg_next == frame.t))
        // {
        //     if (frame.trigger)
        //         rtrg2 = rtrg + 1;

        //     auto bpm = machine::get_bpm() / (25.f / 24);
        //     auto t_per_beat = 60.f / bpm;
        //     auto t_32 = t_per_beat / 32.f;
        //     --rtrg2;
        //     static constexpr float t_per_frame = 1.f / (machine::SAMPLE_RATE / machine::FRAME_BUFFER_SIZE);
        //     trg_next = frame.t + (t_32 / t_per_frame * rtim);
        //     i = start;
        // }

        float s = std::min(start, end);
        float e = std::max(start, end);

        while (size--)
        {
            *p++ = s <= i && i < e ? InterpolateHermite(smpl, i) : 0;
            i += this->start < this->end ? inc : -inc;
        }

        if (loop == 0)
        {

        }
        else if (loop == 1)
        {
            if (i < s || i > e)
                inc = -inc;
        }
        else if (loop == 2)
        {
            if (i < s)
                i += (e-s);
            else if (i > e)
                i -= (e-s);
        }

        of.out = buffer;
    }

    void onDisplay(uint8_t *buffer) override
    {
        auto &smpl = ptr[selection];
        param[1].name = smpl.name;

        gfx::drawEngine(buffer, this);
    }
};

template <typename T>
struct tsample_spec : SampleEngine::sample_spec
{
    virtual float get_float(int index) const;

    tsample_spec(const char *name, const T *data, size_t len, uint16_t sample_rate, int custom)
    {
        this->data = data;
        this->addr_shift = custom;
        this->name = name;
        this->len = len;
        this->sample_rate = sample_rate;
    }
};

struct Am6070sample : tsample_spec<uint8_t>
{
    // https://electricdruid.net/experiments-with-variable-rate-drum-sample-playback/
    // https://electricdruid.net/wp-content/uploads/2018/06/AM6070-uLaw-DAC.pdf

    // 12-bit antilog as per Am6070 datasheet
    // for (int i = 0; i < 128; i++)
    //     antilog[i] = (int16_t)((2 * powf(2.0, i >> 4) * ((i & 15) + 16.5) - 16.5));

    float get_float(int index) const override
    {
        static const int16_t antilog[128] = {
            0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30,
            33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 86, 89, 93,
            99, 107, 115, 123, 131, 139, 147, 155, 163, 171, 179, 187, 195, 203, 211, 219,
            231, 247, 263, 279, 295, 311, 327, 343, 359, 375, 391, 407, 423, 439, 455, 471,
            495, 527, 559, 591, 623, 655, 687, 719, 751, 783, 815, 847, 879, 911, 943, 975,
            1023, 1087, 1151, 1215, 1279, 1343, 1407, 1471, 1535, 1599, 1663, 1727, 1791, 1855, 1919, 1983,
            2079, 2207, 2335, 2463, 2591, 2719, 2847, 2975, 3103, 3231, 3359, 3487, 3615, 3743, 3871, 3999,
            4191, 4447, 4703, 4959, 5215, 5471, 5727, 5903, 6239, 6495, 6751, 7007, 7263, 7519, 7775, 8031};

        if (index < this->len)
        {
            auto ix = reinterpret_cast<const uint8_t *>(this->data)[index];
            return (float)((ix & 0x80) ? -antilog[ix & 0x7F] : antilog[ix]) / INT16_MAX * this->addr_shift;
        }
        else
            return 0;
    }

    Am6070sample(const char *name, const uint8_t *data, size_t len, uint16_t sample_rate, int custom)
        : tsample_spec<uint8_t>(name, data, len, sample_rate, custom)
    {
    }
};