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

#include "../squares-and-circles-api.h"
#include "stmlib/dsp/filter.h"
#include "stmlib/dsp/delay_line.h"
#include <stdio.h>

#define clamp(value, min, max)             \
    (value > max ? max : value < min ? min \
                                     : value)

int32_t time_steps = 16;
float raw = 1.f;
float time = 0.5f;
float color = 0.5f;
float level = 0.5f;
float pan = 0.5f;

constexpr static int delay_len = 48000; // 1s

stmlib::DelayLine<uint16_t, delay_len> delay_mem0;
stmlib::DelayLine<uint16_t, delay_len> delay_mem1;
stmlib::OnePole filterLP0;
stmlib::OnePole filterLP1;
stmlib::OnePole filterHP0;
stmlib::OnePole filterHP1;

inline const float DelayRead(stmlib::DelayLine<uint16_t, delay_len> *line_, float delay)
{
    return static_cast<float>(static_cast<int16_t>(line_->Read(delay))) / 32768.0f;
}

inline void DelayWrite(stmlib::DelayLine<uint16_t, delay_len> *line_, const float sample)
{
    line_->Write(static_cast<uint16_t>(
        stmlib::Clip16(static_cast<int32_t>(sample * 32768.0f))));
}

char time_info[64] = "Time";

void engine::setup()
{
    delay_mem0.Init();
    delay_mem1.Init();
    filterLP0.Init();
    filterLP1.Init();
    filterHP0.Init();
    filterHP1.Init();

    engine::addParam("D/W", &raw);
    engine::addParam(time_info, &time_steps, 1, 128);
    engine::addParam("Feedb", &level);
    engine::addParam("Color", &color);
    engine::addParam("Pan", &pan);
}

float delay = 0;
float t_32 = 0;


void calc_t_step32()
{
    uint32_t clk_bpm = engine_sync::clk_bpm();// / 100;
    if (clk_bpm > 0)
    {
        uint32_t bpm = clk_bpm;
        auto t_per_beat = 6000.f / bpm; // * machine::SAMPLE_RATE
        t_32 = t_per_beat / 32;
    }
    else
    {
        t_32 = 1.f / 128.f;
    }
}

void sync_params()
{
    calc_t_step32();
    time = time_steps * t_32;

    float colorFreq = powf(100.f, 2.f * color - 1.f);
    float lowpassFreq = clamp(20000.f * colorFreq, 20.f, 20000.f) / SAMPLE_RATE;
    float highpassFreq = clamp(20.f * colorFreq, 20.f, 20000.f) / SAMPLE_RATE;

    filterLP0.set_f<stmlib::FREQUENCY_DIRTY>(lowpassFreq);
    filterLP1.set_f<stmlib::FREQUENCY_DIRTY>(lowpassFreq);
    filterHP0.set_f<stmlib::FREQUENCY_DIRTY>(highpassFreq);
    filterHP1.set_f<stmlib::FREQUENCY_DIRTY>(highpassFreq);
}

void engine::process()
{
    sync_params();

    int n = time / t_32;
    float d = n * t_32 * SAMPLE_RATE;

    if (fabsf(d - delay) > SAMPLE_RATE / 10)
        delay = d;
    else
        ONE_POLE(delay, d, 0.01f);

    auto inputL = engine::inputBuffer<0>();
    auto inputR = engine::inputBuffer<1>();
    auto outputL = engine::outputBuffer<0>();
    auto outputR = engine::outputBuffer<1>();

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        float readL = DelayRead(&delay_mem0, (int)delay);
        float readR = DelayRead(&delay_mem1, (int)delay);

        auto inL = inputL[i];
        auto inR = inputR[i];

        inL = filterLP0.Process<stmlib::FILTER_MODE_LOW_PASS>(inL);
        inL = filterHP0.Process<stmlib::FILTER_MODE_HIGH_PASS>(inL);
        inR = filterLP1.Process<stmlib::FILTER_MODE_LOW_PASS>(inR);
        inR = filterHP1.Process<stmlib::FILTER_MODE_HIGH_PASS>(inR);

        DelayWrite(&delay_mem0, (readR + inL * (0 + pan) * 2) * level);
        DelayWrite(&delay_mem1, (readL + inR * (1 - pan) * 2) * level);

        outputL[i] = readL + inputL[i];
        outputR[i] = readR + inputR[i];

        outputL[i] = raw * outputL[i] + (1 - raw) * inputL[i];
        outputR[i] = raw * outputR[i] + (1 - raw) * inputR[i];
    }
}

void engine::draw()
{
    if (engine_sync::clk_bpm() > 0)
    {
        sprintf(time_info, ">t=%d", time_steps);
    }
    else
        sprintf(time_info, ">T:%d ms", (int)(time * 1000));
}