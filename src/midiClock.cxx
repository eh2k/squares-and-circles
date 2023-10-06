
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

class MidiClock : public Engine
{
    uint8_t ppqn = 0;
    uint8_t offset = 0;
    uint8_t impulse = 100;
    int count_down = 0;

    int16_t buffer[FRAME_BUFFER_SIZE] = {};

public:
    MidiClock() : Engine(SEQUENCER_ENGINE | OUT_EQ_VOLT)
    {
        // 24, 8 or 4 ppqn
        param[0].init("ppqn", &ppqn, 0, 0, 2);
        param[0].print_value = [&](char *tmp)
        {
            if (ppqn == 0)
                sprintf(tmp, "4ppqn");
            else if (ppqn == 1)
                sprintf(tmp, "8ppqn");
            else
                sprintf(tmp, "24ppqn");
        };

        param[1].init("Impulse", &impulse, 10, 1, 100);
        param[1].step.i = 1;
        param[1].step2.i = 5;
        param[1].print_value = [&](char *tmp)
        {
            sprintf(tmp, "Impulse\n%dms", impulse);
        };

        param[2].init("Offset", &offset, 0);
        param[2].print_value = [&](char *tmp)
        {
            sprintf(tmp, "Delay\n%dms", offset);
        };
    }

    uint32_t sixteens = 0;
    uint32_t next = -1;
    uint32_t last_t = 0;

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        if (frame.clock)
            last_t = frame.t;

        uint32_t div = ppqn == 0 ? 6 : (ppqn == 1 ? 3 : 1);

        if ((frame.clock % div) == 1 || (ppqn > 1 && frame.clock))
        {
            next = frame.t + (offset * 2);
        }

        if (frame.t == next)
        {
            if (frame.t - last_t > 1000)
            {
                // No external clock - so diy and sync with trigger
                auto one_div_bpm = 100 * machine::get_bpm() / (25 / 24);
                auto t_per_beat = 60 * one_div_bpm;
                auto t_24 = t_per_beat / 24 * div;
                auto samples_per_frame = machine::SAMPLE_RATE / machine::FRAME_BUFFER_SIZE;
                next = frame.t + (t_24 * samples_per_frame);
            }
            else
                next = -1;

            count_down = (impulse * 2);
        }

        int16_t a = count_down-- > 0 ? INT16_MAX : 0;
        of.push<int16_t>(&a, 1);
    }

    void display() override
    {
        gfx::drawEngine(this);

        char tmp[20];
        int bpm = machine::get_bpm();
        int bpm2 = ((bpm % 100) / 10);
        sprintf(tmp, " %d.%dbpm", bpm / 100, bpm2);
        gfx::drawString(58, 1, tmp, 0);
    }
};

void init_midi_clock()
{
    machine::add<MidiClock>("MIDI", "Clock");
}

MACHINE_INIT(init_midi_clock);