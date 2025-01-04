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

// xuild_flags: -fno-inline -mfloat-abi=soft -mfpu=fpv5-d16

#include "../squares-and-circles-api.h"

#include "stmlib/stmlib.h"
#include "stmlib/dsp/dsp.h"

#define private public
#include "braids/macro_oscillator.h"
#include "braids/envelope.h"
#include "braids/settings.h"

#include "braids/vco_jitter_source.h"
#include "braids/quantizer.h"
#include <algorithm>

#define SemitonesToRatioFast
#include "lib/plaits/dsp/chords/chord_bank.cc"

plaits::ChordBank chords_;
int32_t chords_mem[(plaits::kChordNumChords * plaits::kChordNumNotes) + plaits::kChordNumChords + plaits::kChordNumNotes];

braids::MacroOscillator osc1;
braids::MacroOscillator osc2;
braids::Envelope envelope;
braids::VcoJitterSource jitter_source;
braids::Quantizer quantizer;

constexpr int32_t DEFAULT_PITCH = PITCH_PER_OCTAVE * 6;

namespace braids
{
    uint8_t ex_chord[7] = {};
    extern const uint8_t diatonic_chords[8][6];
    extern const uint8_t *custom_chord;

    void Quantizer::Init()
    {
    }

    bool Quantizer::enabled()
    {
        return engine::qz_enabled();
    }

    int32_t Quantizer::Process(int32_t pitch, int32_t root, int8_t *note)
    {
         pitch -= root + DEFAULT_PITCH;
        auto ret = engine::qz_process(pitch, note);
        ret += root + DEFAULT_PITCH;
        return ret;
    }

    int16_t Quantizer::Lookup(uint8_t index)
    {
        return engine::qz_lookup(index) + DEFAULT_PITCH;
    }
}
uint8_t sync_samples[FRAME_BUFFER_SIZE] = {};

int32_t _pitch = 0;
int32_t _shape = 0;
int32_t _timbre = UINT16_MAX / 2;
int32_t _color = UINT16_MAX / 2;
int32_t _attack = 0;
int32_t _decay = UINT16_MAX / 2;

int32_t last_color = -1;
int32_t last_shape = -1;
int32_t last_qz = -1;

void engine::setup()
{
    stmlib::BufferAllocator allocator;
    allocator.Init(chords_mem, sizeof(chords_mem));
    chords_.Init(&allocator);
    chords_.chord_index_quantizer_.hysteresis_ = 0; // input => output
    chords_.Reset();

    braids::settings.Init();
    osc1.Init();
    osc2.Init();
    jitter_source.Init();
    envelope.Init();
    quantizer.Init();

    // std::fill(&sync_samples[0], &sync_samples[FRAME_BUFFER_SIZE], 0);

    // settings.SetValue(SETTING_AD_VCA, true);
    braids::settings.SetValue(braids::SETTING_SAMPLE_RATE, 5);
    braids::settings.SetValue(braids::SETTING_PITCH_RANGE, braids::PITCH_RANGE_EXTERNAL);

    engine::addParam(V_OCT, &_pitch, -4 * PITCH_PER_OCTAVE, 4 * PITCH_PER_OCTAVE); // is added internal to engine::cv
    engine::addParam("Shape", &_shape, braids::MACRO_OSC_SHAPE_CSAW, braids::MACRO_OSC_SHAPE_LAST - 1, (const char **)braids::settings.metadata(braids::Setting::SETTING_OSCILLATOR_SHAPE).strings);
    engine::addParam("Timbre", &_timbre, 0, UINT16_MAX);
    engine::addParam("Color", &_color, 0, UINT16_MAX);
    engine::addParam("Decay", &_decay, 0, UINT16_MAX);
    engine::addParam("Attack", &_attack, 0, UINT16_MAX);
    engine::setMode(ENGINE_MODE_STEREOLIZED);
}

char color_vals[64];

void engine::process()
{
    if (last_shape != _shape || last_qz != __io->qz)
    {
        last_qz = __io->qz;
        last_shape = _shape;
        engine::draw();
    }

    envelope.Update(_attack / 512, _decay / 512);

    if (engine::trig())
    {
        osc1.Strike();
        osc2.Strike();
        envelope.Trigger(braids::ENV_SEGMENT_ATTACK);
    }

    if (engine::gate())
    {
        // Not working witch Attack > 0
        // envelope.Trigger(braids::ENV_SEGMENT_DECAY);
    }

    uint32_t ad_value = envelope.Render();

    int32_t pitch = engine::cv_i32();
    pitch += DEFAULT_PITCH;

    // if (!settings.meta_modulation())
    // {
    //     pitch += settings.adc_to_fm(adc_3);
    // }

    pitch += jitter_source.Render(braids::settings.vco_drift());
    pitch += ad_value * braids::settings.GetValue(braids::SETTING_AD_FM) >> 7;

    CONSTRAIN(pitch, 0, 16383);

    if (braids::settings.vco_flatten())
        pitch = stmlib::Interpolate88(braids::lut_vco_detune, pitch << 2);

    uint32_t gain = _decay < UINT16_MAX ? ad_value : UINT16_MAX;

    if (!__io->tr)
        gain = _decay; // No Trigger patched - use Decay as VCA...

    if ((engine::t() % 12) != 0)
        osc1.set_shape((braids::MacroOscillatorShape)_shape);
    else if ((engine::t() % 12) != 6)
        osc2.set_shape((braids::MacroOscillatorShape)_shape);

    osc1.set_parameters(_timbre >> 1, _color >> 1);

    if (_shape >= 48 && _shape < (48 + 5) && __io->qz > 0)
    {
        if (__io->qz == 1)
        {
            if (last_color != _color)
            {
                last_color = _color;
                chords_.set_chord((float)_color / (plaits::kChordNumChords - 1));
                // chords_.Sort();
                memset(braids::ex_chord, 0, sizeof(braids::ex_chord));
                for (int i = 0; i < chords_.num_notes(); i++)
                {
                    braids::ex_chord[i] = log2f(chords_.ratio(i)) * 12;
                }

                braids::custom_chord = braids::ex_chord;
            }
        }
        else
        {
            osc1.set_parameters(_timbre >> 1, _color << 12);
            braids::custom_chord = nullptr;
        }
    }

    osc1.set_pitch(pitch);

    auto audio_samples = engine::outputBuffer_i16<0>();
    osc1.Render(sync_samples, audio_samples, FRAME_BUFFER_SIZE);

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        audio_samples[i] = (gain * audio_samples[i]) / UINT16_MAX;

    if (engine::is_stereo() && __io->stereo > 0) // Stereo
    {
        const int32_t f = __io->stereo;
        int32_t stereo = (f * f * f) / (255 * 255);

        int32_t timbre = _timbre + stereo;
        if (timbre > UINT16_MAX)
            timbre = _timbre - stereo;

        int32_t color = _color + stereo;
        if (color > UINT16_MAX)
            color = _color - stereo;

        osc2.set_parameters(osc1.parameter_[0], osc1.parameter_[1]);
        osc2.set_pitch(pitch + stereo);

        auto audio_samples = engine::outputBuffer_i16<1>();
        osc2.Render(sync_samples, audio_samples, FRAME_BUFFER_SIZE);

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
            audio_samples[i] = (gain * audio_samples[i]) / UINT16_MAX;
    }
    else
    {
        memcpy(engine::outputBuffer_i16<1>(), engine::outputBuffer_i16<0>(), FRAME_BUFFER_SIZE * sizeof(int16_t));
    }
}

void engine::draw()
{
    if (_shape >= 48 && _shape < (48 + 5) && __io->qz > 0)
    {
        if (__io->qz == 1)
        {
            engine::addParam(".", &_color, 0, plaits::kChordNumChords - 1, (const char **)plaits::chord_names);
        }
        else
        {
            char *p = color_vals;
            p += sprintf(p, "Chord-%d\t0", _color + 1);
            for (int i = 1; i < LEN_OF(braids::diatonic_chords[_color]); i++)
            {
                if (braids::diatonic_chords[_color][i] != 0)
                {
                    p += sprintf(p, "+%X", braids::diatonic_chords[_color][i]);
                }
            }

            engine::addParam("Color", &_color, 0, 7);
            engine::setParamName(&_color, color_vals);
        }
    }
    else
    {
        engine::addParam("Color", &_color, 0, UINT16_MAX);
    }

    if (!__io->tr)
    {
        engine::setParamName(&_decay, "Level");
        engine::setParamName(&_attack, nullptr);
    }
    else if (_decay < UINT16_MAX)
    {
        engine::setParamName(&_decay, "Decay");
        engine::setParamName(&_attack, "Attack");
    }
    else
    {
        engine::setParamName(&_decay, "VCA-off");
        engine::setParamName(&_attack, nullptr);
    }
}

#include "braids/analog_oscillator.cc"
#define kHighestNote kHighestNote2
#define kPitchTableStart kPitchTableStart2
#define kOctave kOctave2
#include "braids/digital_oscillator.cc"
#include "braids/settings.cc"
#include "braids/macro_oscillator.cc"
#include "braids/resources.cc"
#include "stmlib/utils/random.cc"

#define chords chords2
#define mini_wave_line mini_wave_line2
#include "braids/chords_stack.cc"
