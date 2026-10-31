
// Copyright (C)2023 - Eduard Heidt
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

// For details see:
// https://www.nesdev.org/wiki/APU_Noise

#include <math.h>

constexpr float clock_NTSC = 1789773.f;
constexpr float clock_PAL = 1662607.f;

class NESNoise
{
public:
    NESNoise()
    {
        mode_bit = 1;
        period_index = 8;
        shift_reg = 1 | (uint32_t)this;
    }

    void init(uint16_t shift_reg = 1, uint8_t mode_bit = 1, uint8_t period_index = 8)
    {
        this->mode_bit = mode_bit;
        this->period_index = period_index;
        this->shift_reg = shift_reg;
    }

    template <typename T, uint32_t samplerate = 48000, bool ntsc = true>
    T generateSample(T level)
    {
        // APU ticks per sample (37.29 at 48 kHz) as the float of before. It is a multiple of 2^-18 below 64, and so
        // was every value of the float sampling: in fixed point with 18 fractional bits it adds up exactly the same
        constexpr float ticks = (ntsc ? clock_NTSC : clock_PAL) / samplerate;
        constexpr int32_t ticks_q18 = static_cast<int32_t>(ticks * 262144.f);
        static_assert(ticks_q18 == ticks * 262144.f && ticks < 64.f, "ticks per sample: a multiple of 2^-18 below 64");

        // The APU ticks until sampling < 0: the n-th tick returns the sample, the ones before clock the timer on
        // the even ticks. All n at once instead of tick by tick: the LFSR shifts only at the timer's reloads
        const uint32_t n = (sampling >> 18) + 1;    // sampling >= 0
        uint32_t steps = (n - 1 + (tick & 1)) >> 1; // even ticks among tick + 1 ... tick + n - 1
        tick += n;
        sampling += ticks_q18 - static_cast<int32_t>(n << 18);

        while (steps > timer_counter)
        {
            steps -= timer_counter + 1u;

            if (ntsc)
                timer_counter = periodLookup_NTSC[period_index % 16];
            else
                timer_counter = periodLookup_PAL[period_index % 16];

            uint32_t feedback = 1 & ((shift_reg & 0x0001) ^ ((mode_bit == 0) ? ((shift_reg >> 6) & 0x0001) : ((shift_reg >> 1) & 0x0001)));
            shift_reg >>= 1;
            shift_reg |= (feedback << 14);
        }
        timer_counter -= steps;

        return (shift_reg & 0x1) ? 0 : level;
    }

    uint8_t mode_bit;
    uint8_t period_index;
    uint16_t shift_reg;

private:
    uint16_t timer_counter = 0;
    int32_t sampling = 0; // APU ticks to the next sample, 18 fractional bits
    uint32_t tick = 0;
    const uint16_t periodLookup_NTSC[16] = {4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};
    const uint16_t periodLookup_PAL[16] = {4, 8, 14, 30, 60, 88, 118, 148, 188, 236, 354, 472, 708, 944, 1890, 3778};
};