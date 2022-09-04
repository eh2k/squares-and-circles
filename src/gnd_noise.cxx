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
#include "misc/noise.hxx"

using namespace machine;

struct NoiseEngine : public Engine
{
    const char *modes[2] = {">White", ">Pink"};
    PinkNoise<> pink;
    float gain;
    uint8_t mode;

    NoiseEngine() : Engine(0)
    {
        param[0].init("Level", &gain, 1);
        param[1].init(">Type", &mode, 0, 0, LEN_OF(modes) - 1);
    };

    float buffer[machine::FRAME_BUFFER_SIZE];

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            if (mode == 0)
                buffer[i] = pink.white.nextf(-1.f, 1.f) * gain;
            else
                buffer[i] = pink.nextf(-1.f, 1.f) * gain;
        }

        of.push(buffer, machine::FRAME_BUFFER_SIZE);
    }

    void onDisplay(uint8_t *display) override
    {
        param[1].name = modes[mode];

        gfx::drawEngine(display, this);
    }
};

void init_noise()
{
    machine::add<NoiseEngine>(CV, "Noise");
}

MACHINE_INIT(init_noise);