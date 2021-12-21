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

#include "stmlib/stmlib.h"
#include "stmlib/dsp/units.h"
#include "stmlib/dsp/filter.h"
#include "stmlib/dsp/delay_line.h"
#include "machine.h"
#include <vector>

#define clamp(value, min, max)             \
    (value > max ? max : value < min ? min \
                                     : value)

using namespace machine;

struct Delay : public Engine
{
    const char *param_names[5] = {"Time", "Color", "Pan", "Feedb", nullptr};

    float time = 0.5f;
    float color = 0.5f;
    float level = 0.5f;
    float pan = 0.5f;

    constexpr static int delay_len = machine::SAMPLE_RATE; //1s

    stmlib::DelayLine<uint16_t, delay_len> delay_mem[2];
    stmlib::OnePole filterLP[2];
    stmlib::OnePole filterHP[2];

    inline const float DelayRead(stmlib::DelayLine<uint16_t, delay_len> &line_, float delay) const
    {
        return static_cast<float>(static_cast<int16_t>(line_.Read(delay))) / 32768.0f;
    }

    inline void DelayWrite(stmlib::DelayLine<uint16_t, delay_len> &line_, const float sample)
    {
        line_.Write(static_cast<uint16_t>(
            stmlib::Clip16(static_cast<int32_t>(sample * 32768.0f))));
    }

    float bufferL[FRAME_BUFFER_SIZE];
    float bufferR[FRAME_BUFFER_SIZE];

    Delay() : Engine(AUDIO_PROCESSOR | IS_CLOCKED)
    {
        delay_mem[0].Init();
        delay_mem[1].Init();

        SetParam(0, 0, false);
    }

    size_t delay = 0;

    bool calc_t_step32(float *t_32)
    {
        float midi_bpm = 0;
        machine::midi_handler->getPlaybackInfo(&midi_bpm);
        if (midi_bpm > 0)
        {
            auto bpm = midi_bpm / (25.f / 24);
            auto t_per_beat = 60.f / bpm; // * machine::SAMPLE_RATE
            *t_32 = t_per_beat / 32;
            return true;
        }
        else
            return false;
    }

    void Process(const ControlFrame &frame, float **out, float **aux) override
    {
        auto d = time * machine::SAMPLE_RATE;

        float t_32 = 0;
        if (calc_t_step32(&t_32))
        {
            int n = 1 + time / t_32;
            d = n * t_32 * machine::SAMPLE_RATE;
        }

        if (abs(d - delay) > machine::SAMPLE_RATE / 10)
            delay = d;
        else
            ONE_POLE(delay, d, 0.01f);

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            float readL = DelayRead(delay_mem[0], delay);
            float readR = DelayRead(delay_mem[1], delay);

            auto inL = frame.audio_in[0][i];
            auto inR = frame.audio_in[1][i];

            inL = filterLP[0].Process<stmlib::FILTER_MODE_LOW_PASS>(inL);
            inL = filterHP[0].Process<stmlib::FILTER_MODE_HIGH_PASS>(inL);
            inR = filterLP[1].Process<stmlib::FILTER_MODE_LOW_PASS>(inR);
            inR = filterHP[1].Process<stmlib::FILTER_MODE_HIGH_PASS>(inR);

            DelayWrite(delay_mem[0], (readR + inL * (0 + pan) * 2) * level);
            DelayWrite(delay_mem[1], (readL + inR * (1 - pan) * 2) * level);

            bufferL[i] = readL + frame.audio_in[0][i];
            bufferR[i] = readR + frame.audio_in[1][i];
        }

        *out = bufferL;
        *aux = bufferR;
    }

    virtual void OnEncoder(uint8_t param_index, int16_t inc, bool pressed)
    {
        float t_32 = 0;
        if (param_index == 0 && calc_t_step32(&t_32))
        {
            inc *= t_32 * UINT16_MAX;
        }
        else
        {
            if (pressed == false)
                inc *= 8;

            inc *= 256;
        }

        SetParam(param_index, inc, false);
    }

    void SetParams(const uint16_t *params) override
    {
        time = (float)params[0] / UINT16_MAX;
        color = (float)params[1] / UINT16_MAX;

        float colorFreq = std::pow(100.f, 2.f * color - 1.f);
        float lowpassFreq = clamp(20000.f * colorFreq, 20.f, 20000.f) / machine::SAMPLE_RATE;
        float highpassFreq = clamp(20.f * colorFreq, 20.f, 20000.f) / machine::SAMPLE_RATE;

        filterLP[0].set_f<stmlib::FREQUENCY_DIRTY>(lowpassFreq);
        filterLP[1].set_f<stmlib::FREQUENCY_DIRTY>(lowpassFreq);
        filterHP[0].set_f<stmlib::FREQUENCY_DIRTY>(highpassFreq);
        filterHP[1].set_f<stmlib::FREQUENCY_DIRTY>(highpassFreq);

        level = (float)params[3] / UINT16_MAX;
        pan = (float)params[2] / UINT16_MAX;
    }

    char time_info[64] = "Time";
    const char **GetParams(uint16_t *values) override
    {
        param_names[0] = time_info;
        float t_32 = 0;
        if (calc_t_step32(&t_32))
        {
            int n = 1 + time / t_32;
            sprintf(time_info, ">T:%d/32", n);
        }
        else
            sprintf(time_info, ">T:%d ms", (int)(time * 1000));

        values[0] = time * UINT16_MAX;
        values[1] = color * UINT16_MAX;
        values[2] = pan * UINT16_MAX;
        values[3] = level * UINT16_MAX;
        return param_names;
    }
};

void init_delay()
{
    machine::add<Delay>(FX, "Delay");
}

MACHINE_INIT(init_delay);