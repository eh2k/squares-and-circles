#pragma once

#include "open303/src/rosic_Character.h"

// The character RB303 of SYNTH/XX-303, fitted (least squares on 1/6-octave band levels in 10 ms frames) to
// recordings of ReBirth RB-338's 303 over random knob settings and note patterns. Written by a script from the fit's
// parameters, do not edit.

static const rosic::Character characterRB303 = {
    {3.52837067, 2.78575659, 0.345702674, 0.370801855, 0.126332785}, // decay: exp(decay[0] + decay[1]*d + decay[2]*d*d + decay[3]*c + decay[4]*c*m) ms
    55.2137701, false, // accentDecay, accentDecayFromKnob
    0.998500694, 18.4229122, 2.10711165, // envAttack, accentAttack, accentSweepAttack
    97.4729202, 0.418763469, // accentLevelDecay, accentLevelThreshold
    {8.19739276, 2.07829896, -1.1329796, 1.24009643, 0.0457565157, 0.151978966, 1.06391025, -0.913009776, 0.641504658, -0.620558408, -0.0679508489, -2.24512575, -0.446796455, 3.33665593, -2.27528939}, // cutoffBase
    {0.497252146, 0.763303959, 0.00661575068, 3.41644754, -1.05922048, 0.316927557, 0.152124563, -0.610988426, 1.47881866, 1.63233872}, // cutoffEnv
    {1.96143131, 0.0322721858, -0.0790906207, -0.970691936, 0.000864447993}, // cutoffAccent
    4.32192809, 13.4457076, // cutoffMin, cutoffMax
    2.75916661, 0.523447063, // envShape, envShapeNorm
    false, // resonanceSkew
    {0.159672017, 0.054316677, 1.29400068, -0.989661703, -0.307851549, 0.951604289, 0.0616052119, 0.152771318, -0.0273964894, 0.0941275442, -0.484154694, 0.0940864895, -0.377372007}, // resonance
    {-1.40538125, 2.64496765, -1.39169922, 1.0742212, -0.5027612, -0.114402206, 1.61707459, -0.763457548, -0.0606357928, 0.127645008}, 0.487882417, // gain (log2), gainAccent
    15847.393, -6.35563792e-12, // ampDecay, ampTarget
    0.181763571, 22.4767154, 22.4767154, // gateAttack, gateRelease, gateReleaseAccent
    false, 0.0, // hardGate, ampSmoothing
    -0.366281663, 0, {-0.00613419724, -478.238345}, // ampEnvGated, ampAccentGated, ampAccent
    17.2743222, 1.03026456, true, // slideTime, slide2, legatoSlides
    20.2328378, 20.1998201, 1.24706554, // highpass1, highpass2, feedbackHighpass
    {0.520192223, 0.0473753833, -0.045987975}, {0.779003002, 0.112229704}, 0.596721718, // pulseWidth, pulseSag, pulseGain
    0, // pulseSmoothing
    1  // tableOffset
};
