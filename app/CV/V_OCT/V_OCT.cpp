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
#include <algorithm>

ENGINE_ALIAS("CV/V/OCT"); // the name it had before, in the patches of older firmwares

#define ONE_POLE(out, in, coefficient) out += (coefficient) * ((in) - out);

float note = 0;
int32_t tune = 0;
int32_t cv0 = 0;
int16_t cv_ = 0;
float cv_slew = 0; // the slew in float: on the int16_t, each step was cut off and the glide stopped short
float glide = 0;
int16_t* out_cv = nullptr;

void engine::setup()
{
    engine::addParam(V_OCT, &note);
    engine::addParam("@Fine", &tune, INT8_MIN, INT8_MAX);
    engine::addParam("Slew", &glide, 0, 0.5f);

    out_cv = engine::outputBuffer_i16(OUT_VOCT);

    engine::setMode(ENGINE_MODE_COMPACT);
}

void engine::process()
{
    cv0 = engine::cv_i32() // note is added internal
          + (tune << 2);

    ONE_POLE(cv_slew, cv0, powf(1 - glide, 10));
    cv_ = (int16_t)(cv_slew + (cv_slew < 0 ? -0.5f : 0.5f));

    std::fill_n(out_cv, FRAME_BUFFER_SIZE, cv_);
}

void engine::draw()
{
    char tmp[64];
    sprintf(tmp, "INT: %d", cv0);
    gfx::drawString(2, 46, tmp, 0);
    sprintf(tmp, "OUT: %.3fV", ((float)cv_ / PITCH_PER_OCTAVE));
    gfx::drawString(2, 52, tmp, 0);
    gfx::drawWaveform(out_cv, 64);
}
