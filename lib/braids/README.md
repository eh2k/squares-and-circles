# Synthesis Models

## Classic Analog Waveforms

| #  | Model       | Description                   | Timbre             | Color               |
|----|-------------|-------------------------------|--------------------|---------------------|
| 0  | `CSAW`      | CS-80 imperfect saw           | Notch width        | Notch polarity      |
| 1  | `/\/\|-_-_` | Variable waveshape            | Waveshape          | Distortion/filter   |
| 2  | `/\|/\|-_-_`| Classic saw-tooth/square      | Pulse width        | Saw to square       |
| 3  | `FOLD`      | Sine/triangle into wavefolder | Wavefolder amount  | Sine to triangle    |

---

## Digital Synthesis

| #  | Model       | Description                          | Timbre                   | Color                   |
|----|-------------|--------------------------------------|--------------------------|-------------------------|
| 4  | `_\|_\|_\|_`| 2 detuned harmonic combs             | Smoothness               | Detune                  |
| 5  | `SUB-_`     |                                      |                          |                         |
| 6  | `SUB/_`     |                                      |                          |                         |
| 7  | `SYN-_`     | 2 VCOs with hardsync                 | VCO frequency ratio      | VCO balance             |
| 8  | `SYN/\|`    | 2 VCOs with hardsync                 | VCO frequency ratio      | VCO balance             |
| 9  | `/\|/\| x3` | Triple saw waves                    | Osc. 2 detune            | Osc. 3 detune           |
| 10 | `-_ x3`     | Triple square waves                 | Osc. 2 detune            | Osc. 3 detune           |
| 11 | `/\ x3`     | Triple triangle waves               | Osc. 2 detune            | Osc. 3 detune           |
| 12 | `SI x3`     | Triple sine waves                   | Osc. 2 detune            | Osc. 3 detune           |
| 13 | `RING`      | 3 ring-modulated sine waves          | 2/1 frequency ratio      | 3/1 frequency ratio     |
| 14 | `/\|/\|/\|/\|` | Swarm of 7 sawtooth waves         | Detune                   | High-pass filter        |
| 15 | `/\|/\|_\|_\|` | Comb filtered sawtooth           | Delay time               | Neg./pos. feedback      |
| 16 | `TOY*`      | Low-fi circuitbent sounds            | Sample reduction         | Bit toggling            |
| 17 | `ZLPF`      | Direct synthesis of LP waveform      | Cutoff frequency         | Waveshape               |
| 18 | `ZPKF`      | Direct synthesis of Peaking waveform | Cutoff frequency         | Waveshape               |
| 19 | `ZBPF`      | Direct synthesis of BP waveform      | Cutoff frequency         | Waveshape               |
| 20 | `ZHPF`      | Direct synthesis of HP waveform      | Cutoff frequency         | Waveshape               |
| 21 | `VOSM`      | Sawtooth with 2 formants             | Formant 1 frequency      | Formant 2 frequency     |

---

## Vocal Synthesis

| #  | Model       | Description                      | Timbre        | Color              |
|----|-------------|----------------------------------|---------------|--------------------|
| 22 | `VOWL`      | Vowel synthesis (a, e, i, o, u) | Vowel shape   | Gender             |
| 23 | `VFOF`      | Hi-fi vowel synthesis           | Air pressure  | Instrument shape   |

---

## Additive Synthesis

| #  | Model       | Description                | Timbre          | Color               |
|----|-------------|----------------------------|-----------------|---------------------|
| 24 | `HARM`      | Additive synth, 14 harmonics | Harmonic #      | Spectral peakedness |

---

## Frequency Modulation (FM)

| #  | Model       | Description                            | Timbre                | Color                |
|----|-------------|----------------------------------------|-----------------------|----------------------|
| 25 | `FM`        | Plain 2-operator FM                   | Modulation index      | Frequency ratio      |
| 26 | `FBFM`      | Feedback 2-operator FM                | Modulation index      | Frequency ratio      |
| 27 | `WTFM`      | Chaotic 2-operator FM                 | Modulation index      | Frequency ratio      |

---

## Physical Simulations

| #  | Model       | Description                 | Timbre                | Color               |
|----|-------------|-----------------------------|-----------------------|---------------------|
| 28 | `PLUK`      | Plucked strings             | Decay                 | Plucking position   |
| 29 | `BOWD`      | Bowed string                | Friction              | Bowing position     |
| 30 | `BLOW`      | Reed simulation             | Air pressure          | Instrument geometry |
| 31 | `FLUT`      | Flute simulation            | Air pressure          | Instrument geometry |

---

## Percussions

| #  | Model       | Description                 | Timbre                | Color               |
|----|-------------|-----------------------------|-----------------------|---------------------|
| 32 | `BELL`      | Bell sound                  | Decay                 | Harmonicity         |
| 33 | `DRUM`      | Metallic drum sound         | Decay                 | Harmonicity         |
| 34 | `KICK`      | 808 bass drum               | Decay                 | Brightness          |
| 35 | `CYMB`      | Cymbal noise                | Cutoff                | Noisiness           |
| 36 | `SNAR`      | 808 snare drum              | Tone                  | Noisiness/decay     |

---

## Wavetables

| #  | Model       | Description                   | Timbre                | Color               |
|----|-------------|-------------------------------|-----------------------|---------------------|
| 37 | `WTBL`      | 21 wavetables                | Smooth wavetable position | Quantized selection |
| 38 | `WMAP`      | 16x16 waves                 | X position            | Y position          |
| 39 | `WLIN`      | Linear wavetable scanning   | Wavetable position    | Interpolation quality |
| 40 | `WTx4`      | Polyphonic wavetable        | Wavetable position    | Chord type          |

---

## Noise

| #  | Model       | Description                 | Timbre                | Color               |
|----|-------------|-----------------------------|-----------------------|---------------------|
| 41 | `NOIS`      | Tuned noise (2-pole filter) | Filter resonance      | Response (LP to HP) |
| 42 | `TWNQ`      | Noise sent to 2 resonators  | Resonance             | Resonators freq. ratio |
| 43 | `CLKN`      | Clocked digital noise       | Cycle length          | Quantization        |
| 44 | `CLOU`      | Sinusoidal granular synthesis | Grain density       | Frequency dispersion |
| 45 | `PRTC`      | Droplets granular synthesis | Grain density         | Frequency dispersion |
| 46 | `QPSK`      | Modem noises                | Bit-rate              | Modulated data      |

## EASTER EGG

| #  | Model       | Description                 | Timbre                | Color               |
|----|-------------|-----------------------------|-----------------------|---------------------|
| 47 | `****`      |                             |                       |                     |

## Braids Renaissance Chords (https://burns.ca/eurorack.html)

| #  | Model       | Description                 | Timbre                | Color               |
|----|-------------|-----------------------------|-----------------------|---------------------|
| 48 | `//CH`      |                             |                       |                     |
| 49 | `-_CH`      |                             |                       |                     |
| 50 | `/\\CH`     |                             |                       |                     |
| 51 | `SICH`      |                             |                       |                     |
| 52 | `WTCH`      |                             |                       |                     |
| 53 | `//x6`      |                             |                       |                     |
| 54 | `-_x6`      |                             |                       |                     |
| 55 | `/\\x6`     |                             |                       |                     |
| 56 | `SIx6`      |                             |                       |                     |
| 57 | `WTx6`      |                             |                       |                     |