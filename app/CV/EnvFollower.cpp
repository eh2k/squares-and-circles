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
#include <algorithm>
#include "../../lib/streams/follower.h"
#include "../../lib/streams/vactrol.h"
#include "../../lib/streams/audio_cv_meter.h"

streams::Follower _follower;
streams::Vactrol _vactrol;
streams::AudioCvMeter _meter;

int32_t attack_ = 0;
int32_t decay_ = 0;
int32_t mode_ = 0;

namespace gfx
{
    uint8_t i = 0;
    int8_t scope[128] = {};
    void drawScope(int x0, int y)
    {
        for (int x = x0; x < 127; x++)
        {
            if (x % 3 == 0)
                gfx::setPixel(x, y);
            gfx::drawLine(x, y - scope[(i + x) % 128], x + 1, y - scope[(1 + i + x) % 128]);
        }
    }

    void push_scope(int8_t y)
    {
        scope[i++ % 128] = y;
        if (i > 128)
            i = 0;
    }
}

const char *mode_names[] = {
    "Follower",
    "Vactrol",
    ""};

void engine::setup()
{
    engine::addParam("Attack", &attack_, 0, UINT16_MAX);
    engine::addParam("MODE", &mode_, 0, 1, mode_names);
    engine::addParam("Decay", &decay_, 0, INT16_MAX);
    decay_ = INT16_MAX / 2;
    engine::setMode(ENGINE_MODE_COMPACT | ENGINE_MODE_CV_OUT);

    _meter.Init();
    _follower.Init();
    _vactrol.Init();
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto outputL = engine::outputBuffer_i16<0>();

    uint16_t gain;
    uint16_t freq;

    int32_t paramters[3] = {};
    int32_t globals[3] = {};
    globals[0] = attack_;
    globals[2] = decay_;
    paramters[1] = 0;

    if (mode_ == 0)
    {
        _follower.Configure(false, paramters, globals);
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            _follower.Process(0, inputL[i] * INT16_MAX, &gain, &freq);
            outputL[i] = gain >> 2; //_follower.process(inputL[i]) * 10.f;
        }
    }
    else
    {
        _vactrol.Configure(false, paramters, globals);
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            _vactrol.Process(0, inputL[i] * INT16_MAX, &gain, &freq);
            outputL[i] = gain >> 2; //_follower.process(inputL[i]) * 10.f;
        }
    }

    _meter.Process(inputL[0] * INT16_MAX);

    if ((engine::t() % 50) == 0)
    {
        int8_t v = outputL[0] >> 9;
        CONSTRAIN(v, 0, 14);
        gfx::push_scope(v);
    }
}

void engine::draw()
{
    gfx::drawScope(64, 58);
    gfx::drawRect(64, 44, 64, 15);
    gfx::drawRect(0, 44, 63, 15);
    gfx::fillRect(2, 46, (_meter.peak() >> 8), 11);
}

#include "../../lib/streams/svf.cc"
#include "../../lib/streams/follower.cc"
#include "../../lib/streams/vactrol.cc"
#include "../../lib/streams/resources.cc"