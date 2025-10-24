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
#include "clouds/dsp/fx/reverb.h"

float raw = 1.f;
float reverb_amount = 0.75f;
float feedback = 0.5f;
float gain = 1.f;

uint16_t buffer[16384] = {};
clouds::Reverb fx_;

void engine::setup()
{
    fx_.Init(buffer);

    engine::addParam("D/W", &raw);
    engine::addParam("Reverb", &reverb_amount);
    engine::addParam("Damp", &feedback);
    engine::addParam("Gain", &gain);
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto inputR = engine::inputBuffer<1>();
    auto outputL = engine::outputBuffer<0>();
    auto outputR = engine::outputBuffer<1>();

    fx_.set_amount(reverb_amount * 0.54f);
    fx_.set_diffusion(0.7f);
    fx_.set_time(0.35f + 0.63f * reverb_amount);
    fx_.set_input_gain(gain * 0.1f); // 0.1f);
    fx_.set_lp(0.6f + 0.37f * feedback);

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        outputL[i] = inputL[i];
        outputR[i] = inputR[i];
    }

    fx_.Process(outputL, outputR, FRAME_BUFFER_SIZE);

    for (int i = 0; i < FRAME_BUFFER_SIZE; ++i)
    {
        outputL[i] = raw * outputL[i] + (1 - raw) * inputL[i];
        outputR[i] = raw * outputR[i] + (1 - raw) * inputR[i];
    }
}
