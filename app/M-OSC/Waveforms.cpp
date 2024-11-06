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
#include <algorithm>

using namespace braids;

MacroOscillator osc1;
MacroOscillator osc2;
Envelope envelope;
VcoJitterSource jitter_source;

uint8_t sync_samples[FRAME_BUFFER_SIZE] = {};

int32_t _pitch = DEFAULT_NOTE;
int32_t _shape = 0;
int32_t _timbre = UINT16_MAX / 2;
int32_t _color = UINT16_MAX / 2;
int32_t _attack = 0;
int32_t _decay = UINT16_MAX / 2;

void engine::setup()
{
    settings.Init();
    osc1.Init();
    osc2.Init();
    jitter_source.Init();
    envelope.Init();

    // std::fill(&sync_samples[0], &sync_samples[FRAME_BUFFER_SIZE], 0);

    // settings.SetValue(SETTING_AD_VCA, true);
    settings.SetValue(SETTING_SAMPLE_RATE, 5);
    settings.SetValue(SETTING_PITCH_OCTAVE, 4);
    settings.SetValue(SETTING_PITCH_RANGE, PITCH_RANGE_EXTERNAL);

    engine::addParam(V_OCT, &_pitch, 0, 255);
    engine::addParam("Shape", &_shape, braids::MACRO_OSC_SHAPE_CSAW, braids::MACRO_OSC_SHAPE_LAST - 1, (const char **)braids::settings.metadata(braids::Setting::SETTING_OSCILLATOR_SHAPE).strings);
    engine::addParam("Timbre", &_timbre, 0, UINT16_MAX);
    engine::addParam("Color", &_color, 0, UINT16_MAX);
    engine::addParam("Decay", &_decay, 0, UINT16_MAX);
    engine::addParam("Attack", &_attack, 0, UINT16_MAX);
    engine::setMode(ENGINE_MODE_STEREOLIZED);
    
}

void engine::process()
{
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

    int32_t pitchV = engine::cv_i32();
    int32_t pitch = pitchV + (DEFAULT_NOTE + 12) * 128;

    // if (!settings.meta_modulation())
    // {
    //     pitch += settings.adc_to_fm(adc_3);
    // }

    pitch += jitter_source.Render(settings.vco_drift());
    pitch += ad_value * settings.GetValue(SETTING_AD_FM) >> 7;

    CONSTRAIN(pitch, 0, 16383);

    if (settings.vco_flatten())
        pitch = stmlib::Interpolate88(braids::lut_vco_detune, pitch << 2);

    uint32_t gain = _decay < UINT16_MAX ? ad_value : UINT16_MAX;

    if (!__io->tr)
        gain = _decay; // No Trigger patched - use Decay as VCA...

    if ((engine::t() % 12) != 0)
        osc1.set_shape((braids::MacroOscillatorShape)_shape);
    else if ((engine::t() % 12) != 6)
        osc2.set_shape((braids::MacroOscillatorShape)_shape);

    osc1.set_parameters(_timbre >> 1, _color >> 1);
    osc1.set_pitch(pitch + settings.pitch_transposition());

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

        osc2.set_parameters((timbre >> 1), (color >> 1));
        osc2.set_pitch(pitch + settings.pitch_transposition() + stereo);

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
    if (!__io->tr)
    {
        setParamName(&_decay, "Level");
        setParamName(&_attack, nullptr);
    }
    else if (_decay < UINT16_MAX)
    {
        setParamName(&_decay, "Decay");
        setParamName(&_attack, "Attack");
    }
    else
    {
        setParamName(&_decay, "VCA-off");
        setParamName(&_attack, nullptr);
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