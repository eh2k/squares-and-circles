
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

#include "../lib/plaits/dsp/engine2/arpeggiator.h"
#define private public
#define SemitonesToRatioFast
#include "../lib/plaits/dsp/chords/chord_bank.cc"
#include "stmlib/utils/random.cc"

#include "../lib/stmlib/utils/dsp.h"
#include "../DRUMS/resources/peaks_lut_env.hpp"

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

int32_t len = 16;
int32_t pulses = 4;
int32_t rotate = 0;
int32_t swing = 128;

int32_t seq_pos = UINT8_MAX;
bool playing = true;
bool pre_start = false;

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
        if (pattern[seq_pos % _len] && (engine::clock() % 6) < 3)
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

float arp_root = 0;
int32_t mode = 0;

int32_t chord = 0;
plaits::Arpeggiator arpeggiator_;
plaits::ChordBank chords_;
int32_t chords_mem[(plaits::kChordNumChords * plaits::kChordNumNotes) + plaits::kChordNumChords + plaits::kChordNumNotes];

int32_t octaves = 1;
int32_t _slide = 0;

const char *smodes[] = {
    "UP",
    "DOWN",
    "CYCLE",
    "RAND",
};

const char *soctaves[] = {
    "1-OCT",
    "2-OCT",
    "3-OCT",
    "4-OCT",
};

void engine::setup()
{
    arpeggiator_.Init();
    stmlib::BufferAllocator allocator;
    allocator.Init(chords_mem, sizeof(chords_mem));
    chords_.Init(&allocator);
    chords_.chord_index_quantizer_.hysteresis_ = 0; // input => output
    chords_.Reset();

    engine::addParam("#LEN", &len, 1, 20);
    engine::addParam(V_OCT, &arp_root); // arp_root ist automatically added to engine::cv()
    engine::addParam("#HIT", &pulses, 0, 16);
    engine::addParam("Mode", &mode, 0, LEN_OF(smodes) - 1, smodes);
    engine::addParam("#ROT", &rotate, 0, 32);
    engine::addParam("CHRD", &chord, 0, plaits::kChordNumChords - 1, (const char **)plaits::chord_names);
    engine::addParam("RNGE", &octaves, 0, LEN_OF(soctaves) - 1, soctaves);
    engine::addParam("SLIDE", &_slide, 0, 1);
    engine::setMode(ENGINE_MODE_CV_OUT);
}

uint8_t pattern[64] = {};

int32_t gate_until_t = 0;
static int32_t key = 1;

int32_t _len = len;     // for mod vizualization
int32_t _swing = swing; // for mod vizualization

char debug[128] = {};

static int32_t _last_len = -1;
static auto _last_pulses = pulses;
static auto _last_rotate = rotate;
static auto _last_mode = mode;
static uint32_t _slide_phase = UINT32_MAX;
static uint32_t _slide_phase_inc = 0;

uint32_t _t = 0;
float _last_cv = 0;
float _cv = 0;
float _cv_out = 0;

void update_seq_pattern()
{
    CONSTRAIN(pulses, pulses, len);
    rotate %= len;
    _swing = swing;

    if (_last_len != len || _last_pulses != pulses || _last_rotate != rotate || _last_mode != mode)
    {
        if (_last_len != len && _last_len == _last_pulses)
            pulses = len;

        _last_len = len;
        _last_pulses = pulses;
        _last_rotate = rotate;
        _last_mode = mode;
        pre_start = false;
        _len = len;

        make_pattern(pattern, len, pulses, rotate);

        for (int i = len; i < 64; i++)
            pattern[i] = pattern[i - len];
    }

    if (engine::stepChanged())
    {
        seq_pos = engine::step() % len;
        if (seq_pos == 0)
            arpeggiator_.Reset();

        bool trig = pattern[seq_pos];

        if (trig)
        {
            uint32_t n = 1 + (engine::t() - _t) / 2;

            if (_slide > 0)
            {
                int m = n * 2;
                for (int i = seq_pos; !pattern[(i + 1) % _len]; i++)
                    m += n * 2;

                _slide_phase = 0;
                _slide_phase_inc = UINT32_MAX / n;
                gate_until_t = 2 + m;
            }
            else
                gate_until_t = 2 + n; // 10ms trigger

            arpeggiator_.Clock(chords_.num_notes());
            const float ratio = chords_.sorted_ratio(arpeggiator_.note());
            _last_cv = _cv;
            _cv = engine::cv() + arpeggiator_.octave() + log2f(ratio);

            key++;
        }

        _t = engine::t();
    }
}

void engine::process()
{
    if (!playing)
    {
        gate_until_t = 0;
        seq_pos = len - 1;
        pre_start = true;
        return;
    }

    update_seq_pattern();

    arpeggiator_.set_mode((plaits::ArpeggiatorMode)(mode % plaits::ARPEGGIATOR_MODE_LAST));
    arpeggiator_.set_range(1 + octaves);

    chords_.set_chord((float)chord / (plaits::kChordNumChords - 1));
    chords_.Sort();

    int32_t trig = 0;

    if (gate_until_t > 0)
    {
        --gate_until_t;
        trig = 5 * PITCH_PER_OCTAVE; // 5V
    }

    if (_slide_phase < (UINT32_MAX - _slide_phase_inc))
    {
        float e = stmlib::Interpolate824(peaks::lut_env_expo, _slide_phase);
        _slide_phase += _slide_phase_inc;
        _cv_out = _last_cv + ((_cv - _last_cv) * (e / UINT16_MAX));
    }
    else
    {
        _cv_out = _cv;
    }

    std::fill_n(engine::outputBuffer<1>(), FRAME_BUFFER_SIZE, _cv_out);

    std::fill_n(engine::outputBuffer_i16<0>(), FRAME_BUFFER_SIZE, trig);
}

void engine::draw()
{
    // for (int i = 0; i < chords_.num_notes(); i++)
    // {
    //     const float ratio = chords_.sorted_ratio(i);
    //     float notef = engine::cv() + arpeggiator_.octave() + log2f(ratio);

    //     float delta = 10.f;
    //     int note = 0;

    //     int n = 0;
    //     for (float i = -4.f; i < 4.f; i += 1.f / 12)
    //     {
    //         float d = std::fabs(i - notef);
    //         if (d < delta)
    //         {
    //             delta = d;
    //             note = n;
    //         }

    //         ++n;
    //     }
    //     bool now = fabsf(_cv - notef) < 0.001f;

    //     int key = note % 12;
    //     if (key == 1 || key == 3 || key == 6 || key == 8 || key == 10)
    //         gfx::invertRect((key < 5 ? 87 : 89) + (key * 2), (now ? 20 : 21) - 5, 1, now ? 3 : 1);
    //     else
    //         gfx::clearRect((key < 5 ? 87 : 89) + (key * 2), now ? 20 : 21, 1, now ? 3 : 1);
    // }
    draw_eclid_cyrcle(36, 31, pattern, _last_len, _last_rotate);
}