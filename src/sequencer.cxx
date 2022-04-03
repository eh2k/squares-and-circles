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
#include <cmath>

template <int channel>
struct Sequencer : machine::ModulationSource
{
    uint32_t last_trig = 0;

    uint8_t step = 0;
    float seq[8];
    uint8_t pos = 0;
    uint8_t len = 8;

    void eeprom(std::function<void(void *, size_t)> read_write) override
    {
        read_write(&seq, sizeof(seq));
        read_write(&len, sizeof(len));
    }

    Sequencer()
    {
        param[0].init(" pos", &step, step, 0, 8);
        param[0].value_changed = [&]()
        {
            step = std::min(step, len);

            if (step == len)
                param[1].init(" len", &len, len, 1, 8);
            else
                param[1].init(" mod", &seq[step], seq[step], -1, 1);
        };

        param[1].init(" mod", &seq[step], seq[step], -1, 1);
        param[1].value_changed = [&]()
        {
            if (param[1].value == &len)
            {
                step = len;
            }
        };

        param[1].encoder_pressed = [&](bool hold)
        {
            if (step == len)
            {
                param[0].from_uint16(0);
                return false;
            }
            else
            {
                param[0].apply_encoder(1, false);
                return true; // handled!, do not change selection !!
            }
        };
    }

    void process() override
    {
        if (step < len && param[1].value != &seq[step])
        {
            if (this->target->flags & machine::Parameter::IS_V_OCT)
            {
                param[1].init_v_oct(" mod", &seq[step]);
                param[1].step.f = target->step2.f;
            }
            else
            {
                param[1].init(" mod", &seq[step], seq[step], -1.f, 1.f);
                param[1].step.f = target->step2.f;
            }
        }

        if (machine::get_trigger(channel) != last_trig)
        {
            last_trig = machine::get_trigger(channel);
            ++pos;
            pos %= len;
        }


        this->target->modulate(seq[pos] * 10);
    }

    void display(uint8_t *buffer, int x, int y) override
    {
        int h = 10;
        y -= 4;

        float fh = (h - 2);
        if (this->target->flags & machine::Parameter::IS_V_OCT)
        {
            fh /= 4;
        }

        for (int i = 0; i <= len; i++)
        {
            int m = -seq[i] * fh;
            // if (i == step)

            if (i == step && param[1].flags & machine::Parameter::IS_SELECTED)
            {
                int xx = x + 11 + (i * 6);
                for (int yy = 2 + y - h; yy < y + h; yy += 2)
                    gfx::drawPixel(buffer, xx, yy);
            }

            if (i < len)
            {
                gfx::drawRect(buffer, x + 8 + (i * 6), y - h, 7, 1 + h * 2);

                if (i == pos)
                {
                    if (m > 0)
                        gfx::drawRect(buffer, x + 10 + (i * 6), y, 3, m);
                    else
                        gfx::drawRect(buffer, x + 10 + (i * 6), y + m, 3, -m);
                }
                else
                {
                    for (int xx = x; xx < x + 3; xx++)
                        gfx::drawLine(buffer, xx + 10 + (i * 6), y, xx + 10 + (i * 6), y + m);
                }
            }
        }

        if (param[0].flags & machine::Parameter::IS_SELECTED)
        {
            int i = step;
            int xx = x + 11 + (i * 6);

            gfx::drawLine(buffer, xx, y - 2 - h, xx - 3, y - 5 - h);
            gfx::drawLine(buffer, xx, y - 2 - h, xx + 3, y - 5 - h);
        }

        if (param[1].flags & machine::Parameter::IS_SELECTED)
        {
            gfx::drawString(buffer, x + 1, y - 3, ">");
        }
    }
};

void init_sequencer()
{
    // machine::add_modulation_source<Sequencer<0>>("SEQ1");
    // machine::add_modulation_source<Sequencer<1>>("SEQ2");
    // machine::add_modulation_source<Sequencer<2>>("SEQ3");
    // machine::add_modulation_source<Sequencer<3>>("SEQ4");
}

MACHINE_INIT(init_sequencer);