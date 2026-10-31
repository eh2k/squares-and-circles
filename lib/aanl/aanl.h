// aanl.h: Faust's antialiased nonlinearities (aanl.lib 1.6.0, the "aa" library of the Faust libraries) in C++.
//
// Ported from aanl.lib (in this folder, as it came: grame-cncm/faustlibraries a95107fd, 2026-10-09): its auxiliary
// functions, the generic ADAA1 and ADAA2 and its saturators; the wavefolders and the trigonometry are not ported.
// Each function keeps its name, its curve f, its antiderivatives F1 and F2 and its threshold EPS, and comes with
// the author and license that aanl.lib declares for it (MIT, see LICENSE).
//
//   ADAA1, ADAA2, hardclip(2), cubic1, parabolic(2), hyperbolic(2), sinarctan(2), arctan(2), asinh1, asinh2,
//   tanh1: Copyright (C) 2021 Dario Sanfilippo <sanfilippo.dario@gmail.com>, MIT
//   softclipQuadratic1/2: Copyright (C) 2024 David Braun, MIT
//   sinarctanStable, sinarctanTransparent: Copyright (C) 2026 David Braun, MIT
//
// The technique: Parker, Zavalishin, Le Bivic, "Reducing the Aliasing in Nonlinear Waveshaping Using
// Continuous-time Convolution", DAFx 2016. First-order ADAA delays the signal by half a sample, second-order by
// one, and both damp the top of the band (first-order: cos(pi f / fs), -3 dB at fs / 4).
//
// Differences from aanl.lib, the numbers it computes the same:
// - float, as the apps compute (aanl.lib: "it is vital to use double-precision"); AANL_DOUBLE makes it double.
//   In float the difference quotients lose digits when the input is large and moves little: the error is about
//   1e-7 |F1(x)| / |x - x'|. sinarctanStable and sinarctanTransparent have no such quotient.
// - tanh1's F1, log(cosh(x)), as |x| + log(1 + e^(-2|x|)) - log(2), which does not overflow beyond |x| = 89
//   (where aanl.lib's Rcosh() limits cosh() to the largest number and its quotient falls to 0).
// - asinh(x) as sign(x) log(|x| + sqrt(x^2 + 1)), which keeps its digits for large negative x.
//
// Usage: one object per signal, it keeps the samples before.
//
//   aanl::hardclip2 clip;
//   y = clip(x);
//
// and for a curve of one's own, the generic ones with its f and F1 (and F2, the antiderivative of x f(x)):
//
//   aanl::ADAA1 adaa;
//   y = adaa(0.001f, f, F1, x);

#pragma once

#include <float.h>
#include <math.h>

namespace aanl
{
#ifdef AANL_DOUBLE
typedef double real;
static constexpr real EPSILON = DBL_EPSILON; // ma.EPSILON
#else
typedef float real;
static constexpr real EPSILON = FLT_EPSILON;
#endif

// the libm functions in the precision of real, by name, so that no overload of an app's namespace gets in the way
#ifdef AANL_DOUBLE
inline real _sqrt(real x) { return ::sqrt(x); }
inline real _log(real x) { return ::log(x); }
inline real _exp(real x) { return ::exp(x); }
inline real _tanh(real x) { return ::tanh(x); }
inline real _atan(real x) { return ::atan(x); }
inline real _fabs(real x) { return ::fabs(x); }
#else
inline real _sqrt(real x) { return sqrtf(x); }
inline real _log(real x) { return logf(x); }
inline real _exp(real x) { return expf(x); }
inline real _tanh(real x) { return tanhf(x); }
inline real _atan(real x) { return atanf(x); }
inline real _fabs(real x) { return fabsf(x); }
#endif

// ---- auxiliary functions ----------------------------------------------------------------------------------

inline real _clip(real l, real h, real x) { return x < l ? l : (x > h ? h : x); }
inline real signum(real x) { return (real)((x > 0) - (x < 0)); } // ma.signum: 0 at 0
inline real Rsqrt(real x) { return _sqrt(x > 0 ? x : 0); }
inline real Rlog(real x) { return _log(x > EPSILON ? x : EPSILON); }
inline real asinh(real x) { return signum(x) * _log(_fabs(x) + _sqrt(x * x + 1)); } // ma.asinh
inline real logcosh(real x)                                                      // _log(cosh(x)): _log(Rcosh(x))
{
    const real a = _fabs(x);
    return a + _log(1 + _exp(-2 * a)) - (real)0.6931471805599453;
}

// ---- ADAA1: (F1(x) - F1(x')) / (x - x'), or f((x + x') / 2) where |x - x'| <= EPS -------------------------
// Dario Sanfilippo, 2021, MIT.

struct ADAA1
{
    real x1 = 0, F1x1 = 0; // x' and F1(x')

    template <class F, class G>
    real operator()(real eps, F f, G F1, real x)
    {
        const real d = x - x1, F1x = F1(x);
        const real y = _fabs(d) <= eps ? f((real)0.5 * (x + x1)) : (F1x - F1x1) / d;
        x1 = x;
        F1x1 = F1x;
        return y;
    }
};

// ---- ADAA2: T1 + T2 over x, x', x''; F2 is the antiderivative of x f(x) ----------------------------------
// Dario Sanfilippo, 2021, MIT.

struct ADAA2
{
    real x1 = 0, x2 = 0, F1x1 = 0, F1x2 = 0, F2x1 = 0, F2x2 = 0; // x', x'' and their F1, F2

    template <class F, class G, class H>
    real operator()(real eps, F f, G F1, H F2, real x)
    {
        const real F1x = F1(x), F2x = F2(x);
        const real d1 = x - x1, d2 = x2 - x1;
        const real T1 = d1 * d1 <= eps ? (real)0.5 * f((x + 2 * x1) / 3)
                                       : (x * (F1x - F1x1) - (F2x - F2x1)) / (d1 * d1);
        const real T2 = d2 * d2 <= eps ? (real)0.5 * f((x2 + 2 * x1) / 3)
                                       : (x2 * (F1x2 - F1x1) - (F2x2 - F2x1)) / (d2 * d2);
        x2 = x1;
        F1x2 = F1x1;
        F2x2 = F2x1;
        x1 = x;
        F1x1 = F1x;
        F2x1 = F2x;
        return T1 + T2;
    }
};

// ---- saturators -------------------------------------------------------------------------------------------
// They do best with loud inputs: where the input is barely saturated, the plain curve may sound better, as ADAA
// can add noise at low levels (aanl.lib suggests a crossfade by the input's level).

// clip to [-1, 1]. Dario Sanfilippo, 2021, MIT.
struct hardclip
{
    ADAA1 s;
    static real f(real x) { return _clip(-1, 1, x); }
    static real F1(real x) { return _fabs(x) <= 1 ? (real)0.5 * x * x : _fabs(x) - (real)0.5; }
    real operator()(real x) { return s((real)0.001, f, F1, x); }
};

struct hardclip2
{
    ADAA2 s;
    static real F2(real x)
    {
        return _fabs(x) <= 1 ? x * x * x / 3 : ((real)0.5 * x * x - (real)(1.0 / 6.0)) * signum(x);
    }
    real operator()(real x) { return s((real)0.001, hardclip::f, hardclip::F1, F2, x); }
};

// x - x^3 / 3 within [-1, 1], +-2/3 beyond. Dario Sanfilippo, 2021, MIT.
struct cubic1
{
    ADAA1 s;
    static real f(real x) { return _fabs(x) < 1 ? x - x * x * x / 3 : (real)(2.0 / 3.0) * signum(x); }
    static real F1(real x)
    {
        return x <= -1 ? x * (real)(-2.0 / 3.0) - (real)0.25
                       : (x >= 1 ? x * (real)(2.0 / 3.0) - (real)0.25 : x * x / 2 - x * x * x * x / 12);
    }
    real operator()(real x) { return s((real)0.001, f, F1, x); }
};

// x (1 - |x| / 4) within [-2, 2], +-1 beyond. Dario Sanfilippo, 2021, MIT.
struct parabolic
{
    ADAA1 s;
    static real f(real x) { return _fabs(x) <= 2 ? x * (1 - _fabs(x) / 4) : signum(x); }
    static real F1(real x)
    {
        return _fabs(x) <= 2 ? (real)(-1.0 / 12.0) * x * x * (_fabs(x) - 6) : _fabs(x) - (real)(2.0 / 3.0);
    }
    real operator()(real x) { return s((real)0.001, f, F1, x); }
};

struct parabolic2
{
    ADAA2 s;
    static real F2(real x)
    {
        return _fabs(x) <= 2 ? (real)(1.0 / 48.0) * x * x * x * (16 - 3 * _fabs(x))
                            : ((real)0.5 * x * x - (real)(1.0 / 3.0)) * signum(x);
    }
    real operator()(real x) { return s((real)0.001, parabolic::f, parabolic::F1, F2, x); }
};

// x / (1 + |x|). Dario Sanfilippo, 2021, MIT.
struct hyperbolic
{
    ADAA1 s;
    static real f(real x) { return x / (1 + _fabs(x)); }
    static real F1(real x) { return _fabs(x) - _log(1 + _fabs(x)); }
    real operator()(real x) { return s((real)0.001, f, F1, x); }
};

struct hyperbolic2
{
    ADAA2 s;
    static real F2(real x)
    {
        return signum(x) * ((real)0.5 * (3 - 2 * _fabs(x) + x * x + 2 * _log(1 + _fabs(x))) - (real)1.5) + (real)1.5;
    }
    real operator()(real x) { return s((real)0.001, hyperbolic::f, hyperbolic::F1, F2, x); }
};

// sin(_atan(x)) = x / _sqrt(1 + x^2). Dario Sanfilippo, 2021, MIT.
struct sinarctan
{
    ADAA1 s;
    static real f(real x) { return x / _sqrt(1 + x * x); }
    static real F1(real x) { return _sqrt(x * x + 1) - 1; } // F1(0) = 0, to lose fewer digits
    real operator()(real x) { return s((real)0.001, f, F1, x); }
};

struct sinarctan2
{
    ADAA2 s;
    static real F2(real x) { return (real)0.5 * x * _sqrt(x * x + 1) - (real)0.5 * asinh(x); }
    real operator()(real x) { return s((real)0.001, sinarctan::f, sinarctan::F1, F2, x); }
};

// x / _sqrt(1 + x^2), first-order, its quotient written as (x + x') / (F(x) + F(x')) with F(x) = _sqrt(1 + x^2):
// no threshold, no cancellation, exact for any two samples (M. Vicanek, "Note on Alias Suppression in Digital
// Distortion", 2023/2024, eq. 9). Half a sample late, 0 at Nyquist. David Braun, 2026, MIT.
struct sinarctanStable
{
    real x1 = 0, Fx1 = 1;
    real operator()(real x)
    {
        const real Fx = _sqrt(1 + x * x), y = (x + x1) / (Fx + Fx1);
        x1 = x;
        Fx1 = Fx;
        return y;
    }
};

// x / _sqrt(1 + x^2), first-order, with a flat response where it is linear: two one-poles with a = 1 / (3 +
// _sqrt(8)), before and after the shaper, cancel the lowpass of its kernel (half a sample late: (x + 6 x' + x'') /
// 8 for f(x) = x); the chain is then an allpass, 0.71 samples late at DC, 1.41 at Nyquist. The output can exceed
// [-1, 1] by about 7 %. (Vicanek, eqs. 4, 6, 10.) David Braun, 2026, MIT.
struct sinarctanTransparent
{
    real c1 = 0, c2 = 0;                   // the two one-poles' outputs
    real v1 = 0, v2 = 0, Fv1 = 1, Fm1 = 1; // v', v'', F(v'), F((v' + v'') / 2)
    real operator()(real x)
    {
        const real a = (real)(1.0 / (3.0 + 2.8284271247461903)), b = 1 + a;
        const real v = c1 = b * x - a * c1;
        const real Fm = _sqrt(1 + (real)0.25 * (v + v1) * (v + v1));
        const real y = (real)0.25 * ((v + 3 * v1) / (Fm + Fv1) + (3 * v1 + v2) / (Fv1 + Fm1));
        Fm1 = Fm;
        Fv1 = _sqrt(1 + v * v);
        v2 = v1;
        v1 = v;
        return c2 = b * y - a * c2;
    }
};

// a quadratic soft clip: 2x within +-1/3, a parabola to +-1 at +-2/3, +-1 beyond (misceffects.lib's softclip).
// David Braun, 2024, MIT.
struct softclipQuadratic1
{
    ADAA1 s;
    static real f(real x)
    {
        const real a = _fabs(x), sg = x > 0 ? 1 : -1;
        return a < (real)(1.0 / 3.0) ? 2 * x
                                     : (a < (real)(2.0 / 3.0) ? sg * (3 - (2 - a * 3) * (2 - a * 3)) / 3 : sg);
    }
    static real F1(real x)
    {
        const real a = _fabs(x);
        return a < (real)(1.0 / 3.0)
                   ? x * x
                   : (a < (real)(2.0 / 3.0) ? -a / 3 + 2 * x * x - a * a * a + (real)(1.0 / 27.0)
                                            : a - (real)(7.0 / 27.0));
    }
    real operator()(real x) { return s((real)0.001, f, F1, x); }
};

struct softclipQuadratic2
{
    ADAA2 s;
    static real F2(real x)
    {
        const real a = _fabs(x), sg = x > 0 ? 1 : -1;
        return a < (real)(1.0 / 3.0)
                   ? (real)(2.0 / 3.0) * x * x * x
                   : (a < (real)(2.0 / 3.0)
                          ? sg * ((real)(-1.0 / 6.0) * x * x - (real)0.75 * x * x * x * x + (real)(1.0 / 324.0)) +
                                (real)(4.0 / 3.0) * x * x * x
                          : sg * ((real)0.5 * x * x - (real)(5.0 / 108.0)));
    }
    real operator()(real x) { return s((real)0.001, softclipQuadratic1::f, softclipQuadratic1::F1, F2, x); }
};

// _tanh(x), first-order only. Dario Sanfilippo, 2021, MIT.
struct tanh1
{
    ADAA1 s;
    static real f(real x) { return _tanh(x); }
    static real F1(real x) { return logcosh(x); }
    real operator()(real x) { return s((real)0.001, f, F1, x); }
};

// _atan(x), range +-pi/2. Dario Sanfilippo, 2021, MIT.
struct arctan
{
    ADAA1 s;
    static real f(real x) { return _atan(x); }
    static real F1(real x) { return x * _atan(x) - (real)0.5 * _log(x * x + 1); }
    real operator()(real x) { return s((real)0.001, f, F1, x); }
};

struct arctan2
{
    ADAA2 s;
    static real F2(real x) { return (real)0.5 * x * x * _atan(x) - x / 2 + (real)0.5 * _atan(x); }
    real operator()(real x) { return s((real)0.001, arctan::f, arctan::F1, F2, x); }
};

// asinh(x), unbounded. Dario Sanfilippo, 2021, MIT.
struct asinh1
{
    ADAA1 s;
    static real f(real x) { return asinh(x); }
    static real F1(real x) { return x * asinh(x) - _sqrt(1 + x * x) + 1; } // F1(0) = 0
    real operator()(real x) { return s((real)0.001, f, F1, x); }
};

struct asinh2
{
    ADAA2 s;
    static real F2(real x) { return (real)0.25 * (2 * x * x * asinh(x) + asinh(x) - _sqrt(x * x + 1) * x); }
    real operator()(real x) { return s((real)0.001, asinh1::f, asinh1::F1, F2, x); }
};

} // namespace aanl
