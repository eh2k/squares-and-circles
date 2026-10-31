# open303

Open303 (Robin Schmidt, MIT license; its source and the changes in [ReadMe.txt](ReadMe.txt)) with characters: its
oscillator, filter and envelope classes, with a control whose every constant comes from a character
(`rosic_Character.h`), switched with `Open303::setCharacter()` (with the next note: in the middle of one it would
click). The library has one, **Open303** (`rosic_CharacterOpen303.h`, Open303's own voicing from its constants), the
default. Others come with the app that uses them, e.g. fitted to recordings of another voice or taken from a circuit:
[SYNTH/XX-303](../../app/SYNTH/XX-303/README.md) brings its own (`characters/`), Open303 its default.

## Characters

A character holds the constants of the control below and the switches where voicings differ in structure: the
envelope in octaves or partly in Hz (`envShape`), the accent decay from the decay knob, Open303's resonance curve
(`resonanceSkew`), the amp's hard gate, its de-clicker, accent release and hold, the legato slide, the TB-303's accent
sweep circuit, the pulse's width, sag and softness, the table limit. It is written as an aggregate, its fields in their
order; fields added later are at the end, so a character written before has them 0 (off).

The character Open303 against the original Open303 (as SYNTH/Open303, now SYNTH/XX-303, ran it until 2026-10):
0.01 dB (saw), 0.44 dB (square) in the measure of the characters' fit (the mean difference of the 1/6-octave band
levels in 10 ms frames). Open303's square is the pulse of width 0.469 with soft edges (a lowpass at the 49th
harmonic) rather than its tanh table: the 0.44 dB.

An app keeps its characters and their list itself; a character must stay valid while it is used:

```cpp
#include "open303/src/rosic_Open303.h"
static const rosic::Character myCharacter = { /* the fields of rosic_Character.h in their order */ };
open303.setCharacter(&myCharacter); // or &rosic::characterOpen303
```

## What is changed against the original

Until 2026-10 lib/open303 was the original's DSP code with the changes of [ReadMe.txt](ReadMe.txt) up to 2022 (the
version before is in the repository's history, e.g. aac5429). Since then, without what the voice no longer uses (the
sequencer, the precomputed tables SAW303.h and SQUARE303.h, AnalogEnvelope, EllipticQuarterBandFilter, the xbm), and
changed in these files (the changed lines marked `[eh2k]`):

- `rosic_Open303.h/.cpp`: the control (see below), rewritten, and `setCharacter()`. The setters keep Open303's units
  (cutoff 314...2394 Hz, decay 200...2000 ms, resonance, env mod and accent in percent) and take them as knob
  positions 0...1; they only mark the control changed, it is calculated once before the next sample or note.
  `triggerNote()` takes a fractional note, and slides while the gate is still on if the character says so
  (`legatoSlides`). `getSample(&input)` takes an external sample in place of the oscillator's (the voice as a filter
  and VCA, SYNTH/XX-303's Wave AUX). Gone: the AnalogEnvelope amp envelope, the anti-aliasing filter, the oversampling, the sequencer,
  and the Devil Fish setters the characters replace (amp decay, release and sustain, normal attack, accent decay, the
  highpass cutoffs).
- `rosic_Character.h`, `rosic_CharacterOpen303.h` (new).
- `rosic_BlendOscillator.h/.cpp`: the pulse, made from two reads of the saw table, `pulseGain * (saw(t) -
  sag * saw(t + width))`, its width and sag following the pitch (`calculatePulse()`, when the frequency changes),
  optionally through a lowpass that follows the pitch; the table offset from the character (Open303's: tables up to a
  quarter of the Nyquist frequency, it ran 4x oversampled).
- `rosic_BiquadFilter.cpp`: only the modes Open303 uses (LOWPASS12, BANDREJECT), the others bypass.
- `rosic_OnePoleFilter.cpp`: only the modes Open303 uses (HIGHPASS, ALLPASS), the others bypass; `calcCoeffs()` and
  `LeakyIntegrator::calculateCoefficient()` not inlined (the apps' binaries are smaller, the sound the same).
- `wavetable_gen/rosic_MipMappedWaveTable.cpp`: `generateMipMap()` takes its spectrum (8 KB) from the heap, not the
  stack, and returns false without it.
- `rosic_TeeBeeFilter.h`: `setResonanceSkewed()` (the resonance without the `exp()`), a cutoff floor of 20 Hz instead
  of 200 Hz (the character's `cutoffMin` sets it), and in the TB_303 mode only the coefficients it uses.

## The control

Per sample, at 48 kHz; c, q, m, d, a are the cutoff, resonance, env mod, decay and accent knobs (0...1), the
character's fields in brackets:

- **Filter envelope**: Open303's DecayEnvelope, restarted on each note (not on slides), its time constant
  exp(polynomial of d, c and c x m) ms (`decay`), on accented notes `accentDecay` (or, with `accentDecayFromKnob`, the
  normal one). An RC for its attack (`envAttack`).
- **Cutoff**: log2 of the TeeBeeFilter's cutoff in Hz = base(c, q, m) + env(c, q, m) x shaped envelope + accent(c, q,
  m, a) x accent sweep, within `cutoffMin` and `cutoffMax`; base, env and accent depth are polynomials (`cutoffBase`,
  `cutoffEnv`, `cutoffAccent`). The envelope is shaped `log2(1 + k e) / log2(1 + k)` (`envShape` k; 0: in octaves, as
  Open303): it moves the cutoff partly in Hz, fast at the top and slow near the base. The accent sweep is the
  envelope through an RC (`accentSweepAttack`), on accented notes only, or (`accentC` > 0) the TB-303's Accent Sweep
  Circuit: the envelope times the accent knob on accented notes, less a diode's drop (`accentDiode`), through a
  resistor (`accentR1`) into one end of a pot that is the resonance knob's second section, a capacitor at its other
  end (`accentC` the time constant of pot and capacitor), from the wiper through a resistor as large as the pot into
  the cutoff. With q = 0 the sweep is about the envelope itself, with q = 1 it comes from the capacitor, which charges
  slowly and keeps its charge from one accent to the next (each one sweeps higher). Per sample: the diode conducts or
  not, the capacitor is a one-pole towards its target; the coefficients when q changes.
- **Resonance**: the TeeBeeFilter (TB_303 mode, its feedback highpass `feedbackHighpass`) gets a skewed resonance that
  is a polynomial of q, c and m, and of q times the envelope (`resonance`), or Open303's curve (`resonanceSkew`).
- **Amp**: a gate (one-poles `gateAttack` up, `gateRelease` down `gateHold` ms after the gate ends,
  `gateReleaseAccent` after an accent; or Open303's hard gate with its de-clicker, `hardGate`, `ampSmoothing`) times
  the amp envelope (1 on a note, a one-pole of
  `ampDecay` towards `ampTarget`, held at 0) plus some of the filter envelope (`ampEnvGated`, `ampAccentGated`), then
  the accent's level, which is not gated: an envelope of its own (`accentLevelDecay`, restarted by accented notes)
  above a threshold (`accentLevelThreshold`), through an RC (`accentAttack`), times `ampAccent[0] (a + ampAccent[1]
  a^2)`.
- **Gain**: exp2 of a polynomial of q, c and m plus terms of the envelope and the accent (`gain`, `gainAccent`), the
  level at Open303's default volume (-12 dB).
- **Glide**: two one-poles in series (`slideTime`, the second `slide2` times as long).
- **Highpasses**: before and after the filter (`highpass1`, `highpass2`).
