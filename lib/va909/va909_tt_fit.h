// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the toms' constants fitted to the Rob Roy recordings of a TR-909 (written by
// lib/va909/fit/gen_params.py from fit/runs/tt4.txt on the branch va909; MODEL.md). (c) 2026 E. Heidt.
#pragma once
#include "tt.h"

namespace va909
{
    static_assert(ttNumParams == 59, "the params list changed: run gen_params.py again");
    static const TTParams ltFit = {
        (real)15, // vcc
        (real)13.5, // vsat
        (real)0.6, // vd
        (real)0.02585, // vt
        (real)2, // trigMs
        (real)3.173, // accGain
        (real)0.682, // accOff
        (real)14.9, // vtpHi
        (real)200, // rGa
        (real)68000, // r113
        (real)10000, // vr10
        (real)10000, // r112
        (real)0.305637027, // tune3 // fitted, circuit 0.3
        (real)0.787882951, // tune7 // fitted, circuit 0.7
        (real)100000, // r64
        (real)100000, // r66
        (real)1.05611592e-07, // c16 // fitted, circuit 1e-07
        (real)2200000, // r60
        (real)4.95071053e-08, // c17 // fitted, circuit 4.7e-08
        (real)510842.825, // r61 // fitted, circuit 470000
        (real)47000, // r73
        (real)80.5139582, // rOn // fitted, circuit 0
        (real)2.34545622e-08, // c1 // fitted, circuit 2.2e-08
        (real)3.53754482e-08, // c2 // fitted, circuit 3.3e-08
        (real)1.34951e-08, // c3 // fitted, circuit 1.2e-08
        (real)6.75, // th
        (real)3.97851275e-05, // cmpDelay // fitted, circuit 4e-05
        (real)10000, // r71
        (real)1000000, // r72
        (real)7.5e-09, // c24 // fitted, circuit 1e-08
        (real)1000000, // r101
        (real)1000, // r102
        (real)656.2084, // bf18 // fitted, circuit 250
        (real)10000, // r103
        (real)2200, // r104
        (real)47, // r99
        (real)5.48047081e-08, // dIs // fitted, circuit 2.5e-09
        (real)1.75, // dN
        (real)250, // qBf
        (real)4.89740104, // qBr // fitted, circuit 5
        (real)2.07033476e-13, // qIs // fitted, circuit 1e-14
        (real)1.99370078e-07, // c22 // fitted, circuit 2.2e-07
        (real)525911.342, // r107 // fitted, circuit 470000
        (real)264000, // r108 // fitted, circuit 220000
        (real)5.27146207e-07, // c21 // fitted, circuit 6.8e-07
        (real)500000, // vr11
        (real)0.251460649, // decay3 // fitted, circuit 0.3
        (real)0.723124631, // decay7 // fitted, circuit 0.7
        (real)56000, // r111
        (real)80000, // r100 // fitted, circuit 100000
        (real)8.22021526e-07, // c23 // fitted, circuit 6.8e-07
        (real)240440.351, // r109 // fitted, circuit 220000
        (real)5.74378543e-08, // c25 // fitted, circuit 5.6e-08
        (real)433227.798, // r110 // fitted, circuit 470000
        (real)22000, // r114
        (real)50000, // vr12
        (real)1.36041008, // qN // fitted, circuit 1
        (real)1e-05, // qIRef
        (real)5, // br18
    };
    static const TTParams mtFit = {
        (real)15, // vcc
        (real)13.5, // vsat
        (real)0.6, // vd
        (real)0.02585, // vt
        (real)2, // trigMs
        (real)3.173, // accGain
        (real)0.682, // accOff
        (real)14.9, // vtpHi
        (real)200, // rGa
        (real)68000, // r113
        (real)10000, // vr10
        (real)10000, // r112
        (real)0.305637027, // tune3 // fitted, circuit 0.3
        (real)0.787882951, // tune7 // fitted, circuit 0.7
        (real)100000, // r64
        (real)100000, // r66
        (real)1.05611592e-07, // c16 // fitted, circuit 1e-07
        (real)2200000, // r60
        (real)4.95071053e-08, // c17 // fitted, circuit 4.7e-08
        (real)510842.825, // r61 // fitted, circuit 470000
        (real)47000, // r73
        (real)80.5139582, // rOn // fitted, circuit 0
        (real)1.9015779e-08, // c1 // fitted, circuit 1.8e-08
        (real)3.0431768e-08, // c2 // fitted, circuit 2.7e-08
        (real)1.09618907e-08, // c3 // fitted, circuit 1e-08
        (real)6.75, // th
        (real)3.97851275e-05, // cmpDelay // fitted, circuit 4e-05
        (real)10000, // r71
        (real)1000000, // r72
        (real)5.1e-09, // c24 // fitted, circuit 6.8e-09
        (real)1000000, // r101
        (real)1000, // r102
        (real)656.2084, // bf18 // fitted, circuit 250
        (real)10000, // r103
        (real)2200, // r104
        (real)47, // r99
        (real)5.48047081e-08, // dIs // fitted, circuit 2.5e-09
        (real)1.75, // dN
        (real)250, // qBf
        (real)4.89740104, // qBr // fitted, circuit 5
        (real)2.07033476e-13, // qIs // fitted, circuit 1e-14
        (real)1.125e-07, // c22 // fitted, circuit 1.5e-07
        (real)525911.342, // r107 // fitted, circuit 470000
        (real)264000, // r108 // fitted, circuit 220000
        (real)3.525e-07, // c21 // fitted, circuit 4.7e-07
        (real)500000, // vr11
        (real)0.251460649, // decay3 // fitted, circuit 0.3
        (real)0.723124631, // decay7 // fitted, circuit 0.7
        (real)56000, // r111
        (real)80000, // r100 // fitted, circuit 100000
        (real)6.11e-07, // c23 // fitted, circuit 4.7e-07
        (real)240440.351, // r109 // fitted, circuit 220000
        (real)6.57465235e-08, // c25 // fitted, circuit 5.6e-08
        (real)433227.798, // r110 // fitted, circuit 470000
        (real)22000, // r114
        (real)50000, // vr12
        (real)1.36041008, // qN // fitted, circuit 1
        (real)1e-05, // qIRef
        (real)5, // br18
    };
    static const TTParams htFit = {
        (real)15, // vcc
        (real)13.5, // vsat
        (real)0.6, // vd
        (real)0.02585, // vt
        (real)2, // trigMs
        (real)3.173, // accGain
        (real)0.682, // accOff
        (real)14.9, // vtpHi
        (real)200, // rGa
        (real)68000, // r113
        (real)10000, // vr10
        (real)10000, // r112
        (real)0.305637027, // tune3 // fitted, circuit 0.3
        (real)0.787882951, // tune7 // fitted, circuit 0.7
        (real)100000, // r64
        (real)100000, // r66
        (real)1.05611592e-07, // c16 // fitted, circuit 1e-07
        (real)2200000, // r60
        (real)4.95071053e-08, // c17 // fitted, circuit 4.7e-08
        (real)510842.825, // r61 // fitted, circuit 470000
        (real)47000, // r73
        (real)80.5139582, // rOn // fitted, circuit 0
        (real)1.60006199e-08, // c1 // fitted, circuit 1.5e-08
        (real)2.35270166e-08, // c2 // fitted, circuit 2.2e-08
        (real)9.13204201e-09, // c3 // fitted, circuit 8.2e-09
        (real)6.75, // th
        (real)3.97851275e-05, // cmpDelay // fitted, circuit 4e-05
        (real)10000, // r71
        (real)1000000, // r72
        (real)4.2e-09, // c24 // fitted, circuit 5.6e-09
        (real)1000000, // r101
        (real)1000, // r102
        (real)656.2084, // bf18 // fitted, circuit 250
        (real)10000, // r103
        (real)2200, // r104
        (real)47, // r99
        (real)5.48047081e-08, // dIs // fitted, circuit 2.5e-09
        (real)1.75, // dN
        (real)250, // qBf
        (real)4.89740104, // qBr // fitted, circuit 5
        (real)2.07033476e-13, // qIs // fitted, circuit 1e-14
        (real)1.25949359e-07, // c22 // fitted, circuit 1.5e-07
        (real)525911.342, // r107 // fitted, circuit 470000
        (real)264000, // r108 // fitted, circuit 220000
        (real)3.525e-07, // c21 // fitted, circuit 4.7e-07
        (real)500000, // vr11
        (real)0.251460649, // decay3 // fitted, circuit 0.3
        (real)0.723124631, // decay7 // fitted, circuit 0.7
        (real)56000, // r111
        (real)80000, // r100 // fitted, circuit 100000
        (real)5.70433445e-07, // c23 // fitted, circuit 4.7e-07
        (real)240440.351, // r109 // fitted, circuit 220000
        (real)4.66165849e-08, // c25 // fitted, circuit 4.7e-08
        (real)433227.798, // r110 // fitted, circuit 470000
        (real)22000, // r114
        (real)50000, // vr12
        (real)1.36041008, // qN // fitted, circuit 1
        (real)1e-05, // qIRef
        (real)5, // br18
    };
    static_assert(tnNumParams == 17, "the params list changed: run gen_params.py again");
    static const TNParams tnFit = {
        (real)450000, // noiseClk
        (real)22000, // r190
        (real)4.7e-09, // c161
        (real)4.7e-09, // c51
        (real)10000, // r191
        (real)220000, // r192
        (real)4.7e-08, // c53
        (real)1000, // r195
        (real)47, // r199
        (real)22000, // r193
        (real)100000, // r194
        (real)2.85448606e-05, // nIb // fitted, circuit 2e-05
        (real)0.00109400628, // nTau // fitted, circuit 0.001
        (real)2, // trigMs
        (real)250, // qBf
        (real)4.89740104, // qBr // fitted, circuit 5
        (real)0.02585, // vt
    };
}
