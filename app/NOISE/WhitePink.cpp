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

//ENGINE_NAME:NOISE/White/Pink

#include "../squares-and-circles-api.h"
#include "misc/noise.hxx"
#include <stdio.h>

float buffer[FRAME_BUFFER_SIZE];
const char *modes[2] = {">White", ">Pink"};
char mode_txt[6] = {};
PinkNoise<> pink;
float gain = 1.f;
uint8_t mode = 0;

DSP_SETUP
void setup()
{
    dsp_param_f("Level", &gain);
    dsp_param_u8(mode_txt, &mode, 0, LEN_OF(modes) - 1);

    dsp_frame_f(OUTPUT_L, buffer);
};

DSP_PROCESS
void process()
{
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        if (mode == 0)
            buffer[i] = pink.white.nextf(-1.f, 1.f) * gain;
        else
            buffer[i] = pink.nextf(-1.f, 1.f) * gain;
    }
}

GFX_DISPLAY
void display()
{
    sprintf(mode_txt, modes[mode]);
}