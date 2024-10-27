
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

#include <inttypes.h>
#include <algorithm>

// found here:
// https://louridas.github.io/rwa/assignments/musical-rhythms/#toussaint:2005
// https://cgm.cs.mcgill.ca/~godfried/publications/banff.pdf

// https://github.com/grindcode/rhythms/blob/main/src/lib.rs#L129
// https://github.com/computermusicdesign/euclidean-rhythm/blob/master/max-example/euclidSimple.js

void simple_pattern(uint8_t *pattern, int len, int pulses, int rotate)
{
    pulses = std::min(pulses, len);
    uint8_t bucket = 0;
    uint8_t offset = 2 + rotate;

    if (len > 0 && pulses > 0)
        offset += len / pulses - 1;

    for (uint8_t i = 0; i < len; i++)
    {
        uint8_t pos = (offset + i) % len;
        bucket += pulses;
        if (bucket >= len)
        {
            bucket -= len;
            pattern[pos] = 1;
        }
        else
        {
            pattern[pos] = 0;
        }
    }
}

// based on https://bitbucket.org/sjcastroe/bjorklunds-algorithm/src/master/Bjorklund's%20Algorithm/bjorklund.cpp

void bjorklund_pattern(uint8_t *pattern, int len, int pulses, int rotate)
{
    struct Pattern
    {
        uint32_t hits;
        size_t len;
        void append(const Pattern &second)
        {
            hits |= (second.hits << len);
            len += second.len;
        }
    };

    if (pulses > len)
        pulses = len;

    Pattern x = {0x1, 1};
    int x_amount = pulses;
    Pattern y = {0x0, 1};
    int y_amount = len - pulses;

    do
    {
        int x_temp = x_amount;
        int y_temp = y_amount;
        Pattern y_copy = y;

        if (x_temp >= y_temp)
        {
            x_amount = y_temp;
            y_amount = x_temp - y_temp;

            y = x;
        }
        else
        {
            x_amount = x_temp;
            y_amount = y_temp - x_temp;
        }

        x.append(y_copy);
    } while (x_amount > 1 && y_amount > 1);

    int k = 0;
    for (int i = 1; i <= x_amount; i++)
        for (auto j = 0; j < x.len; j++)
            pattern[(k++ + rotate) % len] = (x.hits & (0x1 << j));
    for (int i = 1; i <= y_amount; i++)
        for (auto j = 0; j < y.len; j++)
            pattern[(k++ + rotate) % len] = (y.hits & (0x1 << j));
}

#ifndef make_pattern
#define make_pattern bjorklund_pattern
#endif