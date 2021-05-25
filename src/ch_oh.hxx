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

struct CHOH : public Engine
{
    Engine *_oh;
    Engine *_ch;

    float _ch_vol = 0.75f;

    braids::Envelope _ch_env;
    braids::Envelope _oh_env;
    float _ch_end = 0.5f;
    float _oh_end = 0.75f;
    bool _oh_mute = false;

public:
    CHOH() : _oh(nullptr), _ch(nullptr)
    {
        _ch_env.Init();
        _oh_env.Init();
    }

    virtual bool IsCvTrigger() { return true; }

    void Process(const ControlFrame &frame_, float** out, float** aux) override
    {
        ControlFrame frame;
        memcpy(&frame, &frame_, sizeof(ControlFrame));

        auto trig = frame.trigger;
        frame.trigger = trig && frame.cv_voltage < 0.5f;

        if (frame.trigger)
            _ch_env.Trigger(braids::ENV_SEGMENT_ATTACK);

        int32_t cd = (int32_t)(_ch_end * UINT16_MAX) >> 10;
        _ch_env.Update(0, cd);
        float ch_ad = (float)_ch_env.Render() / UINT16_MAX;

        float* ch_buffer = nullptr;
        _ch->Process(frame, &ch_buffer, &ch_buffer);

        _oh_mute = frame.trigger;

        frame.trigger = frame.cv_voltage > 0.5;

        if (frame.trigger)
            _oh_env.Trigger(braids::ENV_SEGMENT_ATTACK);

        int32_t od = (int32_t)(_oh_end * UINT16_MAX) >> 9;
        _oh_env.Update(0, _oh_mute ? 0 : od);

        float oh_ad = (float)_oh_env.Render() / UINT16_MAX;

        float* oh_buffer = nullptr;
        _oh->Process(frame, &oh_buffer, &oh_buffer);

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
            oh_buffer[i] = 0.9f * ch_buffer[i] * _ch_vol * ch_ad + oh_buffer[i] * oh_ad;

        *out = oh_buffer;
    }

    void SetParams(const uint16_t *params) override
    {
        _ch_vol = params[1];
        _ch_vol /= UINT16_MAX;
        _ch_end = params[2];
        _ch_end /= UINT16_MAX;
        _oh_end = params[3];
        _oh_end /= UINT16_MAX;
    }

    const char **GetParams(uint16_t *values) override
    {
        static const char *_names[]{"", "CH-Lev", "CH-Dec", "OH-Dec", nullptr};
        uint16_t _values[8];
        _oh->GetParams(_values);

        values[0] = _values[1];
        values[1] = _ch_vol * UINT16_MAX;
        values[2] = _ch_end * UINT16_MAX;
        values[3] = _oh_end * UINT16_MAX;
        return _names;
    }
};