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

#include "../squares-and-circles-api.h"
#define FLASHMEM __attribute__((section(".text")))

// #include "../lib/plaits/dsp/engine/virtual_analog_engine.h"
// no virtual functions hack 

#define PLAITS_DSP_ENGINE_VIRTUAL_ANALOG_ENGINE_H_

#include "plaits/dsp/engine/engine.h"
#include "plaits/dsp/oscillator/variable_saw_oscillator.h"
#include "plaits/dsp/oscillator/variable_shape_oscillator.h"

#define VA_VARIANT 2

namespace plaits
{
    class VirtualAnalogEngine
    {
    public:
        VirtualAnalogEngine() {}
        ~VirtualAnalogEngine() {}

        void Init(stmlib::BufferAllocator *allocator);
        void Reset();
        void LoadUserData(const uint8_t *user_data) {}
        void Render(const EngineParameters &parameters,
                    float *out,
                    float *aux,
                    size_t size,
                    bool *already_enveloped);

    private:
        float ComputeDetuning(float detune) const;

        VariableShapeOscillator primary_;
        VariableShapeOscillator auxiliary_;

        VariableShapeOscillator sync_;
        VariableSawOscillator variable_saw_;

        float auxiliary_amount_;
        float xmod_amount_;
        float *temp_buffer_;

        DISALLOW_COPY_AND_ASSIGN(VirtualAnalogEngine);
    };

} // namespace plaits

#include "../lib/plaits/dsp/envelope.h"
#include "../lib/stmlib/algorithms/voice_allocator.h"
#include "../lib/stmlib/utils/random.h"

#include "stmlib/dsp/units.cc"
#include "stmlib/utils/random.cc"
#include "../lib/plaits/dsp/engine/virtual_analog_engine.cc"

plaits::VirtualAnalogEngine voice[6] = {};

uint8_t buffer[LEN_OF(voice) * plaits::kMaxBlockSize * sizeof(float)] = {};
plaits::EngineParameters parameters[LEN_OF(voice)] = {};
plaits::LPGEnvelope lpg[LEN_OF(voice)] = {};
bool enveloped[LEN_OF(voice)] = {};

stmlib::VoiceAllocator<LEN_OF(voice)> allocator;

float timbre = 0.5f;
float morph = 0.5f;
float harmonics = 0.5f;
float pitch = 0;
float pitch_bend = 0;

float decay = 0.5f;
float hf = 1.f;
float pan[LEN_OF(voice)] = {};
float stereo = 0.5f;

float voiceBuff[FRAME_BUFFER_SIZE] = {};
float dummy[FRAME_BUFFER_SIZE] = {};

void engine::setup()
{
    allocator.Init();
    allocator.set_size(LEN_OF(voice));
    engine::addParam(V_OCT, &pitch);
    engine::addParam("Harmo", &harmonics);
    engine::addParam("Timbre", &timbre);
    engine::addParam("Morph", &morph);
    engine::addParam("Decay", &decay);
    engine::addParam("Stereo", &stereo);
    engine::setMode(ENGINE_MODE_MIDI_IN);
    stmlib::BufferAllocator buffAllocator;
    buffAllocator.Init(buffer, sizeof(buffer));

    for (size_t i = 0; i < LEN_OF(voice); i++)
    {
        lpg[i].Init();
        voice[i].Init(&buffAllocator);
    }
}

void engine::process()
{
    auto polyBuffL = engine::outputBuffer<0>();
    auto polyBuffR = engine::outputBuffer<1>();

    std::fill_n(polyBuffL, FRAME_BUFFER_SIZE, 0);
    std::fill_n(polyBuffR, FRAME_BUFFER_SIZE, 0);

    const float short_decay = (200.0f * FRAME_BUFFER_SIZE) / SAMPLE_RATE *
                              stmlib::SemitonesToRatio(-96.0f * decay);

    const float decay_tail = (20.0f * FRAME_BUFFER_SIZE) / SAMPLE_RATE *
                                 stmlib::SemitonesToRatio(-72.0f * decay + 12.0f * hf) -
                             short_decay;

    for (size_t i = 0; i < LEN_OF(voice); i++)
    {
        auto p = parameters[i];
        p.note += (pitch * 12.f) + pitch_bend;
        p.timbre = timbre;
        p.morph = morph;
        p.harmonics = harmonics;

        voice[i].Render(p, voiceBuff, dummy, FRAME_BUFFER_SIZE, &enveloped[i]);

        lpg[i].ProcessPing(0.5f, short_decay, decay_tail, hf);

        float l = cosf(pan[i] * M_PI / 2);
        float r = sinf(pan[i] * M_PI / 2);

        for (int s = 0; s < FRAME_BUFFER_SIZE; s++)
        {
            polyBuffL[s] += (voiceBuff[s] * lpg[i].gain()) * l;
            polyBuffR[s] += (voiceBuff[s] * lpg[i].gain()) * r;
        }

        parameters[i].trigger = plaits::TriggerState::TRIGGER_LOW;
    }
}

void engine::draw()
{
}

void engine::onMidiNote(uint8_t key, uint8_t velocity) // NoteOff: velocity == 0
{
    if (velocity > 0)
    {
        auto ni = allocator.NoteOn(key);
        parameters[ni].trigger = plaits::TriggerState::TRIGGER_RISING_EDGE;
        parameters[ni].note = key;
        parameters[ni].accent = velocity > 100;

        pan[ni] = 0.5f + stereo * (stmlib::Random::GetFloat() - 0.5f);

        lpg[ni].Trigger();
    }
    else
    {
        allocator.NoteOff(key);
    }
}

void engine::onMidiPitchbend(int16_t pitch)
{
    pitch_bend = ((float)pitch / 8192) * 12;
}

void engine::onMidiCC(uint8_t ccc, uint8_t value)
{
    // nothing implemented..
}

void engine::onMidiSysex(uint8_t byte)
{
}