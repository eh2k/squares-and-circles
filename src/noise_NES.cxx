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

#include "machine.h"
#include "misc/nes_noise.hxx"
#include "stmlib/dsp/filter.h"

using namespace machine;

struct NESNoiseEngine : public Engine
{
    NESNoise _nes_noise;
    stmlib::DCBlocker _dc_blocker;
    static constexpr uint8_t max_level = 15;
    uint8_t gain;

    NESNoiseEngine() : Engine()
    {
        param[0].init("Level", &gain, max_level, 0, max_level);
        param[1].init_presets("@Period", &_nes_noise.period_index, _nes_noise.period_index, 0, 15);
        param[2].init("ModeBit", &_nes_noise.mode_bit, _nes_noise.mode_bit, 0, 1);

        _nes_noise.init(1);
        _dc_blocker.Init(0.999f);
    };

    float buffer[machine::FRAME_BUFFER_SIZE];

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            buffer[i] = _nes_noise.generateSample(1.f / max_level) * gain;
        }

        _dc_blocker.Process(&buffer[0], FRAME_BUFFER_SIZE);

        of.push(buffer, machine::FRAME_BUFFER_SIZE);
    }

    void display() override
    {
        gfx::drawEngine(this);

        gfx::drawRect(75, 37, 47, 14);

        for (size_t bit = 0; bit < 15; bit++)
        {
            if ((_nes_noise.shift_reg & (1 << bit)) != 0)
                gfx::fillRect(76 + (bit * 3), 38, 3, 12);
        }
    }
};

void init_nes_noise()
{
    machine::add<NESNoiseEngine>(machine::NOISE, "NES_Noise");
}

MACHINE_INIT(init_noise);