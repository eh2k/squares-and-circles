#pragma once

#include "open303/src/rosic_Character.h"

// The character ABL3 of SYNTH/XX-303, fitted (least squares on 1/6-octave band levels in 10 ms frames) to recordings
// of AudioRealism's ABL3 (the ABL3x VST plug-in, its default emulation, rendered over random knob settings and note
// patterns; only its audio was used), starting from the character TB303 (with its Accent Sweep Circuit). Its level
// brought to that of the character TB303 (the ABL3 itself is about 10 dB louder). Written by a script from the fit's
// parameters, do not edit.

static const rosic::Character characterABL3 = {
    {4.17798914, 2.22643167, -0.0467972372, 0.00199493883, -0.0266931492}, // decay: exp(decay[0] + decay[1]*d + decay[2]*d*d + decay[3]*c + decay[4]*c*m) ms
    69.4487537, false, // accentDecay, accentDecayFromKnob
    0.117383857, 14.1690383, 0, // envAttack, accentAttack, accentSweepAttack
    27.7609576, 0.00373400049, // accentLevelDecay, accentLevelThreshold
    {8.36834237, -0.0871588099, 0.947852912, 1.67132998, -1.05514482, 0.578535635, 0.771323503, 0.0302530885, -2.74380977, -0.486315547, -0.146862198, 2.73917572, 2.5692765, -1.91176301, -1.81932914}, // cutoffBase
    {0.45612533, 0.717455783, 0.0313114277, 2.32554044, -1.09131791, 0.276448913, 0.26373981, -0.755067623, 1.55184532, 1.6008048}, // cutoffEnv
    {9.47841008, 1.20443887, -0.00984327515, -0.00809809661, -0.00112455811}, // cutoffAccent
    4.32192802, 13.7498979, // cutoffMin, cutoffMax
    2.41208443, 0.564763285, // envShape, envShapeNorm
    false, // resonanceSkew
    {0.0469023346, -0.20991138, 1.68198167, -1.23678909, 0.230796432, 0.0844248499, 0.302675116, 0.270320143, 0.0477953036, -0.0280590215, -0.0879984179, -0.0567509632, -0.176454307}, // resonance
    {-1.85682637, 1.13959056, -0.750485579, 1.73221657, -0.381638498, -0.767995561, 1.42790434, -0.0357799517, -0.349326633, 0.513083775}, -0.459894225, // gain (log2), gainAccent
    1500.0, -0.0487975093, // ampDecay, ampTarget
    3.00854465, 3.58470647, 3.45408866, // gateAttack, gateRelease, gateReleaseAccent
    false, 0, // hardGate, ampSmoothing
    0, 0, {0.0425232373, 134.821533}, // ampEnvGated, ampAccentGated, ampAccent
    22.0, 0, true, // slideTime, slide2, legatoSlides
    44.506193, 24.649898, 1.32270103, // highpass1, highpass2, feedbackHighpass
    {0.514999936, 0.0590691774, -0.0439957135}, {0.743160203, 0.1650729}, 0.650858732, // pulseWidth, pulseSag, pulseGain
    48.7248554, // pulseSmoothing
    1, // tableOffset
    0.287812779, 0.576153089, 122.11922, // accentDiode, accentR1, accentC: the Accent Sweep Circuit
    8  // gateHold
};
