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

#include "../../squares-and-circles-api.h"

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

// lib/MoogLadders/src/ImprovedModel.h (D'Angelo/Valimaki) in float and for two channels: double sounds the same
// (93 dB SNR) and its divisions are twice as slow on the M7. The tanh of L and R share one division.
class ImprovedMoogStereo
{
    static constexpr float VT = 0.312f;        // thermal voltage
    static constexpr float k = 1.0f / (2 * VT); // tanh(x / (2 VT))

    float V[2][4] = {};
    float dV[2][4] = {};
    float tV[2][4] = {};
    float g = 0;
    float resonance = 0;
    float sampleRate;
    float h; // 1 / (2 sampleRate), trapezoidal integration

    // the rational tanh of MoogLadders' Util.h, x (27 + x^2) / (27 + 9 x^2), for a and b with one division
    static inline void tanh2(float a, float b, float &ta, float &tb)
    {
        float a2 = a * a, b2 = b * b;
        float A = 27.0f + 9.0f * a2, B = 27.0f + 9.0f * b2;
        float r = 1.0f / (A * B);
        ta = a * (27.0f + a2) * B * r;
        tb = b * (27.0f + b2) * A * r;
    }

public:
    ImprovedMoogStereo(float fs) : sampleRate(fs), h(1.0f / (2 * fs)) {}

    void SetResonance(float r)
    {
        CONSTRAIN(r, 0.001f, 1.0f);
        resonance = r * 6.844399855f;
    }

    void SetCutoff(float c)
    {
        if (c > 20000.0f)
            c = 20000.0f;
        float x = ((float)M_PI * c) / sampleRate;
        g = 4.0f * (float)M_PI * VT * c * (1.0f - x) / (1.0f + x);
    }

    // one sample of both channels
    inline void Process(float inL, float inR, float &outL, float &outR)
    {
        float *vl = V[0], *dl = dV[0], *tl = tV[0];
        float *vr = V[1], *dr = dV[1], *tr = tV[1];

        float a, b;
        tanh2((inL + resonance * vl[3]) * k, (inR + resonance * vr[3]) * k, a, b);
        float d0l = -g * (a + tl[0]), d0r = -g * (b + tr[0]);
        vl[0] += (d0l + dl[0]) * h;
        vr[0] += (d0r + dr[0]) * h;
        dl[0] = d0l;
        dr[0] = d0r;
        tanh2(vl[0] * k, vr[0] * k, tl[0], tr[0]);

        for (int s = 1; s < 4; s++)
        {
            float dsl = g * (tl[s - 1] - tl[s]), dsr = g * (tr[s - 1] - tr[s]);
            vl[s] += (dsl + dl[s]) * h;
            vr[s] += (dsr + dr[s]) * h;
            dl[s] = dsl;
            dr[s] = dsr;
            tanh2(vl[s] * k, vr[s] * k, tl[s], tr[s]);
        }

        outL = vl[3];
        outR = vr[3];
    }
};

float _cutoff = 0.5f;
float _resonance = 0.5f;
float _envMod = 0.5f;
float _envDecay = 0.5f;

DecayEnvelope decay_envelope_;
ImprovedMoogStereo filter0(SAMPLE_RATE * 2);

void engine::setup()
{
    decay_envelope_.Init();

    engine::addParam("Cutoff", &_cutoff);
    engine::addParam("Res.", &_resonance);
    engine::addParam("EnvMod", &_envMod);
    engine::addParam("Decay", &_envDecay);
}

// The mono version filtered L + R. A mono source arrives on both inputs (machine.cpp prepare_aux), so twice the
// input keeps its drive into the tanh and its sound.
constexpr float input_gain = 2.0f;
float last_input[2] = {};

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

    // 2x oversampled: linear interpolation up, the average of the two samples down
    float lastL = last_input[0], lastR = last_input[1];
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        float inL = inputL[i] * input_gain, inR = inputR[i] * input_gain;
        float aL, aR, bL, bR;
        filter0.Process((lastL + inL) * 0.5f, (lastR + inR) * 0.5f, aL, aR);
        filter0.Process(inL, inR, bL, bR);
        outputL[i] = (aL + bL) * 0.5f;
        outputR[i] = (aR + bR) * 0.5f;
        lastL = inL;
        lastR = inR;
    }
    last_input[0] = lastL;
    last_input[1] = lastR;
}
