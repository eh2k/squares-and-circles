// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the TR-909 snare drum modelled from its circuit. (c) 2026 E. Heidt.
//
// The voicing board of the Service Notes (JUN. 15, 1984, p. 11); MODEL.md names each part. As the bass drum: every
// state a capacitor's voltage, every path the schematic's.
//
//   trigger   TRIG turns on Q39, Q39 Q40: TP (+15 V, 2 ms); Q39's collector gates the accent through Q41 (ga) and
//             starts Q42 (held on some ms by C68 / R256)
//   ENV1      TP charges C66 through D56, R247; R248 into the TUNE divider (VR6) discharges it; IC35a/b make the
//             4069's rails +-V of their junction N1: the VCOs' current, so both bend down together
//   VCO-1/2   a Schmitt trigger (IC37a / IC38a) through a 4069 inverter at those rails into an integrator (IC37b, C69 /
//             IC38b, C71): triangles at f = V(rails) / (2 (thHi - thLo) R C); TP resets them (Q44 / Q45) and forces
//             the comparators high; the lower threshold is held by D62/D63 to the TP line's rest
//   drum      the triangles through R283 / R305 into antiparallel diodes, the VCAs Q50 / Q51 at 47 ohm; ENV3: ga
//             charges C83 (D68), R291 into C70 (Q46 holds it at 0 while TP is high), R290 to Q50's base; ENV2: C82
//             (D67), R295 into C79 (Q49), R293 to Q51's base
//   snappy    the noise bus through R286/R285, C85/R302, the Sallen-Key low pass IC40a (8.5 kHz); then C80/R299 into
//             Q48 (ENV4: Q42 charges C67 through D60, TONE (VR7) and R254 to IC34a's -0.1 accb discharge it, R297
//             into the base) and the Sallen-Key high pass IC39a (4 kHz) through R278 into Q47 (ENV5: ga charges C73
//             through D64, Q43 holds Q47's base while TP is high, then C73 empties through R280 into it: a burst);
//             IC39b sums them, SNAPPY (VR9) its feedback, C78 / R294 into the output's summing node
//   output    IC40b, LEVEL (VR8) its feedback: out = -VR8 (ie50 + ie51 + o39b / R294)
#pragma once
#include "va909.h"

namespace va909
{
#define VA909_SD_PARAMS(X)                                                                                             \
    X(vcc, 15.0, "V, the positive supply")                                                                             \
    X(vsat, 13.5, "V, the op amps' output swing")                                                                      \
    X(vd, 0.6, "V, a diode's drop at the charge currents")                                                             \
    X(vt, 0.02585, "V, the thermal voltage")                                                                           \
    X(trigMs, 2.0, "ms, the TRIG pulse")                                                                               \
    X(accGain, 3.128, "IC34b's gain, 1 + R241 / R240 (10k / 4.7k)")                                                    \
    /* ENV1 and the rails */                                                                                           \
    X(vtpHi, 14.9, "V, TP while TRIG is high (Q40 saturated)")                                                         \
    X(r247, 47e3, "ohm, R247: C66's charge")                                                                           \
    X(c66, 0.01e-6, "F, C66")                                                                                          \
    X(r248, 470e3, "ohm, R248: C66 into the rails' junction N1")                                                       \
    X(r249, 100e3, "ohm, R249: TUNE's wiper into N1")                                                                  \
    X(r287, 100e3, "ohm, R287: +15 V to TUNE's top")                                                                   \
    X(vr6, 10e3, "ohm, VR6 TUNE (10k B)")                                                                              \
    X(tune3, 0.3, "TUNE's wiper at 3 of 10, of the track (straight between 0, 3, 7, 10; B: 0.3)")                      \
    X(tune7, 0.7, "TUNE's wiper at 7 of 10")                                                                           \
    X(r284, 10e3, "ohm, R284: TUNE's bottom to ground")                                                                \
    X(railK, 1.0, "the rails over the junction's voltage (IC35a follows N1; 1: exactly)")                               \
    /* the VCOs */                                                                                                     \
    X(r261, 47e3, "ohm, R261 / R264: the 4069's output into the integrators")                                          \
    X(rOn, 0.0, "ohm, the 4069's output resistance at 1 V of rails (falls with them as 1/V; 0: none)")                \
    X(c69, 0.01e-6, "F, C69: VCO-1")                                                                                   \
    X(c71, 0.0068e-6, "F, C71: VCO-2")                                                                                 \
    X(thHi, 6.62, "V, the comparators' upper threshold (+vsat R262 / (R262 + R260); SPICE)")                            \
    X(thLoA, -3.57, "V, the lower one while the other VCO's comparator is high: D62/D63, R275/R274 to the TP line, which a low comparator pulls below 0 V (SPICE)") \
    X(thLoB, -4.41, "V, the lower one while the other's is low as well (the TP line lower: the VCOs coupled; SPICE)")  \
    X(cmpDelay, 40e-6, "s, the comparators' and the 4069's switching: the triangles overshoot by their slope times it") \
    X(r263, 10e3, "ohm, R263 / R268: the reset's resistance (Q44, Q45)")                                               \
    X(r258, 100e3, "ohm, R258: TP into Q44's base (not printed; its base current shifts VCO-1's reset level)")         \
    X(r267, 1e6, "ohm, R267: TP into Q45's base")                                                                      \
    /* the shapers and the drum's VCAs */                                                                              \
    X(r283, 10e3, "ohm, R283: VCO-1 into D65/D66")                                                                     \
    X(r288, 1.5e3, "ohm, R288")                                                                                        \
    X(r289, 47.0, "ohm, R289: Q50's collector")                                                                        \
    X(r305, 10e3, "ohm, R305: VCO-2 into D69/D70")                                                                     \
    X(r304, 2.2e3, "ohm, R304")                                                                                        \
    X(r303, 47.0, "ohm, R303: Q51's collector")                                                                        \
    X(dIs, 2.5e-9, "A, the shapers' diodes' saturation current")                                                       \
    X(dN, 1.75, "their emission coefficient")                                                                          \
    X(qBf, 250.0, "the VCAs' forward gain (Q50, Q51)")                                                                 \
    X(qBr, 5.0, "their reverse gain")                                                                                  \
    X(qIs, 1e-14, "A, their saturation current")                                                                       \
    X(c83, 0.1e-6, "F, C83: ENV3")                                                                                     \
    X(r291, 220e3, "ohm, R291")                                                                                        \
    X(c70, 4.7e-9, "F, C70")                                                                                           \
    X(r290, 22e3, "ohm, R290: into Q50's base")                                                                        \
    X(c82, 0.047e-6, "F, C82: ENV2")                                                                                   \
    X(r295, 220e3, "ohm, R295")                                                                                        \
    X(c79, 0.01e-6, "F, C79")                                                                                          \
    X(r293, 220e3, "ohm, R293: into Q51's base")                                                                       \
    X(rGa, 200.0, "ohm, the gated accent's source (Q41 on) into C83, C82, C73")                                        \
    /* the snappy */                                                                                                   \
    X(noiseClk, 450e3, "Hz, the noise generator's clock; 0: no noise")                                                 \
    X(r286, 10e3, "ohm, R286")                                                                                         \
    X(r285, 3.3e3, "ohm, R285")                                                                                        \
    X(c85, 0.015e-6, "F, C85")                                                                                         \
    X(r302, 220e3, "ohm, R302")                                                                                        \
    X(r301, 22e3, "ohm, R301 / R300: IC40a's low pass")                                                                \
    X(c81, 3.3e-9, "F, C81 (its feedback)")                                                                            \
    X(c84, 220e-12, "F, C84 (to ground)")                                                                              \
    X(c80, 0.022e-6, "F, C80: the low pass's output to R299")                                                          \
    X(r299, 4.7e3, "ohm, R299")                                                                                        \
    X(r298, 47.0, "ohm, R298: Q48's collector")                                                                        \
    X(c76, 2.2e-9, "F, C76 / C75: IC39a's high pass")                                                                  \
    X(r276, 6.8e3, "ohm, R276 (its feedback)")                                                                         \
    X(r277, 47e3, "ohm, R277 (to ground)")                                                                             \
    X(r278, 2.2e3, "ohm, R278")                                                                                        \
    X(r279, 47.0, "ohm, R279: Q47's collector")                                                                        \
    X(nBf, 250.0, "the snappy's VCAs' forward gain (Q47, Q48: 2SD1469)")                                               \
    X(nBr, 5.0, "their reverse gain")                                                                                  \
    X(nIs, 1e-14, "A, their saturation current")                                                                       \
    X(r256, 1e6, "ohm, R256: Q42's base")                                                                              \
    X(c68, 6.8e-9, "F, C68: holds Q42 on after TRIG")                                                                  \
    X(bf42, 200.0, "Q42's gain")                                                                                       \
    X(c67, 0.47e-6, "F, C67: ENV4")                                                                                    \
    X(vr7, 500e3, "ohm, VR7 TONE (500k B, a rheostat)")                                                                \
    X(r254, 100e3, "ohm, R254: TONE to IC34a's output (-0.1 accb)")                                                    \
    X(r297, 220e3, "ohm, R297: ENV4 into Q48's base")                                                                  \
    X(c73, 470e-12, "F, C73: ENV5")                                                                                    \
    X(r280, 220e3, "ohm, R280: ENV5 into Q47's base")                                                                  \
    X(vr9, 10e3, "ohm, VR9 SNAPPY (10k B): IC39b's feedback")                                                          \
    X(r294, 47e3, "ohm, R294: the snappy into the output's summing node")                                              \
    X(vr8, 50e3, "ohm, VR8 LEVEL (50k B) at its top")

    struct SDParams
    {
#define X(name, value, comment) real name;
        VA909_SD_PARAMS(X)
#undef X
    };

    static const SDParams sdCircuit = {
#define X(name, value, comment) (real)value,
        VA909_SD_PARAMS(X)
#undef X
    };

    static const char *const sdParamNames[] = {
#define X(name, value, comment) #name,
        VA909_SD_PARAMS(X)
#undef X
    };
    static constexpr int sdNumParams = sizeof(sdParamNames) / sizeof(sdParamNames[0]);

    inline real *sdParamByIndex(SDParams &p, int i) { return (real *)&p + i; }

    // a biquad, transposed direct form II
    struct Biquad
    {
        real b0, b1, b2, a1, a2, z1 = 0, z2 = 0;
        // Sallen-Key low pass / high pass of unity gain at f0 Hz, quality q (bilinear, prewarped)
        void set(bool highpass, real f0, real q, real fs)
        {
            const real w = std::tan((real)3.14159265 * f0 / fs);
            const real w2 = w * w, n = (real)1 / ((real)1 + w / q + w2);
            if (highpass)
            {
                b0 = n;
                b1 = (real)-2 * n;
            }
            else
            {
                b0 = w2 * n;
                b1 = (real)2 * b0;
            }
            b2 = b0;
            a1 = (real)2 * (w2 - (real)1) * n;
            a2 = ((real)1 - w / q + w2) * n;
        }
        real process(real x)
        {
            const real y = b0 * x + z1;
            z1 = b1 * x - a1 * y + z2;
            z2 = b2 * x - a2 * y;
            return y;
        }
    };

    // a VCA's base current: a Newton step (BaseCurrent::update) every 16 samples (full); between them, where its
    // capacitor moved (by track's 20 mV), BaseCurrent::track's step along the slope, or a Newton step where that step
    // would raise it by more than a quarter (ib grows as e^(dv / vt) under the knee, the step only as 1 + dv / (vt +
    // r ib)). C70 rises 0.1 V (4 vt) a sample at TRIG's end: with the step alone Q50's base current lagged, the tone
    // came 3 samples late, C70 (unloaded) overshot by 0.4 V and the next update jumped 0.6 of the peak in one sample;
    // with the Newton steps (25 ... 65 a hit, all by 1.5 ms after TRIG's end, up to 34 in a frame) within a sample of
    // SPICE (fit/sd_onset.py). One call of update() for both: its code once a VCA
    inline void stepBase(BaseCurrent &b, real v, real r, real vt, real is, bool full)
    {
        const real d = v - b.v0;
        if (full || (d > (real)0.02 && d * b.g > (real)0.25 * b.ib))
            b.update(v, r, vt, is);
        else
            b.track(v, r, vt);
    }

    // a triangle VCO of the snare: phase between thLo and thHi, polyBLAMP at the corners (one sample late)
    struct TriVco
    {
        real tri = 0, corr = 0;
        int dir = 1;
        real step(real d, real lo, real hi) // d: the ramp per sample
        {
            // a threshold that moved past the triangle (the other VCO's comparator flipped) turns it where it is
            if (dir > 0 && hi < tri)
                hi = tri;
            if (dir < 0 && lo > tri)
                lo = tri;
            real corrNext = (real)0;
            real t = tri + (dir > 0 ? d : -d);
            if (t > hi)
            {
                const real over = (t - hi) / d;
                t = hi + hi - t;
                dir = -1;
                corr += -(real)2 * d * over * over * over * (real)(1.0 / 6.0);
                const real u = (real)1 - over;
                corrNext = -(real)2 * d * u * u * u * (real)(1.0 / 6.0);
            }
            else if (t < lo)
            {
                const real over = (lo - t) / d;
                t = lo + lo - t;
                dir = 1;
                corr += (real)2 * d * over * over * over * (real)(1.0 / 6.0);
                const real u = (real)1 - over;
                corrNext = (real)2 * d * u * u * u * (real)(1.0 / 6.0);
            }
            const real out = tri + corr;
            tri = t;
            corr = corrNext;
            return out;
        }
        real reset(real target, real k) // while TP is high: towards target, rising after
        {
            tri = target + (tri - target) * k;
            corr = (real)0;
            dir = 1;
            return tri;
        }
    };

    class SnareDrum
    {
    public:
        static constexpr int kTable = 64;

        void init(real sampleRate, const SDParams *p)
        {
            fs_ = sampleRate;
            dt_ = (real)1 / sampleRate;
            p_ = p;
            memset(&s_, 0, sizeof(s_));
            vco1_ = TriVco();
            vco2_ = TriVco();
            setKnobs((real)0.5, (real)0.5, (real)0.5);
            prepare();
        }

        // the panel's knobs, 0..1
        void setKnobs(real tune, real tone, real snappy)
        {
            if (tune != tune_ || tone != tone_ || snappy != snappy_)
            {
                tune_ = tune;
                tone_ = tone;
                snappy_ = snappy;
                dirty_ = true;
            }
        }

        // tables false: the scalar coefficients only, for parts the tables do not depend on (the shapers' and the VCAs' parts: r283 r288 r289 r303 r304 r305, dIs dN, qBf qBr nBf nBr vt, thHi thLoB)
        void setParams(const SDParams *p, bool tables = true)
        {
            p_ = p;
            if (tables)
                prepare<true>();
            else
                prepare<false>();
        }

        // TRIG with the ACCENT line at acc volts
        void trigger(real acc)
        {
            s_.pulse = trigSamples_;
            accb_ = acc * p_->accGain;
            if (accb_ > p_->vsat)
                accb_ = p_->vsat;
            if (accb_ < (real)0)
                accb_ = (real)0;
            vret_ = (real)-0.1 * accb_; // IC34a: -R243 / R255 of IC34b's output
            s_.count = 0;               // the control steps (every 4 samples) aligned with TRIG
            s_.idle = false;
        }

        bool idle() const { return s_.idle; }

        void process(real *__restrict out, int n)
        {
            if (dirty_)
                knobCoeffs();
            if (s_.idle)
            {
                for (int i = 0; i < n; i++)
                    out[i] = (real)0;
                return;
            }
            const SDParams &p = *p_;
            State s = s_;
            BaseCurrent ib50 = ib50_, ib51 = ib51_, ib48 = ib48_;
            Noise nz = noise_;
            TriVco v1 = vco1_, v2 = vco2_;
            Biquad lp = lp_, hp = hp_;
            const real vd = p.vd, vt = p.vt, vr8 = p.vr8, vr9 = vr9eff_;
            for (int i = 0; i < n; i++)
            {
                const uint32_t c = s.count++;
                const bool full = (c & 15) == 0; // the VCAs' Vbe every 16 samples, sooner where their capacitor moves
                stepBase(ib50, s.v70, p.r290, vt, isEff_, full);
                stepBase(ib51, s.v79, p.r293, vt, isEff_, full);
                stepBase(ib48, s.v67, p.r297, vt, nIsEff_, full);
                const bool tp = s.pulse > 0;
                if (tp)
                    s.pulse--;
                const real ga = tp ? accb_ - (real)0.05 : (real)0; // Q41: the gated accent

                // ENV1 (C66), the rails and the VCOs' slopes, every 4 samples (C66's fastest is 0.17 ms)
                if ((c & 3) == 0)
                {
                    if (tp && s.v66 < vch66_)
                        s.v66 = vch66_ + (s.v66 - vch66_) * k66c4_;
                    else
                        s.v66 = n1rest_ + (s.v66 - n1rest_) * k66d4_;
                    const real rail = railK_ * (n1rest_ + (s.v66 - n1rest_) * w248_);
                    s.rail = rail;
                    // the integrators' current (the switching's delay: the thresholds moved by the slope times it)
                    const real g = rail / (p.r261 + rOn1_ / rail);
                    s.d1 = g * slope1_;
                    s.d2 = g * slope2_;
                }

                // the VCOs (reset while TP is high)
                real t1, t2;
                if (tp)
                {
                    // Q44 / Q45 short C69 / C71 through R263 / R268; their base currents flow into the summing nodes
                    t1 = v1.reset(p.r263 * (s.rail / p.r261 - ib44_), kRst1_);
                    t2 = v2.reset(p.r263 * (s.rail / p.r261 - ib45_), kRst2_);
                }
                else
                {
                    // the lower threshold of each lower while the other's comparator is low as well (falling: dir < 0)
                    const real d1 = s.d1, d2 = s.d2;
                    const real lo1 = v2.dir < 0 ? p.thLoB : p.thLoA, lo2 = v1.dir < 0 ? p.thLoB : p.thLoA;
                    t1 = v1.step(d1, lo1 - d1 * delayN_, p.thHi + d1 * delayN_);
                    t2 = v2.step(d2, lo2 - d2 * delayN_, p.thHi + d2 * delayN_);
                }

                // ENV3 (C83 -> R291 -> C70 -> R290 -> Q50), ENV2 (C82 -> R295 -> C79 -> R293 -> Q51)
                if (tp) // (the gated accent ga only while TP is high)
                {
                    if (ga - vd > s.v83)
                        s.v83 += (ga - vd - s.v83) * kGa83_;
                    if (ga - vd > s.v82)
                        s.v82 += (ga - vd - s.v82) * kGa82_;
                    if (ga - vd > s.v73)
                        s.v73 += (ga - vd - s.v73) * kGa73_;
                    s.v70 = s.v79 = (real)0; // Q46, Q49 hold them
                    s.ib50 = s.ib51 = (real)0;
                }
                else
                {
                    const real i291 = (s.v83 - s.v70) * ir291_;
                    s.v83 -= i291 * dtc83_;
                    s.v70 += (i291 - s.ib50) * dtc70_;
                    const real i295 = (s.v82 - s.v79) * ir295_;
                    s.v82 -= i295 * dtc82_;
                    s.v79 += (i295 - s.ib51) * dtc79_;
                }
                VcaPoint q;
                // (a VCA whose base gets nothing more is skipped: its capacitor is below its Vbe)
                real ie50 = (real)0, ie51 = (real)0;
                if (!tp && (s.ib50 > (real)1e-10 || s.v70 > vbe48_))
                {
                    q1_.at(t1, q);
                    ie50 = vcaCurrent(q, rn50_, s.v70, ib50, s.ib50);
                }
                else
                    s.ib50 = (real)0;
                if (!tp && (s.ib51 > (real)1e-10 || s.v79 > vbe48_))
                {
                    q2_.at(t2, q);
                    ie51 = vcaCurrent(q, rn51_, s.v79, ib51, s.ib51);
                }
                else
                    s.ib51 = (real)0;

                // ENV4: Q42 on through R256 while C68 lets it (Q39 holds C68's end down while TP is high)
                if (tp)
                    s.v68 = vd;
                {
                    real ib42 = (accb_ - vd - s.v68) * ir256_;
                    if (ib42 < (real)0)
                        ib42 = (real)0;
                    if (!tp)
                        s.v68 += ib42 * dtc68_;
                    real ich = (real)0;
                    const real vtop = accb_ - (real)0.1 - vd;
                    if (s.v67 < vtop)
                    {
                        ich = (vtop - s.v67) * irGa_;
                        const real lim = p.bf42 * ib42;
                        if (ich > lim)
                            ich = lim;
                    }
                    s.v67 += (ich - (s.v67 - vret_) * gTone_ - s.ib48) * dtc67_;
                    if (s.v67 < -vd)
                        s.v67 = -vd; // D59
                }

                // the snappy: noise, IC40a's low pass, C80 into Q48; IC39a's high pass into Q47 (ENV5's burst)
                real ie47 = (real)0, ie48 = (real)0;
                if (snappyOn_ && (s.ib48 > (real)1e-9 || s.v67 > vbe48_ || s.v73 > vbe47_ + (real)0.05 || tp))
                {
                    const real vn = noiseAmp_ * nz.next();
                    s.vc85 += (vn - s.vc85) * k85_; // C85 / R302's high pass: its capacitor follows the input
                    const real x = lp.process(vn - s.vc85);
                    s.vc80 += (x - s.vc80) * k80_;
                    // Q48 at R298 (its load on the 47 ohm moves the node by a few %: left out)
                    real f, l;
                    q48_.at2((x - s.vc80) * a48_, f, l);
                    const real b48 = ib48.at(s.v67, l);
                    s.ib48 = b48;
                    ie48 = b48 * f;
                    // the snap: IC39a's high pass into Q47 only while C73 empties into its base (and while TP is
                    // high, so the filter has settled when it starts)
                    if (tp)
                    {
                        hp.process(x);
                        s.ib47 = (real)0; // Q43 holds Q47's base
                    }
                    else if (s.v73 > vbe47_)
                    {
                        const real h = hp.process(x);
                        real b = (s.v73 - vbe47_) * ir280_;
                        s.v73 -= b * dtc73_;
                        s.ib47 = b;
                        real f47, l47;
                        q47_.at2(h * a47_, f47, l47);
                        ie47 = b * f47;
                    }
                    else
                        s.ib47 = (real)0;
                }
                const real o39b = -vr9 * (ie47 + ie48);
                out[i] = -vr8 * (ie50 + ie51 + o39b * ir294_);
            }
            // asleep once the VCAs' base currents are gone (C73 stays at Q47's Vbe, C70/C79/C67 near their Vbe), not
            // while C83 / C82 still charge C70 / C79 (a soft hit, ACCENT under 0.6 V, fell asleep before its tones)
            if (s.pulse == 0 && s.ib50 < (real)3e-9 && s.ib51 < (real)3e-9 && s.ib48 < (real)3e-9 &&
                s.v73 < vbe47_ + (real)0.05 && s.v67 < vbe48_ && s.v83 - s.v70 < (real)0.05 &&
                s.v82 - s.v79 < (real)0.05)
                s.idle = true;
            s_ = s;
            ib50_ = ib50;
            ib51_ = ib51;
            ib48_ = ib48;
            noise_ = nz;
            vco1_ = v1;
            vco2_ = v2;
            lp_ = lp;
            hp_ = hp;
        }

        real env1() const { return s_.v66; }
        real env3() const { return s_.v70; }
        real env2() const { return s_.v79; }
        real env4() const { return s_.v67; }
        real env5() const { return s_.v73; }
        real tri1() const { return vco1_.tri; }
        real tri2() const { return vco2_.tri; }
        real ib50() const { return s_.ib50; } // the VCAs' base currents (A)
        real ib51() const { return s_.ib51; }
        real ib48() const { return s_.ib48; }

    private:
        // a template: the tables' branch is gone at compile time, prepare<true>() compiles as it did without it
        template <bool tables = true>
        void prepare()
        {
            const SDParams &p = *p_;
            trigSamples_ = (int)(p.trigMs * (real)0.001 * fs_ + (real)0.5);
            vch66_ = p.vtpHi - p.vd;
            k66c_ = std::exp(-dt_ / (p.r247 * p.c66));
            k66c4_ = std::exp((real)-4 * dt_ / (p.r247 * p.c66));
            railK_ = p.railK;
            rOn1_ = p.rOn;
            slope1_ = dt_ / p.c69;
            delayN_ = p.cmpDelay / dt_;
            slope2_ = dt_ / p.c71;
            ib44_ = (p.vtpHi - p.vd) / p.r258;
            ib45_ = (p.vtpHi - p.vd) / p.r267;
            kRst1_ = std::exp(-dt_ / (p.r263 * p.c69));
            kRst2_ = std::exp(-dt_ / (p.r263 * p.c71));
            isEff_ = p.qIs * ((real)1 / p.qBf + (real)1 / p.qBr);
            nIsEff_ = p.nIs * ((real)1 / p.nBf + (real)1 / p.nBr);
            const real lo = (p.thLoB < -p.thHi ? p.thLoB : -p.thHi) * (real)1.1, hi = p.thHi * (real)1.1;
            const real k1 = p.r289 / (p.r288 + p.r289), k2 = p.r303 / (p.r304 + p.r303);
            const real is = p.dIs, nvt = p.dN * p.vt, r283 = p.r283, r305 = p.r305;
            const real rl1 = p.r288 + p.r289, rl2 = p.r304 + p.r303;
            if (tables)
            {
                q1_.fill(lo, hi, p.qBf, p.qBr, p.vt, [=](real v) { return diodeShaper(v, r283, rl1, is, nvt) * k1; });
                q2_.fill(lo, hi, p.qBf, p.qBr, p.vt, [=](real v) { return diodeShaper(v, r305, rl2, is, nvt) * k2; });
            }
            rn50_ = p.r288 * p.r289 / (p.r288 + p.r289);
            rn51_ = p.r304 * p.r303 / (p.r304 + p.r303);
            ir291_ = (real)1 / p.r291;
            ir295_ = (real)1 / p.r295;
            dtc83_ = dt_ / p.c83;
            dtc70_ = dt_ / p.c70;
            dtc82_ = dt_ / p.c82;
            dtc79_ = dt_ / p.c79;
            kGa83_ = (real)1 - std::exp(-dt_ / (p.rGa * p.c83));
            kGa82_ = (real)1 - std::exp(-dt_ / (p.rGa * p.c82));
            kGa73_ = (real)1 - std::exp(-dt_ / (p.rGa * p.c73));
            irGa_ = (real)1 / p.rGa;
            ir256_ = (real)1 / p.r256;
            dtc68_ = dt_ / p.c68;
            dtc67_ = dt_ / p.c67;
            dtc73_ = dt_ / p.c73;
            ir280_ = (real)1 / p.r280;
            vbe47_ = (real)0.4; // Q47's base at the burst's end (C73 falls to some 0.4 V in SPICE)
            vbe48_ = (real)0.45; // ENV4 under it: Q48 takes nothing (its base current under 1 nA)
            ir294_ = (real)1 / p.r294;
            // the noise: the bus's density after R286/R285, C85/R302 (a high pass), IC40a, C80 into R299/R298
            const real a = (real)0.5 * p.vcc * p.r285 / (p.r286 + p.r285);
            noiseAmp_ = p.noiseClk > (real)0 ? a * std::sqrt(fs_ / p.noiseClk) * (real)1.7320508 : (real)0;
            const real rth = p.r286 * p.r285 / (p.r286 + p.r285);
            k85_ = (real)1 - std::exp(-dt_ / ((p.r302 + rth) * p.c85));
            lp_.set(false, (real)1 / ((real)6.2831853 * p.r301 * std::sqrt(p.c81 * p.c84)), (real)0.5 * std::sqrt(p.c81 / p.c84), fs_);
            hp_.set(true, (real)1 / ((real)6.2831853 * p.c76 * std::sqrt(p.r276 * p.r277)), (real)0.5 * std::sqrt(p.r277 / p.r276), fs_);
            k80_ = (real)1 - std::exp(-dt_ / ((p.r299 + p.r298) * p.c80));
            a48_ = p.r298 / (p.r299 + p.r298);
            a47_ = p.r279 / (p.r278 + p.r279);
            rn48_ = p.r298 * p.r299 / (p.r298 + p.r299);
            if (tables)
            {
                q48_.fill((real)-0.25, (real)0.25, p.nBf, p.nBr, p.vt, [](real x) { return x; });
                q47_.fill((real)-0.25, (real)0.25, p.nBf, p.nBr, p.vt, [](real x) { return x; });
            }
            knobCoeffs();
        }

        void knobCoeffs()
        {
            const SDParams &p = *p_;
            dirty_ = false;
            // TUNE: VR6 (B) between R287 (to +15 V) and R284: its wiper's voltage and source resistance
            const real tw = potTaper(tune_, p.tune3, p.tune7);
            const real rtop = p.r287 + p.vr6 * ((real)1 - tw), rbot = p.r284 + p.vr6 * tw;
            const real vw = p.vcc * rbot / (rtop + rbot), rw = rtop * rbot / (rtop + rbot);
            // C66 against N1: N1 = (vw / (R249 + rw) + v66 / R248) / (1 / (R249 + rw) + 1 / R248), at rest vw
            const real g1 = (real)1 / (p.r249 + rw), g2 = (real)1 / p.r248;
            n1rest_ = vw;
            w248_ = g2 / (g1 + g2);
            const real rdis = p.r248 + p.r249 + rw; // C66 discharges through R248 and the divider's side
            k66d_ = std::exp(-dt_ / (rdis * p.c66));
            k66d4_ = std::exp((real)-4 * dt_ / (rdis * p.c66));
            s_.rail = n1rest_ * railK_ + (s_.v66 - n1rest_) * w248_ * railK_;
            const real g = s_.rail / (p.r261 + rOn1_ / (s_.rail > (real)0.1 ? s_.rail : (real)0.1));
            s_.d1 = g * slope1_;
            s_.d2 = g * slope2_;
            // TONE: VR7 in series to R254 to IC34a's output (-0.1 accb)
            gTone_ = (real)1 / (p.vr7 * tone_ + p.r254);
            snappyOn_ = snappy_ > (real)0.001;
            vr9eff_ = p.vr9 * snappy_;
        }

        struct State
        {
            int pulse;
            uint32_t count;
            bool idle;
            real v66, v83, v70, v82, v79, v68, v67, v73, rail, d1, d2;
            real ib50, ib51, ib48, ib47;
            real vc85, vc80;
        } s_;

        const SDParams *p_ = nullptr;
        real fs_ = 48000, dt_ = (real)1 / 48000;
        real tune_ = -1, tone_ = -1, snappy_ = -1;
        bool dirty_ = true, snappyOn_ = true;
        int trigSamples_ = 96;
        real accb_ = 0;
        real vch66_, k66c_, k66d_, k66c4_, k66d4_, n1rest_, w248_, railK_, rOn1_, slope1_, slope2_, delayN_, ib44_, ib45_, kRst1_, kRst2_;
        real isEff_, nIsEff_, rn50_, rn51_, ir291_, ir295_, dtc83_, dtc70_, dtc82_, dtc79_, kGa83_, kGa82_, kGa73_;
        real irGa_, ir256_, dtc68_, dtc67_, dtc73_, ir280_, vbe47_, vbe48_, ir294_, vret_ = 0, gTone_, vr9eff_;
        real noiseAmp_, k85_, k80_, a48_, a47_, rn48_;
        VcaTable<kTable> q1_, q2_;
        VcaTable<32> q48_, q47_;
        BaseCurrent ib50_, ib51_, ib48_;
        Noise noise_;
        TriVco vco1_, vco2_;
        Biquad lp_, hp_;
    };
}
