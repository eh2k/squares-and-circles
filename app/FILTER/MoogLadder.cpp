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

#define tanh fast_tanh

#include "MoogLadders/src/ImprovedModel.h"
#include "plaits/dsp/envelope.h"
#include "stmlib/dsp/units.cc"
#include <algorithm>

using namespace stmlib;
using namespace plaits;

const float a0 = (440.0f / 8.0f) / SAMPLE_RATE;

inline float NoteToFrequency(float midi_note)
{
    midi_note -= 9.0f;
    CONSTRAIN(midi_note, -128.0f, 127.0f);
    return a0 * 0.25f * SemitonesToRatio(midi_note);
}

float _cutoff = 0.5f;
float _resonance = 0.5f;
float _envMod = 0.5f;
float _envDecay = 0.5f;

DecayEnvelope decay_envelope_;
ImprovedMoog filter0(SAMPLE_RATE*2);

void engine::setup()
{
    decay_envelope_.Init();

    engine::addParam("Cutoff", &_cutoff);
    engine::addParam("Res.", &_resonance);
    engine::addParam("EnvMod", &_envMod);
    engine::addParam("Decay", &_envDecay);
}

float tmp[FRAME_BUFFER_SIZE * 2 + 2] = {};
float last_input = 0.f;

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

    filter0.SetCutoff(cutoff * SAMPLE_RATE / 2);
    filter0.SetResonance(_resonance);

#if 0
    std::copy_n(inputL, FRAME_BUFFER_SIZE, outputL);
    filter0.Process(outputL, FRAME_BUFFER_SIZE);
#else

    for (int i = 0; i < (FRAME_BUFFER_SIZE * 2); i += 2)
    {
        float input = inputL[i / 2] + inputR[i / 2];
        tmp[i] = (last_input + input) * 0.5f;
        tmp[i + 1] = input;
        last_input = input;
    }

    filter0.Process(tmp, FRAME_BUFFER_SIZE * 2);

    for (int i = 0; i < FRAME_BUFFER_SIZE * 2; i += 2)
    {
        outputL[i / 2] = (tmp[i] + tmp[i + 1]) / 2;
    }

#endif

    std::copy_n(outputL, FRAME_BUFFER_SIZE, outputR);
}
