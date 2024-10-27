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

#include "../squares-and-circles-api.h"
#include "../lib/stmlib/utils/ring_buffer.h"

extern "C"
{
#include "../lib/fft/fft4g.h"
#include "../lib/fft/fft4g.c"
}

#include <algorithm>
#include <math.h>

static float apmlitude = 1.f;
static int32_t band = 0;
static const char *sbands[3] = {" 16", " 32", " 64"};

struct Complex
{
    float re;
    float im;
};

const size_t N = 1024;
stmlib::RingBuffer<Complex, N> spectrum;
float w[N * 2];
constexpr int sq_n = 32; // sqrt(N);
int ip[4 + sq_n];        // new int[(int)ceil(4.0 + sqrt((real_t)N))];
float magnitudes[N / 2] = {};
float window[N];
int count = 0;

void engine::setup()
{
    apmlitude = 1.f;
    engine::addParam("_Scale", &apmlitude, 0.f, 2.f);
    engine::addParam("_Bands", &band, 0, 2, sbands);
    engine::setMode(ENGINE_MODE_COMPACT);
    ip[0] = 0; // indicate that re-initialization is necesarry
    spectrum.Init();

    for (int n = 0; n < N; n++)
        window[n] = 0.5f * (1.f - cosf((2.f * M_PI * n) / N));
}

void set(float *target, const float *src, float amp, float offset)
{
    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        target[i] = src[i] * amp + offset;
}

void engine::process()
{
    const float m_offset = log10f(N);

    auto inputL = engine::inputBuffer<0>();
    auto outputL = engine::outputBuffer<0>();
    set(outputL, inputL, apmlitude, 0);

    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i += 2)
    {
        if (spectrum.readable() == (N / 2))
        {
            float *buff = const_cast<float *>(&spectrum.ImmediateReadPtr(0)->re);
            rdft(N, 1, buff, ip, w);

            // for some reason, this routine returns the second half of the spectrum (the complex conjugate
            // values of the desired first half), so we need to take the complex conjugates:
            for (int n = 3; n < N; n += 2) // start at n=3 (imaginary part of the first bin after DC)
                buff[n] = -buff[n];

            magnitudes[0] = buff[0];

            for (int k = 1; k < N / 2; k++)
            {
                float re = buff[2 * k];
                float im = buff[2 * k + 1];
                magnitudes[k] += sqrtf(re * re + im * im);
                // auto db =  sqrtf(re * re + im * im);
                // auto db =  20 * log10f(db);
                // auto db = logf(re * re + im * im);
                // magnitudes[k] = db;
            }
            count++;
            spectrum.Flush();
        }
        else
        {
            float h = (float)window[spectrum.readable() * 2];
            float h2 = (float)window[spectrum.readable() * 2 + 1];
            Complex v = {
                outputL[i] * h / sq_n,
                outputL[i + 1] * h2 / sq_n,
            };
            spectrum.Overwrite(v);
        }
    }
}

float fft_read(int s, int e)
{
    float sum = 0;
    while (s <= e)
        sum += magnitudes[s++];

    return (sum / count);
}

float level2[64];
float level[64];

void draw_spectrum(int yy, int hh)
{
    float logN2 = logf(N / 2);

    int bands = 16;
    if (band == 1)
        bands = 32;
    else if (band == 2)
        bands = 64;

    int to = int(expf(logN2 * 0 / bands));
    for (int b = 0; b < bands; b++)
    {
        int fr = to;
        to = int(expf(logN2 * (b + 1) / bands));

        if (level2[b] > 0)
            level2[b] -= 0.1f;
        else
            level2[b] = 0;

        level[b] = 10.f / hh + log10f(fft_read(fr, to));
        level2[b] = std::max(level[b], level2[b]);
    }

    int w = (128 / bands);
    for (size_t i = 0; i < bands; i++)
    {
        auto y = level[i] * hh;
        gfx::fillRect(i * w, yy - y, w - 1, y);

        y = level2[i] * hh;
        gfx::fillRect(i * w, yy - 1 - y, w - 1, 1);
    }

    // uint32_t ylim = 58;
    // for (uint32_t i = 0; i < 128; i++)
    // {
    //     auto y = 40 + -(magnitudes[i] / 2);
    //     gfx::drawLine(i, 58, i, y);
    // };

    for (uint32_t i = 0; i < (N / 2); i++)
        magnitudes[i] = 0;
    count = 0;
}

void engine::draw()
{
    draw_spectrum(60, 32);
}
void engine::screensaver()
{
    gfx::clearRect(0, 0, 128, 64);
    draw_spectrum(64, 48);
}