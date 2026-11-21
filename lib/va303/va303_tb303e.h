// (c) 2026 E. Heidt.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// TB-303e: TB-303d fitted again with the x0xb0x's Accent pot (at 9, 12, 3 o'clock and its right stop, at Cutoff left and
// 12 h) weighed three times: its accent is stronger and lasts longer than dinsync's TB-303's (+25 dB over the plain note
// at the onset, +11 dB at 100 ms, at Cutoff left), D27 a soft diode (accVcaKnee) (MODEL.md, "TB-303e"; fit/README.md).
// The values from fit/runs/r.txt (fit_r.sh); the fields in the order of VA303_PARAMS.
#pragma once
#include "va303.h"

namespace va303
{
    static const Params tb303eParams = {
        -1,            // sawSign: the saw's polarity into the filter (the 303's falls)
        120.665,       // hp1: Hz, the coupling into the filter (measured on a 303 by Open303)
        0.561011,      // sqWidth: the pulse's width at C2 (65.4 Hz)
        -0.075,        // sqWidthK: its change with the pitch: width = sqWidth + sqWidthK (1 - 65.4 Hz / f), a x0xb0x's
        0,             // sqWidthOct: its change per octave, + sqWidthOct log2(f / 65.4 Hz): the units measured, about -0.05 (0: none)
        0.564826,      // sqGain: the pulse's amplitude relative to the saw's (the recordings' square is as loud as their saw)
        44.7245,       // sqSag: Hz, the pulse's coupling: its top sags
        210.427,       // sqRise: Hz, the pulse's rising edge (one-pole); it falls at once
        5.50137,       // cutLow: log2 Hz of the converter's part of the cutoff with Cutoff at 0, no envelope, no accent
        6.23032,       // cutRange: octaves over the Cutoff pot (linear pot into the exponential converter)
        14850.9,       // cutTop: Hz, the converter's soft top (0: none)
        0,             // cutPotTop: the Cutoff pot's right stop on its line: its last fifth flatter (0: straight)
        200.71,        // cutFloor: Hz, the ladder's bias apart from the converter: the cutoff with the Cutoff pot down (0: none)
        2.902,         // envFloor: the envelope's share of that bias per unit of the Env Mod pot, linear in Hz (0: none)
        0,             // accFloor: the accent sweep's share of that bias per unit of the sweep, linear in Hz, as envFloor (0: none)
        2.90188,       // cap1: the bottom capacitor relative to the others (C18 0.018 uF / 0.033 uF)
        0.350161,      // topG: the top pair's conductance (a single diode an arm) relative to the ladder's pairs
        569.099,       // topLeak: kOhm an arm, the top node's load: R113/R114 100k behind C27/C25 (0: none)
        0.735326,      // driveIn: the input pair: the saw's amplitude in units of its linear range
        0.05,          // driveLadder: the diode pairs: the stages' voltages in units of their linear range
        1.0003,        // resoMax: the feedback at Reso 1 relative to the linear ladder's oscillation threshold
        0.892543,      // resoCurve: k = kOsc resoMax (q + resoCurve q (q - 1)); 0: linear pot
        0,             // resoS: the law's S: + resoS q (q - 1) (2 q - 1), k held at 0 (< 0: flat at both ends, steep between)
        -3.95347,      // resoCut: octaves the cutoff moves per unit of Reso (the pot's load on the bias; 0: none)
        6218.24,       // resoCutHz: Hz: resoCut grows with the cutoff, fc^2 / (fc^2 + resoCutHz^2) of it (0: the same everywhere)
        68.9269,       // fbHp: Hz, the feedback's coupling capacitor (Open303's 150 Hz; the recordings' level stays with Reso)
        0,             // fbHp2: Hz, the feedback's second coupling (C15 and C23 are both in its way; 0: none)
        51.0762,       // hp2: Hz, the coupling out of the filter (Open303's)
        -9.52002,      // gain: dB, the voice's level at the amp's level 1
        0.157328,      // resoGain: the output's gain with the resonance, 1 + resoGain k (1: the passband's loss 1/(1+k) made up)
        1.28134,       // megAttack: ms, the capacitor's charge at the gate
        54.3879,       // megRs: kOhm in series with the Decay pot (R138 68k + R117 22k)
        816.143,       // megRpot: kOhm, the Decay pot VR6 (1M A)
        1,             // megC: uF, the envelope's capacitor C62
        0.913519,      // decayTaper: the Decay pot's audio taper: (10^(k d) - 1) / (10^k - 1); 0 linear
        5.79625,       // envDepth: octaves at Env Mod 1 for the envelope's peak (9.8 V through 220k into 2.2k)
        0.85585,       // envTaper: the Env Mod pot's audio taper (VR5 50k A)
        0.175947,      // envR61: R61 10k under VR5 relative to the pot: the envelope's share at Env Mod 0 (0.2; 0: none)
        -0.267393,     // envRest: the envelope's resting level, of its peak: Env Mod moves the base cutoff
        0.433515,      // accDiode: the diode's drop, of the envelope's peak (0.6 V of 9.8 V)
        0.0200971,     // accR1: R46 47k relative to the Resonance pot's second section (50k)
        0.9294,        // accRm: R72 100k relative to the pot
        128.079,       // accC: ms, the pot (50k) times C13 (1 uF)
        25.2528,       // accDepth: octaves per unit of the wiper's voltage (the envelope's peak; 9.8 V through 100k into 2.2k)
        7.96156,       // accVca: the accent's share of the amp's level per unit of Accent times the envelope above the threshold
        0.55751,       // accVcaThr: the threshold (the diode D27) on Accent times the envelope, of the envelope's peak
        0.0740128,     // accVcaKnee: D27's soft knee: the width of its turning on, of the envelope's peak (0: a switch)
        0,             // accHp: Hz, the coupling of the accent into the amp (0: none)
        5.23519,       // accC36: ms, the accent into the amp through R119 47k into C36 0.033 uF, out by R120 (0: none)
        201.265,       // accVcaTau: ms, the fall of the accent's drive into the amp, measured slower than the MEG (0: the MEG's)
        0.412842,      // vegAttack: ms, the amp's attack
        1497.86,       // vegTau: ms, the VEG's fall while the gate is held (C42 1 uF, R123 1.5 M)
        -0.0440695,    // vegTarget: what it falls towards: the VCA's current ends before the capacitor is full
        15.4288,       // gateHold: ms the amp stays open after the gate
        6.30274,       // gateFall: ms of the linear fall after the hold
        -120,          // bleed: dB, the VCA's leak while it is closed (the recordings' floor after a note)
        0.851421,      // vcaSat: the output's saturation level, of the voice's level (0: none)
        1.41775,       // satAsym: the negative side's limit relative to the positive one's (0: alike)
        22,            // slideTau: ms, the slide's RC (100k into 0.22 uF)
    };
    // the aggregate is positional: a field added to VA303_PARAMS needs this file written again
    static_assert(numParams == 59, "regenerate with lib/va303/fit/gen_params.py");
}
