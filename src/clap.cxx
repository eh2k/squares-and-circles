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
#include "stmlib/utils/random.h"
#include "stmlib/dsp/filter.h"
#include "plaits/dsp/envelope.h"
#include "machine.h"

#ifndef TEST
#include "pgmspace.h"
#else
#define FLASHMEM
#endif

#include "base/SampleEngine.hxx"
#include "claps/cp909.h"
#include "claps/cp808.h"

struct TR909_CP : public SampleEngine
{
    const tsample_spec<int16_t> _sound = {"", (const int16_t *)cp909_raw, cp909_raw_len / 2, 44100, 0};

    TR909_CP() : SampleEngine(&_sound, 0, 1) {}
};

struct TR808_CP : public SampleEngine
{
    const tsample_spec<int16_t> _sound = {"", (const int16_t *)cp808_raw, cp808_raw_len / 2, 44100, 0};

    TR808_CP() : SampleEngine(&_sound, 0, 1) {}
};

#include "clouds/dsp/fx/diffuser.h"

using namespace machine;

class Clap : public Engine
{
private:
    clouds::Diffuser diffusor_;
    uint16_t buffer_diffusor[2048 + 1024];

    float buffer[FRAME_BUFFER_SIZE];
    float bufferAux[FRAME_BUFFER_SIZE];
    stmlib::Svf bpf_;

    float freq;
    float long_decay;
    float crispy;
    float diffuse = 0;

    plaits::DecayEnvelope env;

    TR808_CP cp;
    TR808_CP cp_loop;

public:
    Clap() : Engine(TRIGGER_INPUT)
    {
        diffuse = 0.2f;
        diffusor_.Init(&buffer_diffusor[0]);

        crispy = 2.5f;
        env.Init();
        bpf_.Init();
        freq = 0.3f;
        long_decay = 0.5f;

        param[0].init("Pitch", &freq, freq);
        param[1].init("Decay", &long_decay, long_decay);
        param[2].init("Crispy", &crispy, crispy);
        param[3].init("Diffuse", &diffuse, diffuse);

        cp_loop.loop = true;
        cp_loop.param[2].from_uint16(0.8f * UINT16_MAX);
        cp_loop.param[3].from_uint16(0.4f * UINT16_MAX);
    }

    uint32_t t = -1;
    float e = 0;
    float decay = 0.01f;
    float gain = 1.0f;

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        sync_params();

        const int ms = 48000 / 1000;

        if (frame.trigger)
        {
            t = 0;
        }

        OutputFrame cp_of;
        OutputFrame cp_loop_of;
        cp.process(frame, cp_of);
        cp_loop.process(frame, cp_loop_of);
        float* cp_out = (float *)cp_of.out;
        float* cp_loop_out = (float *)cp_loop_of.out;

        for (int i = 0; i < machine::FRAME_BUFFER_SIZE; i++)
        {
            if (t == 0 || t == 11 * ms || t == 23 * ms)
            {
                gain = 0.99f;
                decay = 0.005;
                env.Trigger();
            }
            else if (t == 31 * ms)
            {
                gain = 0.99f;
                env.Trigger();
                decay = (1.1f - long_decay) * 0.001f;
            }

            t++;

            env.Process(decay);

            float noise = (stmlib::Random::GetFloat() - 0.5f);
            noise = (1.f - crispy) * bpf_.Process<stmlib::FILTER_MODE_BAND_PASS_NORMALIZED>(noise);
            noise += cp_out[i] + cp_loop_out[i] * 10;
            noise *= 3;

            e = env.value() * gain;

            buffer[i] = e * noise;

            noise = (stmlib::Random::GetFloat() - 0.5f);
            noise = (1.f - crispy) * bpf_.Process<stmlib::FILTER_MODE_BAND_PASS_NORMALIZED>(noise);
            noise += cp_out[i] + cp_loop_out[i] * 5;
            noise *= 6;
            e = env.value() * gain;

            bufferAux[i] = e * noise;
        }

        diffusor_.Process(buffer, bufferAux, machine::FRAME_BUFFER_SIZE);

        of.out = buffer;
        of.aux = bufferAux;
    }

    void sync_params()
    {
        diffusor_.set_amount(diffuse);

        bpf_.set_g_r(0.02f + freq * 0.2f, 1 - freq * 0.1f);

        cp.param[0].from_uint16(0.99f * UINT16_MAX * freq);
        cp.param[2].from_uint16((1.f - crispy) * 0.1f * UINT16_MAX);

        cp_loop.param[0].from_uint16(0.99f * UINT16_MAX * freq);
    }
};

class Clap2 : public machine::Engine
{
private:
    float buffer[machine::FRAME_BUFFER_SIZE];
    stmlib::Svf bpf_;

    float freq;
    float long_decay;

    plaits::DecayEnvelope env;

public:
    Clap2()
    {
        env.Init();
        bpf_.Init();
        freq = 0.1f;
        long_decay = 0.5f;
        bpf_.set_g_r(0.1f + freq * 0.25f, 1 - freq * 0.25f);

        param[0].init("Color", &freq, freq);
        param[1].init("Decay", &long_decay, long_decay);
    }

    int t = 0;
    float e = 0;

    void process(const machine::ControlFrame &frame, OutputFrame &of) override
    {
        bpf_.set_g_r(0.1f + freq * 0.25f, 1 - freq * 0.25f);

        const int ms = 48000 / 1000;
        static float decay = 0.01f;
        static float gain = 1.0f;
        if (frame.trigger)
        {
            t = 0;
        }

        for (int i = 0; i < machine::FRAME_BUFFER_SIZE; i++)
        {
            if (t < 30 * ms)
            {
                gain = 0.8f;
                decay = 0.01;
                if (t % (int)(10 * ms) == 0)
                    env.Trigger();
            }
            else if (t == 30 * ms)
            {
                gain = 0.99f;
                env.Trigger();
                decay = (1.1f - long_decay) * 0.001f;
            }

            t++;

            float noise = (stmlib::Random::GetFloat() - 0.5f);
            float filtered_noise = bpf_.Process<stmlib::FILTER_MODE_BAND_PASS_NORMALIZED>(noise);
            env.Process(decay);
            e = env.value() * gain;

            buffer[i] = e * filtered_noise * 3;
        }

        of.out = buffer;
    }
};

void init_clap()
{
    machine::add<Clap>(machine::DRUM, "Clap");
    // machine::add<TR909_CP>(machine::DRUM, "TR909-Clap");
    // machine::add<TR808_CP>(machine::DRUM, "TR808-Clap");
    // machine::add<Clap2>(machine::DEV, "Clap2");
}

MACHINE_INIT(init_clap);