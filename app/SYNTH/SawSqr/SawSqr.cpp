// Copyright (C)2021 - E.Heidt
//
// Author: E.Heidt (eh2k@gmx.de)
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

ENGINE_NAME("M-OSC/SawSqr");

#include <array>
#include <cmath>
#include <algorithm>
#include "plaits/dsp/oscillator/oscillator.h"
#include "stmlib/dsp/units.h"
#include "plaits/dsp/envelope.h"

float clamp(float x, float a, float b)
{
    return std::max(a, std::min(x, b));
}

float lerp(float a, float b, float x)
{
    return a + (b - a) * x;
}

struct SawSqrOsc
{
    plaits::Oscillator osc;
    plaits::Oscillator osc2;
    plaits::DecayEnvelope decay_envelope_;

    float pitch = 0;
    float waveform = 0.f;
    float envDecay = 1.f;

    SawSqrOsc()
    {
        osc.Init();
        osc2.Init();
        decay_envelope_.Init();
    }

    inline float noteToFrequency(float midi_note)
    {
        constexpr float a0 = (440.0f / 8.0f) / SAMPLE_RATE;
        midi_note -= 9.0f;
        CONSTRAIN(midi_note, -128.0f, 127.0f);
        return a0 * 0.25f * stmlib::SemitonesToRatio(midi_note);
    }

    inline float process(float note)
    {
        float saw;
        float sq;

        float f = noteToFrequency(note);
        this->osc.Render<plaits::OSCILLATOR_SHAPE_SAW>(f, .5f, &saw, 1);
        this->osc2.Render<plaits::OSCILLATOR_SHAPE_SQUARE>(f, .5f, &sq, 1);

        float out = lerp(saw, sq, clamp(this->waveform, 0.0f, 1.0f));

        if (engine::hasTrig())
        {
            const float short_decay =
                (10.0f * FRAME_BUFFER_SIZE) / SAMPLE_RATE * stmlib::SemitonesToRatio(-96.0f * envDecay);
            decay_envelope_.Process(short_decay);

            return out * decay_envelope_.value();
        }
        else
        {
            return out * envDecay;
        }
    }
};

SawSqrOsc _voice;

float *audio_out = nullptr;

void engine::setup()
{
    audio_out = engine::outputBuffer_f32(OUT_AUDIO_L);

    engine::addParam(V_OCT, &_voice.pitch);
    engine::addParam("SAW/SQR", &_voice.waveform);
    engine::addParam("Decay", &_voice.envDecay);
}

void engine::process()
{
    if (engine::trig())
        _voice.decay_envelope_.Trigger();

    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        audio_out[i] = _voice.process((DEFAULT_NOTE) + engine::cv() * 12);
}

void engine::draw()
{
    engine::setParamName(&_voice.envDecay, engine::hasTrig() ? "Decay" : "Level");

    float last_v = 0.f;
    for (int i = 0; i < 64; i++)
    {
        float saw = i/64.f * 2.f - 1.f;
        float sq = ((i > 32) ? 1.f : -1.f);
        float v = lerp(saw, sq, clamp(_voice.waveform, 0.0f, 1.0f));
        if(i % 2 == 0)
            gfx::setPixel(64+i, 48);
        gfx::drawLine(64+i, 48 + last_v * 10, 64+i+1, 48 + v * 10);
        last_v = v;
    }
}

#include "lib/plaits/resources.cc"
#include "lib/stmlib/dsp/units.cc"