# aanl

Faust's antialiased nonlinearities, [aanl.lib](aanl.lib) (the `aa` library of the Faust libraries, version 1.6.0, as
it came from [grame-cncm/faustlibraries](https://github.com/grame-cncm/faustlibraries/blob/master/aanl.lib) at
a95107fd, 2026-10-09; Dario Sanfilippo, David Braun, MIT), ported to C++ for the apps: [aanl.h](aanl.h), one
header.

A waveshaper makes harmonics above Nyquist, and they fold back as aliases. Antiderivative antialiasing (ADAA) takes
the curve's antiderivative instead of the curve: first-order ADAA puts out `(F1(x) - F1(x')) / (x - x')`, the mean
of the curve over the straight line between the last two samples, second-order the same with a triangle over three
samples (Parker, Zavalishin, Le Bivic, DAFx 2016). That costs an antiderivative and a division a sample, half a
sample of delay (one for second-order) and a little of the top of the band (first-order: `cos(pi f / fs)`, -3 dB at
fs / 4), and it does best with low oversampling beside it, as aanl.lib says.

## What is here

| | |
|---|---|
| `ADAA1`, `ADAA2` | the generic ones, for a curve of one's own: `y = adaa(eps, f, F1, x)` (and `F2`, the antiderivative of x f(x)) |
| `hardclip`, `hardclip2` | clip to ±1 |
| `cubic1` | x - x³/3, ±2/3 beyond ±1 |
| `parabolic`, `parabolic2` | x (1 - \|x\|/4), ±1 beyond ±2 |
| `hyperbolic`, `hyperbolic2` | x / (1 + \|x\|) |
| `sinarctan`, `sinarctan2` | x / sqrt(1 + x²) |
| `sinarctanStable` | the same, first-order, written without a quotient of differences (exact in float) |
| `sinarctanTransparent` | the same with a flat response where it is linear (an allpass, 0.71 samples late) |
| `softclipQuadratic1`, `softclipQuadratic2` | 2x up to ±1/3, a parabola to ±1 at ±2/3 |
| `tanh1` | tanh(x) |
| `arctan`, `arctan2` | atan(x) |
| `asinh1`, `asinh2` | asinh(x), unbounded |

The names are aanl.lib's: a trailing 2 is second-order, the others are first-order. Each is an object that keeps
the samples before, one per signal:

```cpp
#include "aanl/aanl.h"
static aanl::hardclip2 _clip;
out[i] = _clip(in[i] * gain);
```

Not ported: aanl.lib's wavefolders (`triangleFold`, `sineFold`) and its trigonometry (`cosine`, `arcsin`, ...).
SYNTH/XX-303's Drive has the sine folder's case t = 1, r = 1 inline, with the closed forms aanl.lib gives for it, and
uses `tanh1`, `hardclip` and `asinh1` from here.

## Float

aanl.lib wants double ("it is vital to use double-precision or more"); the apps compute in float, and so does
aanl.h (`AANL_DOUBLE` makes it double, for tests). The quotient `(F1(x) - F1(x')) / (x - x')` loses digits when the
input is large and moves little: the error is about 1e-7 |F1(x)| / |x - x'|, and the threshold below which the
curve is taken at the middle instead (aanl.lib's 0.001) decides how large it gets. For SYNTH/XX-303's diode, at
96 kHz with the voice's levels, 0.01 keeps the error below -92 dB of the output (0.001: -75 dB) and the aliasing the
same; [its Drive](../../app/SYNTH/XX-303/drive/Drive.h) passes 0.01 to `ADAA1`. `sinarctanStable` and
`sinarctanTransparent` have no such quotient.

Two functions are written differently from aanl.lib, with the same numbers: tanh1's `log(cosh(x))` as
`|x| + log(1 + e^(-2|x|)) - log(2)` (aanl.lib limits cosh() to the largest number, and its quotient falls to 0
beyond |x| = 89 in float), and `asinh(x)` as `sign(x) log(|x| + sqrt(x² + 1))`, which keeps its digits for large
negative x. The math goes through `aanl::_sqrt()`, `_log()`, ..., which take `sqrtf()`, `logf()`, ... by name, so
that an app's own `sqrt()` overloads (lib/open303's `rosic` namespace) don't get in the way.

## Test

[aanl_test.cpp](aanl_test.cpp), in double: every F1 is the antiderivative of its f and every F2 that of x f(x)
(central differences over -6 ... 6, within 2e-8), and every saturator puts out f of a slow input half a sample
(first-order) or one sample (second-order) earlier; tanh1 holds at x = 100.

```
g++ -O2 -std=c++17 lib/aanl/aanl_test.cpp -o aanl_test && ./aanl_test
```

## License

MIT, see [LICENSE](LICENSE): aanl.lib's functions © 2021 Dario Sanfilippo and © 2024, 2026 David Braun, as aanl.lib
declares them; the port © 2026 E.Heidt.
