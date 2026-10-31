#ifndef rosic_CharacterOpen303_h
#define rosic_CharacterOpen303_h

#include "rosic_Character.h"

// [eh2k] Open303's own voicing (the original Open303 as SYNTH/Open303, now SYNTH/XX-303, ran it until 2026-10),
// written in the terms of the character, from its constants: Open303::calculateEnvModScalerAndOffset() (the measured mapping: cutoff
// 313.8...2394.4 Hz, envScaler and envOffset turned into the base and env polynomials, in octaves), the constructor
// (highpasses 44.486 and 24.167 Hz, feedback highpass 150 Hz, amp release 1 ms / 50 ms on accents, the de-clicker at
// 200 Hz, the amp's 0.45 and 4 x accent of the envelope while the note is on), the app (decay 200...2000 ms, accent
// decay = decay, accent attack 3 ms, slide 60 ms x 0.2, no amp decay), the TeeBeeFilter's 200 Hz floor and resonance
// skew, and MipMappedWaveTable::fillWithSquare303() (a pulse of width 0.469 at half level, its tanh edges as a lowpass
// at the 49th harmonic). Not the same in every bit: the square is a pulse from the saw table, the voice float; against
// the original 0.01 dB (saw) and 0.44 dB (square) in the measure of the characters' fit (the mean difference of the
// 1/6-octave band levels in 10 ms frames).

namespace rosic
{
  const Character characterOpen303 = {
    {5.29831736655, 2.30258509299, 0.0, 0.0, 0.0},  // decay: 200 * 10^d ms
    200.0, true,                          // accentDecay, accentDecayFromKnob
    0.0, 0.0, 3.0,                        // envAttack, accentAttack, accentSweepAttack
    100.0, 0.0,                           // accentLevelDecay, accentLevelThreshold (no accent level)
    {8.07681559705, 2.85859433164, -0.00615152005129, 0.0, 0.0, 0.0, 0.0, -1.11103131239, -0.30606428885,
     0.0, 0.0, -0.0203097110683, 0.0, 0.0, 0.0}, // cutoffBase
    {0.736965594166, 0.127379306476, 0.0, 3.77399632511, 0.4205524633, 0.0, 0.0, 0.0, 0.0, 0.0}, // cutoffEnv
    {1.0, 0.0, 0.0, 0.0, 0.0},            // cutoffAccent
    7.64385618977, 14.2877123795,         // cutoffMin, cutoffMax: 200 Hz, 20 kHz
    0.0, 0.0,                             // envShape, envShapeNorm: in octaves
    true,                                 // resonanceSkew
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, // resonance (unused)
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, 0.0, // gain, gainAccent
    1000.0, 1.0,                          // ampDecay, ampTarget: no decay
    0.0, 1.0, 50.0,                       // gateAttack, gateRelease, gateReleaseAccent
    true, 200.0,                          // hardGate, ampSmoothing
    0.45, 4.0, {0.0, 0.0},                // ampEnvGated, ampAccentGated, ampAccent
    12.0, 0.0, false,                     // slideTime, slide2, legatoSlides
    44.486, 24.167, 150.0,                // highpass1, highpass2, feedbackHighpass
    {0.468778667, 0.0, 0.0}, {1.0, 0.0}, 0.5, // pulseWidth, pulseSag, pulseGain
    49.0,                                 // pulseSmoothing: the tanh's soft edges, roughly
    2                                     // tableOffset
  };
}

#endif
