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
#include "machine.h"

using namespace machine;

struct Scope : public Engine
{
    float level = 0;
    int8_t wav[128];
    int j = 0;

    Scope() : Engine(AUDIO_PROCESSOR)
    {
    }

    void Process(const ControlFrame &frame, float **out, float **aux) override
    {
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
            level += frame.audio_in[0][i];

        level /= FRAME_BUFFER_SIZE;

        wav[(j++) % 128] = level * 24;

        *out = frame.audio_in[0];
        *aux = frame.audio_in[1];
    }

    void SetParams(const uint16_t *params) override
    {
    }

    const char **GetParams(uint16_t *values) override
    {
        static const char *names[]{nullptr};
        return names;
    }

    void OnDisplay(uint8_t *display)
    {
        for (int i = 0; i < 128; i += 2)
            gfx::drawLine(display, i, wav[i] + 40, i + 1, wav[i + 1] + 40);

        gfx::drawEngine(display, this);
    }
};

class CpuMonitor : public machine::Engine
{
    uint32_t loop = 0;

public:
    CpuMonitor()
    {
    }

    void Process(const machine::ControlFrame &frame, float **out, float **aux) override
    {
        for (uint32_t i = 0; i < loop; i++)
            asm volatile("nop");
    }

    void SetParams(const uint16_t *params) override
    {
        loop = params[0];
        loop *= 8;
    }

    const char **GetParams(uint16_t *rvalues) override
    {
        static const char *_names[] = {"%d nops", nullptr};

        rvalues[0] = loop / 8;

        return _names;
    }

    void OnDisplay(uint8_t *display)
    {
        char tmp[32];
        uint32_t procesing = machine::get_engine(0)->processing_time +
                             machine::get_engine(1)->processing_time +
                             machine::get_engine(2)->processing_time +
                             machine::get_engine(3)->processing_time;

        sprintf(tmp, "load_per_frame %lu", procesing);
        gfx::drawString(display, 4, 45, tmp);

        gfx::drawEngine(display, this);
    }
};

void init_scope()
{
    machine::add<Scope>(DEV, "Scope");
    machine::add<CpuMonitor>(DEV, "CPU-Monitor");
}

MACHINE_INIT(init_scope);
