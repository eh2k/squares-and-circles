// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the rim shot's constants fitted to the Rob Roy recordings of a TR-909 (written by
// lib/va909/fit/gen_params.py from fit/runs/rs3.txt on the branch va909; MODEL.md). (c) 2026 E. Heidt.
#pragma once
#include "rs.h"

namespace va909
{
    static_assert(rsNumParams == 40, "the params list changed: run gen_params.py again");
    static const RSParams rsFit = {
        (real)13.5, // vsat
        (real)0.02585, // vt
        (real)5, // trigV
        (real)1.8e-08, // c111
        (real)22000, // r396
        (real)22000, // r395
        (real)22000, // r399
        (real)3.128, // accGain
        (real)2700, // r401
        (real)3.78292371e-08, // c119 // fitted, circuit 4.7e-08
        (real)0.000533436891, // tCharge // fitted, circuit 0.00032
        (real)800000, // r403 // fitted, circuit 1e+06
        (real)445967.597, // rb1 // fitted, circuit 470000
        (real)1.11506326e-08, // c1 // fitted, circuit 1e-08
        (real)2200, // rs1
        (real)306618.629, // rb2 // fitted, circuit 330000
        (real)2.87634153e-08, // c2 // fitted, circuit 2.7e-08
        (real)2200, // rs2
        (real)485237.785, // rb3 // fitted, circuit 470000
        (real)4.88709001e-09, // c3 // fitted, circuit 4.7e-09
        (real)2200, // rs3
        (real)12000, // r408
        (real)12000, // r415
        (real)11020.9626, // r417 // fitted, circuit 3300
        (real)1e-10, // dIs // fitted, circuit 2.5e-09
        (real)1.75, // dN
        (real)12000, // r423
        (real)680, // r418
        (real)250, // qBf
        (real)1.00057184, // qBr // fitted, circuit 5
        (real)1e-14, // qIs
        (real)100000, // vr19
        (real)2.2e-10, // c120
        (real)1e-08, // c121
        (real)27329.1085, // r422 // fitted, circuit 22000
        (real)220000, // r420
        (real)2, // outK
        (real)0.6, // vdSpike
        (real)10000, // r398
        (real)0.65, // vbe63
    };
}
