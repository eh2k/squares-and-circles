// Copyright (C)2024 - E.Heidt
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

static float apmlitude = 1.f;
static float offset = 0.f;
static float input = 0.f;
static float output = 0.f;

uint32_t i = 0;
static int8_t scope[128] = {};

void engine::setup()
{
    apmlitude = 1.f;
    engine::addParam("_Scale", &apmlitude, 0.f, 2.f);
    engine::addParam("_Offset", &offset, -1.f, 1.f);
    engine::setMode(ENGINE_MODE_COMPACT);
}

void push_scope(int8_t scope[128], int8_t y)
{
    scope[i++ % 128] = y;
    if (i > 128)
        i = 0;
}

void set(float *target, const float *src, float amp, float offset)
{
    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        target[i] = src[i] * amp + offset;
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto outputL = engine::outputBuffer<0>();

    input = inputL[0];
    set(outputL, inputL, apmlitude, offset);
    output = outputL[0];

    if ((engine::t() % 50) == 0)
        push_scope(scope, outputL[0] * 20);
}

void engine::draw()
{
    int y = 38;
    for (int x = 0; x < 127; x++)
    {
        if (x % 3 == 0)
            gfx::setPixel(x, y);
        gfx::drawLine(x, y - scope[(i + x) % 128], x + 1, y - scope[(1 + i + x) % 128]);
    }

    char tmp[64];
    sprintf(tmp, "IN:%+1.2fV", input * 5);
    gfx::drawString(0, 54, tmp, 0);
    sprintf(tmp, "OUT:%+1.2fV", output * 5);
    gfx::drawString(64, 54, tmp, 0);
}