// Copyright (C)2022 - Eduard Heidt
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

extern "C"
{
#include "soundpipe/revsc.h"
}

using namespace machine;

struct ReverbSC : public Engine
{
    sp_data sp_data_;
    sp_revsc sp_revsc_;
    static constexpr uint32_t AUX_SIZE = 107440; // sp_revsc_.aux.size
    uint8_t *mem;

    float raw = 1.f;
    float reverb_amount;
    float feedback;
    float gain;

    float bufferL[FRAME_BUFFER_SIZE];
    float bufferR[FRAME_BUFFER_SIZE];

    ReverbSC() : Engine(AUDIO_PROCESSOR)
    {
        mem = (uint8_t *)machine::malloc(AUX_SIZE);
        if (mem)
        {
            sp_data_.sr = machine::SAMPLE_RATE;
            sp_data_.aux.ptr = &mem[0];
            sp_data_.aux.size = AUX_SIZE;
            sp_revsc_init(&sp_data_, &sp_revsc_);
            param[0].init("D/W", &raw, raw);
            param[1].init("Feedback", &sp_revsc_.feedback, 0.97f, 0, 1);
            param[2].init("LpFreq", &sp_revsc_.lpfreq, 10000, 0, machine::SAMPLE_RATE / 2);
        }
    }

    ~ReverbSC() override
    {
        machine::mfree(mem);
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        if (mem == nullptr)
            return;

        float *ins[] = {machine::get_aux(AUX_L), machine::get_aux(AUX_R)};

        for (int i = 0; i < FRAME_BUFFER_SIZE; ++i)
        {
            sp_revsc_compute(&sp_data_, &sp_revsc_, &ins[0][i], &ins[1][i], &bufferL[i], &bufferR[i]);
            bufferL[i] = raw * bufferL[i] + (1 - raw) * ins[0][i];
            bufferR[i] = raw * bufferR[i] + (1 - raw) * ins[1][i];
        }

        of.out = bufferL;
        of.aux = bufferR;
    }

    void display() override
    {
        gfx::drawEngine(this, mem ? nullptr : machine::OUT_OF_MEMORY);
    }
};

void init_reverbSC()
{
    machine::add<ReverbSC>(FX, "ReverbSC");
}

MACHINE_INIT(init_reverbSC);