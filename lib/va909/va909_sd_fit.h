// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the snare drum's constants fitted to the Rob Roy recordings of a TR-909 (written by
// lib/va909/fit/gen_params.py from fit/runs/sd3.txt on the branch va909; MODEL.md). (c) 2026 E. Heidt.
#pragma once
#include "sd.h"

namespace va909
{
    static_assert(sdNumParams == 79, "the params list changed: run gen_params.py again");
    static const SDParams sdFit = {
        (real)15, // vcc
        (real)13.5, // vsat
        (real)0.6, // vd
        (real)0.02585, // vt
        (real)2, // trigMs
        (real)3.128, // accGain
        (real)14.9, // vtpHi
        (real)47000, // r247
        (real)1.70931614e-08, // c66 // fitted, circuit 1e-08
        (real)897253.611, // r248 // fitted, circuit 470000
        (real)100000, // r249
        (real)100000, // r287
        (real)10000, // vr6
        (real)0.192384599, // tune3 // fitted, circuit 0.3
        (real)0.756843869, // tune7 // fitted, circuit 0.7
        (real)10000, // r284
        (real)1, // railK
        (real)47000, // r261
        (real)0, // rOn
        (real)1.13260162e-08, // c69 // fitted, circuit 1e-08
        (real)7.83741052e-09, // c71 // fitted, circuit 6.8e-09
        (real)6.62, // thHi
        (real)-3.57, // thLoA
        (real)-4.41, // thLoB
        (real)9.94659283e-06, // cmpDelay // fitted, circuit 4e-05
        (real)10000, // r263
        (real)100000, // r258
        (real)1000000, // r267
        (real)10000, // r283
        (real)1500, // r288
        (real)47, // r289
        (real)10000, // r305
        (real)2200, // r304
        (real)47, // r303
        (real)6.89019585e-10, // dIs // fitted, circuit 2.5e-09
        (real)1.31268564, // dN // fitted, circuit 1.75
        (real)250, // qBf
        (real)6.52529792, // qBr // fitted, circuit 5
        (real)3.30345247e-15, // qIs // fitted, circuit 1e-14
        (real)3.79356269e-08, // c83 // fitted, circuit 1e-07
        (real)606016.444, // r291 // fitted, circuit 220000
        (real)4.72704698e-09, // c70 // fitted, circuit 4.7e-09
        (real)22000, // r290
        (real)2.70611501e-08, // c82 // fitted, circuit 4.7e-08
        (real)228342.876, // r295 // fitted, circuit 220000
        (real)2.07102976e-08, // c79 // fitted, circuit 1e-08
        (real)220000, // r293
        (real)200, // rGa
        (real)450000, // noiseClk // fitted, circuit 450000
        (real)10000, // r286
        (real)3300, // r285
        (real)1.5e-08, // c85
        (real)220000, // r302
        (real)22000, // r301
        (real)7.41059433e-10, // c81 // fitted, circuit 3.3e-09
        (real)3.03921775e-10, // c84 // fitted, circuit 2.2e-10
        (real)2.2e-08, // c80
        (real)4700, // r299
        (real)47, // r298
        (real)1.54950406e-09, // c76 // fitted, circuit 2.2e-09
        (real)6800, // r276
        (real)47000, // r277
        (real)2200, // r278
        (real)47, // r279
        (real)250, // nBf
        (real)4.51046511, // nBr // fitted, circuit 5
        (real)1e-14, // nIs
        (real)1000000, // r256
        (real)6.8e-09, // c68
        (real)200, // bf42
        (real)7.10483815e-07, // c67 // fitted, circuit 4.7e-07
        (real)621352.212, // vr7 // fitted, circuit 500000
        (real)95231.3758, // r254 // fitted, circuit 100000
        (real)220000, // r297
        (real)4.7e-10, // c73
        (real)220000, // r280
        (real)15334.8041, // vr9 // fitted, circuit 10000
        (real)47000, // r294
        (real)50000, // vr8
    };
}
