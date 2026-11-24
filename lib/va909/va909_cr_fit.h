// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the crash's constants fitted to the Rob Roy recordings of a TR-909 (written by
// lib/va909/fit/gen_params.py from fit/runs/cr3.txt on the branch va909; MODEL.md). (c) 2026 E. Heidt.
#pragma once
#include "hh.h"

namespace va909
{
    static_assert(cyNumParams == 20, "the params list changed: run gen_params.py again");
    static const CYParams crFit = {
        (real)6800, // r443
        (real)10000, // vr25
        (real)4.7e-10, // c131
        (real)1.9487, // kOsc // fitted, circuit 1.95
        (real)2.758e-06, // tOsc // fitted, circuit 2.7e-06
        (real)0.2121, // tune3 // fitted, circuit 0.3
        (real)0.7944, // tune7 // fitted, circuit 0.7
        (real)0.02585, // vt
        (real)0.0180904133, // vNode // fitted, circuit 0.011
        (real)22, // r431
        (real)250, // qBf
        (real)4.6196955, // qBr // fitted, circuit 5
        (real)2.60771635e-05, // i0 // fitted, circuit 2e-05
        (real)60.9563416, // aEnd // fitted, circuit 64
        (real)0.909660287, // aPow // fitted, circuit 1
        (real)0.0447, // accK
        (real)270000, // rRef
        (real)1e-09, // cRef
        (real)47000, // r425
        (real)13.5, // vsat
    };
}
