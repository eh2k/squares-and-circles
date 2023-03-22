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

#include "machine.h"
#include "misc/analyze_fft.hxx"

struct ScreensaverSC : machine::Screensaver
{
    float peak[4];

    void display() override
    {
        memset(gfx::display_buffer, 0, 128 * 64 / 8);
        for (size_t i = 0; i < LEN_OF(peak); i++)
        {
            int s = 2 + std::min(5.f, 5 * peak[i]);

            int yy = std::min(4, (int)(peak[i] * 4));
            for (int y = 0; y < yy; y++)
            {
                if (peak[i] < 3)
                {
                    gfx::drawCircle(8 + (i * 32), 72 - (y * 16), s);
                    gfx::drawCircle(24 + (i * 32), 72 - (y * 16), s);
                }
                else
                {
                    gfx::fillCircle(8 + (i * 32), 72 - (y * 16), s);
                    gfx::fillCircle(24 + (i * 32), 72 - (y * 16), s);
                }
            }

            if (peak[i] < 2)
            {
                gfx::drawRect(-s + 8 + (i * 32), -s + 72 - (yy * 16), s * 2, s * 2);
                gfx::drawRect(-s + 24 + (i * 32), -s + 72 - (yy * 16), s * 2, s * 2);
            }
            else
            {
                gfx::fillRect(-s + 8 + (i * 32), -s + 72 - (yy * 16), s * 2, s * 2);
                gfx::fillRect(-s + 24 + (i * 32), -s + 72 - (yy * 16), s * 2, s * 2);
            }

            if (peak[i] > 0)
                peak[i] -= 0.2f;
        }
    }

    void process(const machine::OutputFrame *frames) override
    {
        for (size_t channel = 0; channel < 4; channel++)
            for (size_t i = 0; i < 4; i++)
            {
                if (frames[channel].out)
                    peak[channel] = std::max(peak[channel], fabsf(frames[channel].out[i]));
            }
    }
};

struct ScreensaverFFT : machine::Screensaver
{
    AnalyzeFFT<1024> fft;
    int16_t fft_buff[machine::FRAME_BUFFER_SIZE];

    void display() override
    {
        memset(gfx::display_buffer, 0, 128 * 64 / 8);
        fft.display(gfx::display_buffer, 0);
    }

    void push_data(uint8_t channel, float *buffer, size_t size)
    {
        if (channel == 0)
            memset(fft_buff, 0, sizeof(fft_buff));

        for (size_t i = 0; i < size; i++)
        {
            fft_buff[i] += INT16_MAX * buffer[i];
        }

        fft.process(fft_buff, size);
    }
};

void init_screensaver()
{
    machine::screensaver = new ScreensaverSC();
}

MACHINE_INIT(init_screensaver);