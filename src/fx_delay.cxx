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
    float time = 0.5f;
    float color = 0.5f;
    float level = 0.5f;
    float pan = 0.5f;

    constexpr static int delay_len = 48000; // 1s

    stmlib::DelayLine<uint16_t, delay_len> *delay_mem[2];
    stmlib::OnePole filterLP[2];
    stmlib::OnePole filterHP[2];

    inline const float DelayRead(stmlib::DelayLine<uint16_t, delay_len> *line_, float delay) const
    {
        return static_cast<float>(static_cast<int16_t>(line_->Read(delay))) / 32768.0f;
    }

    inline void DelayWrite(stmlib::DelayLine<uint16_t, delay_len> *line_, const float sample)
    {
        line_->Write(static_cast<uint16_t>(
            stmlib::Clip16(static_cast<int32_t>(sample * 32768.0f))));
    }

    float bufferL[FRAME_BUFFER_SIZE];
    float bufferR[FRAME_BUFFER_SIZE];

    Delay() : Engine(AUDIO_PROCESSOR)
    {
        if (delay_mem[0] = (stmlib::DelayLine<uint16_t, delay_len> *)machine::malloc(sizeof(stmlib::DelayLine<uint16_t, delay_len>)))
            delay_mem[0]->Init();
        if (delay_mem[1] = (stmlib::DelayLine<uint16_t, delay_len> *)machine::malloc(sizeof(stmlib::DelayLine<uint16_t, delay_len>)))
            delay_mem[1]->Init();

        param[0].init("Time", &time, time);
        param[1].init("Color", &color, color);
        param[2].init("Pan", &pan, pan);
        param[3].init("Feedb", &level, level);
    }

    ~Delay() override
    {
        machine::mfree(delay_mem[0]);
        machine::mfree(delay_mem[1]);
    }

    float delay = 0;
    float t_32 = 0;

    bool calc_t_step32()
    {
        float midi_bpm = 1.f / 100 * machine::get_bpm();
        if (midi_bpm > 0)
        {
            auto bpm = midi_bpm / (25.f / 24);
            auto t_per_beat = 60.f / bpm; // * machine::SAMPLE_RATE
            t_32 = t_per_beat / 32;
            return true;
        }
        else
        {
            t_32 = 1.f / 256.f;
            return false;
        }
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        if (delay_mem[0] == nullptr || delay_mem[1] == nullptr)
            return;

        sync_params();

        int n = 1 + time / t_32;
        float d = n * t_32 * machine::SAMPLE_RATE;

        if (fabsf(d - delay) > machine::SAMPLE_RATE / 10)
            delay = d;
        else
            ONE_POLE(delay, d, 0.01f);

        float *ins[] = {machine::get_aux(AUX_L), machine::get_aux(AUX_R)};

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            float readL = DelayRead(delay_mem[0], (int)delay);
            float readR = DelayRead(delay_mem[1], (int)delay);

            auto inL = ins[0][i];
            auto inR = ins[1][i];

            inL = filterLP[0].Process<stmlib::FILTER_MODE_LOW_PASS>(inL);
            inL = filterHP[0].Process<stmlib::FILTER_MODE_HIGH_PASS>(inL);
            inR = filterLP[1].Process<stmlib::FILTER_MODE_LOW_PASS>(inR);
            inR = filterHP[1].Process<stmlib::FILTER_MODE_HIGH_PASS>(inR);

            DelayWrite(delay_mem[0], (readR + inL * (0 + pan) * 2) * level);
            DelayWrite(delay_mem[1], (readL + inR * (1 - pan) * 2) * level);

            bufferL[i] = readL + ins[0][i];
            bufferR[i] = readR + ins[1][i];
        }

        of.out = bufferL;
        of.aux = bufferR;
    }

    void sync_params()
    {
        calc_t_step32();
        param[0].setStepValue(t_32);

        float colorFreq = std::pow(100.f, 2.f * color - 1.f);
        float lowpassFreq = clamp(20000.f * colorFreq, 20.f, 20000.f) / machine::SAMPLE_RATE;
        float highpassFreq = clamp(20.f * colorFreq, 20.f, 20000.f) / machine::SAMPLE_RATE;

        filterLP[0].set_f<stmlib::FREQUENCY_DIRTY>(lowpassFreq);
        filterLP[1].set_f<stmlib::FREQUENCY_DIRTY>(lowpassFreq);
        filterHP[0].set_f<stmlib::FREQUENCY_DIRTY>(highpassFreq);
        filterHP[1].set_f<stmlib::FREQUENCY_DIRTY>(highpassFreq);
    }

    char time_info[64] = "Time";
    void display() override
    {
        if (calc_t_step32())
        {
            int n = 1 + time / t_32;
            sprintf(time_info, ">t=%d", n);
        }
        else
            sprintf(time_info, ">T:%d ms", (int)(time * 1000));

        param[0].name = time_info;

        float midi_bpm = 1.f / 100 * machine::get_bpm();
        if (midi_bpm > 0)
        {
            char tmp[16];
            sprintf(tmp, "BPM:%.1f", midi_bpm); // dtostrf(midi_bpm, 2, 1, &tmp[4]);
            gfx::drawString(10, 28, tmp, 0);
        }

        if (delay_mem[0] == nullptr || delay_mem[1] == nullptr)
            gfx::drawEngine(this, machine::OUT_OF_MEMORY);
        else
            gfx::drawEngine(this);
    }
};

void init_delay()
{
    machine::add<Delay>(FX, "Delay");
}

MACHINE_INIT(init_delay);