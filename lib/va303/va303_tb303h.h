// (c) 2026 E. Heidt.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// TB-303h: TB-303g with the Reso pot's S (resoS: little at a quarter and half its turn, most of it at three quarters)
// and the ring at full Reso that dinsync's TB-303 and the x0xb0x have: the top node's load near the circuit's 100k,
// the feedback a little past the linear ladder's oscillation point, its second coupling (fbHp2); dinsync's ring decay
// within 0.05 octaves at Reso 50 / 75 / 100 % (MODEL.md, "TB-303h"; fit/README.md). The values from fit/runs/zf.txt
// (fit_zf.sh); the fields in the order of VA303_PARAMS.
#pragma once
#include "va303.h"

namespace va303
{
    static const Params tb303hParams = {
        -1,            // sawSign: the saw's polarity into the filter (the 303's falls)
        154.352,       // hp1: Hz, the coupling into the filter (measured on a 303 by Open303)
        0.563994,      // sqWidth: the pulse's width at C2 (65.4 Hz)
        -0.075,        // sqWidthK: its change with the pitch: width = sqWidth + sqWidthK (1 - 65.4 Hz / f), fitted to the TB-03's
        0,             // sqWidthOct: its change per octave, + sqWidthOct log2(f / 65.4 Hz): the units measured, about -0.05 (0: none)
        0.589064,      // sqGain: the pulse's amplitude relative to the saw's (the recordings' square is as loud as their saw)
        49.9307,       // sqSag: Hz, the pulse's coupling: its top sags
        166.003,       // sqRise: Hz, the pulse's rising edge (one-pole); it falls at once
        5.26284,       // cutLow: log2 Hz of the converter's part of the cutoff with Cutoff at 0, no envelope, no accent (the 303's held notes: 80 Hz)
        6.53095,       // cutRange: octaves over the Cutoff pot (linear pot into the exponential converter; measured: 185 ... 1629 Hz over 25 ... 100 %)
        15253.4,       // cutTop: Hz, the converter's soft top (0: none)
        0,             // cutPotTop: the Cutoff pot's right stop on its line: its last fifth flatter (0: straight)
        213.523,       // cutFloor: Hz, the ladder's bias apart from the converter: the cutoff with the Cutoff pot down (0: none)
        2.8166,        // envFloor: the envelope's share of that bias per unit of the Env Mod pot, linear in Hz (0: none)
        21.6253,       // accFloor: the accent sweep's share of that bias per unit of the sweep, linear in Hz, as envFloor (0: none)
        3.33157,       // cap1: the bottom capacitor relative to the others (C18 0.018 uF / 0.033 uF)
        0.336008,      // topG: the top pair's conductance (a single diode an arm) relative to the ladder's pairs
        122.058,       // topLeak: kOhm an arm, the top node's load: R113/R114 100k behind C27/C25 (0: none)
        0.75267,       // driveIn: the input pair: the saw's amplitude in units of its linear range
        0.0500001,     // driveLadder: the diode pairs: the stages' voltages in units of their linear range
        1.01305,       // resoMax: the feedback at Reso 1 relative to the linear ladder's oscillation threshold
        0.501421,      // resoCurve: k = kOsc resoMax (q + resoCurve q (q - 1)); 0: linear pot
        -0.988258,     // resoS: the law's S: + resoS q (q - 1) (2 q - 1), k held at 0 (< 0: flat at both ends, steep between)
        -3.98903,      // resoCut: octaves the cutoff moves per unit of Reso (the pot's load on the bias; 0: none)
        6438.2,        // resoCutHz: Hz: resoCut grows with the cutoff, fc^2 / (fc^2 + resoCutHz^2) of it (0: the same everywhere)
        62.6762,       // fbHp: Hz, the feedback's coupling capacitor (Open303's 150 Hz; the recordings' level stays with Reso)
        27.3365,       // fbHp2: Hz, the feedback's second coupling (C15 and C23 are both in its way; 0: none)
        13.6667,       // hp2: Hz, the coupling out of the filter (Open303's)
        -8.53541,      // gain: dB, the voice's level at the amp's level 1
        0.153881,      // resoGain: the output's gain with the resonance, 1 + resoGain k (1: the passband's loss 1/(1+k) made up)
        0.715203,      // megAttack: ms, the capacitor's charge at the gate
        55.9315,       // megRs: kOhm in series with the Decay pot (R138 68k and the diodes' end at D26: some 75 with 1 uF)
        830.091,       // megRpot: kOhm, the Decay pot VR6 (1M A)
        1,             // megC: uF, the envelope's capacitor C62
        1.03878,       // decayTaper: the Decay pot's audio taper: (10^(k d) - 1) / (10^k - 1); 0 linear
        5.84204,       // envDepth: octaves at Env Mod 1 for the envelope's peak (9.8 V through 220k into 2.2k)
        0.801706,      // envTaper: the Env Mod pot's audio taper (VR5 50k A)
        0.188946,      // envR61: R61 10k under VR5 relative to the pot: the envelope's share at Env Mod 0 (0.2; 0: none)
        -0.272271,     // envRest: the envelope's resting level, of its peak: Env Mod moves the base cutoff
        0.339976,      // accDiode: the diode's drop, of the envelope's peak (0.6 V of 9.8 V)
        1.41776,       // accR1: R46 47k relative to the Resonance pot's second section (50k)
        2.52168,       // accRm: R72 100k relative to the pot
        37.6249,       // accC: ms, the pot (50k) times C13 (1 uF)
        12.7314,       // accDepth: octaves per unit of the wiper's voltage (the envelope's peak; 9.8 V through 100k into 2.2k)
        5.70659,       // accVca: the accent's share of the amp's level per unit of Accent times the envelope above the threshold
        0.691957,      // accVcaThr: the threshold (the diode D27) on Accent times the envelope, of the envelope's peak
        0.0952131,     // accVcaKnee: D27's soft knee: the width of its turning on, of the envelope's peak (0: a switch)
        0,             // accHp: Hz, the coupling of the accent into the amp (0: none)
        0.189258,      // accC36: ms, the accent into the amp through R119 47k into C36 0.033 uF, out by R120 (0: none)
        159.704,       // accVcaTau: ms, the fall of the accent's drive into the amp, measured slower than the MEG (0: the MEG's)
        2.29325,       // vegAttack: ms, the amp's attack
        1497.86,       // vegTau: ms, the VEG's fall while the gate is held (C42 1 uF, R123 1.5 M)
        -0.0440695,    // vegTarget: what it falls towards: the VCA's current ends before the capacitor is full
        1.14097,       // gateHold: ms the amp stays open after the gate
        25.5041,       // gateFall: ms of the linear fall after the hold
        -120,          // bleed: dB, the VCA's leak while it is closed (the recordings' floor after a note)
        1.057,         // vcaSat: the output's saturation level, of the voice's level (0: none)
        0.9017,        // satAsym: the negative side's limit relative to the positive one's (0: alike)
        22,            // slideTau: ms, the slide's RC (100k into 0.22 uF)
    };
    // the aggregate is positional: a field added to VA303_PARAMS needs this file written again
    static_assert(numParams == 59, "regenerate with lib/va303/fit/gen_params.py");
}
