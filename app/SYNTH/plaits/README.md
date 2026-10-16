# SYNTH/plaits

<img align="right" src="plaits.png" width=256px />

Fourteen engines from Mutable Instruments Plaits ([lib/plaits](../../../lib/plaits)) in one binary: seven macro
oscillators, six drum voices and the virtual analog VCF engine from Plaits' second bank. The HARMONICS, TIMBRE and
MORPH knobs are named after what they do in each model, and Plaits' low-pass gate (LPG) and decay envelope come
with it.

## Engines

| Engine | |
|---|---|
| M-OSC/Virt.Analog | detuned variable square and variable saw |
| M-OSC/Waveshaping | slope oscillator into waveshaper and wavefolder |
| M-OSC/2-OP-FM | two-operator FM with feedback |
| M-OSC/Grain | granular formant oscillator, crossfades into the Z oscillator (AUX) |
| M-OSC/Harmonic | additive, 24 harmonics, crossfades into 8 organ harmonics (AUX) |
| M-OSC/Wavetable | four 8 x 8 wavetable banks, crossfades into a 5-bit version (AUX) |
| M-OSC/Chord | chords from divide-down string-synth oscillators or wavetable waves |
| DRUMS/Analog BD | analog bass drum with overdrive |
| DRUMS/909ish-BD | synthetic bass drum |
| DRUMS/Analog SD | analog snare |
| DRUMS/909ish-SD | synthetic snare |
| DRUMS/Analog HH | hi-hat: six square oscillators, resonant band-pass |
| DRUMS/Analog HH2 | hi-hat: three ring-modulated square pairs |
| SYNTH/ClassicVAVCF | VA oscillator with sub into a resonant 2- or 4-pole low-pass |

<img align="right" src="plaits_classic_vavcf.png" width=256px />
<img align="right" src="plaits_909.png" width=256px />

The screenshots show SYNTH/ClassicVAVCF and DRUMS/909ish-BD, the one at the top M-OSC/Virt.Analog.

## Parameters

### Common knobs

| Knob | Range | Default | |
|---|---|---|---|
| **V_OCT** | -4 ... +4 octaves | 0 (MIDI 36) | pitch (not on the drums) |
| **Decay** | 0 ... 1 | 0.5 | LPG and envelope decay. With the Trig input set to `-` it becomes **Level** and the engine drones |
| **Color** | 0 ... 1 | 0.5 | LPG colour: higher is brighter, toward a plain VCA |
| **AuxMix**, **PD-Mix** | 0 ... 1 | 0.5 | crossfade from Plaits' OUT to AUX |

Knob layout: M-OSC engines have six knobs (left encoder V_OCT, third knob, Decay; right encoder second, fourth,
last). Drums have four (left: Pitch, Tone; right: second knob, Decay). ClassicVAVCF: left V_OCT, Cutoff, EnvMod;
right Morph, Harsh, Decay.

### M-OSC/Virt.Analog

| Knob | Range | Default | |
|---|---|---|---|
| **Detune** | 0 ... 1 | 0.5 (unison) | saw against square, -24 ... +24 semitones, with plateaus at 0, 7, 12, 19, 24 |
| **Square** | 0 ... 1 | 0.5 | narrow pulse to square at 0.5, above that hard sync |
| **CSAW** | 0 ... 1 | 0.5 | triangle tilting into a saw, then a saw with a widening notch |

### M-OSC/Waveshaping

| Knob | Range | Default | |
|---|---|---|---|
| **Waveform** | 0 ... 1 | 0.8 | waveshaper curve |
| **Fold** | 0 ... 1 | 0.8 | wavefolder amount |
| **Asym** | 0 ... 1 | 0.75 | slope of the source, triangle at 0 |

### M-OSC/2-OP-FM

| Knob | Range | Default | |
|---|---|---|---|
| **Ratio** | 0 ... 1 | 0.8 | modulator frequency, -12 ... +36 semitones, with plateaus |
| **Mod** | 0 ... 1 | 0.8 | modulation index |
| **Feedb.** | 0 ... 1 | 0.75 | below 0.5 feedback into frequency, above into phase, none at 0.5 |

### M-OSC/Grain

| Knob | Range | Default | |
|---|---|---|---|
| **Ratio** | 0 ... 1 | 0.8 | OUT: second formant ±24 semitones. AUX: Z oscillator mode |
| **Frm/Fq.** | 0 ... 1 | 0.8 | formant frequency |
| **Width** | 0 ... 1 | 0.75 | carrier shape |
| **PD-Mix** | 0 ... 1 | 0.5 | OUT (grainlets) into AUX (Z oscillator) |

### M-OSC/Harmonic

| Knob | Range | Default | |
|---|---|---|---|
| **Bump** | 0 ... 1 | 0.8 | number of bumps in the spectrum |
| **Peak** | 0 ... 1 | 0.8 | the most prominent harmonic |
| **Shape** | 0 ... 1 | 0.75 | bump slope, flat and wide at 0, narrow at 1 |
| **AuxMix** | 0 ... 1 | 0.5 | OUT (24 harmonics) into AUX (organ harmonics) |

### M-OSC/Wavetable

| Knob | Range | Default | |
|---|---|---|---|
| **Bank** | 0 ... 0.5 | 0 | up to 0.43 moves through the four banks; above that Row and Column snap more and more to whole waves |
| **Row** | 0 ... 1 | 0.8 | TIMBRE, one axis of the bank |
| **Column** | 0 ... 1 | 0.75 | MORPH, the other axis |
| **AuxMix** | 0 ... 1 | 0.5 | OUT into AUX (5-bit) |

Without user data the fourth bank is a scrambled pick of the built-in waves.

### M-OSC/Chord

| Knob | Range | Default | |
|---|---|---|---|
| **Chord** | OCT, Fifth, min, min7, min9, min11, Maj, Maj7, Maj9, Sus4, 69, 6th, 10th, D7, D7(b9), HD, FD | Maj9 | chord (Jon Butler's table) |
| **Inv.** | 0 ... 1 | 0.5 | inversion and transposition |
| **Shape** | 0 ... 1 | 0.5 | below about 0.47 string-synth registrations, above about 0.53 wavetable waves |

### DRUMS/Analog BD and DRUMS/909ish-BD

| Knob | Range | Default | |
|---|---|---|---|
| **Pitch** | -1 ... +1 octave | 0 (MIDI 36) | pitch |
| **Drive** (Analog BD) | 0 ... 1 | 0.8 | attack FM, then self FM, overdrive above 0.5 |
| **Punch** (909ish-BD) | 0 ... 1 | 0.8 | FM envelope amount, then its decay |
| **Tone** | 0 ... 1 | 0.5 | tone |
| **Decay** | 0 ... 1 | 0.5 | decay |

### DRUMS/Analog SD and DRUMS/909ish-SD

| Knob | Range | Default | |
|---|---|---|---|
| **Pitch** | -1 ... +1 octave | 0 (MIDI 60) | pitch |
| **Snappy** | 0 ... 1 | 0.5 | snare noise level |
| **Tone** | 0 ... 1 | 0.5 | Analog SD: tone. 909ish-SD: FM amount |
| **Decay** | 0 ... 1 | 0.5 | decay |

### DRUMS/Analog HH and DRUMS/Analog HH2

| Knob | Range | Default | |
|---|---|---|---|
| **Pitch** | -1 ... +1 octave | 0 (MIDI 60) | pitch of the metallic noise |
| **Noise** | 0 ... 1 | 0.5 | blend toward clocked random noise |
| **Tone** | 0 ... 1 | 0.9 | band-pass/high-pass frequency, 150 Hz ... 16 kHz |
| **Decay** | 0 ... 1 | 0.6 | decay |

### SYNTH/ClassicVAVCF

| Knob | Range | Default | |
|---|---|---|---|
| **Morph** | 0 ... 1 | 0.5 | saw, square at 0.5, narrowing pulse, back to square. Sub octave fades in at both ends |
| **Cutoff** | 0 ... 1 | 0.5 | cutoff, tracks the note: 2 octaves below at 0, up to 8 above (max 12 kHz) |
| **Harsh** | 0 ... 1 | 0.5 | 4-pole on the left, 2-pole on the right, no resonance in the middle, more drive toward 1 |
| **EnvMod** | -1 ... +1 | 0 | envelope depth on the cutoff, up or down |
| **Decay** | 0 ... 0.99 | 0.5 | see above |

## Inputs and outputs

- Trig: pings the LPG and starts the envelope, or strikes the drum. With the Trig input set to `-`, oscillators
  play at the Level set by the Decay knob and the drums sound continuously (Plaits' unpatched mode).
- V/OCT on every engine. The drums have no V_OCT knob, so no -2 octave bias: Pitch and V/OCT add to MIDI 36 (BD)
  or 60 (SD, HH).
- Output is mono on both channels.

## Notes

- Runs at 48 kHz in 24-sample blocks. Plaits computes its envelope and LPG steps for 12-sample blocks, here they
  advance once per 24 samples. The 1 ms trigger delay of the original is off.
- The engine's memory comes from the heap, not the RAM figure below. If that fails it shows "OUT OF MEMORY" and
  stays silent.

Code 122 KB, RAM 1.6 KB.

## License and credits

- `plaits.cpp`: © 2022 E.Heidt, MIT license (see [License](../../../README.md#license)).
- [lib/plaits](../../../lib/plaits): Mutable Instruments Plaits, © Emilie Gillet, MIT license.
