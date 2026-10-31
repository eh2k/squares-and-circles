#pragma once

#include "open303/src/rosic_Character.h"

// The character TB03 of SYNTH/XX-303, fitted (least squares on 1/6-octave band levels in 10 ms frames) to
// recordings of the TB-03 over random knob settings and note patterns. Written by a script from the fit's parameters,
// do not edit.

static const rosic::Character characterTB03 = {
    {3.11837238, 2.13360583, 0.133373044, 0.545677729, 0.284505081}, // decay: exp(decay[0] + decay[1]*d + decay[2]*d*d + decay[3]*c + decay[4]*c*m) ms
    60.0796703, false, // accentDecay, accentDecayFromKnob
    1.01156317, 7.65632609, 2.07320616, // envAttack, accentAttack, accentSweepAttack
    86.5997249, 0.100150439, // accentLevelDecay, accentLevelThreshold
    {8.29323055, -0.295014644, 0.854344725, 1.72024261, -1.08429038, 0.566625267, 0.676040559, 0.220672128, -2.84614197, -0.327642499, -0.0476512538, 2.71498966, 2.28045231, -2.0639926, -2.1280063}, // cutoffBase
    {0.274814706, 0.726802865, -0.0876673523, 2.48496706, -1.03540913, 0.309531188, 0.167475943, -0.622709253, 1.54032995, 1.75369158}, // cutoffEnv
    {2.01911977, 0.0186100735, -0.0621941904, -0.981085657, 0.0037098986}, // cutoffAccent
    4.32192809, 14.1827163, // cutoffMin, cutoffMax
    2.98241977, 0.50159374, // envShape, envShapeNorm
    false, // resonanceSkew
    {0.00796327537, -0.237425382, 1.91804572, -1.19510492, 0.0521912799, 0.0502035617, 0.349970996, 0.0928109793, -0.0484782586, 0, 0, 0.0146301045, -0.0548025541}, // resonance
    {-1.83049555, 0.254954192, 0.52042393, 1.48653624, -0.480917225, -0.131272598, 2.0948057, -0.902142209, 0.112585457, -0.316021887}, -0.034845135, // gain (log2), gainAccent
    2846.39511, -0.235604851, // ampDecay, ampTarget
    0.668509321, 5.10975902, 5.10975902, // gateAttack, gateRelease, gateReleaseAccent
    false, 0.0, // hardGate, ampSmoothing
    -0.481043728, 0, {0.0169779446, 139.879119}, // ampEnvGated, ampAccentGated, ampAccent
    17.9844135, 1.01660591, true, // slideTime, slide2, legatoSlides
    19.9372181, 19.9827083, 1.2472477, // highpass1, highpass2, feedbackHighpass
    {0.507482245, 0.0770079937, -0.0226959102}, {0.679632108, 0.167622704}, 0.625816032, // pulseWidth, pulseSag, pulseGain
    0, // pulseSmoothing
    1  // tableOffset
};
