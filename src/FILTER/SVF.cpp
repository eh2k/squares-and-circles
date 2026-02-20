// Copyright (C)2025 - E.Heidt
//
// Author: eh2k@gmx.de
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

// https://www.earlevel.com/main/2003/03/02/the-digital-state-variable-filter/

#include "../squares-and-circles-api.h"
#include <array>
#include <cmath>
#include <algorithm>
#include "stmlib/dsp/units.h"

inline float clamp(float x, float a, float b)
{
    return fmax(a, fmin(x, b));
}

// https://www.musicdsp.org/en/latest/Filters/142-state-variable-filter-chamberlin-version.html
struct SVFilter
{
    float lastLp = 0, lastHp = 0, lastBp = 0;
    inline void process(float input, float kf, float kq)
    {
        auto lp = lastLp + kf * lastBp;
        auto hp = input - lp - kq * lastBp;
        auto bp = lastBp + kf * hp;
        lastLp = lp;
        lastHp = hp;
        lastBp = bp;
    }
};

class Filter
{
    SVFilter f[2];
    float kf = 0.1f, kq = .3f;

  public:
    void set_f_q(float f, float q)
    {
        kq = q;
        kf = stmlib::Pow2Fast<1>(std::min(0.f, f));
        // kf = std::pow(2.f, std::min(0.f, f));
    }

    inline auto process(float input)
    {
        // 2x oversampling

        f[0].process(input, kf, kq);
        f[1].process(f[0].lastLp, kf, kq);

        f[0].process(input, kf, kq);
        f[1].process(f[0].lastLp, kf, kq);

        return f[1].lastLp;
    }
};

float venv = 0, fenv = 0;
float att = 0, aenv = 0;

float _cutoff = 0.28f;
float _resonance = 0.6f;
float _envmod = 0.3f;
float _decay = 0.f;
float _overdrive = 0.f;

Filter flt[2] = {};

float *audio_out[2] = {nullptr, nullptr};
float *audio_in[2] = {nullptr, nullptr};

void engine::setup()
{
    audio_out[0] = engine::outputBuffer_f32(OUT_AUDIO_L);
    audio_out[1] = engine::outputBuffer_f32(OUT_AUDIO_R);

    engine::addParam("Cutoff", &_cutoff);
    engine::addParam("Reso", &_resonance);
    engine::addParam("EnvMod", &_envmod);
    engine::addParam("Decay", &_decay);
    engine::addParam("Overdrive", &_overdrive);
}

void engine::process()
{
    audio_in[0] = engine::inputBuffer<0>();
    audio_in[1] = engine::inputBuffer<1>();

    if (engine::trig())
    {
        fenv = 1;
        att = 0;
        aenv = engine::accent() ? 1.f : 0.f;
    }

    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        att = std::min(att + 2000.f / SAMPLE_RATE, 1.f);

        if (engine::gate() || !engine::hasTrig())
            venv = 1;
        
        venv *= .995f;
        fenv *= std::exp(-(1.f - _decay) * .004f);

        for (int ch = 0; ch < 2; ++ch)
        {
            float v = audio_in[ch][i];

            flt[ch].set_f_q(_cutoff * 5.f - 5.f + fenv * _envmod * 5.f + aenv * .4f,
                            1.f - _resonance * .8f - aenv * .1f);

            v = flt[ch].process(v);
            v *= venv * att * (aenv + 1.f) * .5f;

            if (_overdrive > 0)
                v = tanhf(v * (1 + _overdrive * 8.f));

            audio_out[ch][i] = v;
        }

        aenv *= .999f;
    }
}