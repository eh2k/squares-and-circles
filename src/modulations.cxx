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

#ifdef TEENSYDUINO
#include <Entropy.h>
#else
struct
{
    int32_t random() { return rand(); }
    float randomf(float min, float max) { return (float)rand() / INT32_MAX * (max - min) + min; }
} Entropy;
#endif

template <int channel>
struct CV : machine::ModulationSource
{
    float process() override
    {
        return machine::get_cv(channel); // 1V/OCT : -3.5V..6.5V
    }
};

template <int channel>
struct SH : machine::ModulationSource
{
    float value = 0;
    uint32_t last_trig = 0;
    float process() override
    {
        if (machine::get_trigger(channel) != last_trig)
        {
            last_trig = machine::get_trigger(channel);
            value = machine::get_cv(channel);
        }

        return value;
    }
};

template <int channel>
struct Rand : machine::ModulationSource
{
    float value = 0;
    uint32_t last_trig = 0;
    float process() override
    {
        if (machine::get_trigger(channel) != last_trig)
        {
            last_trig = machine::get_trigger(channel);
            value = Entropy.randomf(-10, 10);
        }

        return value;
    }
};

template <typename T>
void ADD_MODULATION_SOURCE(const char *name)
{
    static T instance;
    machine::add_modulation_source(name, &instance);
}

void init_modulations()
{
    ADD_MODULATION_SOURCE<CV<0>>("CV1");
    ADD_MODULATION_SOURCE<CV<1>>("CV2");
    ADD_MODULATION_SOURCE<CV<2>>("CV3");
    ADD_MODULATION_SOURCE<CV<3>>("CV4");

    ADD_MODULATION_SOURCE<SH<0>>("SH1");
    ADD_MODULATION_SOURCE<SH<1>>("SH2");
    ADD_MODULATION_SOURCE<SH<2>>("SH3");
    ADD_MODULATION_SOURCE<SH<3>>("SH4");

    ADD_MODULATION_SOURCE<Rand<0>>("RND1");
    ADD_MODULATION_SOURCE<Rand<1>>("RND2");
    ADD_MODULATION_SOURCE<Rand<2>>("RND3");
    ADD_MODULATION_SOURCE<Rand<3>>("RND4");
}

MACHINE_INIT(init_modulations);