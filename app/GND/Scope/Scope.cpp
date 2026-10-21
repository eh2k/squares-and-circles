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

#include "../../squares-and-circles-api.h"
#include <utility>

float apmlitude = 1.f;
int32_t x_scale = 32;
float offset = 0.f;
float input = 0.f;
float output = 0.f;

float *inputL = nullptr;
float *inputR = nullptr;
float *outputL = nullptr;
float *outputR = nullptr;

float scope[FRAME_BUFFER_SIZE] = {};

void engine::setup()
{
    apmlitude = 1.f;
    engine::addParam("_Y-Scale", &apmlitude, 0.f, 2.f);
    engine::addParam("_Offset", &offset, -1.f, 1.f);
    engine::addParam("_X-Scale", &x_scale, 0, 63);
    engine::setMode(ENGINE_MODE_COMPACT);

}

void set(float *target, const float *src, float amp, float offset)
{
    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        target[i] = src[i] * amp + offset;
}

void engine::process()
{
    inputL = engine::inputBuffer<0>();
    inputR = engine::inputBuffer<1>();
    outputL = engine::outputBuffer<0>();
    outputR = engine::outputBuffer<1>();

    input = (inputL[0] + inputR[0]) / 2.f;
    set(outputL, inputL, apmlitude, offset);
    set(outputR, inputR, apmlitude, offset);
    output = (outputL[0] + outputR[0]) / 2.f;

    for (size_t j = 0; j < FRAME_BUFFER_SIZE; j++)
        scope[j] = (outputL[j] + outputR[j]) / 2.f;
}

void engine::draw()
{
    gfx::drawScope(38, scope, x_scale);

    char tmp[64];
    sprintf(tmp, "IN:%+1.2fV", input * 5);
    gfx::drawString(0, 54, tmp, 0);
    sprintf(tmp, "OUT:%+1.2fV", output * 5);
    gfx::drawString(64, 54, tmp, 0);
}

void engine::screensaver()
{
    gfx::clearRect(0, 0, 128, 64);
    gfx::drawScope(32, scope, x_scale);
}
