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

#ifdef TEENSYDUINO
#include <Entropy.h>
#else
struct
{
    int32_t random() { return rand(); }
    float randomf(float min, float max) { return (float)rand() / INT32_MAX * (max - min) + min; }
} Entropy;
#endif

struct common
{
    template <int attenuverter_index = 0, class T>
    static void display(T *_this, uint8_t *buffer, int x, int y)
    {
        if (_this->src > 0)
        {
            auto m = -_this->value / 10.f * 7;
            gfx::drawRect(buffer, x + 8, y - 7, 5, 15);
            gfx::drawLine(buffer, x + 10, y, x + 10, y + m);
        }

        gfx::DrawKnob(buffer, x + 13, y, "~-/+\n", _this->param[attenuverter_index].to_uint16(), false);
        if (std::fabs(_this->attenuverter) < 0.01f)
            gfx::drawString(buffer, x + 19, y - 7, "|", 0);

        if (_this->param[attenuverter_index].flags & machine::Parameter::IS_SELECTED)
            gfx::drawString(buffer, x + 1, y - 3, ">");
    }
};

template <int channel>
struct CV : machine::ModulationSource
{
    float value = 0;
    float attenuverter = 0;

    void eeprom(std::function<void(void *, size_t)> read_write) override
    {
        read_write(&attenuverter, sizeof(attenuverter));
    }

    CV()
    {
        param[0].init(" +/-", &attenuverter, attenuverter, -1, +1);
    }

    void process() override
    {
        value = machine::get_cv(channel); // 1V/OCT : -3.5V..6.5V
        target->modulate(value * attenuverter);
    }

    void display(uint8_t *buffer, int x, int y) override
    {
        common::display(this, buffer, x, y);
    }
};

template <int channel>
struct SH : CV<channel>
{
    uint32_t last_trig = 0;
    void process() override
    {
        if (machine::get_trigger(channel) != last_trig)
        {
            last_trig = machine::get_trigger(channel);
            this->value = machine::get_cv(channel);
        }

        this->target->modulate(this->value * this->attenuverter);
    }
};

template <int channel>
struct Rand : CV<channel>
{
    uint32_t last_trig = 0;
    void process() override
    {
        if (machine::get_trigger(channel) != last_trig)
        {
            last_trig = machine::get_trigger(channel);
            this->value = Entropy.randomf(-10, 10);
        }

        this->target->modulate(this->value * this->attenuverter);
    }
};

#include "peaks/modulations/lfo.h"

template <int channel>
struct PeaksLFO : machine::ModulationSource
{
    float value = 0;
    float attenuverter = 0;

    uint16_t params_[4];
    peaks::GateFlags flags[2];

    peaks::Lfo _processor;

    void eeprom(std::function<void(void *, size_t)> read_write) override
    {
        read_write(&params_[0], sizeof(params_));
        read_write(&attenuverter, sizeof(attenuverter));
    }

    PeaksLFO()
    {
        _processor.Init();
        param[0].init("Shape", &params_[1], 0);
        param[1].init("Freq.", &params_[0], INT16_MAX);
        param[2].init(" +/-", &attenuverter, attenuverter, -1, +1);
        
        params_[2] = INT16_MAX; //Parameter
        params_[3] = INT16_MAX; //Phase
    }

    uint32_t last_trig = 0;
    void process() override
    {
        _processor.Configure(params_, peaks::CONTROL_MODE_FULL);

        if (channel >= 0 && machine::get_trigger(channel) != last_trig)
        {
            last_trig = machine::get_trigger(channel);

            flags[0] = peaks::GATE_FLAG_RISING;
            flags[1] = peaks::GATE_FLAG_FALLING;
        }
        else
        {
            flags[0] = peaks::GATE_FLAG_LOW;
            flags[1] = peaks::GATE_FLAG_LOW;
        }

        int16_t ivalue = 0;
        _processor.Process(flags, &ivalue, 1);
        value = (float)ivalue / INT16_MAX * 10.f;

        this->target->modulate(value * this->attenuverter);
    }

    void display(uint8_t *buffer, int x, int y) override
    {
        auto shape = static_cast<peaks::LfoShape>(params_[1] * peaks::LFO_SHAPE_LAST >> 16);
        switch (shape)
        {
        case peaks::LFO_SHAPE_SINE:
            param[0].name = ">Sine";
            break;
        case peaks::LFO_SHAPE_TRIANGLE:
            param[0].name = ">Triangle";
            break;
        case peaks::LFO_SHAPE_SQUARE:
            param[0].name = ">Square";
            break;
        case peaks::LFO_SHAPE_STEPS:
            param[0].name = ">Steps";
            break;
        case peaks::LFO_SHAPE_NOISE:
            param[0].name = ">Noise";
            break;
        default:
            param[0].name = ">?????";
            break;
        }

        common::display<2>(this, buffer, x, y);
    }
};

#include "braids/envelope.h"

template <int channel>
struct Envelope : machine::ModulationSource
{
    float value = 0;
    float attenuverter = 0;
    uint16_t attack = 0;
    uint16_t decay = 0;

    braids::Envelope _processor;

    void eeprom(std::function<void(void *, size_t)> read_write) override
    {
        read_write(&attack, sizeof(attack));
        read_write(&decay, sizeof(decay));
        read_write(&attenuverter, sizeof(attenuverter));
    }

    Envelope()
    {
        _processor.Init();
        param[0].init("Att.", &attack, 0);
        param[1].init("Dec.", &decay, INT16_MAX);
        param[2].init(" +/-", &attenuverter, attenuverter, -1, +1);
    }

    uint32_t last_trig = 0;
    void process() override
    {
        _processor.Update(attack >> 9, decay >> 9);

        if (channel >= 0 && machine::get_trigger(channel) != last_trig)
        {
            last_trig = machine::get_trigger(channel);
            _processor.Trigger(braids::ENV_SEGMENT_ATTACK);
        }

        value = ((float)_processor.Render() / UINT16_MAX) * 10.f;

        this->target->modulate(value * this->attenuverter);
    }

    void display(uint8_t *buffer, int x, int y) override
    {
        common::display<2>(this, buffer, x, y);
    }
};

void init_modulations()
{
    machine::add_modulation_source<CV<0>>("CV1");
    machine::add_modulation_source<CV<1>>("CV2");
    machine::add_modulation_source<CV<2>>("CV3");
    machine::add_modulation_source<CV<3>>("CV4");

    machine::add_modulation_source<SH<0>>("SH1");
    machine::add_modulation_source<SH<1>>("SH2");
    machine::add_modulation_source<SH<2>>("SH3");
    machine::add_modulation_source<SH<3>>("SH4");

    machine::add_modulation_source<Rand<0>>("RND1");
    machine::add_modulation_source<Rand<1>>("RND2");
    machine::add_modulation_source<Rand<2>>("RND3");
    machine::add_modulation_source<Rand<3>>("RND4");

    machine::add_modulation_source<Envelope<0>>("ENV1");
    machine::add_modulation_source<Envelope<1>>("ENV2");
    machine::add_modulation_source<Envelope<2>>("ENV3");
    machine::add_modulation_source<Envelope<3>>("ENV4");

    machine::add_modulation_source<PeaksLFO<-1>>("LFO");
    machine::add_modulation_source<PeaksLFO<0>>("LFO1");
    machine::add_modulation_source<PeaksLFO<1>>("LFO2");
    machine::add_modulation_source<PeaksLFO<2>>("LFO3");
    machine::add_modulation_source<PeaksLFO<3>>("LFO4");
}

MACHINE_INIT(init_modulations);