// Copyright (C)2021 - Eduard Heidt
//
// Author: Eduard Heidt (eh2k@gmx.de)
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

#pragma once

#include "machine.h"
#include "braids/envelope.h"

using namespace machine;

struct HiHatsEngine : public Engine
{
    Engine *_oh = nullptr;
    Engine *_ch = nullptr;

    float _pitch = 0;
    float _ch_vol = 0.75f;

    braids::Envelope _ch_env;
    braids::Envelope _oh_env;
    uint8_t _ch_end;
    uint8_t _oh_end;
    bool _oh_mute = false;
    float bufferOut[machine::FRAME_BUFFER_SIZE];

public:
    HiHatsEngine() : Engine(TRIGGER_INPUT | ACCENT_INPUT | VOCT_INPUT)
    {
        _ch_env.Init();
        _oh_env.Init();

        param[0].init("", &_pitch);
        param[1].init("CH-Lev", &_ch_vol);
        param[2].init("CH-Dec", &_ch_end, 32, 32, 127);
        param[2].step.i = 8;
        param[3].init("OH-Dec", &_oh_end, 80, 32, 127);
        param[3].step.i = 8;
    }

    void process(const ControlFrame &frame_, OutputFrame &of) override
    {
        ControlFrame frame;
        memcpy(&frame, &frame_, sizeof(ControlFrame));

        frame.trigger = frame.trigger && !frame.accent;

        if (frame.trigger)
            _ch_env.Trigger(braids::ENV_SEGMENT_ATTACK);

        _ch_env.Update(0, _ch_end);

        OutputFrame ch_of;
        _ch->process(frame, ch_of);

        _oh_mute = frame.trigger;

        frame.trigger = frame.accent;

        if (frame.trigger)
            _oh_env.Trigger(braids::ENV_SEGMENT_ATTACK);

        _oh_env.Update(0, _oh_mute ? 0 : _oh_end);

        OutputFrame oh_of;
        _oh->process(frame, oh_of);

        auto ch_ad = (float)_ch_env.Render() / UINT16_MAX;
        auto oh_ad = (float)_oh_env.Render() / UINT16_MAX;

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
            bufferOut[i] = (float)((ch_of.out[i] * _ch_vol * ch_ad) + (oh_of.out[i] * oh_ad));

        of.push(bufferOut, LEN_OF(bufferOut));
    }
};