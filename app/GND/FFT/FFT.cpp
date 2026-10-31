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

#include "../../squares-and-circles-api.h"

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

const size_t N = 1024;
// process() fills a block with N/2 windowed sample pairs, draw() transforms it (rdft: 54k instructions). A block takes
// 21.3 ms, a draw() comes every 50 ms: up to 3 blocks wait for it while process() fills the 4th
constexpr int kBlocks = 4;
static float _blocks[kBlocks][N];
static int _fill = 0;   // the block process() fills
static size_t _pos = 0; // its sample pairs
static uint32_t _done = 0;
static volatile uint32_t _seq[kBlocks] = {};  // _done of a complete block
static volatile uint8_t _ready[kBlocks] = {}; // complete, for draw()
static volatile int8_t _busy = -1;            // the block draw() transforms
float w[N / 2];          // the cos/sin tables of rdft(): w[0...n/2-1]
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
    makewt(N >> 2, ip, w); // the tables rdft() makes on its first call
    makect(N >> 2, ip, w + (N >> 2));

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
    auto inputL = engine::inputBuffer<0>();
    auto inputR = engine::inputBuffer<1>();
    auto outputL = engine::outputBuffer<0>();
    auto outputR = engine::outputBuffer<1>();
    set(outputL, inputL, apmlitude, 0);
    set(outputR, inputR, apmlitude, 0);

    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i += 2)
    {
        if (_pos == N / 2) // complete: the pair of this step is skipped (it was the step of the FFT)
        {
            _seq[_fill] = ++_done;
            _ready[_fill] = 1;
            _fill = (_fill + 1) % kBlocks;
            if (_fill == _busy)
                _fill = (_fill + 1) % kBlocks;
            _ready[_fill] = 0; // the oldest block, draw() didn't get to it
            _pos = 0;
            continue;
        }
        float h = (float)window[_pos * 2];
        float h2 = (float)window[_pos * 2 + 1];
        _blocks[_fill][_pos * 2] = outputL[i] * h / sq_n;
        _blocks[_fill][_pos * 2 + 1] = outputL[i + 1] * h2 / sq_n;
        _pos++;
    }
}

// The spectra of the blocks complete, the oldest first, into magnitudes
static void transform()
{
    for (int i = 0; i < kBlocks; i++)
    {
        int b = -1;
        for (int k = 0; k < kBlocks; k++)
            if (_ready[k] && (b < 0 || _seq[k] < _seq[b]))
                b = k;
        if (b < 0)
            break;
        _busy = b;
        __asm__ volatile("" ::: "memory");
        if (_ready[b]) // process() didn't take it in the meantime
        {
            float *buff = _blocks[b];
            // rdft(N, 1, buff, ip, w) with the tables of setup()
            bitrv2(N, ip + 2, buff);
            cftfsub(N, buff, w);
            rftfsub(N, buff, N >> 2, w + (N >> 2));
            const float xi = buff[0] - buff[1];
            buff[0] += buff[1];
            buff[1] = xi;

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
            __asm__ volatile("" ::: "memory");
            _ready[b] = 0;
        }
        _busy = -1;
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
    transform();

    float logN2 = logf(N / 2);

    int bands = 16;
    if (band == 1)
        bands = 32;
    else if (band == 2)
        bands = 64;

    const bool fresh = count > 0; // no FFT block since the last draw: keep the levels (fft_read() would be 0/0)
    int to = int(expf(logN2 * 0 / bands));
    for (int b = 0; b < bands; b++)
    {
        int fr = to;
        to = int(expf(logN2 * (b + 1) / bands));

        if (level2[b] > 0)
            level2[b] -= 0.1f;
        else
            level2[b] = 0;

        if (fresh)
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

    if (fresh)
    {
        for (uint32_t i = 0; i < (N / 2); i++)
            magnitudes[i] = 0;
        count = 0;
    }
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