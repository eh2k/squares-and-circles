
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
    int16_t oct = 0;
    int16_t note = 0;
    uint8_t midi_note = 0;
    float cv = 0;

    float buffer[FRAME_BUFFER_SIZE] = {};

public:
    VoltsPerOctave() : Engine(OUT_EQ_VOLT)
    {
    }

    void Process(const ControlFrame &frame, float **out, float **aux) override
    {
        midi_note = frame.midi.key;

        cv = frame.cv_voltage;
        cv += (frame.midi.key - 60 + (oct * 12) + note) / 12.f;

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            buffer[i] = (float)cv;
        }

        *out = buffer;
    }

    void SetParams(const uint16_t *params) override
    {
        oct = (int16_t)(params[0] - INT16_MAX) >> 12;
        note = (int16_t)(params[1] - INT16_MAX) >> 11;
    }

    const char **GetParams(uint16_t *values) override
    {
        static char tmp0[20];
        static char tmp1[20];
        static const char *names[]{tmp0, tmp1, nullptr};
        sprintf(tmp0, "\t%+d", oct);
        sprintf(tmp1, "\t%+d", note);
        values[0] = (oct << 12) + INT16_MAX;
        values[1] = (note << 11) + INT16_MAX;
        return names;
    }

    void OnEncoder(uint8_t param_index, int16_t inc, bool pressed) override
    {
        auto &select = param_index == 0 ? oct : note;
        if (inc > 0)
            select++;
        else
            select--;

        if (note > 12)
        {
            oct++;
            note = 1;
        }
        else if (note < -12)
        {
            oct--;
            note = -1;
        }

        CONSTRAIN(note, -12, 12);
        CONSTRAIN(oct, -3, 5);
    }

    void OnDisplay(uint8_t *display) override
    {
        gfx::drawString(display, 8, 16, "Octave");
        gfx::drawString(display, 78, 16, "Note");

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