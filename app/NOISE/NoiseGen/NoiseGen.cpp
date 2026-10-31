// Copyright (C)2022 - Eduard Heidt
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

// Crackle and Dust adapted from HetrickCV (https://github.com/mhetrick/hetrickcv, CC0-1.0):
// https://github.com/mhetrick/hetrickcv/blob/master/src/Crackle.cpp
// https://github.com/mhetrick/hetrickcv/blob/master/src/Dust.cpp

// build_flags: -mfloat-abi=hard -mfpu=fpv5-d16 -ffast-math -std=c++17

#include "../../squares-and-circles-api.h"
#include "misc/noise.hxx"
#include "Crackle.hpp"
#include "Dust.hpp"

#include <cmath>
#include <algorithm>

ENGINE_NAME("NOISE/NoiseGen");
ENGINE_ALIAS("NOISE/WhitePink");

enum
{
    WHITE,
    PINK,
    BROWN,
    CRACKLE,
    BROKEN,
    DUST,
    DUST_UNI,
};

const char *modes[] = {">White", ">Pink", ">Brown", ">Crackle", ">Broken", ">Dust", ">Dust+"};

static PinkNoise<> pinkL;
static PinkNoise<> pinkR;
static BrownNoise brownL;
static BrownNoise brownR;

static Crackle _crackleL;
static Crackle _crackleR;
static Dust _dustL;
static Dust _dustR;

// The crackle map gives only values >= 0: a highpass of ~4 Hz takes the DC out, like an AC-coupled output.
static struct
{
    float in = 0;
    float out = 0;

    float process(float x)
    {
        out = x - in + 0.9995f * out;
        in = x;
        return out;
    }
} _dc[2];

static float gain = 1.f;
static int32_t mode = 0;
static float stereo = 0.f;
static int8_t scope[128] = {};
static uint8_t scope_pos = 0;

// The fourth knob is Chaos for Crackle/Broken, Rate for Dust/Dust+ and hidden for White/Pink/Brown. One slot, 0 ... 1:
// draw() keeps the value of the other group and swaps it in when the mode group changes.
enum
{
    GROUP_NONE,
    GROUP_CHAOS,
    GROUP_RATE,
};

static float amount = 0.85f;                       // Crackle: map gain d = 1 + amount^3 (Chaos 1.7 of HetrickCV's 0..2)
static float group_amount[3] = {0.f, 0.85f, 0.25f}; // Dust: amount^3 impulses per sample (Rate 1 of 0..4: 750 / s)
static int32_t group_shown = -1;
static float amount_shown = 0.f;
static const char *group_name[3] = {"", "Chaos", "Rate"};

static int32_t group_of(int32_t m)
{
    return m >= DUST ? GROUP_RATE : m >= CRACKLE ? GROUP_CHAOS : GROUP_NONE;
}

void engine::setup()
{
    engine::addParam("Level", &gain);
    engine::addParam("@Mode", &mode, 0, LEN_OF(modes) - 1, modes);
    engine::addParam("Stereo", &stereo);
    engine::addParam("", &amount);
    engine::setMode(ENGINE_MODE_COMPACT);

    brownL.init();
    brownR.init();

    // the map is deterministic: R starts elsewhere so that the channels differ
    _crackleR.y1 = _crackleR.lasty1 = 0.2743f;
}

void engine::process()
{
    auto bufferL = engine::outputBuffer<0>();
    auto bufferR = engine::outputBuffer<1>();
    const bool sparse = mode >= CRACKLE;
    const float chaos = amount * 2.f; // the HetrickCV ranges: Chaos 0 ... 2, Rate 0 ... 4
    const float rate = amount * 4.f;

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        switch (mode)
        {
        case WHITE:
            bufferL[i] = pinkL.white.nextf(-1.f, 1.f) * gain;
            bufferR[i] = pinkR.white.nextf(-1.f, 1.f) * gain;
            break;
        case PINK:
            bufferL[i] = pinkL.nextf(-1.f, 1.f) * gain;
            bufferR[i] = pinkR.nextf(-1.f, 1.f) * gain;
            break;
        case BROWN:
            bufferL[i] = brownL.nextf(-1.f, 1.f) * gain;
            bufferR[i] = brownR.nextf(-1.f, 1.f) * gain;
            break;
        case CRACKLE:
        case BROKEN:
            bufferL[i] = _dc[0].process(mode == CRACKLE ? _crackleL.classic(chaos) : _crackleL.broken(chaos)) * gain;
            bufferR[i] = _dc[1].process(mode == CRACKLE ? _crackleR.classic(chaos) : _crackleR.broken(chaos)) * gain;
            break;
        case DUST:
        case DUST_UNI:
            bufferL[i] = _dustL.process(rate, mode == DUST) * gain;
            bufferR[i] = _dustR.process(rate, mode == DUST) * gain;
            break;
        }
    }

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        // sparse signals: mono is the left channel (a mean of two would halve the clicks)
        float tmp = sparse ? bufferL[i] * (1 - stereo) : (bufferL[i] + bufferR[i]) / 2.f * (1 - stereo);

        bufferL[i] = bufferL[i] * stereo + tmp;
        bufferR[i] = bufferR[i] * stereo + tmp;

        scope[scope_pos] = bufferL[i] * 7;
        ++scope_pos %= 128;
    }
}

void engine::draw()
{
    const int32_t g = group_of(mode);
    if (g != group_shown)
    {
        // no swap on the first draw, nor when amount changed with the mode (a patch was loaded): then amount is the
        // value of the patch
        if (group_shown >= 0 && amount == amount_shown)
        {
            group_amount[group_shown] = amount;
            amount = group_amount[g];
        }
        engine::setParamName(&amount, group_name[g]);
        group_shown = g;
    }
    amount_shown = amount;

    for (size_t i = 0; i < 128; i++)
    {
        auto y = scope[(i + scope_pos) % 128];
        gfx::drawLine(i, 52, i, 52 - y);
    }
}
