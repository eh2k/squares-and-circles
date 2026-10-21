# FILTER/TeeBee

<img align="right" src="TeeBee.png" width=256px />

The TB-303 filter section from [Open303](../../../lib/open303/ReadMe.txt) (Robin Schmidt's rosic library) as a stereo
effect: the `TeeBeeFilter` diode ladder in TB_303 mode, Open303's envelope-to-cutoff mapping, and the fixed
high-pass, all-pass and notch filters around it. Each channel has its own filter, both follow one envelope.

```
input -> HP 44.5 Hz -> TB-303 ladder -> AP 14 Hz -> HP 24.2 Hz -> notch 7.5 Hz -> x3 -> output
                          ^
         cutoff = Cutoff * 2^(envelope sweep + Accent sweep)
```

## Parameters

| Knob | Range | Default | |
|---|---|---|---|
| **Cutoff** | 313 Hz ... 2.39 kHz | 866 Hz | nominal cutoff (Open303's range) |
| **Res** | 1 ... 100 % | 50 % | resonance, 100 % self-oscillates |
| **EnvMode** | 0 ... 1 | 0.5 | envelope depth (Env Mod) |
| **Decay** | 200 ms ... 2 s | 632 ms | envelope decay |
| **Accent** | 0 ... 1 | 1 | extra sweep: the envelope through a 30 ms RC, scaled by Accent |

Left encoder: Cutoff, EnvMode, Accent. Right encoder: Res, Decay.

EnvMode doesn't just add sweep on top, it also lowers the resting cutoff (Open303's fit of 303 measurements). At Cutoff 0.5 and
EnvMode 0.5 a trigger sends the cutoff up about 1.9 octaves and it settles about 0.9 octave below the nominal value.
Even at EnvMode 0 the envelope moves it a little.

## Usage

Feed it from the I/O page: **#&lt;track&gt; &lt;engine&gt;** for other tracks (AUDIO-OUT of those to `-` if you only
want the filtered sound), or **AUX-INPUT** with a jack or `USB`, level from **AUX-LEVEL**.

TRIG-INPUT, or a note on the track's MIDI channel, restarts the envelope on both channels. Accent is a knob only,
there is no accent or V/OCT input.

## Implementation

The TB_303 mode is a linear four-stage diode ladder ("ala mystran & kunn", with a link to Tim Stinchcombe's
analysis in the code) whose feedback goes through a 150 Hz high-pass, with coefficients from polynomial fits of
the cutoff. Runs at 48 kHz without oversampling. The filter's drive setting has no effect in this mode.

`TeeBee.cpp` takes Open303's code apart for speed (4.6x faster than the plain classes, DSM 58k -> 12.7k cycles a
frame), the output stays the same within 1 LSB of 16 bits (QEMU, 91 ... 103 dB SNR):
- the envelope, the cutoff and the ladder's coefficients are computed once for both channels, and only the
  TB_303 part of `TeeBeeFilter::calculateCoefficientsApprox4()`;
- the `exp()`s of Decay, Cutoff and Res only when the knob moves (a new Res also takes effect at once now, in Open303
  only with the next cutoff change);
- float instead of double, except the envelope (`y *= c` would drift in float);
- the 7.5 Hz notch as a state variable filter: the same response as Open303's RBJ biquad, which needs double that
  low;
- 2^x of the cutoff as a polynomial instead of `powf()` per sample.

Code 4.5 KB, RAM 480 bytes.

## License and credits

- `TeeBee.cpp`: © 2025 E.Heidt, MIT license (see [License](../../../README.md#license)).
- Open303 by Robin Schmidt (rs-met.com), MIT license.
