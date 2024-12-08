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

// ENGINE_NAME: CV/V/OCT

#include "../squares-and-circles-api.h"
#include <algorithm>

#define ONE_POLE(out, in, coefficient) out += (coefficient) * ((in) - out);

float note = 0;
int32_t tune = 128;
int32_t cv0 = 0;
int16_t cv_ = 0;
float glide = 0;

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

void engine::setup()
{
    engine::addParam(V_OCT, &note);
    engine::addParam("Fine", &tune, 0, 254);
    engine::addParam("Slew", &glide, 0, 0.5f);
    engine::setMode(ENGINE_MODE_COMPACT | ENGINE_MODE_CV_OUT);
}

void engine::process()
{
    cv0 = engine::qz_process(PITCH_PER_OCTAVE * 2 + engine::cv_i32(), 0, nullptr) // note is added internal
          + (((int)tune - 128) << 2);

    ONE_POLE(cv_, cv0, powf(1 - glide, 10));

    std::fill_n(engine::outputBuffer_i16<0>(), FRAME_BUFFER_SIZE, cv_);

    if ((engine::t() % 50) == 0)
        gfx::push_scope((float)cv_ / PITCH_PER_OCTAVE * 4);
}

void engine::draw()
{
    char tmp[64];
    sprintf(tmp, "OUT:%.2fV", ((float)cv_ / PITCH_PER_OCTAVE));
    gfx::drawString(4 + 64, 32, tmp, 0);
    gfx::drawScope(0, 50);
}
