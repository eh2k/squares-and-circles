// (c) 2026 E. Heidt.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// The TB-303: the constants of va303::Params fitted to recordings of one (lib/va303/MODEL.md, fit/README.md).
// Written by fit/gen_params.py from v5.txt with noteDelay=4 (a constant until 2026-11-08) sqWidthK=-0.075; the fields
// in the order of VA303_PARAMS.
#pragma once
#include "va303.h"

namespace va303
{
    static const Params tb303Params = {
        -1,            // sawSign: the saw's polarity into the filter (the 303's falls)
        133.944,       // hp1: Hz, the coupling into the filter (measured on a 303 by Open303)
        0.559105,      // sqWidth: the pulse's width at C2 (65.4 Hz)
        -0.075,        // sqWidthK: its change with the pitch: width = sqWidth + sqWidthK (1 - 65.4 Hz / f), a x0xb0x's
        0,             // sqWidthOct: its change per octave, + sqWidthOct log2(f / 65.4 Hz): the units measured, about -0.05 (0: none)
        0.625649,      // sqGain: the pulse's amplitude relative to the saw's (the recordings' square is as loud as their saw)
        47.7418,       // sqSag: Hz, the pulse's coupling: its top sags
        235.308,       // sqRise: Hz, the pulse's rising edge (one-pole); it falls at once
        6.29303,       // cutLow: log2 Hz of the converter's part of the cutoff with Cutoff at 0, no envelope, no accent
        4.44526,       // cutRange: octaves over the Cutoff pot (linear pot into the exponential converter)
        9672.78,       // cutTop: Hz, the converter's soft top (0: none)
        0,             // cutPotTop: the Cutoff pot's right stop on its line: its last fifth flatter (0: straight)
        61.8448,       // cutFloor: Hz, the ladder's bias apart from the converter: the cutoff with the Cutoff pot down (0: none)
        2.70695,       // envFloor: the envelope's share of that bias per unit of the Env Mod pot, linear in Hz (0: none)
        0,             // accFloor: the accent sweep's share of that bias per unit of the sweep, linear in Hz, as envFloor (0: none)
        0.545,         // cap1: the bottom capacitor relative to the others (C18 0.018 uF / 0.033 uF)
        1,             // topG: the top pair's conductance (a single diode an arm) relative to the ladder's pairs
        0,             // topLeak: kOhm an arm, the top node's load: R113/R114 100k behind C27/C25 (0: none)
        0.224809,      // driveIn: the input pair: the saw's amplitude in units of its linear range
        0.680366,      // driveLadder: the diode pairs: the stages' voltages in units of their linear range
        0.862399,      // resoMax: the feedback at Reso 1 relative to the linear ladder's oscillation threshold
        1.19172,       // resoCurve: k = kOsc resoMax (q + resoCurve q (q - 1)); 0: linear pot
        0,             // resoS: the law's S: + resoS q (q - 1) (2 q - 1), k held at 0 (< 0: flat at both ends, steep between)
        -0.619961,     // resoCut: octaves the cutoff moves per unit of Reso (the pot's load on the bias; 0: none)
        0,             // resoCutHz: Hz: resoCut grows with the cutoff, fc^2 / (fc^2 + resoCutHz^2) of it (0: the same everywhere)
        51.6378,       // fbHp: Hz, the feedback's coupling capacitor (Open303's 150 Hz; the recordings' level stays with Reso)
        0,             // fbHp2: Hz, the feedback's second coupling (C15 and C23 are both in its way; 0: none)
        2.3667,        // hp2: Hz, the coupling out of the filter (Open303's)
        -10.7418,      // gain: dB, the voice's level at the amp's level 1
        0.230285,      // resoGain: the output's gain with the resonance, 1 + resoGain k (1: the passband's loss 1/(1+k) made up)
        0.379981,      // megAttack: ms, the capacitor's charge at the gate
        66.6198,       // megRs: kOhm in series with the Decay pot (R138 68k + R117 22k)
        854.964,       // megRpot: kOhm, the Decay pot VR6 (1M A)
        1,             // megC: uF, the envelope's capacitor C62
        0.831641,      // decayTaper: the Decay pot's audio taper: (10^(k d) - 1) / (10^k - 1); 0 linear
        5.70817,       // envDepth: octaves at Env Mod 1 for the envelope's peak (9.8 V through 220k into 2.2k)
        0.592235,      // envTaper: the Env Mod pot's audio taper (VR5 50k A)
        0,             // envR61: R61 10k under VR5 relative to the pot: the envelope's share at Env Mod 0 (0.2; 0: none)
        -0.281032,     // envRest: the envelope's resting level, of its peak: Env Mod moves the base cutoff
        0.287325,      // accDiode: the diode's drop, of the envelope's peak (0.6 V of 9.8 V)
        0.0375089,     // accR1: R46 47k relative to the Resonance pot's second section (50k)
        1.88977,       // accRm: R72 100k relative to the pot
        61.8999,       // accC: ms, the pot (50k) times C13 (1 uF)
        9.12411,       // accDepth: octaves per unit of the wiper's voltage (the envelope's peak; 9.8 V through 100k into 2.2k)
        1.69245,       // accVca: the accent's share of the amp's level per unit of Accent times the envelope above the threshold
        0.0520318,     // accVcaThr: the threshold (the diode D27) on Accent times the envelope, of the envelope's peak
        0,             // accVcaKnee: D27's soft knee: the width of its turning on, of the envelope's peak (0: a switch)
        0.52781,       // accHp: Hz, the coupling of the accent into the amp (0: none)
        0,             // accC36: ms, the accent into the amp through R119 47k into C36 0.033 uF, out by R120 (0: none)
        0,             // accVcaTau: ms, the fall of the accent's drive into the amp, measured slower than the MEG (0: the MEG's)
        0.14667,       // vegAttack: ms, the amp's attack
        1508.2,        // vegTau: ms, the VEG's fall while the gate is held (C42 1 uF, R123 1.5 M)
        -0.0679013,    // vegTarget: what it falls towards: the VCA's current ends before the capacitor is full
        13.8928,       // gateHold: ms the amp stays open after the gate
        9.52822,       // gateFall: ms of the linear fall after the hold
        -62.0122,      // bleed: dB, the VCA's leak while it is closed (the recordings' floor after a note)
        0,             // vcaSat: the output's saturation level, of the voice's level (0: none)
        0,             // satAsym: the negative side's limit relative to the positive one's (0: alike)
        22,            // slideTau: ms, the slide's RC (100k into 0.22 uF)
    };
    // the aggregate is positional: a field added to VA303_PARAMS needs this file written again
    static_assert(numParams == 59, "regenerate with lib/va303/fit/gen_params.py");
}
