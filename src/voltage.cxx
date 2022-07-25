
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

#include "machine.h"

using namespace machine;

class VoltsPerOctave : public Engine
{
    char tmp[64];
    uint8_t note = DEFAULT_NOTE;
    uint8_t tune = 32;
    int32_t cv = 0;

public:
    VoltsPerOctave() : Engine(OUT_EQ_VOLT | VOCT_INPUT)
    {
        param[0].init_v_oct("Tone", &note);
        param[1].init("\t", &tune, tune, 0, 64);
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        cv = frame.cv_voltage_ +
             (((int)note - DEFAULT_NOTE) * machine::PITCH_PER_OCTAVE / 12) +
             (((int)tune - 32) << 3);

        of.push(&cv, 1);
    }

    void onDisplay(uint8_t *display) override
    {
        gfx::drawString(display, 18, 16, "Note");
        gfx::drawString(display, 82, 16, "Tune");

        sprintf(tmp, "DAC: %.2fV", ((float)cv / machine::PITCH_PER_OCTAVE));
        gfx::drawString(display, 64, 53, tmp, 0);

        gfx::drawEngine(display, this);
    }
};

void init_voltage()
{
    machine::add<VoltsPerOctave>(CV, "V/OCT");
}

MACHINE_INIT(init_voltage);