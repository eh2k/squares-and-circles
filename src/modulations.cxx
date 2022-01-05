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

template <int channel>
struct CV : machine::ModulationSource
{
    float process() override
    {
        return machine::cv_voltage[channel]; // 1V/OCT : -3.5V..6.5V
    }
};

static CV<0> cv1;
static CV<1> cv2;
static CV<2> cv3;
static CV<3> cv4;

template <int channel>
struct SH : machine::ModulationSource
{
    float value = 0;
    uint32_t last_trig = 0;
    float process() override
    {
        if (machine::trigger[channel] != last_trig)
        {
            last_trig = machine::trigger[channel];
            value = machine::cv_voltage[channel];
        }

        return value;
    }
};

static SH<0> sh1;
static SH<1> sh2;
static SH<2> sh3;
static SH<3> sh4;

template <int channel>
struct Rand : machine::ModulationSource
{
    float value = 0;
    uint32_t last_trig = 0;
    float process() override
    {
        if (machine::trigger[channel] != last_trig)
        {
            last_trig = machine::trigger[channel];
            value = (stmlib::Random::GetFloat() * 20 - 10);
        }

        return value;
    }
};

static Rand<0> rnd0;
static Rand<1> rnd1;
static Rand<2> rnd2;
static Rand<3> rnd3;

void init_modulations()
{
    machine::add_modulation_source("CV1", &cv1);
    machine::add_modulation_source("CV2", &cv2);
    machine::add_modulation_source("CV3", &cv3);
    machine::add_modulation_source("CV4", &cv4);
    machine::add_modulation_source("SH1", &sh1);
    machine::add_modulation_source("SH2", &sh2);
    machine::add_modulation_source("SH3", &sh3);
    machine::add_modulation_source("SH4", &sh4);
    machine::add_modulation_source("TR1\nRND", &rnd0);
    machine::add_modulation_source("TR2\nRND", &rnd1);
    machine::add_modulation_source("TR3\nRND", &rnd2);
    machine::add_modulation_source("TR4\nRND", &rnd3);
}