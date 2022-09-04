
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

#include <inttypes.h>

// 31-bit Park-Miller-Carta Pseudo-Random Number Generator
// http://www.firstpr.com.au/dsp/rand31/

struct WhiteNoise
{
    uint32_t seed;

    WhiteNoise()
    {
        seed = (uint32_t)(void*)this;
    }

    int32_t next() // 0 to INT32_MAX
    {
        uint32_t lo, hi;
        lo = seed;

        hi = 16807 * (lo >> 16);
        lo = 16807 * (lo & 0xFFFF);
        lo += (hi & 0x7FFF) << 16;
        lo += hi >> 15;
        lo = (lo & 0x7FFFFFFF) + (lo >> 31);

        seed = lo;
        return (int32_t)lo;
    }

    float nextf(float min, float max)
    {
        return ((float)next() / INT32_MAX) * (max - min) + min;
    }
};

/** Based on "The Voss algorithm"
http://www.firstpr.com.au/dsp/pink-noise/
*/

template <int N = 8>
struct PinkNoise
{
    WhiteNoise white;

    int key = 0;
    float values[N] = {};

    float nextf(float min, float max)
    {
        int last_key = key;
        key++;
        if (key > (0x1 << N))
            key = 0;

        // Exclusive-Or previous value with current value. This gives
        // a list of bits that have changed.
        int diff = last_key ^ key;

        float sum = 0.f;
        for (int i = 0; i < N; i++)
        {
            if (diff & (1 << i))
            {
                values[i] = white.nextf(min, max) / N;
            }
            sum += values[i];
        }
        return sum;
    }
};