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
#include <utility>

static float apmlitude = 1.f;
static int32_t x_scale = 32;
static float offset = 0.f;
static float input = 0.f;
static float output = 0.f;

static int scope_n = 0;
static float scope_max = -1000.f;
static float scope_min = +1000.f;
static uint32_t scope_pos = 0;
static std::pair<float, float> scope[128 * 4] = {};

void engine::setup()
{
    apmlitude = 1.f;
    engine::addParam("_Y-Scale", &apmlitude, 0.f, 2.f);
    engine::addParam("_Offset", &offset, -1.f, 1.f);
    engine::addParam("_X-Scale", &x_scale, 0, 63);
    engine::setMode(ENGINE_MODE_COMPACT);
}

void draw_scope(int y);
void push_scope(float y, float ymax)
{
    scope[scope_pos++ % LEN_OF(scope)] = std::make_pair(y * 20, ymax * 20);
    if (scope_pos >= ((x_scale > FRAME_BUFFER_SIZE) ? 128 : LEN_OF(scope)))
        scope_pos = 0;
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

    int n = x_scale;
    if (x_scale > FRAME_BUFFER_SIZE)
    {
        n = (x_scale - FRAME_BUFFER_SIZE) * FRAME_BUFFER_SIZE;
    }

    for (size_t j = 0; j < FRAME_BUFFER_SIZE; j++)
    {
        scope_max = fmax(scope_max, outputL[j]);
        scope_min = fmin(scope_min, outputL[j]);

        if (++scope_n > n)
        {
            push_scope(scope_min, scope_max);
            scope_n = 0;
            scope_max = -1000;
            scope_min = +1000;
        }
    }
}

void engine::draw()
{
    draw_scope(38);
}

void engine::screensaver()
{
    gfx::clearRect(0, 0, 128, 64);
    draw_scope(32);
}

void draw_scope(int y)
{
    if (x_scale < FRAME_BUFFER_SIZE)
    {
        int start = -1;
        int end = -1;

        for (int x = 0; x < LEN_OF(scope) - 1; x++)
        {
            if (
                (scope[(scope_pos + x) % LEN_OF(scope)].first > 0 && scope[(scope_pos + x + 1) % LEN_OF(scope)].first <= 0))
                if (start < 0) {
                    start = (scope_pos + x);
                }
                else if (end < 0)
                {
                    end = (scope_pos + x);
                    break;
                }
        }

        if (start < 0)
            start = scope_pos;

        if (end < 0)
            end = start + LEN_OF(scope);

        for (int x = 0; x < 127; x++)
        {
            if (x % 3 == 0)
                gfx::setPixel(x, y);

            gfx::drawLine(
                x, y + scope[(start + x) % LEN_OF(scope)].first,
                x + 1, y + scope[(start + x + 1) % LEN_OF(scope)].first);

            // if (scope[(start + i) % LEN_OF(scope)] > 0 && scope[(start + i + 1) % LEN_OF(scope)] <= 0)
            //     gfx::drawLine(x, y - 20, x, y + 20);
        }
    }
    else
    {
        for (int x = 0; x < 127; x++)
        {
            if (x % 3 == 0)
                gfx::setPixel(x, y);

            gfx::drawLine(
                x, y + scope[(scope_pos + x) % 128].first,
                x, y + scope[(scope_pos + x) % 128].second);

            // if (scope[(start + i) % LEN_OF(scope)] > 0 && scope[(start + i + 1) % LEN_OF(scope)] <= 0)
            //     gfx::drawLine(x, y - 20, x, y + 20);
        }
    }

    char tmp[64];
    sprintf(tmp, "IN:%+1.2fV", input * 5);
    gfx::drawString(0, 54, tmp, 0);
    sprintf(tmp, "OUT:%+1.2fV", output * 5);
    gfx::drawString(64, 54, tmp, 0);
}
