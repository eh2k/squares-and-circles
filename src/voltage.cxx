
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
#include "stmlib/dsp/dsp.h"

using namespace machine;

class VoltsPerOctave : public Engine
{
    char tmp[64];
    float note = 0;
    float tune = 0;
    uint8_t midi_note = 0;
    float cv = 0;

    float buffer[FRAME_BUFFER_SIZE] = {};

public:
    VoltsPerOctave() : Engine(OUT_EQ_VOLT)
    {
        param[0].init_v_oct("Tone", &note);
        param[1].init("\t", &tune, 0, -12, 12);
        param[1].step.f = 1/48.f;
        param[1].setStepValue(1.f);
    }

    void Process(const ControlFrame &frame, float **out, float **aux) override
    {
        midi_note = frame.midi.key;

        cv = frame.cv_voltage;
        cv += (frame.midi.key - 60 + (note * 12) + tune) / 12.f;

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            buffer[i] = (float)cv;
        }

        *out = buffer;
    }

    void OnDisplay(uint8_t *display) override
    {
        gfx::drawString(display, 18, 16, "Note");
        gfx::drawString(display, 82, 16, "Tune");

        sprintf(tmp, "MIDI: %d", midi_note);
        gfx::drawString(display, 0, 49, tmp);
        sprintf(tmp, "DAC: %.2fV", cv);
        gfx::drawString(display, 64, 49, tmp);

        gfx::drawEngine(display, this);
    }
};

void init_voltage()
{
    machine::add<VoltsPerOctave>(CV, "V/OCT");
}

MACHINE_INIT(init_voltage);