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

#include "stmlib/stmlib.h"
#include "machine.h"

#include "open303/src/rosic_Open303.h"
using namespace machine;
using namespace rosic;

struct Open303Engine : public Engine, rosic::Open303
{
    Open303Engine() : Engine(TRIGGER_INPUT | ACCENT_INPUT | VOCT_INPUT)
    {
        Open303::setSampleRate(machine::SAMPLE_RATE, 1);
        Open303::setWaveform(0.0f);
        Open303::setTuning(Open303::tuning);
        Open303::setAmpSustain(0);
        Open303::setAccentAttack(Open303::accentAttack);
        Open303::setPitchBend(0);
        Open303::setSlideTime(Open303::slideTime);
        Open303::setVolume(-12);
        Open303::filter.setMode(TeeBeeFilter::TB_303);

        param[0].init_v_oct("Freq", &_pitch);
        param[1].init("Acc", &_acc, 100, 0.f, 100.f);
        param[2].init("Freq", &_cutoff, 0);
        param[3].init("Res", &_res, 100, 0.f, 100.f);
        param[4].init("Env", &_env, 0, 1.f, 100.f);
        param[5].init("Dec", &_dec, 0);
    }

    float buffer[FRAME_BUFFER_SIZE];

    float _pitch = 0;
    float _acc = 0;
    float _cutoff = 0;
    float _res = 0;
    float _env = 0;
    float _dec = 0;

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        Open303::setAccent(_acc);
        Open303::setCutoff(linToExp(_cutoff, 0.0, 1.0, 314.0, 2394.0));
        Open303::setResonance(_res);
        Open303::setEnvMod(_env);
        Open303::setDecay(linToExp(_dec, 0.0, 1.0, 200.0, 2000.0));

        float note = (float)machine::DEFAULT_NOTE + 24 + (_pitch + frame.cv_voltage()) * 12;

        if (frame.trigger || frame.accent)
        {
            Open303::triggerNote(note, frame.accent);
        }

        Open303::oscFreq = pitchToFreq(note, tuning);

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            buffer[i] = Open303::getSample();
        }

        of.out = buffer;
    }
};

void init_open303()
{
    machine::add<Open303Engine>(machine::SYNTH, "Open303");
}

MACHINE_INIT(init_open303);
