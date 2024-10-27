
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

#include "../squares-and-circles-api.h"
#include "../lib/misc/euclidean.h"
#include <algorithm>
#include <math.h>

// build_flags: -fno-inline -mfloat-abi=soft -mfpu=fpv5-d16

// This app is copiled with soft-fpu - for running on teensy3.2 & teensy 4.x

/*
https://www.nullhardware.com/blog/fixed-point-sine-and-cosine-for-embedded-systems/

Implements the 5-order polynomial approximation to sin(x).
@param i   angle (with 2^15 units/circle)
@return    16 bit fixed point Sine value (4.12) (ie: +4096 = +1 & -4096 = -1)

The result is accurate to within +- 1 count. ie: +/-2.44e-4.
*/

int16_t fpsin(int16_t i)
{
    /* Convert (signed) input to a value between 0 and 8192. (8192 is pi/2, which is the region of the curve fit). */
    /* ------------------------------------------------------------------- */
    i <<= 1;
    uint8_t c = i < 0; // set carry for output pos/neg

    if (i == (i | 0x4000)) // flip input value to corresponding value in range [0..8192)
        i = (1 << 15) - i;
    i = (i & 0x7FFF) >> 1;
    /* ------------------------------------------------------------------- */

    /* The following section implements the formula:
     = y * 2^-n * ( A1 - 2^(q-p)* y * 2^-n * y * 2^-n * [B1 - 2^-r * y * 2^-n * C1 * y]) * 2^(a-q)
    Where the constants are defined as follows:
    */
    uint32_t A1 = 3370945099UL, B1 = 2746362156UL, C1 = 292421UL;
    uint32_t n = 13, p = 32, q = 31, r = 3, a = 12;

    uint32_t y = (C1 * ((uint32_t)i)) >> n;
    y = B1 - (((uint32_t)i * y) >> r);
    y = (uint32_t)i * (y >> n);
    y = (uint32_t)i * (y >> n);
    y = A1 - (y >> (p - q));
    y = (uint32_t)i * (y >> n);
    y = (y + (1UL << (q - a - 1))) >> (q - a); // Rounding

    return c ? -y : y;
}

// Cos(x) = sin(x + pi/2)
inline int16_t fpcos(int16_t i) { return fpsin((int16_t)(((uint16_t)(i)) + 8192U)); }

// https://xbm.jazzychad.net/
const uint8_t xmb_note_6x8[8] = {0xc8, 0xd8, 0xe8, 0xd8, 0xe8, 0xce, 0xcf, 0xc6};
const uint8_t xmb_rest_6x8[8] = {0xd6, 0xde, 0xd0, 0xd6, 0xde, 0xd0, 0xd0, 0xd0};

const uint8_t xmb_arrow_up_5x6[6] = {0xe4, 0xee, 0xff, 0xe4, 0xe4, 0xe4};
const uint8_t xmb_arrow_dn_5x6[6] = {0xe4, 0xe4, 0xe4, 0xf5, 0xee, 0xe4};
const uint8_t xmb_circle_7x7[7] = {0x9c, 0xa2, 0xc1, 0xc1, 0xc1, 0xa2, 0x9c};
const uint8_t xmb_filled_circle_7x7[7] = {0x9c, 0xa2, 0xdd, 0xdd, 0xdd, 0xa2, 0x9c};
const uint8_t xbm_play_11x11[] = {0x03, 0xf8, 0x0d, 0xf8, 0x31, 0xf8, 0xc1, 0xf8, 0x01, 0xfb, 0x01, 0xfc, 0x01, 0xfb, 0xc1, 0xf8, 0x31, 0xf8, 0x0d, 0xf8, 0x03, 0xf8};
const uint8_t xbm_stop_7x11[] = {0x80, 0xf7, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xf7, 0x80};

bool playing = true;
bool pre_start = false;
uint8_t seq_pos = UINT8_MAX;
int32_t swing = 128;

void draw_eclid_cyrcle(int x, int y, uint8_t pattern[64], int _len, int rot)
{
    auto pattern2 = pattern;

    int xx = x;
    int yy = y;

    if ((fpsin((INT16_MAX / (_len))) / 194) > 6)
    {
        gfx::drawCircle(xx + 3, 31 + 3, 21);

        for (int i = 0; i < _len; i++)
        {
            int a = (INT16_MAX / (_len)) * i;

            if (MOD(i, 2) == 1)
                a += (-128 + swing) * (INT16_MAX / _len) / 6;

            int x = fpsin(a) / 194;
            int y = -fpcos(a) / 194;
            x += xx;
            y += yy;

            gfx::clearCircle(x + 3, y + 3, 3);
        }
    }

    for (int i = 0; i < _len; i++)
    {
        int a = (INT16_MAX / (_len)) * i;

        if (MOD(i, 2) == 1)
            a += (-128 + swing) * (INT16_MAX / _len) / 6;

        int dx = fpsin(a);
        int dy = -fpcos(a);

        int x = xx + dx / 194;
        int y = yy + dy / 194;

        if (pattern[i] || pattern2[i])
            gfx::fillRect(x + 2, y + 2, 3, 3); // gfx::drawXbm(x, y, 7, 7, xmb_filled_circle_7x7);

        if (i == rot)
            gfx::drawRect(x, y, 7, 7);
        else
            gfx::drawCircle(x + 3, y + 3, 3); //

        bool cursor = i == MOD(seq_pos, _len);

        if (cursor && !pre_start)
            gfx::drawRect(x + 1, y + 1, 5, 5);
    }

    if (playing && pre_start && MOD(millis(), 300) > 150)
    {
    }
    else
    {
        int x = xx - 2;
        int y = 40 - 7;
        if (pattern[seq_pos % _len])
        {
            if (x < 64)
                gfx::fillRect(x - 3, y - 7, 17, 17);
            else
                gfx::fillCircle(x + 5, y + 1, 8);
        }
        else
        {
            if (x < 64)
                gfx::drawRect(x - 2, y - 7, 16, 16);
            else
                gfx::drawCircle(x + 5, y + 1, 8);
        }
    }
}

int32_t len = 16;
int32_t pulses = 4;
int32_t rotate = 0;

int32_t lenB = 16;
int32_t pulsesB = 4;
int32_t rotateB = 0;

void engine::setup()
{
    engine::addParam("#LEN", &len, 1, 20);
    engine::addParam("#LEN", &lenB, 1, 20);
    engine::addParam("#HIT", &pulses, 0, 16);
    engine::addParam("#HIT", &pulsesB, 0, 16);
    engine::addParam("#ROT", &rotate, 0, 32);
    engine::addParam("#ROT", &rotateB, 0, 32);
    engine::setMode(ENGINE_MODE_CV_OUT);
}

uint8_t pattern[64] = {};
uint8_t pattern2[64] = {};

uint8_t gate_until_t = 0;
uint8_t gate_until2_t = 0;
uint8_t next_clock = 1;

int hit1 = 0;
int hit2 = 0;

char debug[128] = {};

static int32_t _last_len = -1;
static auto _last_pulses = pulses;
static auto _last_rotate = rotate;

static int32_t _last_lenB = -1;
static auto _last_pulsesB = pulsesB;
static auto _last_rotateB = rotateB;

void update_seq_pattern()
{
    CONSTRAIN(pulses, pulses, len);
    rotate %= len;

    if (_last_len != len || _last_pulses != pulses || _last_rotate != rotate ||
        _last_lenB != lenB || _last_pulsesB != pulsesB || _last_rotateB != rotateB)
    {
        _last_len = len;
        _last_pulses = pulses;
        _last_rotate = rotate;

        _last_lenB = lenB;
        _last_pulsesB = pulsesB;
        _last_rotateB = rotateB;

        pre_start = false;

        make_pattern(pattern, len, pulses, rotate);
        make_pattern(pattern2, lenB, pulsesB, rotateB);
    }

    if (engine::stepChanged())
    {
        seq_pos = engine::step();

        bool trig = pattern[seq_pos % len];

        if (trig)
        {
            hit1 = 64;
            gate_until_t += 2; // 10ms trigger
        }

        bool trig2 = pattern2[seq_pos % lenB];
        if (trig2)
        {
            hit2 = 64;
            gate_until2_t += 2;
        }
    }
}

void engine::process()
{
    if (!playing)
    {
        gate_until_t = gate_until2_t = 0;
        seq_pos = len - 1;
        next_clock = 1;
        pre_start = true;
        return;
    }

    update_seq_pattern();

    int32_t trig = 0;

    if (gate_until_t > 0)
    {
        --gate_until_t;
        trig = 5 * PITCH_PER_OCTAVE; // 5V
    }

    std::fill_n(engine::outputBuffer_i16<0>(), FRAME_BUFFER_SIZE, trig);

    trig = 0;
    if (gate_until2_t > 0)
    {
        --gate_until2_t;
        trig = 5 * PITCH_PER_OCTAVE; // 5V
    }

    std::fill_n(engine::outputBuffer_i16<1>(), FRAME_BUFFER_SIZE, trig);
}

void engine::draw()
{
    char tmp[16];
    sprintf(tmp, "%02d", engine::step());
    gfx::drawStringCenter(64, 12, tmp, 0);
    draw_eclid_cyrcle(36, 31, pattern, _last_len, _last_rotate);
    draw_eclid_cyrcle(36 + 49, 31, pattern2, _last_lenB, _last_rotateB);
}

void engine::screensaver()
{
    gfx::clearRect(0, 0, 128, 64);
    draw_eclid_cyrcle(36 - 4, 31, pattern, _last_len, _last_rotate);
    draw_eclid_cyrcle(36 + 49 + 4, 31, pattern2, _last_lenB, _last_rotateB);
}