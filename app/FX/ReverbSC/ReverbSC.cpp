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

#include "../../squares-and-circles-api.h"

#include "soundpipe/revsc.c"

static sp_data sp_data_ = {};
static sp_revsc sp_revsc_ = {};
static uint8_t mem[107440]; // sp_revsc_.aux.size

static float raw = 1.f;

// The tail adds up: with a steady input it reaches ~1/(1 - feedback^2) of its energy, +12 dB at soundpipe's default
// 0.97, and peaks over 3. Csound's manual: 0.6 is a small room, 0.8 a small hall, 0.9 a large hall. What still goes
// over (long feedback, dense input) runs into a soft knee above 0.8 instead of the DAC's hard clipping.
static inline float soft_knee(float x)
{
    constexpr float knee = 0.8f;
    const float a = fabsf(x);
    if (a <= knee)
        return x;
    const float y = knee + (1.f - knee) * tanhf((a - knee) / (1.f - knee));
    return x < 0 ? -y : y;
}

void engine::setup()
{
    sp_data_.sr = SAMPLE_RATE;
    sp_data_.aux.ptr = &mem[0];
    sp_data_.aux.size = sizeof(mem);
    sp_revsc_init(&sp_data_, &sp_revsc_);

    sp_revsc_.feedback = 0.85f;
    sp_revsc_.lpfreq = 10000;

    engine::addParam("D/W", &raw);
    engine::addParam("Feedback", &sp_revsc_.feedback);
    engine::addParam("LpFreq", &sp_revsc_.lpfreq, 0, (SAMPLE_RATE / 2));
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto inputR = engine::inputBuffer<1>();
    auto outputL = engine::outputBuffer<0>();
    auto outputR = engine::outputBuffer<1>();

    for (int i = 0; i < FRAME_BUFFER_SIZE; ++i)
    {
        sp_revsc_compute(&sp_data_, &sp_revsc_, &inputL[i], &inputR[i], &outputL[i], &outputR[i]);
        outputL[i] = soft_knee(raw * outputL[i] + (1 - raw) * inputL[i]);
        outputR[i] = soft_knee(raw * outputR[i] + (1 - raw) * inputR[i]);
    }
}