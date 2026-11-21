// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the hand clap's constants fitted to the Rob Roy recordings of a TR-909 (written by
// lib/va909/fit/gen_params.py from fit/runs/cp3.txt on the branch va909; MODEL.md). (c) 2026 E. Heidt.
#pragma once
#include "cp.h"

namespace va909
{
    static_assert(cpNumParams == 45, "the params list changed: run gen_params.py again");
    static const CPParams cpFit = {
        (real)15, // vee
        (real)13.5, // vsat
        (real)0.6, // vd
        (real)0.02585, // vt
        (real)2, // trigMs
        (real)3.128, // accGain
        (real)1000000, // r229
        (real)2.22592264e-08, // c62 // fitted, circuit 2.7e-08
        (real)-5, // thB
        (real)-10, // thD
        (real)96438.1235, // r220 // fitted, circuit 82000
        (real)3.76e-07, // c65 // fitted, circuit 4.7e-07
        (real)330, // r223
        (real)470000, // r224
        (real)15000, // r238
        (real)0.00022, // tauPd
        (real)9.7, // jumpPd
        (real)-0.385424884, // vtm // fitted, circuit -0.33
        (real)15000, // r235
        (real)1.05262782, // otaK // fitted, circuit 1
        (real)48624.4382, // r202 // fitted, circuit 39000
        (real)1000, // r201
        (real)2.2e-08, // c56
        (real)78847.5187, // r237 // fitted, circuit 82000
        (real)1e-09, // c58
        (real)4.7e-07, // c55
        (real)1000, // r204
        (real)2.76896105e-07, // c57 // fitted, circuit 4.7e-07
        (real)3300, // r205
        (real)47, // r203
        (real)5.17530693e-08, // c59 // fitted, circuit 1e-07
        (real)437635.348, // r233 // fitted, circuit 470000
        (real)6.85953e-08, // c61 // fitted, circuit 1e-07
        (real)908404.454, // r234 // fitted, circuit 470000
        (real)200, // rGa
        (real)250, // qBf
        (real)30, // qBr // fitted, circuit 5
        (real)1e-14, // qIs
        (real)0.55, // v61rest
        (real)450000, // noiseClk // fitted, circuit 450000
        (real)47000, // r208
        (real)10000, // r209
        (real)175771.388, // r207 // fitted, circuit 150000
        (real)4.77214798e-09, // c42 // fitted, circuit 4.7e-09
        (real)50000, // vr20
    };
}
