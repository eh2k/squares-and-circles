#ifndef rosic_Character_h
#define rosic_Character_h

#include "GlobalDefinitions.h"

namespace rosic
{

  /**

  [eh2k] A character of the voice: every constant of its control and the switches where voicings differ in
  structure, so that Open303::setCharacter() can change between them while it plays. The knobs as positions 0...1:
  c cutoff, q resonance, m env mod, d decay, a accent. Open303's own voicing is rosic_CharacterOpen303.h (from
  Open303's constants, the default); an app can bring others, e.g. fitted to recordings of another voice (see
  ../README.md).

  The fields in this order: the characters are written as aggregates.

  */

  struct Character
  {
    // the filter envelope: its time constant (ms) on normal notes exp(decay[0] + decay[1]*d + decay[2]*d*d
    // + decay[3]*c + decay[4]*c*m), on
    // accented notes accentDecay (or, with accentDecayFromKnob and the accent knob above 0, the normal one, as
    // the Open303 app sets it)
    real_t decay[5];
    real_t accentDecay;
    bool   accentDecayFromKnob;

    // the RCs after the envelope (ms): its attack, the accent's level and the accent's sweep of the cutoff
    real_t envAttack;
    real_t accentAttack;
    real_t accentSweepAttack;

    // the accent's level: an envelope of its own (ms), restarted by accented notes, above a threshold (0...1)
    real_t accentLevelDecay;
    real_t accentLevelThreshold;

    // log2 of the cutoff in Hz = base + env*shaped envelope + accent*accent sweep, within cutoffMin...cutoffMax:
    // base   = [0] + [1]c + [2]c^2 + [3]c^3 + [4]q + [5]q^2 + [6]cq + m([7] + [8]c + [9]q + [10]m + [11]c^2)
    //          + m^2 c([12] + [13]c + [14]mc)
    // env    = [0] + [1]c + [2]q + m([3] + [4]c + [5]q + [6]m) + [7]c^2 + m^2 c([8] + [9]c)
    // accent = a([0] + [1]c + [2]q + [3]m + [4]a)
    real_t cutoffBase[15];
    real_t cutoffEnv[10];
    real_t cutoffAccent[5];
    real_t cutoffMin;
    real_t cutoffMax;

    // the envelope's shape: log2(1 + envShape*envelope)*envShapeNorm, between octaves (envShape 0: the envelope
    // as it is) and Hz (a large envShape)
    real_t envShape;
    real_t envShapeNorm;

    // the TeeBeeFilter's skewed resonance = [0] + [1]q + [2]q^2 + [3]q^3 + c([4] + [5]q + [6]q^2) + c^2([9] + [10]q)
    // + m([11] + [12]q) + q([7] + [8]q)*envelope, or with resonanceSkew Open303's (1 - e^-3q)/(1 - e^-3)
    bool   resonanceSkew;
    real_t resonance[13];

    // the gain, log2 (at the app's volume of -12 dB) = [0] + [1]q + [2]q^2 + [3]c + [4]cq + [5]c^2 + m([8] + [9]q)
    // + ([6] + [7]q)*envelope + gainAccent*accent level
    real_t gain[10];
    real_t gainAccent;

    // the amp: gate * amp envelope + gate' * (ampEnvGated*envelope + ampAccentGated*a*envelope on accented notes)
    // + ampAccent[0]*(a + ampAccent[1]*a^2)*accent level, then (ampSmoothing > 0) a 2-pole lowpass of that many Hz.
    // gate: one-poles of gateAttack and gateRelease ms (gateReleaseAccent after accented notes); gate' the same, or
    // with hardGate the gate itself (on or off at once). The amp envelope: 1 on a note, then a one-pole of ampDecay
    // ms towards ampTarget, held at 0.
    real_t ampDecay;
    real_t ampTarget;
    real_t gateAttack;
    real_t gateRelease;
    real_t gateReleaseAccent;
    bool   hardGate;
    real_t ampSmoothing;
    real_t ampEnvGated;
    real_t ampAccentGated;
    real_t ampAccent[2];

    // the glide: two one-poles, slideTime and slide2*slideTime ms (slide2 0: one); legatoSlides: a note that starts
    // while the gate is still on slides to its pitch, else it starts anew
    real_t slideTime;
    real_t slide2;
    bool   legatoSlides;

    // the highpasses before and after the filter, and in its feedback (Hz)
    real_t highpass1;
    real_t highpass2;
    real_t feedbackHighpass;

    // the pulse (setWaveform(1)): pulseGain*(saw(t) - sag*saw(t + width)), width = [0] + [1]x + [2]x^2,
    // sag = [0] + [1]x, x the octaves above C2 (held to -1.5...2), through a one-pole lowpass at pulseSmoothing
    // times its frequency (0: none; soft edges); the wavetables up to nyquist/2 (tableOffset 1) or nyquist/4 (2)
    real_t pulseWidth[3];
    real_t pulseSag[2];
    real_t pulseGain;
    real_t pulseSmoothing;
    int    tableOffset;

    // the accent's sweep as the TB-303's circuit (accentC > 0; else the RC accentSweepAttack above): the envelope
    // times the accent knob on accented notes, less a diode's drop (accentDiode), through accentR1 into one end of a
    // pot that is the resonance knob's second section (q 0: its wiper at that end), a capacitor at its other end,
    // the wiper through a resistor as large as the pot into the filter's control; accentR1 relative to the pot,
    // accentC the time constant of the pot and the capacitor (ms). The sweep is the wiper's voltage, cutoffAccent
    // its depth in octaves (without the factor a). The capacitor keeps its charge from one accent to the next.
    real_t accentDiode;
    real_t accentR1;
    real_t accentC;

    // the amp's gate stays on for gateHold ms after the gate (then gateRelease)
    real_t gateHold;
  };

}

#endif
