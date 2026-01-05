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

// build_flags: -fno-inline -mfloat-abi=hard -mfpu=fpv5-d16 -ffast-math

#include "../squares-and-circles-api.h"
#define private public
#include "lib/peaks/drums/high_hat.h"
#include "lib/braids/envelope.h"

peaks::HighHat _oh;
peaks::HighHat _ch;
braids::Envelope _ohEnv;
braids::Envelope _chEnv;

float _pitch = 0.5f;
float _levelCH = 0.75f;
float _decayCH = 0;
float _decayOH = 0.5f;

int16_t buffer[FRAME_BUFFER_SIZE];
peaks::GateFlags flags[FRAME_BUFFER_SIZE];

void engine::setup()
{
    _oh.Init();
    _ch.Init();

    uint16_t decay = UINT16_MAX;
    _oh.Configure(&decay, peaks::CONTROL_MODE_FULL);
    _ch.Configure(&decay, peaks::CONTROL_MODE_FULL);

    _ohEnv.Init(); //TODO: Use _ch.vca_envelope_ instead -  only 4 steps ???!
    _chEnv.Init();

    std::fill(&flags[0], &flags[FRAME_BUFFER_SIZE], peaks::GATE_FLAG_LOW);

    engine::addParam(".", &_pitch);
    engine::addParam("CH-Lev", &_levelCH);
    engine::addParam("CH-Dec", &_decayCH);
    engine::addParam("OH-Dec", &_decayOH);
}

void engine::process()
{
    bool oh_mute = false;

    if (engine::trig() && !engine::accent())
    {
        _chEnv.Trigger(braids::ENV_SEGMENT_ATTACK);
        _ch.vca_envelope_.Trigger(_levelCH * 32768 * 15);
        oh_mute = true;
    }

    _ch.Process(flags, buffer, FRAME_BUFFER_SIZE);

    _chEnv.Update(0, 32 + _decayCH * 95);
    auto ch_ad = (float)_chEnv.Render() / UINT16_MAX;

    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        engine::outputBuffer<0>()[i] = (float)buffer[i] / INT16_MAX * ch_ad;

    if (engine::accent())
    {
        _ohEnv.Trigger(braids::ENV_SEGMENT_ATTACK);
        _oh.vca_envelope_.Trigger(32768 * 15);
    }

    _oh.Process(flags, buffer, FRAME_BUFFER_SIZE);

    _ohEnv.Update(0, oh_mute ? 0 : (32 + _decayOH * 95));
    auto oh_ad = (float)_ohEnv.Render() / UINT16_MAX;

    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        engine::outputBuffer<0>()[i] += (float)buffer[i] / INT16_MAX * oh_ad;
}

#include "lib/peaks/drums/high_hat.cc"
#include "lib/peaks/resources.cc"
#include "lib/braids/resources.cc"