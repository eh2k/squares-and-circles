# FILTER/SVF

<img align="right" src="SVF.png" width=256px />

Stereo 4-pole lowpass set up like a bass synth filter section: envelope on the cutoff, accent, a gated VCA and
overdrive. Each channel is two Chamberlin state-variable lowpasses in series, stepped twice per sample (musicdsp.org
"State Variable Filter (Chamberlin version)" and the EarLevel article, both linked in the code).

```
input L/R -> SVF lowpass -> SVF lowpass -> VCA (gate, accent) -> overdrive (tanh) -> output L/R
                  ^
cutoff = Cutoff + EnvMod * envelope + accent
```

## Parameters

| Knob | Range | Default | |
|---|---|---|---|
| **Cutoff** | ~480 Hz ... 16 kHz | ~1.3 kHz | base cutoff, 5 octaves over the knob |
| **Reso** | 0 ... 1 | 0.6 | resonance |
| **EnvMod** | 0 ... 5 octaves | 1.5 octaves | envelope depth |
| **Decay** | 5.2 ms ... hold | 5.2 ms | envelope decay: 10 ms at 0.5, 52 ms at 0.9, 0.5 s at 0.99, no decay at 1 |
| **Overdrive** | off, gain 1 ... 9 | off | tanh saturation after the VCA |

Left encoder: Cutoff, EnvMod, Overdrive. Right encoder: Reso, Decay.

Cutoff, envelope and accent together top out at 16 kHz.

## Usage

The audio source is set on the I/O page: **#&lt;track&gt; &lt;engine&gt;** for other tracks (their AUDIO-OUT to `-` to
hear only the filter), or **AUX-INPUT** with a jack or `USB` and **AUX-LEVEL** as the level.

- TRIG-INPUT (or a MIDI note) restarts the filter envelope and opens the VCA with a short fade-in. The VCA follows
  the gate and closes (about 4 ms time constant) after it goes low. With TRIG-INPUT set to `-` the VCA stays open.
- ACCENT-INPUT: if it fires together with a trigger, that note gets accented: cutoff +0.4 octave, a bit more
  resonance, double level. The accent decays in about 21 ms.
- No V/OCT input, the cutoff doesn't track pitch.

Without accent the output is at half level (-6 dB); only the start of an accented note reaches full level.

Code 2.7 KB, RAM 252 bytes.

## License and credits

- `SVF.cpp`: © 2025 E.Heidt, MIT license (see [License](../../../README.md#license)).
