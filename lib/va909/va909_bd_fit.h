// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the bass drum's constants fitted to the Rob Roy recordings of a TR-909 (written by
// lib/va909/fit/gen_params.py from fit/runs/bd3.txt on the branch va909; MODEL.md). (c) 2026 E. Heidt.
#pragma once
#include "bd.h"

// the ACCENT line's volts of the recordings (taken as the 909's normal level) and the mix out's high pass (Hz)
#define VA909_ACC_NORMAL 2.5547f
#define VA909_MIX_HP 123.39f

namespace va909
{
    static_assert(bdNumParams == 76, "the params list changed: run gen_params.py again");
    static const BDParams bdFit = {
        (real)15, // vcc
        (real)13.5, // vsat
        (real)0.6, // vd
        (real)0.02585, // vt
        (real)2, // trigMs
        (real)14.9, // vtpHi
        (real)0.31, // vtpLo
        (real)1800, // r21 // fitted, circuit 2200
        (real)2.86481381e-07, // c9 // fitted, circuit 3.3e-07
        (real)27769.8279, // r57 // fitted, circuit 22000
        (real)115839.076, // vr2 // fitted, circuit 100000
        (real)0.0948863511, // tune3 // fitted, circuit 0.03
        (real)0.407530839, // tune7 // fitted, circuit 0.244
        (real)340599.662, // r26 // fitted, circuit 330000
        (real)1507004.75, // r27 // fitted, circuit 1.5e+06
        (real)150000, // r28
        (real)47000, // r59
        (real)3.73619002e-08, // c14 // fitted, circuit 3.3e-08
        (real)100000, // r49
        (real)47000, // r50
        (real)2200, // r51
        (real)10000, // r52
        (real)6.50669673e-09, // dIs // fitted, circuit 2.5e-09
        (real)1.75135664, // dN // fitted, circuit 1.75
        (real)2200, // r53
        (real)47, // r43
        (real)250, // qBf
        (real)4.34246374, // qBr // fitted, circuit 5
        (real)1e-12, // qIs // fitted, circuit 1e-14
        (real)376000, // r54 // fitted, circuit 470000
        (real)3.128, // accGain
        (real)1800, // r35
        (real)3.52451159e-07, // c8 // fitted, circuit 3.3e-07
        (real)47000, // r58
        (real)800885.137, // vr5 // fitted, circuit 1e+06
        (real)0.114212175, // decay3 // fitted, circuit 0.03
        (real)0.389578171, // decay7 // fitted, circuit 0.244
        (real)47000, // r36
        (real)10000, // r22
        (real)0.1, // vce4
        (real)6800, // r20
        (real)3.67904046e-08, // c7 // fitted, circuit 3.3e-08
        (real)470000, // r18
        (real)470000, // r17
        (real)0.573691795, // vbe3 // fitted, circuit 0.55
        (real)47000, // r32
        (real)1000000, // r33
        (real)250, // bf3
        (real)200, // bf4
        (real)22000, // r41
        (real)3.3e-08, // c12
        (real)376000, // r42 // fitted, circuit 470000
        (real)0, // r495
        (real)1.3, // accMin
        (real)14.9, // vq8
        (real)6.8e-09, // c5
        (real)22000, // r14
        (real)3.3e-09, // c2
        (real)100000, // r7
        (real)100000, // r15
        (real)39000, // r16
        (real)1e-10, // c4
        (real)4.7e-09, // c3
        (real)4.7e-07, // c11
        (real)22000, // r44
        (real)450000, // noiseClk // fitted, circuit 450000
        (real)12000, // r10
        (real)4700, // r9
        (real)4.7e-07, // c15
        (real)5946.26646, // r46 // fitted, circuit 4700
        (real)7.69230769e-08, // c13 // fitted, circuit 1e-07
        (real)4688.85578, // r45 // fitted, circuit 4700
        (real)330, // r56
        (real)572.920661, // vr3 // fitted, circuit 500
        (real)47, // r55
        (real)100000, // vr4
    };
}
