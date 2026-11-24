// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the voices of a TR-909 modelled from its circuit. (c) 2026 E. Heidt.
//
// Shared pieces of the voices (bd.h, ...): the arithmetic, fast exp2/log2, one-pole coefficients, a transistor's emitter current in saturation (the 909's VCAs), the noise generator.
// MODEL.md says what each voice is and where each constant comes from.
#pragma once
#include <cmath>
#include <cstdint>
#include <cstring>

#ifndef VA909_REAL
#define VA909_REAL float
#endif

namespace va909
{
    typedef VA909_REAL real;

    // 2^x: the integer part into the exponent, the fraction by a minimax polynomial of degree 5 (9e-8 relative)
    inline real exp2Fast(real x)
    {
        if constexpr (sizeof(real) == 4)
        {
            if (x < (real)-126)
                x = (real)-126;
            if (x > (real)126)
                x = (real)126;
            int i = (int)x;
            if ((real)i > x)
                i--;
            const real f = x - (real)i;
            real q = (real)1.8775767e-3;
            q = q * f + (real)8.9893397e-3;
            q = q * f + (real)5.5826318e-2;
            q = q * f + (real)2.4015361e-1;
            q = q * f + (real)6.9315308e-1;
            q = q * f + (real)9.9999994e-1;
            union
            {
                float f;
                int32_t i;
            } u = {(float)q};
            u.i += i * (1 << 23);
            return (real)u.f;
        }
        else
            return std::exp2(x);
    }

    // log2(x), x > 0: the exponent and the mantissa's log2 as m q(m), q of degree 5 (2e-6 absolute)
    inline real log2Fast(real x)
    {
        if constexpr (sizeof(real) == 4)
        {
            union
            {
                float f;
                int32_t i;
            } u = {(float)x};
            const real e = (real)(((u.i >> 23) & 255) - 127);
            u.i = (u.i & 0x007FFFFF) | 0x3F800000; // the mantissa in [1, 2)
            const real m = (real)u.f - (real)1;
            real q = (real)-2.64661388e-2;
            q = q * m + (real)1.23473247e-1;
            q = q * m + (real)-2.79557574e-1;
            q = q * m + (real)4.58278152e-1;
            q = q * m + (real)-7.18282973e-1;
            q = q * m + (real)1.44255318;
            return e + m * q;
        }
        else
            return std::log2(x);
    }

    inline real expFast(real x) { return exp2Fast(x * (real)1.442695041); }
    inline real logFast(real x) { return log2Fast(x) * (real)0.6931471806; }

    // the factor of a decay towards a target over one sample with the time constant tau (seconds)
    inline real decayCoeff(real tau, real dt) { return tau > (real)0 ? std::exp(-dt / tau) : (real)0; }

    // A transistor in saturation as the 909's VCAs use it: the base fed a current ib, the emitter in an op amp's
    // summing node, the collector at vce (a few mV, from a 47 ohm node). Ebers-Moll with both junctions on:
    // ie = ib (1 + 1/bf - u) / (1/bf + u/br), u = exp(-vce/vt): a current ~ ib br vce / vt for small vce, at most
    // ~ bf ib for vce > 0 and -br ib for vce < 0 (the inverse transistor), so even harmonics and a DC part ~ ib.
    struct SatVca
    {
        real ibf, ibr, nvt; // 1/bf, 1/br, -1/vt
        void init(real bf, real br, real vt)
        {
            ibf = (real)1 / bf;
            ibr = (real)1 / br;
            nvt = (real)-1 / vt;
        }
        real gain(real vce) const // ie / ib
        {
            const real u = std::exp(vce * nvt);
            return ((real)1 + ibf - u) / (ibf + u * ibr);
        }
    };

    // The base current of a VCA transistor fed from a capacitor's voltage v through r: ib = (v - vbe) / r with
    // vbe = vt ln(ib / is). update() solves it (Newton from the last solution, every few samples: v moves slowly)
    // and keeps the slope g = d ib / d v = 1 / (r + vt / ib); at() goes from there linearly, with the base's swing
    // dv (L(vce), VcaTables: vbe lower by it)
    struct BaseCurrent
    {
        real ib = (real)1e-12;
        real v0 = (real)0;
        real g = (real)0;
        void update(real v, real r, real vt, real is)
        {
            real x = ib + g * (v - v0); // the last solution moved along its slope, then a Newton step
            if (x < (real)1e-13)
                x = (real)1e-13;
            {
                const real f = r * x + vt * logFast(x / is) - v;
                const real d = r + vt / x;
                x -= f / d;
                if (x < (real)1e-13)
                    x = (real)1e-13;
            }
            ib = x;
            v0 = v;
            g = x / (r * x + vt); // 1 / (r + vt / ib)
        }
        real at(real v, real dv) const
        {
            const real x = ib + g * (v - v0 + dv);
            return x > (real)0 ? x : (real)0;
        }
        // when v has moved more than dvMax since the last update: a step along the slope (d ib / d v = 1 / (r + vt / ib),
        // an Euler step of that ODE in v), the slope anew; update() corrects it every few samples
        void track(real v, real r, real vt, real dvMax = (real)0.02)
        {
            const real d = v - v0;
            if (d > dvMax || d < -dvMax)
            {
                real x = ib + g * d;
                if (x < (real)1e-13)
                    x = (real)1e-13;
                ib = x;
                v0 = v;
                g = x / (r * x + vt);
            }
        }
    };

    // A triangle through rin into antiparallel diodes to ground, loaded by rl: the node's voltage u from
    // (v - u) / rin = 2 is sinh(u / nvt) + u / rl, by Newton (at setup: the voices' shaper tables)
    inline real diodeShaper(real v, real rin, real rl, real is, real nvt)
    {
        // the start: the divider's voltage, but not beyond where the diodes alone would take all of v / rin (from
        // further out the steps of 0.05 V did not reach the knee: a drive over some 4 V through a light load stayed
        // up to volts too high)
        real u = v * rl / (rin + rl);
        const real ud = nvt * std::log((v < (real)0 ? -v : v) / (rin * is) + (real)1);
        if (u > ud)
            u = ud;
        if (u < -ud)
            u = -ud;
        for (int k = 0; k < 60; k++)
        {
            real e = u / nvt;
            if (e > (real)40)
                e = (real)40;
            if (e < (real)-40)
                e = (real)-40;
            const real ex = std::exp(e), emx = std::exp(-e);
            const real f = (v - u) / rin - is * (ex - emx) - u / rl;
            const real df = (real)-1 / rin - is * (ex + emx) / nvt - (real)1 / rl;
            real du = -f / df;
            if (du > (real)0.05)
                du = (real)0.05;
            if (du < (real)-0.05)
                du = (real)-0.05;
            u += du;
            if (du < (real)1e-9 && du > (real)-1e-9)
                break;
        }
        return u;
    }

    // a function as a table over [lo, hi], linear between its points, held beyond
    template <int N>
    struct Table
    {
        real lo, scale, y[N];
        template <typename F>
        void fill(real lo_, real hi, F f)
        {
            lo = lo_;
            scale = (real)(N - 1) / (hi - lo_);
            for (int k = 0; k < N; k++)
                y[k] = f(lo_ + (real)k / scale);
        }
        real operator()(real v) const
        {
            real x = (v - lo) * scale;
            if (x < (real)0)
                x = (real)0;
            if (x > (real)(N - 1) - (real)0.001)
                x = (real)(N - 1) - (real)0.001;
            const int k = (int)x;
            const real f = x - (real)k;
            return y[k] + f * (y[k + 1] - y[k]);
        }
        real operator()(real v, real &slope) const // and its slope
        {
            real x = (v - lo) * scale;
            if (x < (real)0)
                x = (real)0;
            if (x > (real)(N - 1) - (real)0.001)
                x = (real)(N - 1) - (real)0.001;
            const int k = (int)x;
            const real f = x - (real)k;
            const real d = y[k + 1] - y[k];
            slope = d * scale;
            return y[k] + f * d;
        }
    };

    // A VCA transistor of the 909 (Q12, Q6) in its circuit: the collector at a node of a few mV (the open circuit
    // voltage vo behind rn: R43 47 ohm / R53, R55 47 ohm / VR3), the base fed from a capacitor's voltage v through
    // r, the emitter in IC11a's summing node. Its emitter current: ie = ib F(vce) (SatVca), the base current
    // ib = (v - vbe) / r with vbe = vb0 - L(vce), vb0 the base's voltage at vce = 0 (BaseCurrent, every few
    // samples) and L(vce) = vt ln((1/bf + u/br) / (1/bf + 1/br)) its swing with the collector: Ebers-Moll's
    // ib = is e^(vbe/vt) (1/bf + u/br), so a collector below the emitter (u > 1) lowers vbe and raises ib (in the
    // decay, when v is little over vbe, this modulates ib: BaseCurrent::at; the sign checked against ngspice,
    // fit/vcabase.py: it was +L until 2026-11-13, ib then off by up to 130 %); the collector current loads the node:
    // vce = vo - rn (ie - ib), one Newton step from vce = vo, F and L moved along their slopes. F, F', L, L' in one
    // table over vce (VcaTable), or over the triangle's voltage with vo (bd.h: the shaper in front).
    struct VcaPoint
    {
        real f, df, l, dl;
    };

    inline void vcaPoint(const SatVca &s, real vt, real ibf, real ibr, real vo, real h, VcaPoint &q)
    {
        auto L = [=](real x) { return vt * std::log((ibf + std::exp(-x / vt) * ibr) / (ibf + ibr)); };
        q.f = s.gain(vo);
        q.df = (s.gain(vo + h) - s.gain(vo - h)) / (h + h);
        q.l = L(vo);
        q.dl = (L(vo + h) - L(vo - h)) / (h + h);
    }

    // the emitter current from a point of the table; ib the base current
    inline real vcaCurrent(const VcaPoint &q, real rn, real v, const BaseCurrent &base, real &ib)
    {
        real b = base.at(v, q.l);
        if (b <= (real)0)
        {
            ib = (real)0;
            return (real)0;
        }
        const real e = rn * b * q.df; // < 0.2: 1 / (1 + e) as 1 - e
        const real d = rn * b * (q.f - (real)1) * ((real)1 - e); // vo - vce
        b = base.at(v, q.l - q.dl * d);
        ib = b;
        return b * (q.f - q.df * d);
    }

    // as vcaCurrent, its Newton step's derivative exact for a node the transistor loads hard: the toms' base currents
    // at a hit (tens of uA) into 47 ohm, where vcaCurrent's 1 / (1 + e) as 1 - e put the emitter current 12 ... 21 %
    // over SPICE's (fit/ttvca.py; a second step changed nothing)
    inline real vcaCurrent2(const VcaPoint &q, real rn, real v, const BaseCurrent &base, real &ib)
    {
        real b = base.at(v, q.l);
        if (b <= (real)0)
        {
            ib = (real)0;
            return (real)0;
        }
        const real e = rn * b * q.df;
        // one Newton step as vcaCurrent's, its derivative exact for a heavy load, 1 - e for a light one
        const real d = rn * b * (q.f - (real)1) * (e < (real)0.1 ? (real)1 - e : (real)1 / ((real)1 + e));
        b = base.at(v, q.l - q.dl * d);
        ib = b;
        return b * (q.f - q.df * d);
    }

    // VcaPoints over [lo, hi], linear between
    template <int N>
    struct VcaTable
    {
        real lo, scale;
        VcaPoint y[N];
        template <typename F> // F(x) -> vo
        void fill(real lo_, real hi, real bf, real br, real vt, F vo)
        {
            SatVca s;
            s.init(bf, br, vt);
            lo = lo_;
            scale = (real)(N - 1) / (hi - lo_);
            for (int k = 0; k < N; k++)
                vcaPoint(s, vt, (real)1 / bf, (real)1 / br, vo(lo_ + (real)k / scale), (real)1e-4, y[k]);
        }
        void at(real v, VcaPoint &q) const
        {
            real x = (v - lo) * scale;
            if (x < (real)0)
                x = (real)0;
            if (x > (real)(N - 1) - (real)0.001)
                x = (real)(N - 1) - (real)0.001;
            const int k = (int)x;
            const real t = x - (real)k;
            const VcaPoint &a = y[k], &b = y[k + 1];
            q.f = a.f + t * (b.f - a.f);
            q.df = a.df + t * (b.df - a.df);
            q.l = a.l + t * (b.l - a.l);
            q.dl = a.dl + t * (b.dl - a.dl);
        }
        // F and L only (a node whose load the transistor hardly moves)
        void at2(real v, real &f, real &l) const
        {
            real x = (v - lo) * scale;
            if (x < (real)0)
                x = (real)0;
            if (x > (real)(N - 1) - (real)0.001)
                x = (real)(N - 1) - (real)0.001;
            const int k = (int)x;
            const real t = x - (real)k;
            const VcaPoint &a = y[k], &b = y[k + 1];
            f = a.f + t * (b.f - a.f);
            l = a.l + t * (b.l - a.l);
        }
    };

    // an audio taper pot: the share of its resistance at rotation x (0..1), straight between 0, 0.3, 0.7 and 1 (at3,
    // at7 there): the knob's marks 0, 3, 7, 10 the recordings were made at
    inline real potTaper(real x, real at3, real at7)
    {
        if (x <= (real)0)
            return (real)0;
        if (x < (real)0.3)
            return x * (at3 / (real)0.3);
        if (x < (real)0.7)
            return at3 + (x - (real)0.3) * ((at7 - at3) / (real)0.4);
        if (x < (real)1)
            return at7 + (x - (real)0.7) * (((real)1 - at7) / (real)0.3);
        return (real)1;
    }

    // white noise, uniform in -1..1 (xorshift32)
    struct Noise
    {
        uint32_t s = 0x9E3779B9u;
        real next()
        {
            s ^= s << 13;
            s ^= s >> 17;
            s ^= s << 5;
            return (real)(int32_t)s * (real)4.656612873e-10;
        }
    };
}
