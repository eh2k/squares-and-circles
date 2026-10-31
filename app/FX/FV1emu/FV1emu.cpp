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

ENGINE_NAME("FX/@!FV1");

#define INLINE __attribute__((always_inline))

#include "fv1/FV1_DASM.hpp"

// program

static float raw = 0;
static int32_t _fx = 0;
static int32_t _fx_loaded = -1;
static const char *_fx_names[256] = {};
static float pot0, pot1, pot2;
static int32_t _fx_count = 0;
static char _pot_names[3][17] = {}; // the blob's names fill their 16 bytes without a terminator
static bool _loaded = false;        // a program is in the FV1

static uint32_t mem[sizeof(FV1) / sizeof(uint32_t)];
static FV1 *fv1 = nullptr;

struct FX
{
    uint8_t props;
    char name[32];
    char pot0[16];
    char pot1[16];
    char pot2[16];
    char dummy[63 - 16];
    uint8_t prog[512];
};

static_assert(sizeof(FX) == 128 + 512, "FX struct size mismatch");

void engine::setup()
{
    const FX *fx = (const FX *)&__data[8];
    auto n = __data[4];
    _fx_count = n;

    fv1 = new (mem) FV1(); // Issue with static constructor !?

    raw = 1.f;
    pot0 = 0.5f;
    pot1 = 0.5f;
    pot2 = 0.5f;
    int i = 0;
    for (i = 0; i < n; ++i)
    {
        _fx_names[i] = fx[i].name;
    }

    engine::addParam("D/W", &raw);
    if (n > 0)
        engine::addParam("FX", &_fx, 0, n - 1, _fx_names);

    engine::addParam("A", &pot0);
    engine::addParam("B", &pot1);
    engine::addParam("C", &pot2);
}

void engine::process()
{
    if (_fx != _fx_loaded && _fx >= 0 && _fx < _fx_count)
    {
        const FX *fx = (const FX *)&__data[8];
        _loaded = fv1_load(fv1, fx[_fx].prog, ::malloc) != nullptr;
        const char *names[3] = {fx[_fx].pot0, fx[_fx].pot1, fx[_fx].pot2};
        for (int i = 0; i < 3; i++)
            memcpy(_pot_names[i], names[i], 16);
        engine::setParamName(&pot0, _pot_names[0]);
        engine::setParamName(&pot1, _pot_names[1]);
        engine::setParamName(&pot2, _pot_names[2]);
        _fx_loaded = _fx;
    }

    auto inputL = engine::inputBuffer<0>();
    auto inputR = engine::inputBuffer<1>();
    auto outputL = engine::outputBuffer<0>();
    auto outputR = engine::outputBuffer<1>();

    if (!_loaded)
    {
        memcpy(outputL, inputL, sizeof(float) * FRAME_BUFFER_SIZE); // no program: dry
        memcpy(outputR, inputR, sizeof(float) * FRAME_BUFFER_SIZE);
        return;
    }

    if (raw > 0)
        fv1->execute(inputL, inputR, pot0, pot1, pot2, outputL, outputR, FRAME_BUFFER_SIZE);

    for (int i = 0; i < FRAME_BUFFER_SIZE; ++i)
    {
        outputL[i] = raw * outputL[i] + (1 - raw) * inputL[i];
        outputR[i] = raw * outputR[i] + (1 - raw) * inputR[i];
    }
}

void engine::draw()
{
    if (!_loaded && _fx_loaded >= 0)
        gfx::drawStringCenter(64, 30, "OUT OF MEMORY");
}
