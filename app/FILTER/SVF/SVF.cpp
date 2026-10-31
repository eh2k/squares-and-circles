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

#include "../../squares-and-circles-api.h"
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

struct Filter
{
    SVFilter f[2];

    // kf from the cutoff in octaves below the top (0)
    static inline float kf_from(float f)
    {
        return stmlib::Pow2Fast<1>(std::min(0.f, f));
        // return std::pow(2.f, std::min(0.f, f));
    }

    inline auto process(float input, float kf, float kq)
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

    // what doesn't change within the frame, and the state in locals: the stores to the outputs could alias the globals
    const float fenv_k = std::exp(-(1.f - _decay) * .004f);
    const bool open = engine::gate() || !engine::hasTrig();
    const float cutoff = _cutoff * 5.f - 5.f;
    const float envmod = _envmod;
    const float reso = 1.f - _resonance * .8f;
    const bool overdrive = _overdrive > 0;
    const float drive = 1 + _overdrive * 8.f;
    float venv_ = venv, fenv_ = fenv, att_ = att, aenv_ = aenv;
    Filter l = flt[0], r = flt[1];
    const float *in_l = audio_in[0], *in_r = audio_in[1];
    float *out_l = audio_out[0], *out_r = audio_out[1];

    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        att_ = std::min(att_ + 2000.f / SAMPLE_RATE, 1.f);

        if (open)
            venv_ = 1;

        venv_ *= .995f;
        fenv_ *= fenv_k;

        // both channels: the same cutoff and resonance, L and R interleaved
        const float kf = Filter::kf_from(cutoff + fenv_ * envmod * 5.f + aenv_ * .4f);
        const float kq = reso - aenv_ * .1f;
        const float vca = venv_ * att_ * (aenv_ + 1.f) * .5f;

        float vl = l.process(in_l[i], kf, kq) * vca;
        float vr = r.process(in_r[i], kf, kq) * vca;

        if (overdrive)
        {
            vl = tanhf(vl * drive);
            vr = tanhf(vr * drive);
        }

        out_l[i] = vl;
        out_r[i] = vr;

        aenv_ *= .999f;
    }

    venv = venv_;
    fenv = fenv_;
    att = att_;
    aenv = aenv_;
    flt[0] = l;
    flt[1] = r;
}
