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
#include "stmlib/dsp/filter.h"
#include "machine.h"
#include <vector>

#include "clouds/dsp/fx/reverb.h"

using namespace machine;

struct CloudsReverb : public Engine
{
    float raw = 0;
    uint16_t buffer[16384];
    clouds::Reverb fx_;

    float bufferL[FRAME_BUFFER_SIZE];
    float bufferR[FRAME_BUFFER_SIZE];

    CloudsReverb() : Engine(AUDIO_PROCESSOR)
    {
        raw = 1.f;
        memset(buffer, 0, sizeof(buffer));
        fx_.Init(buffer);
        uint16_t defaults[] = {UINT16_MAX, INT16_MAX + INT16_MAX / 2, INT16_MAX, UINT16_MAX};
        SetParams(defaults);
    }

    void Process(const ControlFrame &frame, float **out, float **aux) override
    {
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            bufferL[i] = frame.audio_in[0][i];
            bufferR[i] = frame.audio_in[1][i];
        }

        fx_.Process(bufferL, bufferR, FRAME_BUFFER_SIZE);

        for (int i = 0; i < FRAME_BUFFER_SIZE; ++i)
        {
            bufferL[i] = raw * bufferL[i] + (1 - raw) * frame.audio_in[0][i];
            bufferR[i] = raw * bufferR[i] + (1 - raw) * frame.audio_in[1][i];
        }

        *out = bufferL;
        *aux = bufferR;
    }

    uint16_t reverb_amount;
    uint16_t feedback;
    float gain;

    void SetParams(const uint16_t *params) override
    {
        raw = (float)params[0] / UINT16_MAX;
        reverb_amount = params[1];
        feedback = params[2];
        gain = (float)params[3] / UINT16_MAX;

        fx_.set_amount(reverb_amount * 0.54f / UINT16_MAX);
        fx_.set_diffusion(0.7f);
        fx_.set_time(0.35f + 0.63f * reverb_amount / UINT16_MAX);
        fx_.set_input_gain(gain * 0.1f); // 0.1f);
        fx_.set_lp(0.6f + 0.37f * feedback / UINT16_MAX);
    }

    const char **GetParams(uint16_t *values) override
    {
        static const char *names[]{"D/W", "Reverb", "Damp", "Gain", nullptr};
        values[0] = raw * UINT16_MAX;
        values[1] = reverb_amount;
        values[2] = feedback;
        values[3] = gain * UINT16_MAX;
        return names;
    }
};

#include "clouds/dsp/fx/diffuser.h"

struct CloudsDiffuser : public Engine
{
    float raw = 0;
    clouds::Diffuser fx_;
    uint16_t buffer[2048 + 1024];

    float bufferL[FRAME_BUFFER_SIZE];
    float bufferR[FRAME_BUFFER_SIZE];

    CloudsDiffuser() : Engine(AUDIO_PROCESSOR)
    {
        raw = 1.f;
        fx_.Init(&buffer[0]);

        uint16_t defaults[] = {UINT16_MAX};
        SetParams(defaults);
    }

    void Process(const ControlFrame &frame, float **out, float **aux) override
    {
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            bufferL[i] = frame.audio_in[0][i];
            bufferR[i] = frame.audio_in[1][i];
        }

        fx_.Process(bufferL, bufferR, FRAME_BUFFER_SIZE);

        *out = bufferL;
        *aux = bufferR;
    }

    void SetParams(const uint16_t *params) override
    {
        raw = (float)params[0] / UINT16_MAX;
        fx_.set_amount(raw);
    }

    const char **GetParams(uint16_t *values) override
    {
        values[0] = raw * UINT16_MAX;
        static const char *names[]{"Amount", nullptr};
        return names;
    }
};

void init_reverb()
{
    machine::add<CloudsReverb>(FX, "Reverb");
    machine::add<CloudsDiffuser>(FX, "Diffusor");
}

MACHINE_INIT(init_reverb);