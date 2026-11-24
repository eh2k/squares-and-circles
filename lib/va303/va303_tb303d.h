// (c) 2026 E. Heidt.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// TB-303d: the constants fitted to dinsync's TB-303 and to the user's x0xb0x together, the x0xb0x weighed as much as all
// of dinsync's notes (its Cutoff pot at five positions with Reso 0 and full, a run of accents at full Reso: what
// dinsync's sets do not have). Reso lowers the cutoff the more the higher it is (resoCutHz), the Cutoff pot's lower
// part lower and steeper; the accents at full Reso bright from the first (MODEL.md, "TB-303d"; fit/README.md).
// The values from fit/runs/n.txt (fit_n.sh); the fields in the order of VA303_PARAMS.
#pragma once
#include "va303.h"

namespace va303
{
    static const Params tb303dParams = {
        -1,            // sawSign: the saw's polarity into the filter (the 303's falls)
        138.949,       // hp1: Hz, the coupling into the filter (measured on a 303 by Open303)
        0.561039,      // sqWidth: the pulse's width at C2 (65.4 Hz)
        -0.075,        // sqWidthK: its change with the pitch: width = sqWidth + sqWidthK (1 - 65.4 Hz / f), a x0xb0x's
        0,             // sqWidthOct: its change per octave, + sqWidthOct log2(f / 65.4 Hz): the units measured, about -0.05 (0: none)
        0.579954,      // sqGain: the pulse's amplitude relative to the saw's (the recordings' square is as loud as their saw)
        46.9511,       // sqSag: Hz, the pulse's coupling: its top sags
        210.7,         // sqRise: Hz, the pulse's rising edge (one-pole); it falls at once
        5.59197,       // cutLow: log2 Hz of the converter's part of the cutoff with Cutoff at 0, no envelope, no accent
        6.18622,       // cutRange: octaves over the Cutoff pot (linear pot into the exponential converter)
        15759.9,       // cutTop: Hz, the converter's soft top (0: none)
        0,             // cutPotTop: the Cutoff pot's right stop on its line: its last fifth flatter (0: straight)
        212.831,       // cutFloor: Hz, the ladder's bias apart from the converter: the cutoff with the Cutoff pot down (0: none)
        2.78125,       // envFloor: the envelope's share of that bias per unit of the Env Mod pot, linear in Hz (0: none)
        0,             // accFloor: the accent sweep's share of that bias per unit of the sweep, linear in Hz, as envFloor (0: none)
        2.98638,       // cap1: the bottom capacitor relative to the others (C18 0.018 uF / 0.033 uF)
        0.260221,      // topG: the top pair's conductance (a single diode an arm) relative to the ladder's pairs
        113.75,        // topLeak: kOhm an arm, the top node's load: R113/R114 100k behind C27/C25 (0: none)
        0.674601,      // driveIn: the input pair: the saw's amplitude in units of its linear range
        0.05,          // driveLadder: the diode pairs: the stages' voltages in units of their linear range
        1.00219,       // resoMax: the feedback at Reso 1 relative to the linear ladder's oscillation threshold
        0.874691,      // resoCurve: k = kOsc resoMax (q + resoCurve q (q - 1)); 0: linear pot
        0,             // resoS: the law's S: + resoS q (q - 1) (2 q - 1), k held at 0 (< 0: flat at both ends, steep between)
        -2.62912,      // resoCut: octaves the cutoff moves per unit of Reso (the pot's load on the bias; 0: none)
        4892.71,       // resoCutHz: Hz: resoCut grows with the cutoff, fc^2 / (fc^2 + resoCutHz^2) of it (0: the same everywhere)
        60.2664,       // fbHp: Hz, the feedback's coupling capacitor (Open303's 150 Hz; the recordings' level stays with Reso)
        0,             // fbHp2: Hz, the feedback's second coupling (C15 and C23 are both in its way; 0: none)
        18.7443,       // hp2: Hz, the coupling out of the filter (Open303's)
        -11.179,       // gain: dB, the voice's level at the amp's level 1
        0.19838,       // resoGain: the output's gain with the resonance, 1 + resoGain k (1: the passband's loss 1/(1+k) made up)
        0.627502,      // megAttack: ms, the capacitor's charge at the gate
        56.8432,       // megRs: kOhm in series with the Decay pot (R138 68k + R117 22k)
        818.213,       // megRpot: kOhm, the Decay pot VR6 (1M A)
        1,             // megC: uF, the envelope's capacitor C62
        0.883003,      // decayTaper: the Decay pot's audio taper: (10^(k d) - 1) / (10^k - 1); 0 linear
        5.85339,       // envDepth: octaves at Env Mod 1 for the envelope's peak (9.8 V through 220k into 2.2k)
        0.7992,        // envTaper: the Env Mod pot's audio taper (VR5 50k A)
        0.154586,      // envR61: R61 10k under VR5 relative to the pot: the envelope's share at Env Mod 0 (0.2; 0: none)
        -0.270645,     // envRest: the envelope's resting level, of its peak: Env Mod moves the base cutoff
        0.372013,      // accDiode: the diode's drop, of the envelope's peak (0.6 V of 9.8 V)
        0.327257,      // accR1: R46 47k relative to the Resonance pot's second section (50k)
        2.34061,       // accRm: R72 100k relative to the pot
        58.7887,       // accC: ms, the pot (50k) times C13 (1 uF)
        15.1695,       // accDepth: octaves per unit of the wiper's voltage (the envelope's peak; 9.8 V through 100k into 2.2k)
        3.26561,       // accVca: the accent's share of the amp's level per unit of Accent times the envelope above the threshold
        0.255984,      // accVcaThr: the threshold (the diode D27) on Accent times the envelope, of the envelope's peak
        0,             // accVcaKnee: D27's soft knee: the width of its turning on, of the envelope's peak (0: a switch)
        0,             // accHp: Hz, the coupling of the accent into the amp (0: none)
        1.55,          // accC36: ms, the accent into the amp through R119 47k into C36 0.033 uF, out by R120 (0: none)
        122.26,        // accVcaTau: ms, the fall of the accent's drive into the amp, measured slower than the MEG (0: the MEG's)
        0.498592,      // vegAttack: ms, the amp's attack
        1497.86,       // vegTau: ms, the VEG's fall while the gate is held (C42 1 uF, R123 1.5 M)
        -0.0440695,    // vegTarget: what it falls towards: the VCA's current ends before the capacitor is full
        14.8755,       // gateHold: ms the amp stays open after the gate
        7.16102,       // gateFall: ms of the linear fall after the hold
        -119.939,      // bleed: dB, the VCA's leak while it is closed (the recordings' floor after a note)
        1.63941,       // vcaSat: the output's saturation level, of the voice's level (0: none)
        1.26453,       // satAsym: the negative side's limit relative to the positive one's (0: alike)
        22,            // slideTau: ms, the slide's RC (100k into 0.22 uF)
    };
    // the aggregate is positional: a field added to VA303_PARAMS needs this file written again
    static_assert(numParams == 59, "regenerate with lib/va303/fit/gen_params.py");
}
