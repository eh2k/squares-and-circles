
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
        while (true)
        {
            tick++;
            sampling -= 1.f;

            if (sampling < 0)
            {
                if (ntsc)
                    sampling += (clock_NTSC / samplerate);
                else
                    sampling += (clock_PAL / samplerate);

                return (shift_reg & 0x1) ? 0 : level;
            }

            if ((tick % 2) == 0)
            {
                if (timer_counter > 0)
                {
                    timer_counter--;
                }
                else
                {
                    if (ntsc)
                        timer_counter = periodLookup_NTSC[period_index % 16];
                    else
                        timer_counter = periodLookup_PAL[period_index % 16];

                    uint32_t feedback = 1 & ((shift_reg & 0x0001) ^ ((mode_bit == 0) ? ((shift_reg >> 6) & 0x0001) : ((shift_reg >> 1) & 0x0001)));
                    shift_reg >>= 1;
                    shift_reg |= (feedback << 14);
                }
            }
        }
        return 0;
    }

    uint8_t mode_bit;
    uint8_t period_index;
    uint16_t shift_reg;

private:
    uint16_t timer_counter = 0;
    float sampling = 0;
    uint32_t tick = 0;
    const uint16_t periodLookup_NTSC[16] = {4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};
    const uint16_t periodLookup_PAL[16] = {4, 8, 14, 30, 60, 88, 118, 148, 188, 236, 354, 472, 708, 944, 1890, 3778};
};