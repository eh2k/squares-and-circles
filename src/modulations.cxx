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
#include "stmlib/utils/random.h"

struct ModulationBase : machine::ModulationSource
{
    float value;
    float attenuverter = 0;

    void display(uint8_t *buffer, int x, int y) override
    {
        if (src > 0)
        {
            gfx::drawRect(buffer, x + 1, y + 32, 63, 6);
            auto m = value / 10.f * 32;

            gfx::drawLine(buffer, x + 32, y + 34, x + 32 + m, y + 34);
            gfx::drawLine(buffer, x + 32, y + 35, x + 32 + m, y + 35);
        }

        x += 20;

        size_t attenuverter_index = 0;
        while (this->param[attenuverter_index].value != &attenuverter && attenuverter_index < LEN_OF(param))
            attenuverter_index++;

        bool sel = this->param[attenuverter_index].flags & machine::Parameter::IS_SELECTED;

        gfx::DrawKnob(buffer, x + 30, y + 7, sel ? "~" : " ", this->param[attenuverter_index].to_uint16(), sel);
        gfx::drawString(buffer, x + 18, y - 3, "-  +", 1);
        // if (std::fabs(attenuverter) < 0.01f)
        //     gfx::drawString(buffer, x + 19, y - 7, "|", 0);

        x -= 24;

        ModulationSource::display(buffer, x, y + 2);
    }
};

struct CV : ModulationBase
{
    uint8_t cv_channel = 0;
    uint8_t tr_channel = 0;

    void eeprom(std::function<void(void *, size_t)> read_write) override
    {
        read_write(&cv_channel, sizeof(cv_channel));
        read_write(&attenuverter, sizeof(attenuverter));
    }

    CV()
    {
        param[0].init("SRC", &cv_channel, cv_channel, 0, machine::get_io_info(1) - 1);
        param[0].print_value = [&](char *tmp)
        {
            machine::get_io_info(1, cv_channel, tmp);
        };
        param[1].init("OP", &tr_channel, tr_channel, 0, machine::get_io_info(0));
        param[1].print_value = [&](char *tmp)
        {
            if (tr_channel == 0)
                sprintf(tmp, "Thru");
            else if (tr_channel <= 4)
                sprintf(tmp, "S&H-T%d", tr_channel);
            else if (tr_channel <= 8)
                sprintf(tmp, "T&H-T%d", tr_channel - 4);
            else
                machine::get_io_info(0, tr_channel - 1, tmp);
        };

        param[2].init(".", &attenuverter, attenuverter, -1, +1);
    }

    void process(machine::Parameter &target, machine::ControlFrame &frame) override
    {
        if (tr_channel == 0 || (tr_channel <= 4 && machine::get_trigger(tr_channel - 1)) || (tr_channel <= 8 && machine::get_gate(tr_channel - 4)))
            value = machine::get_cv(cv_channel);

        float a = attenuverter;

        if(!(target.flags & machine::Parameter::IS_V_OCT))
            a *= 3;  // ADC is -3...6V ...faktor 3 for non V/OCT 

        target.modulate(value * a);
    }
};

struct RND : ModulationBase
{
    uint8_t tr_channel = 0;

    float randomf(float min, float max)
    {
        return stmlib::Random::GetFloat() * (max - min) + min;
    }

    void eeprom(std::function<void(void *, size_t)> read_write) override
    {
        read_write(&tr_channel, sizeof(tr_channel));
        read_write(&attenuverter, sizeof(attenuverter));
    }

    RND()
    {
        stmlib::Random::Seed(reinterpret_cast<uint32_t>(this));

        param[0].init("TRIG", &tr_channel, tr_channel, 0, machine::get_io_info(0));
        param[0].print_value = [&](char *tmp)
        {
            if (tr_channel == 0)
                sprintf(tmp, "!");
            else
                machine::get_io_info(0, tr_channel - 1, tmp);
        };
        param[1].init(".", &attenuverter, attenuverter, -1, +1);
    }

    void process(machine::Parameter &target, machine::ControlFrame &frame) override
    {
        if (tr_channel == 0)
        {
            if (frame.trigger)
                this->value = randomf(-10, 10);
        }
        else
        {
            if (machine::get_trigger(tr_channel - 1))
                this->value = randomf(-10, 10);
        }

        target.modulate(this->value * this->attenuverter);
    }
};

#include "peaks/modulations/lfo.h"

#include "peaks/modulations/multistage_envelope.h"

#include "braids/envelope.h"

struct Envelope : ModulationBase
{
    uint8_t attack = 0;
    uint8_t decay = 0;
    uint8_t tr_channel = 0;

    braids::Envelope _processor;

    void eeprom(std::function<void(void *, size_t)> read_write) override
    {
        read_write(&tr_channel, sizeof(tr_channel));
        read_write(&attack, sizeof(attack));
        read_write(&decay, sizeof(decay));
        read_write(&attenuverter, sizeof(attenuverter));
    }

    Envelope()
    {
        _processor.Init();
        param[0].init("TRIG", &tr_channel, tr_channel, 0, machine::get_io_info(0));
        param[0].print_value = [&](char *tmp)
        {
            if (tr_channel == 0)
                sprintf(tmp, "!");
            else
                machine::get_io_info(0, tr_channel - 1, tmp);
        };
        param[1].init("Att.", &attack, 0);
        param[2].init("Dec.", &decay);
        param[3].init(".", &attenuverter, attenuverter, -1, +1);
    }

    void process(machine::Parameter &target, machine::ControlFrame &frame) override
    {
        _processor.Update(attack >> 1, decay >> 1);

        bool trigger = false;
        if (tr_channel == 0)
        {
            trigger = frame.trigger;
        }
        else
        {
            trigger = machine::get_trigger(tr_channel - 1);
        }

        if (trigger)
            _processor.Trigger(braids::ENV_SEGMENT_ATTACK);

        value = ((float)_processor.Render() / UINT16_MAX) * 10.f;

        target.modulate(value * this->attenuverter);
    }
};

template <peaks::LfoShape mode>
struct LFO : ModulationBase
{
    uint8_t tr_channel = 0;
    uint8_t shape = mode;

    uint16_t rate;
    peaks::Lfo _processor;

    void eeprom(std::function<void(void *, size_t)> read_write) override
    {
        read_write(&rate, sizeof(rate));
        read_write(&attenuverter, sizeof(attenuverter));
    }

    LFO()
    {
        _processor.Init();
        _processor.set_level(40960);
        _processor.set_rate(rate);
        _processor.set_parameter(INT16_MAX - 32768);
        _processor.set_reset_phase(INT16_MAX - 32768);

        param[0].init("TRIG", &tr_channel, tr_channel, 0, 1 + machine::get_io_info(0));
        param[0].print_value = [&](char *tmp)
        {
            if (tr_channel == 0)
                sprintf(tmp, "-");
            else if (tr_channel - 1 == 0)
                sprintf(tmp, "!");
            else
                machine::get_io_info(0, tr_channel - 2, tmp);
        };

        if (mode == peaks::LFO_SHAPE_LAST)
        {
            _processor.set_shape(peaks::LFO_SHAPE_SINE);
            param[1].init("Shape", &shape, peaks::LFO_SHAPE_SINE, 0, peaks::LFO_SHAPE_LAST - 1);
            param[1].step2 = param[1].step;
            param[2].init("Freq.", &rate, INT16_MAX);
            param[3].init(".", &attenuverter, attenuverter, -1, +1);
        }
        else
        {
            _processor.set_shape((peaks::LfoShape)shape);
            param[1].init("Freq.", &rate, INT16_MAX);
            param[2].init(".", &attenuverter, attenuverter, -1, +1);
        }
    }

    uint32_t last_trig = 0;
    void process(machine::Parameter &target, machine::ControlFrame &frame) override
    {
        _processor.set_shape((peaks::LfoShape)shape);
        _processor.set_rate(rate);

        peaks::GateFlags flags[] = {peaks::GATE_FLAG_LOW, peaks::GATE_FLAG_LOW};

        if (tr_channel == 0)
        {
            // Free LFO
        }
        else if (tr_channel == 1)
        {
            if (frame.trigger)
            {
                flags[0] = peaks::GATE_FLAG_RISING;
                flags[1] = peaks::GATE_FLAG_FALLING;
            }
        }
        else
        {
            if (machine::get_trigger(tr_channel - 2))
            {
                flags[0] = peaks::GATE_FLAG_RISING;
                flags[1] = peaks::GATE_FLAG_FALLING;
            }
        }

        int16_t ivalue = 0;
        _processor.Process(flags, &ivalue, 1);
        value = (float)ivalue / INT16_MAX * 10.f;
        target.modulate(value * this->attenuverter);
    }

    void display(uint8_t *buffer, int x, int y) override
    {
        if (mode == peaks::LFO_SHAPE_LAST)
        {
            switch (shape)
            {
            case peaks::LFO_SHAPE_SINE:
                param[1].name = ">Sine";
                break;
            case peaks::LFO_SHAPE_TRIANGLE:
                param[1].name = ">Triangle";
                break;
            case peaks::LFO_SHAPE_SQUARE:
                param[1].name = ">Square";
                break;
            case peaks::LFO_SHAPE_STEPS:
                param[1].name = ">Steps";
                break;
            case peaks::LFO_SHAPE_NOISE:
                param[1].name = ">Noise";
                break;
            default:
                param[1].name = ">?????";
                break;
            }

            ModulationBase::display(buffer, x, y);
        }
    }
};

void init_modulations()
{
    machine::add_modulation_source<CV>("CV");
    machine::add_modulation_source<RND>("RND");
    machine::add_modulation_source<Envelope>("ENV");
    machine::add_modulation_source<LFO<peaks::LFO_SHAPE_LAST>>("LFO");
}

MACHINE_INIT(init_modulations);