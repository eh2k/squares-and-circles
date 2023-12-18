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
#include "plaits/resources.h"

#ifndef FLASHMEM
#include "pgmspace.h"
#endif

#include "../app/DRUMS/Djembe.bin.h"
#include "../app/NOISE/NES.bin.h"
#include "../app/NOISE/808_squares.bin.h"
#include "../app/NOISE/WhitePink.bin.h"
#include "../app/FX/Rev-Dattorro.bin.h"
#include "../app/SPEECH/SAM.bin.h"

void init_engines();

void init_engines2()
{
#undef MACHINE_INIT
#define MACHINE_INIT(init_fun)             \
    void init_fun() __attribute__((weak)); \
    init_fun();

    static uint32_t p[5];
    p[0] = (uint32_t)plaits::fm_patches_table[0];
    p[1] = (uint32_t)plaits::fm_patches_table[1];
    p[2] = (uint32_t)plaits::fm_patches_table[2];
    p[3] = (uint32_t)machine::flash_read("DXFMSYX0");
    p[4] = 0;

    machine::register_symbol("fm_patches_table", p);

    MACHINE_INIT(init_modulations);
    MACHINE_INIT(init_quantizer);

    MACHINE_INIT(init_voltage);
    machine::add(__NOISE_WhitePink_bin, __NOISE_WhitePink_bin_len);
    machine::add(__NOISE_NES_bin, __NOISE_NES_bin_len);
    machine::add(__NOISE_808_squares_bin, __NOISE_808_squares_bin_len);
    MACHINE_INIT(init_midi_monitor);
    MACHINE_INIT(init_midi_clock);
    MACHINE_INIT(init_drums_peaks);
    MACHINE_INIT(init_braids);
    MACHINE_INIT(init_plaits);
    MACHINE_INIT(init_tr909);
    MACHINE_INIT(init_tr707);

    MACHINE_INIT(init_claps);
    MACHINE_INIT(init_reverb);
    MACHINE_INIT(init_reverbSC);
    machine::add(__FX_Rev_Dattorro_bin, __FX_Rev_Dattorro_bin_len);
    machine::add(__DRUMS_Djembe_bin, __DRUMS_Djembe_bin_len);
    MACHINE_INIT(init_rings);
    MACHINE_INIT(init_speech);
    machine::add(__SPEECH_SAM_bin, __SPEECH_SAM_bin_len);
    MACHINE_INIT(init_delay);
    MACHINE_INIT(init_fv1);
    MACHINE_INIT(init_midi_polyVA)
    MACHINE_INIT(init_dxfm);
    MACHINE_INIT(init_open303);
    MACHINE_INIT(init_aux);
    MACHINE_INIT(init_juno60_chorus);
    MACHINE_INIT(init_plaits2);
    init_engines();
}

int main()
{
    machine::setup(GIT_COMMIT_SHA, init_engines2);

    while (true)
        machine::loop();

    return 0;
}