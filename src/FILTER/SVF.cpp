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


// Adapted from: plaits virtual_analog_vcf_engine.h / virtual_analog_vcf_engine.cc

#include "../squares-and-circles-api.h"
#include "plaits/dsp/envelope.h"
#include "stmlib/dsp/dsp.h"
#include "stmlib/dsp/filter.h"
#include "stmlib/dsp/units.h"

using namespace stmlib;

const float a0 = (440.0f / 8.0f) / SAMPLE_RATE;

inline float NoteToFrequency(float midi_note)
{
    midi_note -= 9.0f;
    CONSTRAIN(midi_note, -128.0f, 127.0f);
    return a0 * 0.25f * stmlib::SemitonesToRatio(midi_note);
}

float _cutoff = 0.5f;
float _q = 0.5f;
float _envMod = 0.5f;
float _envDecay = 0.5f;

stmlib::Svf svfL[2];
stmlib::Svf svfR[2];
plaits::DecayEnvelope decay_envelope_;

void engine::setup()
{
    svfL[0].Init();
    svfL[1].Init();
    svfR[0].Init();
    svfR[1].Init();

    decay_envelope_.Init();

    engine::addParam("Cutoff", &_cutoff);
    engine::addParam("Q", &_q);
    engine::addParam("EnvMod", &_envMod);
    engine::addParam("Decay", &_envDecay);
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto inputR = engine::inputBuffer<1>();
    auto outputL = engine::outputBuffer<0>();
    auto outputR = engine::outputBuffer<1>();

    if (engine::trig())
        decay_envelope_.Trigger();

    const float short_decay = (200.0f * FRAME_BUFFER_SIZE) / SAMPLE_RATE * SemitonesToRatio(-96.0f * _envDecay);

    decay_envelope_.Process(short_decay);
    float cutoff = _cutoff + _envMod * decay_envelope_.value();
    CONSTRAIN(cutoff, 0.f, 1.f);

    const float f0 = NoteToFrequency(DEFAULT_NOTE + (engine::cv() * 12));
    cutoff = f0 * SemitonesToRatio((cutoff - 0.2f) * 120.0f);

    const float q = _q * 4;
    const float stage2_gain = 1.f;
    const float sub_gain = 1.0f;
    const float gain = 1.f;
    float input, lp, hp;

    for (size_t i = 0; i < FRAME_BUFFER_SIZE; ++i)
    {
        svfL[0].set_f_q<FREQUENCY_FAST>(cutoff, 0.5f + q);
        svfL[1].set_f_q<FREQUENCY_FAST>(cutoff, 0.5f + 0.025f * q);

        input = SoftClip((inputL[i] * sub_gain) * gain);
        svfL[0].Process<FILTER_MODE_LOW_PASS, FILTER_MODE_HIGH_PASS>(input, &lp, &hp);
        lp = SoftClip(lp * gain);
        lp += stage2_gain * (SoftClip(svfL[1].Process<FILTER_MODE_LOW_PASS>(lp)) - lp);
        outputL[i] = lp;

        svfR[0].set_f_q<FREQUENCY_FAST>(cutoff, 0.5f + q);
        svfR[1].set_f_q<FREQUENCY_FAST>(cutoff, 0.5f + 0.025f * q);

        input = SoftClip((inputR[i] * sub_gain) * gain);
        svfR[0].Process<FILTER_MODE_LOW_PASS, FILTER_MODE_HIGH_PASS>(input, &lp, &hp);
        lp = SoftClip(lp * gain);
        lp += stage2_gain * (SoftClip(svfR[1].Process<FILTER_MODE_LOW_PASS>(lp)) - lp);
        outputR[i] = lp;
    }
}

#include "lib/stmlib/dsp/units.cc"