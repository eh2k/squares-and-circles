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

#undef MACHINE_INIT
#define MACHINE_INIT(init_fun) \
    extern void init_fun();    \
    init_fun();

int main()
{
    MACHINE_INIT(init_screensaver);
    MACHINE_INIT(init_voltage);
    MACHINE_INIT(init_noise);
    MACHINE_INIT(init_midi_monitor);
    MACHINE_INIT(init_midi_clock);
    MACHINE_INIT(init_quantizer);
    MACHINE_INIT(init_peaks);
    MACHINE_INIT(init_braids);
    MACHINE_INIT(init_plaits);
    MACHINE_INIT(init_sample_roms);
    MACHINE_INIT(init_clap);
    MACHINE_INIT(init_reverb);
    MACHINE_INIT(init_reverbSC);
    MACHINE_INIT(init_faust);
    MACHINE_INIT(init_rings);
    MACHINE_INIT(init_speech);
    MACHINE_INIT(init_sam);
    MACHINE_INIT(init_delay);
    MACHINE_INIT(init_modulations);
    MACHINE_INIT(init_fv1);
    MACHINE_INIT(init_midi_polyVA)

    machine::setup("0.0N4", 0);

    while (true)
        machine::loop();

    return 0;
}