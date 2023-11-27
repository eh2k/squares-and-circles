
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

namespace gfx
{
    void drawEngineWithScope(machine::Engine *engine, int8_t scope[128], int i, int y)
    {
        for (int x = 0; x < 127; x++)
        {
            if (x % 3 == 0)
                gfx::drawPixel(x, y);
            gfx::drawLine(x, y - scope[(i + x) % 128], x + 1, y - scope[(1 + i + x) % 128]);
        }

        gfx::drawEngineCompact(engine);
    }

    void push_scope(int8_t scope[128], uint8_t &i, int8_t y)
    {
        scope[i++ % 128] = y;
        if (i > 128)
            i = 0;
    }
}

using namespace machine;

class VoltsPerOctave : public Engine
{
    char tmp[64];
    uint16_t note = DEFAULT_NOTE * machine::PITCH_PER_OCTAVE / 12;
    uint8_t tune = 128;
    int32_t cv0 = 0;
    int32_t cv = 0;
    float glide = 0;

public:
    VoltsPerOctave() : Engine(OUT_EQ_VOLT | VOCT_INPUT)
    {
        param[0].init_v_oct("Tone", &note);
        param[1].init("Fine", &tune, tune, 0, 254);
        param[2].init("Slew", &glide, 0, 0, 0.5f);
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        cv0 = frame.qz_voltage(this->io, (machine::PITCH_PER_OCTAVE * 2) + ((int)note - (DEFAULT_NOTE * machine::PITCH_PER_OCTAVE / 12))) +
              (((int)tune - 128) << 2);

        ONE_POLE(cv, cv0, powf(1 - glide, 10));

        of.push_voltage(&cv, 1);

        if ((frame.t % 50) == 0)
            gfx::push_scope(scope, i, (float)cv / machine::PITCH_PER_OCTAVE * 4);
    }

    int8_t scope[128] = {};
    uint8_t i = 0;
    void display() override
    {
        sprintf(tmp, "OUT:%.2fV", ((float)cv / machine::PITCH_PER_OCTAVE));
        gfx::drawString(4 + 64, 32, tmp, 0);
        gfx::drawEngineWithScope(this, scope, i, 50);
    }
};

class LFOEngine : public Engine
{
    Parameter output;
    ModulationSource *_mod = nullptr;
    float cv = 0;

public:
    LFOEngine() : Engine(OUT_EQ_VOLT | TRIGGER_INPUT)
    {
        _mod = machine::create_modulation("LFO");
        if (_mod)
        {
            *_mod->param[0].value.u8p = 1; // set trigger source
            param[0] = _mod->param[2];     // Freq
            param[1] = _mod->param[1];     // Shape
            // param[2] = _mod->param[3];     // Attenuverter
            *_mod->param[3].value.fp = 0.8f; // param[2].name = "+-";

            output.init(".", &cv, 0, -1.f, 1.f);
        }
    }

    ~LFOEngine() override
    {
        machine::mfree(_mod);
    }

    int8_t scope[128] = {};
    uint8_t i = 0;
    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        cv = 0.f;
        _mod->process(output, (ControlFrame &)frame);

        int32_t v = 5.f * cv * machine::PITCH_PER_OCTAVE;
        of.push_voltage(&v, 1);

        if ((frame.t % 50) == 0)
            gfx::push_scope(scope, i, cv * 10);
    }

    void display() override
    {
        gfx::drawEngineWithScope(this, scope, i, 44);
    }
};

class ADEnvelope : public Engine
{
    Parameter output;
    ModulationSource *_mod = nullptr;
    float cv = 0;

public:
    ADEnvelope() : Engine(OUT_EQ_VOLT | TRIGGER_INPUT)
    {
        _mod = machine::create_modulation("ENV");
        if (_mod)
        {
            *_mod->param[0].value.u8p = 0; // set trigger source
            param[0] = _mod->param[1];     // Attack
            param[0].name = "Attack";
            param[1] = _mod->param[2]; // Decay
            param[1].name = "Decay";
            *_mod->param[3].value.fp = 0.8f; // Attenuverter param[2].name = "+-";
            output.init(".", &cv, 0, -1.f, 1.f);
        }
    }

    ~ADEnvelope() override
    {
        machine::mfree(_mod);
    }

    int8_t scope[128] = {};
    float y = 0;
    uint8_t i = 0;
    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        cv = 0.f;
        _mod->process(output, (ControlFrame &)frame);

        int32_t v = 5.f * cv * machine::PITCH_PER_OCTAVE;
        of.push_voltage(&v, 1);

        if ((frame.t % 50) == 0)
        {
            gfx::push_scope(scope, i, y * 20);
            y = 0;
        }
        else
            y = std::max(y, cv);
    }

    void display() override
    {
        gfx::drawEngineWithScope(this, scope, i, 54);
    }
};

class EFEngine : public Engine
{
    Parameter output;
    ModulationSource *_mod = nullptr;
    float cv = 0;

public:
    EFEngine() : Engine(OUT_EQ_VOLT | AUDIO_PROCESSOR_MONO)
    {
        _mod = machine::create_modulation("EF");
        if (_mod)
        {
            *_mod->param[0].value.u8p = 0;   // set trigger source
            param[0] = _mod->param[1];       // Attack
            param[1] = _mod->param[2];       // Decay
            *_mod->param[3].value.fp = 0.8f; // Attenuverter param[2].name = "+-";

            output.init(".", &cv, 0, -1.f, 1.f);
        }
    }

    ~EFEngine() override
    {
        machine::mfree(_mod);
    }

    int8_t scope[128] = {};
    uint8_t i = 0;
    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        cv = 0.f;
        if (this->io->aux > 0)
        {
            *_mod->param[0].value.u8p = this->io->aux - 1;
            _mod->process(output, (ControlFrame &)frame);
        }

        int32_t v = 5.f * cv * machine::PITCH_PER_OCTAVE;
        of.push_voltage(&v, 1);

        if ((frame.t % 50) == 0)
            gfx::push_scope(scope, i, cv * 20);
    }

    void display() override
    {
        gfx::drawEngineWithScope(this, scope, i, 54);
    }
};

void init_cv_peaks();

void init_voltage()
{
    machine::add<VoltsPerOctave>(CV, "V/OCT");
    machine::add<ADEnvelope>(CV, "EnvGen_AD");
    init_cv_peaks();
    machine::add<LFOEngine>(CV, "LFO");
    machine::add<EFEngine>(CV, "EnvFollower");
}