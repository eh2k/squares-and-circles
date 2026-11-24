// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the hi-hat's constants fitted to the Rob Roy recordings of a TR-909 (written by
// lib/va909/fit/gen_params.py from fit/runs/hh3.txt on the branch va909; MODEL.md). (c) 2026 E. Heidt.
#pragma once
#include "hh.h"

namespace va909
{
    static_assert(hhNumParams == 24, "the params list changed: run gen_params.py again");
    static const HHParams hhFit = {
        (real)31570, // fclk
        (real)0.02585, // vt
        (real)0.0159211145, // vNode // fitted, circuit 0.011
        (real)22, // r482
        (real)250, // qBf
        (real)23.0091061, // qBr // fitted, circuit 5
        (real)6.40220976e-06, // i0 // fitted, circuit 2e-05
        (real)0.029, // accK
        (real)0.062, // sigK
        (real)1.13309289e-06, // c135 // fitted, circuit 1e-06
        (real)220000, // r453
        (real)1e-09, // c156
        (real)10000, // r451
        (real)80000, // vr21 // fitted, circuit 100000
        (real)0.252712181, // ch3 // fitted, circuit 0.3
        (real)0.84530881, // ch7 // fitted, circuit 0.7
        (real)100000, // r452
        (real)800000, // vr23 // fitted, circuit 1e+06
        (real)0.100088585, // oh3 // fitted, circuit 0.04
        (real)0.311920046, // oh7 // fitted, circuit 0.25
        (real)0.00808172608, // cPast // fitted, circuit 0.01
        (real)0.002, // tTrig
        (real)47000, // r483
        (real)13.5, // vsat
    };
}
