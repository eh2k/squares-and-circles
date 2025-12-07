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

// ENGINE_NAME:NOISE/WhitePink

#include "../squares-and-circles-api.h"
#include "misc/noise.hxx"

const char *modes[3] = {">White", ">Pink", ">Brown"};
PinkNoise<> pinkL;
PinkNoise<> pinkR;
BrownNoise brownL;
BrownNoise brownR;
float gain = 1.f;
int32_t mode = 0;
float stereo = 0.f;

void engine::setup()
{
    engine::addParam("Level", &gain);
    engine::addParam("@Mode", &mode, 0, LEN_OF(modes) - 1, modes);
    engine::addParam("Stereo", &stereo);

    brownL.init();
    brownR.init();
}
void engine::process()
{
    auto bufferL = engine::outputBuffer<0>();
    auto bufferR = engine::outputBuffer<1>();

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        switch (mode)
        {
        case 0:
            bufferL[i] = pinkL.white.nextf(-1.f, 1.f) * gain;
            bufferR[i] = pinkR.white.nextf(-1.f, 1.f) * gain;
            break;
        case 1:
            bufferL[i] = pinkL.nextf(-1.f, 1.f) * gain;
            bufferR[i] = pinkR.nextf(-1.f, 1.f) * gain;
            break;
        case 2:
            bufferL[i] = brownL.nextf(-1.f, 1.f) * gain;
            bufferR[i] = brownR.nextf(-1.f, 1.f) * gain;
            break;
        }

        float tmp = (bufferL[i] + bufferR[i]) / 2.f * (1 - stereo);

        bufferL[i] = bufferL[i] * stereo + tmp;
        bufferR[i] = bufferR[i] * stereo + tmp;
    }
}