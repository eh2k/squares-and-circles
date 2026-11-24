// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the TR-909 rim shot modelled from its circuit. (c) 2026 E. Heidt.
//
// The voicing board of the Service Notes (JUN. 15, 1984, p. 11); MODEL.md names each part.
//
//   trigger   TRIG through C111 (0.018 uF) into R396 / R395 (22k each, D89): a spike of some 0.3 ms; R399 into Q63,
//             which turns on Q64: the accent (IC48b, 1 + R406 / R397) charges C119 through R401 (2.7k): ENV
//   F1..F3    the spike (R396 / R395's junction) into the + inputs of IC48a, IC49a, IC49b, each with a bridged T in
//             its feedback (R407 470k, C113 / C112 0.01 uF, R394 2.2k: 495 Hz; R414 330k, C116 / C115 0.027 uF, R411
//             2.2k: 219 Hz; R416 470k, C118 / C117 4700 pF, R404 2.2k: 1.05 kHz): H(s) = 1 + s Rb Cb g / (s^2 Ca Cb Rb
//             + s (Ca + Cb) + g), g = 1 / Rs, a gain of Rb / (2 Rs) at the resonance: they ring at the op amps' swing
//   clip      R408, R415 (12k), R417 (3.3k; 12k up to SN 415300) into antiparallel diodes D91 / D92, R423 (12k) into
//             R418 (680 ohm): Q65's collector (selected), its base from ENV through R403 (1M), its emitter in IC50a's
//             summing node (LEVEL VR19 100k B with C120 220 pF)
//   output    C121 / C122 (0.01 uF), R422 22k, R420 220k: IC50b's Sallen-Key high pass (some 230 Hz), R421 / R419
//             halve it
#pragma once
#include "va909.h"
#include "sd.h" // Biquad

namespace va909
{
#define VA909_RS_PARAMS(X)                                                                                             \
    X(vsat, 13.5, "V, the op amps' output swing")                                                                      \
    X(vt, 0.02585, "V, the thermal voltage")                                                                           \
    X(trigV, 5.0, "V, TRIG's height (the latch's 5 V)")                                                                \
    X(c111, 0.018e-6, "F, C111: TRIG's edge into R396 / R395")                                                         \
    X(r396, 22e3, "ohm, R396")                                                                                         \
    X(r395, 22e3, "ohm, R395 (the spike at their junction)")                                                           \
    X(r399, 22e3, "ohm, R399: into Q63's base (held at its Vbe while the spike lasts), beside them for C111")         \
    X(accGain, 3.128, "IC48b's gain: 1 + R406 / R397 (10k / 4.7k)")                                                    \
    X(r401, 2.7e3, "ohm, R401: C119's charge through Q64")                                                             \
    X(c119, 0.047e-6, "F, C119: ENV")                                                                                  \
    X(tCharge, 0.32e-3, "s, Q63 / Q64 on: while the spike holds Q63's base over its Vbe (1.9 V of 5)")                                           \
    X(r403, 1e6, "ohm, R403: ENV into Q65's base")                                                                     \
    X(rb1, 470e3, "ohm, R407: F1's bridge")                                                                            \
    X(c1, 0.01e-6, "F, C113 / C112")                                                                                   \
    X(rs1, 2.2e3, "ohm, R394")                                                                                         \
    X(rb2, 330e3, "ohm, R414: F2's bridge")                                                                            \
    X(c2, 0.027e-6, "F, C116 / C115")                                                                                  \
    X(rs2, 2.2e3, "ohm, R411")                                                                                         \
    X(rb3, 470e3, "ohm, R416: F3's bridge")                                                                            \
    X(c3, 4700e-12, "F, C118 / C117")                                                                                  \
    X(rs3, 2.2e3, "ohm, R404")                                                                                         \
    X(r408, 12e3, "ohm, R408: F1 into the diodes")                                                                     \
    X(r415, 12e3, "ohm, R415: F2")                                                                                     \
    X(r417, 3.3e3, "ohm, R417: F3 (12k up to SN 415300)")                                                              \
    X(dIs, 2.5e-9, "A, D91 / D92's saturation current")                                                                \
    X(dN, 1.75, "their emission coefficient")                                                                          \
    X(r423, 12e3, "ohm, R423")                                                                                         \
    X(r418, 680.0, "ohm, R418: Q65's collector")                                                                       \
    X(qBf, 250.0, "Q65's forward gain")                                                                                \
    X(qBr, 5.0, "its reverse gain")                                                                                    \
    X(qIs, 1e-14, "A, its saturation current")                                                                         \
    X(vr19, 100e3, "ohm, VR19 LEVEL (100k B) at its top")                                                              \
    X(c120, 220e-12, "F, C120: across LEVEL")                                                                          \
    X(c121, 0.01e-6, "F, C121 / C122: the output's high pass")                                                         \
    X(r422, 22e3, "ohm, R422 (its feedback)")                                                                          \
    X(r420, 220e3, "ohm, R420 (to ground)")                                                                            \
    X(outK, 2.0, "IC50b's gain: R421 / R419 hold its - input, where R422 feeds back, at half the output")             \
    X(vdSpike, 0.6, "V, D89's drop: the spike passes it into R396 / R395")                                         \
    X(r398, 10e3, "ohm, R398: Q63's base to ground")                                                                 \
    X(vbe63, 0.65, "V, Q63's base while it conducts")

    struct RSParams
    {
#define X(name, value, comment) real name;
        VA909_RS_PARAMS(X)
#undef X
    };

    static const RSParams rsCircuit = {
#define X(name, value, comment) (real)value,
        VA909_RS_PARAMS(X)
#undef X
    };

    static const char *const rsParamNames[] = {
#define X(name, value, comment) #name,
        VA909_RS_PARAMS(X)
#undef X
    };
    static constexpr int rsNumParams = sizeof(rsParamNames) / sizeof(rsParamNames[0]);

    inline real *rsParamByIndex(RSParams &p, int i) { return (real *)&p + i; }

    class RimShot
    {
    public:
        void init(real sampleRate, const RSParams *p)
        {
            fs_ = sampleRate;
            memset(&s_, 0, sizeof(s_));
            s_.idle = true;
            setParams(p);
        }

        // tables false: the scalar coefficients only, for parts the tables do not depend on (the three outputs' resistors r408 r415 r417, r418 r423, vsat, dIs dN, qBf qBr vt)
        void setParams(const RSParams *p, bool tables = true)
        {
            if (tables)
                update<true>(p);
            else
                update<false>(p);
        }

    private:
        // a template: the tables' branch is gone at compile time, update<true>() compiles as setParams() did without it
        template <bool tables>
        void update(const RSParams *p)
        {
            p_ = p;
            const real dt = (real)1 / fs_;
            const real rs = p->r396 + p->r395;
            dtc111_ = dt * (real)0.25 / p->c111;
            (void)rs;
            kDiv_ = p->r395 / (p->r396 + p->r395);
            irs_ = (real)1 / (p->r396 + p->r395);
            ir399_ = (real)1 / p->r399;
            ir399398_ = (real)1 / (p->r399 + p->r398);
            k398_ = p->r398 / (p->r399 + p->r398);
            chargeN_ = (int)(p->tCharge * fs_ + (real)0.5);
            kCharge_ = (real)1 - std::exp(-dt / (p->r401 * p->c119));
            dtc119_ = dt / p->c119;
            isEff_ = p->qIs * ((real)1 / p->qBf + (real)1 / p->qBr);
            res_[0].set(p->rb1, p->c1, p->rs1, fs_);
            res_[1].set(p->rb2, p->c2, p->rs2, fs_);
            res_[2].set(p->rb3, p->c3, p->rs3, fs_);
            // the three outputs into the diodes: their Thevenin source
            const real g = (real)1 / p->r408 + (real)1 / p->r415 + (real)1 / p->r417;
            rth_ = (real)1 / g;
            w1_ = rth_ / p->r408, w2_ = rth_ / p->r415, w3_ = rth_ / p->r417;
            const real rl = p->r423 + p->r418, k = p->r418 / rl;
            const real hi = p->vsat * (w1_ + w2_ + w3_) * (real)1.05;
            const real rth = rth_, is = p->dIs, nvt = p->dN * p->vt;
            if (tables)
            {
                clip_.fill(-hi, hi, [=](real v) { return diodeShaper(v, rth, rl, is, nvt) * k; });
                const real vmax = diodeShaper(hi, rth, rl, is, nvt) * k * (real)1.2;
                vca_.fill(-vmax, vmax, p->qBf, p->qBr, p->vt, [](real v) { return v; });
            }
            rn_ = p->r418 * (p->r423 + rth_) / (p->r418 + p->r423 + rth_);
            kLp_ = (real)1 - std::exp(-dt / (p->vr19 * p->c120));
            hp_.set(true, (real)1 / ((real)6.2831853 * p->c121 * std::sqrt(p->r422 * p->r420)),
                    std::sqrt(p->r422 * p->r420) / ((real)2 * p->r422), fs_);
        }

    public:
        // TRIG with the ACCENT line at acc volts
        void trigger(real acc)
        {
            const RSParams &p = *p_;
            real a = acc * p.accGain;
            if (a > p.vsat)
                a = p.vsat;
            accb_ = a;
            s_.spike = p.trigV; // C111's far end (D89's anode), falling
            s_.charge = chargeN_;
            s_.idle = false;
        }

        bool idle() const { return s_.idle; }

        void process(real *__restrict out, int n)
        {
            if (s_.idle)
            {
                for (int i = 0; i < n; i++)
                    out[i] = (real)0;
                return;
            }
            const RSParams &p = *p_;
            State s = s_;
            BaseCurrent b = b65_;
            Resonator r0 = res_[0], r1 = res_[1], r2 = res_[2];
            Biquad hp = hp_;
            for (int i = 0; i < n; i++)
            {
                const uint32_t c = s.count++;
                // the spike into the bridged Ts; Q64 charges C119 while it lasts
                // C111's far end (D89's anode) after TRIG's edge: it empties through D89 into R396 / R395 (the
                // spike at their junction) and through R399 into Q63's base, which holds it at its Vbe while it
                // conducts (R398 to ground below); four Euler steps a sample while it lasts
                const real x = s.spike > p.vdSpike ? (s.spike - p.vdSpike) * kDiv_ : (real)0;
                if (s.spike > (real)1e-3)
                    for (int k = 0; k < 4; k++)
                    {
                        const real va = s.spike;
                        const real id = va > p.vdSpike ? (va - p.vdSpike) * irs_ : (real)0;
                        const real iq = va * k398_ > p.vbe63 ? (va - p.vbe63) * ir399_ : va * ir399398_;
                        s.spike -= (id + iq) * dtc111_;
                    }
                if (s.charge > 0)
                {
                    s.charge--;
                    if (accb_ > s.v119)
                        s.v119 += (accb_ - s.v119) * kCharge_;
                }
                const real y1 = r0.process(x, p.vsat), y2 = r1.process(x, p.vsat), y3 = r2.process(x, p.vsat);
                const real node = clip_(y1 * w1_ + y2 * w2_ + y3 * w3_);
#ifdef VA909_PROBE
                probe_[0] = y1, probe_[1] = y2, probe_[2] = y3, probe_[3] = node;
#endif
                if ((c & 15) == 0)
                    b.update(s.v119, p.r403, p.vt, isEff_);
                else
                    b.track(s.v119, p.r403, p.vt);
                real ie = (real)0;
                if (s.charge > 0 || s.ib > (real)1e-8)
                {
                    // Q65's collector: the node's open circuit voltage behind rn_ (R418 || R423 and the diodes'
                    // source), the collector current (ie - ib) pulling it; some Newton steps: through 680 ohm it
                    // moves far, where the drums' 47 ohm hardly let it (vcaCurrent's one step)
                    // (within the node's reach: |vce| at most |node| and what the base current, flowing out of the
                    // collector, adds through rn_; each step at most vt: F is exponential there)
                    // from the last sample's solution: it moves little from one to the next (one or two steps)
                    VcaPoint q;
                    real vce = s.vce, bb = (real)0;
                    for (int k = 0; k < 8; k++)
                    {
                        vca_.at(vce, q);
                        bb = b.at(s.v119, q.l);
                        if (bb <= (real)0)
                            break;
                        real step = (vce - node + rn_ * bb * (q.f - (real)1)) / ((real)1 + rn_ * bb * q.df);
                        if (step > p.vt)
                            step = p.vt;
                        if (step < -p.vt)
                            step = -p.vt;
                        const real lim = (node > (real)0 ? node : -node) + rn_ * bb + (real)1e-3;
                        vce -= step;
                        if (vce > lim)
                            vce = lim;
                        if (vce < -lim)
                            vce = -lim;
                        if (step < (real)1e-5 && step > (real)-1e-5)
                            break;
#ifdef VA909_PROBE
                        newtonSteps_++;
#endif
                    }
                    s.vce = vce;
                    vca_.at(vce, q);
                    bb = b.at(s.v119, q.l);
                    if (bb < (real)0)
                        bb = (real)0;
                    s.ib = bb;
                    ie = bb * q.f;
#ifdef VA909_PROBE
                    probe_[3] = vce, probe_[6] = node, probe_[7] = bb;
#endif
                    s.v119 -= s.ib * dtc119_;
                }
                else
                    s.ib = (real)0;
                // IC50a (LEVEL with C120), IC50b's high pass, R421 / R419
                real v = -p.vr19 * ie;
                if (v > p.vsat)
                    v = p.vsat;
                if (v < -p.vsat)
                    v = -p.vsat;
                s.lp += (v - s.lp) * kLp_;
#ifdef VA909_PROBE
                probe_[4] = ie, probe_[5] = s.lp;
#endif
                out[i] = p.outK * hp.process(s.lp);
            }
            // asleep once Q65's base current is 55 dB under a hit's (C119 only empties through it: ib ~ C vt / t at
            // the end, 2 nA would take 0.6 s) and the bridged Ts have rung out
            if (s.charge == 0 && s.ib < (real)1e-8 && r0.quiet() && r1.quiet() && r2.quiet())
                s.idle = true;
            s_ = s;
            b65_ = b;
            res_[0] = r0, res_[1] = r1, res_[2] = r2;
            hp_ = hp;
        }

        real env() const { return s_.v119; }
#ifdef VA909_PROBE
        // F1, F2, F3, Q65's collector, its emitter current, IC50a's output, the node's open circuit voltage, Q65's base
        // current (fit/rsnative PROBE=)
        real probe_[8] = {};
        unsigned newtonSteps_ = 0;
#endif

    private:
        // F1 ... F3: an op amp with a bridged T in its feedback (Rb from the output to the - input, Ca from there to
        // the T's node X, Cb from X to the output, Rs from X to ground), the spike at the + input. The states: Ca's
        // voltage qa (- input to X) and Cb's qb (output to X). While the op amp follows, the - input is the spike x and
        // the output x - qa + qb; at its swing the output stays there and the - input floats (vout - qb + qa) until it
        // passes x again: the T rings on from the clipped output, not from what the closed loop would have put out.
        // Each mode a linear system, the trapezoidal rule (the resonance some 0.2 % high at 1 kHz).
        struct Resonator
        {
            real l11, l12, l21, l22, lb1, lb2; // linear: q' = Ad q + Bd (u + u') / 2, u = x
            real c11, c12, c21, c22, cb1, cb2; // at the swing: u = +-vsat
            real qa = 0, qb = 0, u = 0;
            int clamp = 0;
            static void disc(real a11, real a12, real a21, real a22, real b2, real dt, real &d11, real &d12, real &d21,
                             real &d22, real &e1, real &e2)
            {
                // (I - A dt / 2)^-1 (I + A dt / 2), (I - A dt / 2)^-1 B dt
                const real h = dt * (real)0.5;
                const real m11 = (real)1 - a11 * h, m12 = -a12 * h, m21 = -a21 * h, m22 = (real)1 - a22 * h;
                const real det = m11 * m22 - m12 * m21;
                const real i11 = m22 / det, i12 = -m12 / det, i21 = -m21 / det, i22 = m11 / det;
                const real p11 = (real)1 + a11 * h, p12 = a12 * h, p21 = a21 * h, p22 = (real)1 + a22 * h;
                d11 = i11 * p11 + i12 * p21, d12 = i11 * p12 + i12 * p22;
                d21 = i21 * p11 + i22 * p21, d22 = i21 * p12 + i22 * p22;
                e1 = i12 * b2 * dt, e2 = i22 * b2 * dt;
            }
            void set(real rb, real cc, real rs, real fs)
            {
                const real dt = (real)1 / fs, ca = cc, cb = cc;
                const real ka = (real)1 / (rb * ca), b2 = (real)1 / (rs * cb);
                disc(-ka, ka, ((real)1 / rb - (real)1 / rs) / cb, (real)-1 / (rb * cb), b2, dt, l11, l12, l21, l22, lb1,
                     lb2);
                disc(-ka, ka, (real)1 / (rb * cb), -((real)1 / rs + (real)1 / rb) / cb, b2, dt, c11, c12, c21, c22, cb1,
                     cb2);
            }
            real process(real x, real vsat)
            {
                real y;
                if (clamp == 0)
                {
                    const real w = (u + x) * (real)0.5;
                    const real a = l11 * qa + l12 * qb + lb1 * w, b = l21 * qa + l22 * qb + lb2 * w;
                    qa = a, qb = b, u = x;
                    y = x - qa + qb;
                    if (y > vsat)
                        clamp = 1, y = vsat, u = vsat;
                    else if (y < -vsat)
                        clamp = -1, y = -vsat, u = -vsat;
                }
                else
                {
                    const real v = clamp > 0 ? vsat : -vsat;
                    const real a = c11 * qa + c12 * qb + cb1 * v, b = c21 * qa + c22 * qb + cb2 * v;
                    qa = a, qb = b;
                    y = v;
                    const real vm = v - qb + qa; // the - input
                    if (clamp > 0 ? vm > x : vm < x)
                        clamp = 0, y = x - qa + qb, u = x;
                }
                return y;
            }
            bool quiet() const
            {
                return clamp == 0 && qa < (real)1e-4 && qa > (real)-1e-4 && qb < (real)1e-4 && qb > (real)-1e-4;
            }
        };

        struct State
        {
            uint32_t count;
            int charge;
            bool idle;
            real spike, v119, ib, lp, vce;
        } s_;

        const RSParams *p_ = nullptr;
        real fs_ = 48000, dtc111_ = 0, irs_ = 0, ir399_ = 0, ir399398_ = 0, k398_ = 0, kDiv_ = 0, kCharge_ = 0;
        real dtc119_ = 0, isEff_ = 0, accb_ = 0;
        real rth_ = 0, w1_ = 0, w2_ = 0, w3_ = 0, rn_ = 0, kLp_ = 0;
        int chargeN_ = 38;
        Resonator res_[3];
        Table<128> clip_;
        VcaTable<64> vca_;
        BaseCurrent b65_;
        Biquad hp_;
    };
}
