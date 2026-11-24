// (c) 2026 E. Heidt.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// TB-303i: the circuit's ladder (C18 0.018 / C19 ... C26 0.033 uF: cap1 0.545, the top pair 0.67) with the couplings
// both units' held notes at Reso 0 want (into the filter 46 Hz, out of it 112 Hz: Open303's 44.5 and the VCA's input
// R121 220k / C21 0.01 uF with C27) and the Cutoff pot's last fifth flatter (cutPotTop 0.884): the phases of dinsync's
// TB-303 and of the x0xb0x within 10 ... 35 degrees, where TB-303h's bent capacitors are 80 ... 123 off; the ring at
// full Reso with the feedback 1.016 of the threshold and the top node's load 62k (0.07 octaves; TB-303h 0.05); the
// pulse both units have, an ideal one of width 0.54 at C2 falling 0.05 an octave (sqWidthOct); dinsync's band levels
// 1.23 dB (TB-303h 1.13) (MODEL.md, "TB-303i"; fit/README.md). The values from fit/runs/zsi.txt (fit_zx.sh,
// fit_zxr.sh, the ring's scan, fit_zxs.sh, fit_zsi.sh); the fields in the order of VA303_PARAMS.
#pragma once
#include "va303.h"

namespace va303
{
    static const Params tb303iParams = {
        -1,            // sawSign: the saw's polarity into the filter (the 303's falls)
        46.0352,       // hp1: Hz, the coupling into the filter (measured on a 303 by Open303)
        0.539265,      // sqWidth: the pulse's width at C2 (65.4 Hz)
        0,             // sqWidthK: its change with the pitch: width = sqWidth + sqWidthK (1 - 65.4 Hz / f), fitted to the TB-03's
        -0.05,         // sqWidthOct: its change per octave, + sqWidthOct log2(f / 65.4 Hz): the units measured, about -0.05 (0: none)
        0.480732,      // sqGain: the pulse's amplitude relative to the saw's (the recordings' square is as loud as their saw)
        8.7,           // sqSag: Hz, the pulse's coupling: its top sags
        24000,         // sqRise: Hz, the pulse's rising edge (one-pole); it falls at once
        4.52897,       // cutLow: log2 Hz of the converter's part of the cutoff with Cutoff at 0, no envelope, no accent (the 303's held notes: 80 Hz)
        6.5284,        // cutRange: octaves over the Cutoff pot (linear pot into the exponential converter; measured: 185 ... 1629 Hz over 25 ... 100 %)
        16376.6,       // cutTop: Hz, the converter's soft top (0: none)
        0.883943,      // cutPotTop: the Cutoff pot's right stop on its line: its last fifth flatter (0: straight)
        125.373,       // cutFloor: Hz, the ladder's bias apart from the converter: the cutoff with the Cutoff pot down (0: none)
        2.82089,       // envFloor: the envelope's share of that bias per unit of the Env Mod pot, linear in Hz (0: none)
        25.3832,       // accFloor: the accent sweep's share of that bias per unit of the sweep, linear in Hz, as envFloor (0: none)
        0.545,         // cap1: the bottom capacitor relative to the others (C18 0.018 uF / 0.033 uF)
        0.66856,       // topG: the top pair's conductance (a single diode an arm) relative to the ladder's pairs
        61.9827,       // topLeak: kOhm an arm, the top node's load: R113/R114 100k behind C27/C25 (0: none)
        0.718007,      // driveIn: the input pair: the saw's amplitude in units of its linear range
        0.589567,      // driveLadder: the diode pairs: the stages' voltages in units of their linear range
        1.01568,       // resoMax: the feedback at Reso 1 relative to the linear ladder's oscillation threshold
        0.418711,      // resoCurve: k = kOsc resoMax (q + resoCurve q (q - 1)); 0: linear pot
        -1.24098,      // resoS: the law's S: + resoS q (q - 1) (2 q - 1), k held at 0 (< 0: flat at both ends, steep between)
        -3.93596,      // resoCut: octaves the cutoff moves per unit of Reso (the pot's load on the bias; 0: none)
        6001.2,        // resoCutHz: Hz: resoCut grows with the cutoff, fc^2 / (fc^2 + resoCutHz^2) of it (0: the same everywhere)
        52.3645,       // fbHp: Hz, the feedback's coupling capacitor (Open303's 150 Hz; the recordings' level stays with Reso)
        42.824,        // fbHp2: Hz, the feedback's second coupling (C15 and C23 are both in its way; 0: none)
        111.663,       // hp2: Hz, the coupling out of the filter (Open303's)
        -8.11052,      // gain: dB, the voice's level at the amp's level 1
        0.129861,      // resoGain: the output's gain with the resonance, 1 + resoGain k (1: the passband's loss 1/(1+k) made up)
        0.80815,       // megAttack: ms, the capacitor's charge at the gate
        54.8432,       // megRs: kOhm in series with the Decay pot (R138 68k and the diodes' end at D26: some 75 with 1 uF)
        793.887,       // megRpot: kOhm, the Decay pot VR6 (1M A)
        1,             // megC: uF, the envelope's capacitor C62
        1.05835,       // decayTaper: the Decay pot's audio taper: (10^(k d) - 1) / (10^k - 1); 0 linear
        5.82102,       // envDepth: octaves at Env Mod 1 for the envelope's peak (9.8 V through 220k into 2.2k)
        0.846817,      // envTaper: the Env Mod pot's audio taper (VR5 50k A)
        0.201899,      // envR61: R61 10k under VR5 relative to the pot: the envelope's share at Env Mod 0 (0.2; 0: none)
        -0.276226,     // envRest: the envelope's resting level, of its peak: Env Mod moves the base cutoff
        0.353461,      // accDiode: the diode's drop, of the envelope's peak (0.6 V of 9.8 V)
        1.41824,       // accR1: R46 47k relative to the Resonance pot's second section (50k)
        2.47082,       // accRm: R72 100k relative to the pot
        37.188,        // accC: ms, the pot (50k) times C13 (1 uF)
        13.1931,       // accDepth: octaves per unit of the wiper's voltage (the envelope's peak; 9.8 V through 100k into 2.2k)
        7.07358,       // accVca: the accent's share of the amp's level per unit of Accent times the envelope above the threshold
        0.711722,      // accVcaThr: the threshold (the diode D27) on Accent times the envelope, of the envelope's peak
        0.0944025,     // accVcaKnee: D27's soft knee: the width of its turning on, of the envelope's peak (0: a switch)
        0,             // accHp: Hz, the coupling of the accent into the amp (0: none)
        0.368149,      // accC36: ms, the accent into the amp through R119 47k into C36 0.033 uF, out by R120 (0: none)
        154.77,        // accVcaTau: ms, the fall of the accent's drive into the amp, measured slower than the MEG (0: the MEG's)
        2.26572,       // vegAttack: ms, the amp's attack
        1497.86,       // vegTau: ms, the VEG's fall while the gate is held (C42 1 uF, R123 1.5 M)
        -0.0440695,    // vegTarget: what it falls towards: the VCA's current ends before the capacitor is full
        1.141,         // gateHold: ms the amp stays open after the gate
        25.5916,       // gateFall: ms of the linear fall after the hold
        -120,          // bleed: dB, the VCA's leak while it is closed (the recordings' floor after a note)
        1.18964,       // vcaSat: the output's saturation level, of the voice's level (0: none)
        1.27108,       // satAsym: the negative side's limit relative to the positive one's (0: alike)
        22,            // slideTau: ms, the slide's RC (100k into 0.22 uF)
    };
    // the aggregate is positional: a field added to VA303_PARAMS needs this file written again
    static_assert(numParams == 59, "regenerate with lib/va303/fit/gen_params.py");
}
