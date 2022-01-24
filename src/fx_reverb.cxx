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
    float reverb_amount;
    float feedback;
    float gain;

    uint16_t buffer[16384];
    clouds::Reverb fx_;

    float bufferL[FRAME_BUFFER_SIZE];
    float bufferR[FRAME_BUFFER_SIZE];

    CloudsReverb() : Engine(AUDIO_PROCESSOR)
    {
        raw = 1.f;
        memset(buffer, 0, sizeof(buffer));
        fx_.Init(buffer);

        param[0].init("D/W", &raw, raw);
        param[1].init("Reverb", &reverb_amount, 0.75f);
        param[2].init("Damp", &feedback, 0.5f);
        param[3].init("Gain", &gain, 1.f);
    }

    void Process(const ControlFrame &frame, float **out, float **aux) override
    {
        fx_.set_amount(reverb_amount * 0.54f);
        fx_.set_diffusion(0.7f);
        fx_.set_time(0.35f + 0.63f * reverb_amount);
        fx_.set_input_gain(gain * 0.1f); // 0.1f);
        fx_.set_lp(0.6f + 0.37f * feedback);

        float *ins[] = {machine::get_aux(AUX_L), machine::get_aux(AUX_R)};

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            bufferL[i] = ins[0][i];
            bufferR[i] = ins[1][i];
        }

        fx_.Process(bufferL, bufferR, FRAME_BUFFER_SIZE);

        for (int i = 0; i < FRAME_BUFFER_SIZE; ++i)
        {
            bufferL[i] = raw * bufferL[i] + (1 - raw) * ins[0][i];
            bufferR[i] = raw * bufferR[i] + (1 - raw) * ins[1][i];
        }

        *out = bufferL;
        *aux = bufferR;
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
        param[0].init("Amount", &raw, raw);
    }

    void Process(const ControlFrame &frame, float **out, float **aux) override
    {
        fx_.set_amount(raw);

        float *ins[] = {machine::get_aux(-2), machine::get_aux(-1)};

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            bufferL[i] = ins[0][i];
            bufferR[i] = ins[1][i];
        }

        fx_.Process(bufferL, bufferR, FRAME_BUFFER_SIZE);

        *out = bufferL;
        *aux = bufferR;
    }
};

void init_reverb()
{
    machine::add<CloudsReverb>(FX, "Reverb");
    //machine::add<CloudsDiffuser>(FX, "Diffusor");
}

MACHINE_INIT(init_reverb);