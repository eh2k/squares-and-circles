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

// build_flags: -fno-inline -mfloat-abi=soft -mfpu=fpv5-d16

#include "../squares-and-circles-api.h"
#include <algorithm>

static int32_t _turing_prob = 50;
static int32_t _turing_length = 8;
static uint8_t _turing_byte = 0;
static int32_t _turing_shift_reg = 0x8000;

static int32_t _pulse1 = 0;
static int32_t _pulse2 = 15;

struct
{
    int32_t _shift_reg = 0;
    int32_t _rng_seed = 20071204;
} _reset_state;

uint32_t _rng_state;
inline void Seed(uint32_t seed)
{
    _rng_state = seed;
}

inline uint32_t Rand()
{
    _rng_state = _rng_state * 1664525L + 1013904223L;
    return _rng_state;
}

const char *pulse_modes[21] = {};

int32_t note_from_scale(uint8_t bits)
{
    //origin: https://github.com/Chysn/O_C-HemisphereSuite/blob/893deeb27ecacddad638ff9180f244a411623e35/software/o_c_REV/enigma/EnigmaOutput.h#L104

    uint8_t mask = 0;
    for (uint8_t s = 0; s < bits; s++)
        mask |= (0x01 << s);
    int note_shift = bits == 7 ? 0 : 64; // Note types under 7-bit start at Middle C
    int note_number = (_turing_byte & mask) + note_shift;
    CONSTRAIN(note_number, 0, 127);
    return engine::qz_lookup(note_number);
}

int32_t output(uint32_t mode, const char **name)
{
#define _OR(b) (((_turing_byte & b) != 0) ? INT16_MAX : 0);
#define _AND(b) (((_turing_byte & b) == b) ? INT16_MAX : 0);
    switch (mode)
    {
    case 0:
    {
        *name = "Pulse:1";
        return _OR(0b10000000);
    }
    case 1:
    {
        *name = "P:1+2";
        return _OR(0b11000000);
    }
    case 2:
    {
        *name = "P:1&2";
        return _AND(0b11000000);
    }
    case 3:
    {
        *name = "Pulse:2";
        return _OR(0b01000000);
    }
    case 4:
    {
        *name = "P:2+4";
        return _OR(0b01010000);
    }
    case 5:
    {
        *name = "P:2&4";
        return _AND(0b01010000);
    }
    case 6:
    {
        *name = "Pulse:3";
        return _AND(0b00100000);
    }
    case 7:
    {
        *name = "Pulse:4";
        return _AND(0b00010000);
    }
    case 8:
    {
        *name = "Pulse:5";
        return _AND(0b00001000);
    }
    case 9:
    {
        *name = "P:4+7";
        return _OR(0b00010010);
    }
    case 10:
    {
        *name = "P:4&7";
        return _AND(0b00010010);
    }
    case 11:
    {
        *name = "Pulse:6";
        return _AND(0b00000100);
    }
    case 12:
    {
        *name = "P:+1247";
        return _OR(0b11010010);
    }
    case 13:
    {
        *name = "P:&1247";
        return _AND(0b11010010);
    }
    case 14:
    {
        *name = "Pulse:7";
        return _AND(0b00000010);
    }
    case 15:
    {
        *name = "CV_5V";
        return (int32_t)_turing_byte * (5 * PITCH_PER_OCTAVE / 255);
    }
    case 16:
    {
        *name = "NOTE-7";
        return note_from_scale(7);
    }
    case 17:
    {
        *name = "NOTE-6";
        return note_from_scale(6);
    }
    case 18:
    {
        *name = "NOTE-5";
        return note_from_scale(5);
    }
    case 19:
    {
        *name = "NOTE-4";
        return note_from_scale(4);
    }
    case 20:
    {
        *name = "NOTE-3";
        return note_from_scale(3);
    }
    case LEN_OF(pulse_modes):
    {
        break;
    }
    }

    return 0;
}

void engine::setup()
{
    Seed(reinterpret_cast<uint32_t>(&_turing_shift_reg));

    for (int i = 0; i < LEN_OF(pulse_modes); i++)
        output(i, &pulse_modes[i]);

    engine::addParam("@Prob\n%d%%", &_turing_prob, 0, 100);
    engine::addParam("@Length\n%d", &_turing_length, 1, 16);
    engine::addParam("@Out1\n%s", &_pulse1, 0, LEN_OF(pulse_modes) - 1, pulse_modes);
    engine::addParam("@Out2\n%s", &_pulse2, 0, LEN_OF(pulse_modes) - 1, pulse_modes);

    engine::setPatchStateEx(&_reset_state, sizeof(_reset_state));

    engine::setMode(ENGINE_MODE_COMPACT | ENGINE_MODE_CV_OUT);
}

static int32_t trig_pulse1 = 0;
static int32_t trig_pulse2 = 0;
static int32_t cv1 = 0;
static int32_t cv2 = 0;

static int32_t _last_prob = -1; // do not save on first run...
static int32_t _last_steps = -1;
static bool _inited = false;

void engine::process()
{
    if (_last_prob != _turing_prob || _last_steps != _turing_length)
    {
        if (_inited)
        {
            // save state on change (skip first time)
            _reset_state._rng_seed = _rng_state;
            _reset_state._shift_reg = _turing_shift_reg;
        }
        else
            _inited = true;

        _last_prob = _turing_prob;
        _last_steps = _turing_length;
    }

    if (engine_sync::stepChanged())
    {
        if (engine_sync::stepReset()) // Second Trig!
        {
            _rng_state = _reset_state._rng_seed;
            _turing_shift_reg = _reset_state._shift_reg;
        }

        bool next_bit = (_turing_shift_reg & (0x8000 >> (_turing_length - 1))) ? 1 : 0;

        _turing_shift_reg = _turing_shift_reg >> 1;

        if ((Rand() % 100) < _turing_prob)
            next_bit = 1 - next_bit;

        if (next_bit)
            _turing_shift_reg |= 0x8000;

        _turing_byte = (_turing_shift_reg >> 8) & static_cast<uint32_t>(0xFF);

        const char *dummy;
        cv1 = output(_pulse1, &dummy);
        cv2 = output(_pulse2, &dummy);

        if (cv1 == INT16_MAX)
        {
            cv1 = 5 * PITCH_PER_OCTAVE;
            trig_pulse1 = engine_sync::samples_per_step() / 2;
        }
        else
            cv1 += engine::cv_i32();

        if (cv2 == INT16_MAX)
        {
            cv2 = 5 * PITCH_PER_OCTAVE;
            trig_pulse2 = engine_sync::samples_per_step() / 2;
        }
        else
            cv2 += engine::cv_i32();
    }

    if (trig_pulse1 > 0)
    {
        trig_pulse1 -= FRAME_BUFFER_SIZE;

        if (trig_pulse1 <= 0)
            cv1 = 0;
    }

    if (trig_pulse2 > 0)
    {
        trig_pulse2 -= FRAME_BUFFER_SIZE;

        if (trig_pulse2 <= 0)
            cv2 = 0;
    }

    std::fill_n(engine::outputBuffer_i16<0>(), FRAME_BUFFER_SIZE, cv1);
    std::fill_n(engine::outputBuffer_i16<1>(), FRAME_BUFFER_SIZE, cv2);
}

void engine::draw()
{
    gfx::drawCircle(2 + 42, 2 + 28, 2);
    if (cv1 > 0)
        gfx::fillCircle(2 + 42, 2 + 28, 2);
    gfx::drawCircle(2 + 42 + 64, 2 + 28, 2);
    if (cv2 > 0)
        gfx::fillCircle(2 + 42 + 64, 2 + 28, 2);

    for (size_t bit = 0; bit < 8; bit++)
    {
        if ((_turing_byte & (0x80 >> bit)) != 0)
            gfx::fillRect(2 + bit * 16, 46, 12, 12);
        else
            gfx::drawRect(2 + bit * 16, 46, 12, 12);
    }
}

void engine::screensaver()
{
    gfx::clearRect(0, 0, 128, 64);

    for (size_t bit = 0; bit < 8; bit++)
    {
        if ((_turing_byte & (0x80 >> bit)) != 0)
            gfx::fillRect(2 + bit * 16, 32, 12, 12);
        else
            gfx::drawRect(2 + bit * 16, 32, 12, 12);
    }
}