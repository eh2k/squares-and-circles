#pragma once

#include "open303/src/rosic_Character.h"

// The character TB303 of SYNTH/XX-303: the analog Roland TB-303. Its structure and its times from its circuit (the
// schematic of the service notes, as Kevin Lightner redrew it) and Robin Whittle's measurements (Devil Fish manual,
// "Investigating some unique aspects of the TB-303's sound"): the Accent Sweep Circuit, the volume envelope, the gate,
// the glide. What the schematic leaves open (the knobs' curves of cutoff, resonance, env mod and accent, the levels,
// the depths) fitted to recordings of the TB-03, Roland's recreation of the TB-303, as there are no recordings of a
// TB-303 here; the pulse is the TB03 character's. The fitted values written by a script from the fit's parameters,
// the comments by hand.

static const rosic::Character characterTB303 = {
    // MEG: "DECAY VR MAX T=2.5 sec, MIN T=200 ms", T from 100 % to 10 %: tau = T / ln 10 = 86.9 ... 1086 ms
    {4.46428179, 2.52572864, 0.0, 0.0, 0.0}, // decay: 86.9 ms * 12.5^d
    86.8588964, false, // accentDecay: the Decay pot shorted, its minimum; accentDecayFromKnob
    0.088805681, 0.251155498, 0.0, // envAttack, accentAttack (fitted; the accent's current into the VCA through R120
                        // 22k, C36 0.033 uF, R119 47k: 0.5 ms), accentSweepAttack (the circuit instead)
    86.8588964, 0.0950888768, // accentLevelDecay: the MEG on accented notes; accentLevelThreshold (fitted)
    {8.02870042, -0.228028186, 0.88882693, 1.68734175, -1.00847398, 0.649442427, 0.744991009, 0.115254178, -3.24045318, -0.398920631, -0.0719938825, 2.52377011, 2.28836646, -2.04408224, -1.99850967}, // cutoffBase (fitted)
    {0.266136433, 0.768795511, -0.0374129924, 2.09172153, -1.00841645, 0.301956268, 0.194789279, -0.607893518, 1.64854866, 1.85389652}, // cutoffEnv (fitted)
    {9.20027012, 1.19691699, -0.0141299021, -0.0120227983, -0.00632181146}, // cutoffAccent (fitted; from the circuit: 9.9 + 1.2c octaves)
    4.32192809, 14.1827163, // cutoffMin, cutoffMax
    2.74356054, 0.525096713, // envShape, envShapeNorm (fitted)
    false,              // resonanceSkew
    {0.0663906552, -0.270164863, 1.75908672, -1.20040531, 0.0764070572, 0.0516598558, 0.355777617, 0.177792145, 0.0357185843, -0.0158556382, -0.00631080743, -0.0538369516, -0.157163605}, // resonance (fitted)
    {-0.478319012, -1.49259988, 1.26378373, 0.726441524, 0.135990012, 0.215654995, 0.509461998, -0.39262326, -0.308017656, -0.0590042456}, 0.24449295, // gain (log2, fitted), gainAccent
    1500.0, -0.13, // ampDecay: VEG, C42 1 uF charging through R123 1.5M; ampTarget: the VCA's current (through Q31,
                        // R131 220k) ends before C42 is full: silent after about 3.3 s ("~3 to 4 s")
    1.4, 3.0, 3.0, // gateAttack: 3 ms (10...90 %), gateRelease, gateReleaseAccent: after the hold, the TB-303's 8 ms
                        // linear fall as a one-pole
    false, 0.0,         // hardGate, ampSmoothing
    0.0, 0.0, {0.0267353728, 91.0872427}, // ampEnvGated, ampAccentGated: the MEG drives the VCA only on accented
                        // notes, and not gated: ampAccent (fitted; at Acc 1 2.5 x the normal level, as in the circuit)
    22.0, 0.0, true,    // slideTime: the DAC's 100k into C35 0.22 uF; slide2; legatoSlides
    44.486, 24.167, 1.32482463, // highpass1, highpass2 (Open303's: 32 Hz -5 dB on a TB-303), feedbackHighpass (fitted)
    {0.507482245, 0.0770079937, -0.0226959102}, {0.679632108, 0.167622704}, 0.625816032, // pulseWidth, pulseSag, pulseGain: the TB-03
                        // character's, its width with the pitch
    0.0,                // pulseSmoothing
    1,                  // tableOffset: up to half the Nyquist frequency
    // the Accent Sweep Circuit: the Accent pot's share of the MEG through D24 and R46 47k into the Resonance pot's
    // second section (100k), C13 1 uF at its other end, its wiper through R72 100k into the filter's control
    0.209145238, 0.496707148, 100.075031, // accentDiode, accentR1 (relative to the pot), accentC (ms): fitted; from the
                        // circuit 0.061 (0.6 V of 9.8 V), 0.47 (47k of 100k), 100 (100k x 1 uF)
    8.0                 // gateHold: "8 ms of normal volume" after the gate
};
