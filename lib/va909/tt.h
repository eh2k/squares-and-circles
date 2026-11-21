// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the TR-909 toms modelled from their circuit. (c) 2026 E. Heidt.
//
// The voicing board of the Service Notes (JUN. 15, 1984, p. 11); MODEL.md names each part. The low tom's
// designators; the mid and the high tom are the same circuit with other capacitors (TTParams' c*, MODEL.md).
//
//   trigger   TRIG turns on Q16; Q16 turns on Q15 (TP: +15 V through R91 100 ohm, 2 ms) and Q17, which gates the
//             accent (IC18b: 1 + R119 / R120, R118 220k to -15 V) onto the envelopes' capacitors (ga)
//   rails     IC14a buffers TUNE (VR10 between R113 68k and R112 10k): the 4069's (IC16) upper rail; IC14b makes
//             the lower one: -(the upper) - R64 (v16 / R60 + v17 / R61), C16 / C17 (ENV4) charged by TP through
//             D15 / D14, emptied into IC14b's summing node: the slopes up get steeper, the pitch bends down
//   VCO-1..3  a Schmitt trigger (IC15a, R67 / R69: +-vsat / 2) through two 4069 inverters at those rails into an
//             integrator (IC15b, R73 47k, C18 / C19 / C20): triangles, down at the upper rail's slope, up at the
//             lower's; TP forces the comparators low (D13) and shorts the capacitors through Q13 and R71 10k
//   attack    TP charges C24 (D17); R101 1M empties it into Q18's base: Q18 bypasses R103 with R102 1k for some
//             10 ms, VCO-1 hits its shaper harder
//   VCAs      each triangle through 10k into antiparallel diodes, 2.2k into 47 ohm: the collectors of Q19 / Q20 / Q21
//             (selected), their emitters in IC18a's summing node (LEVEL VR12 its feedback); their bases: Q19 ENV1
//             (C22 through R107 470k) and ENV2 (R108 220k), Q20 ENV2 (C21 -> R100 -> C23 -> R109 220k; DECAY VR11
//             + R111 empty C21), Q21 ENV3 (C25 through R110 470k); ga charges C22, C21, C25 through D24, D19, D25
//   noise     the tom noise (IC27, Q33, shared by the three toms: TomNoise) into Q21's collector through R114 22k
#pragma once
#include "va909.h"
#include "sd.h" // Biquad

namespace va909
{
#define VA909_TT_PARAMS(X)                                                                                             \
    X(vcc, 15.0, "V, the supplies")                                                                                    \
    X(vsat, 13.5, "V, the op amps' output swing")                                                                      \
    X(vd, 0.6, "V, a diode's drop")                                                                                    \
    X(vt, 0.02585, "V, the thermal voltage")                                                                           \
    X(trigMs, 2.0, "ms, the TRIG pulse")                                                                               \
    X(accGain, 3.173, "IC18b's gain: 1 + R119 / R120 + R119 / R118 (10k / 4.7k, 10k / 220k)")                          \
    X(accOff, 0.682, "V, its offset: 15 V R119 / R118 (R118 to -15 V)")                                                \
    X(vtpHi, 14.9, "V, TP while TRIG is high (Q15 saturated)")                                                         \
    X(rGa, 200.0, "ohm, the gated accent's source (Q17, R96 100) into C22, C21, C25")                                  \
    /* the rails */                                                                                                    \
    X(r113, 68e3, "ohm, R113: +15 V to TUNE's top")                                                                    \
    X(vr10, 10e3, "ohm, VR10 TUNE (10k B)")                                                                            \
    X(r112, 10e3, "ohm, R112: TUNE's bottom to ground")                                                                \
    X(tune3, 0.3, "TUNE's wiper at 3 of 10")                                                                           \
    X(tune7, 0.7, "TUNE's wiper at 7 of 10")                                                                           \
    X(r64, 100e3, "ohm, R64: IC14b's feedback")                                                                        \
    X(r66, 100e3, "ohm, R66: the upper rail into IC14b")                                                               \
    X(c16, 0.1e-6, "F, C16: ENV4's slow part")                                                                         \
    X(r60, 2.2e6, "ohm, R60: C16 into IC14b")                                                                          \
    X(c17, 0.047e-6, "F, C17: ENV4's fast part")                                                                       \
    X(r61, 470e3, "ohm, R61: C17 into IC14b")                                                                          \
    /* the VCOs */                                                                                                     \
    X(r73, 47e3, "ohm, R73 / R78 / R75: the 4069's output into the integrators")                                       \
    X(rOn, 0.0, "ohm, the 4069's output resistance at 1 V of rail (falls with it as 1/V)")                            \
    X(c1, 0.022e-6, "F, C18: VCO-1 (MT C32 0.018 uF, HT C97 0.015 uF)")                                                \
    X(c2, 0.033e-6, "F, C19: VCO-2 (MT C33 0.027 uF, HT C102 0.022 uF)")                                               \
    X(c3, 0.012e-6, "F, C20: VCO-3 (MT C34 0.01 uF, HT C103 0.0082 uF)")                                               \
    X(th, 6.75, "V, the comparators' thresholds: +-vsat R67 / (R67 + R69)")                                            \
    X(cmpDelay, 40e-6, "s, the comparators' and the 4069's switching: the triangles overshoot by their slope times it") \
    X(r71, 10e3, "ohm, R71 / R77 / R84: the reset's resistance (Q13, Q14, Q22)")                                       \
    X(r72, 1e6, "ohm, R72: TP into Q13's base (its base current shifts the reset level)")                              \
    /* the attack */                                                                                                   \
    X(c24, 0.01e-6, "F, C24: Q18's base (MT C38 6800 pF, HT C101 5600 pF)")                                            \
    X(r101, 1e6, "ohm, R101: C24 into Q18's base")                                                                     \
    X(r102, 1e3, "ohm, R102: Q18's collector to VCO-1")                                                                \
    X(bf18, 250.0, "Q18's gain")                                                                                       \
    /* the shapers and the VCAs */                                                                                     \
    X(r103, 10e3, "ohm, R103 / R105 / R117: a VCO into its diodes")                                                    \
    X(r104, 2.2e3, "ohm, R104 / R106 / R116")                                                                          \
    X(r99, 47.0, "ohm, R99 / R98 / R115: the VCA's collector")                                                         \
    X(dIs, 2.5e-9, "A, the shapers' diodes' saturation current")                                                       \
    X(dN, 1.75, "their emission coefficient")                                                                          \
    X(qBf, 250.0, "the VCAs' forward gain")                                                                            \
    X(qBr, 5.0, "their reverse gain")                                                                                  \
    X(qIs, 1e-14, "A, their saturation current")                                                                       \
    X(c22, 0.22e-6, "F, C22: ENV1 (MT C36, HT C99 0.15 uF)")                                                           \
    X(r107, 470e3, "ohm, R107: ENV1 into Q19's base")                                                                  \
    X(r108, 220e3, "ohm, R108: ENV2 into Q19's base")                                                                  \
    X(c21, 0.68e-6, "F, C21: ENV2 (MT C35, HT C98 0.47 uF)")                                                           \
    X(vr11, 500e3, "ohm, VR11 DECAY (500k B, a rheostat)")                                                             \
    X(decay3, 0.3, "DECAY's share at 3 of 10")                                                                         \
    X(decay7, 0.7, "DECAY's share at 7 of 10")                                                                         \
    X(r111, 56e3, "ohm, R111: DECAY's end")                                                                            \
    X(r100, 100e3, "ohm, R100: C21 into C23")                                                                          \
    X(c23, 0.68e-6, "F, C23 (MT C37, HT C100 0.47 uF)")                                                                \
    X(r109, 220e3, "ohm, R109: ENV2 into Q20's base")                                                                  \
    X(c25, 0.056e-6, "F, C25: ENV3 (HT C106 0.047 uF)")                                                                \
    X(r110, 470e3, "ohm, R110: ENV3 into Q21's base")                                                                  \
    X(r114, 22e3, "ohm, R114: the tom noise into Q21's collector")                                                     \
    X(vr12, 50e3, "ohm, VR12 LEVEL (50k B) at its top")                                                                \
    X(qN, 1.0, "the VCAs' base-emitter emission coefficient where the envelopes reach Vbe (1: the ideal junction)")   \
    X(qIRef, 1e-5, "A, the base current at which that law meets the ideal one (qIs)")                                  \
    X(br18, 5.0, "Q18's reverse gain")

    struct TTParams
    {
#define X(name, value, comment) real name;
        VA909_TT_PARAMS(X)
#undef X
    };

    static const TTParams ltCircuit = {
#define X(name, value, comment) (real)value,
        VA909_TT_PARAMS(X)
#undef X
    };

    // the mid and the high tom: the low one's with their capacitors
    inline TTParams ttCircuit(int tom)
    {
        TTParams p = ltCircuit;
        if (tom == 1)
        {
            p.c1 = (real)0.018e-6, p.c2 = (real)0.027e-6, p.c3 = (real)0.01e-6;
            p.c24 = (real)6800e-12, p.c22 = (real)0.15e-6, p.c21 = p.c23 = (real)0.47e-6;
        }
        else if (tom == 2)
        {
            p.c1 = (real)0.015e-6, p.c2 = (real)0.022e-6, p.c3 = (real)8200e-12;
            p.c24 = (real)5600e-12, p.c22 = (real)0.15e-6, p.c21 = p.c23 = (real)0.47e-6, p.c25 = (real)0.047e-6;
        }
        return p;
    }

    static const char *const ttParamNames[] = {
#define X(name, value, comment) #name,
        VA909_TT_PARAMS(X)
#undef X
    };
    static constexpr int ttNumParams = sizeof(ttParamNames) / sizeof(ttParamNames[0]);

    inline real *ttParamByIndex(TTParams &p, int i) { return (real *)&p + i; }

#define VA909_TN_PARAMS(X)                                                                                             \
    X(noiseClk, 450e3, "Hz, the noise generator's clock (the bus of the snare and the clap); 0: none")                 \
    X(r190, 22e3, "ohm, R190: the noise bus into C161")                                                                \
    X(c161, 4700e-12, "F, C161: a low pass")                                                                           \
    X(c51, 4700e-12, "F, C51 / C52: IC27b's high pass")                                                                \
    X(r191, 10e3, "ohm, R191 (its feedback)")                                                                          \
    X(r192, 220e3, "ohm, R192 (to ground)")                                                                            \
    X(c53, 0.047e-6, "F, C53: into Q33's collector")                                                                   \
    X(r195, 1e3, "ohm, R195")                                                                                          \
    X(r199, 47.0, "ohm, R199: Q33's collector")                                                                        \
    X(r193, 22e3, "ohm, R193: IC27a's feedback")                                                                       \
    X(r194, 100e3, "ohm, R194: the filtered noise into IC27a besides Q33 (47k up to SN 426700)")                       \
    X(nIb, 20e-6, "A, Q33's base current at a tom's TRIG (Q34 from C54)")                                              \
    X(nTau, 1e-3, "s, its fall while C54 (4700 pF; 2200 pF up to SN 426700) empties")                                  \
    X(trigMs, 2.0, "ms, the TRIG pulse: the burst")                                                                    \
    X(qBf, 250.0, "Q33's forward gain")                                                                                \
    X(qBr, 5.0, "its reverse gain")                                                                                    \
    X(vt, 0.02585, "V, the thermal voltage")

    struct TNParams
    {
#define X(name, value, comment) real name;
        VA909_TN_PARAMS(X)
#undef X
    };

    static const TNParams tnCircuit = {
#define X(name, value, comment) (real)value,
        VA909_TN_PARAMS(X)
#undef X
    };

    static const char *const tnParamNames[] = {
#define X(name, value, comment) #name,
        VA909_TN_PARAMS(X)
#undef X
    };
    static constexpr int tnNumParams = sizeof(tnParamNames) / sizeof(tnParamNames[0]);

    // The tom noise: the noise bus through R190 / C161, IC27b's Sallen-Key high pass (some 720 Hz, Q 2.3), C53
    // into Q33's 47 ohm collector; Q33's base gets a burst at each tom's TRIG; IC27a sums Q33's emitter and R194's
    // share of the filtered noise. Its output in volts, into the toms' Q21 (R114).
    class TomNoise
    {
    public:
        void init(real sampleRate, const TNParams *p)
        {
            fs_ = sampleRate;
            setParams(p);
        }
        void setParams(const TNParams *p)
        {
            p_ = p;
            const real dt = (real)1 / fs_;
            amp_ = p->noiseClk > (real)0 ? (real)0.5 * (real)15 * std::sqrt(fs_ / p->noiseClk) * (real)1.7320508 : (real)0;
            k161_ = (real)1 - std::exp(-dt / (p->r190 * p->c161));
            hp_.set(true, (real)1 / ((real)6.2831853 * p->c51 * std::sqrt(p->r191 * p->r192)),
                    (real)0.5 * std::sqrt(p->r192 / p->r191), fs_);
            k53_ = (real)1 - std::exp(-dt / ((p->r195 + p->r199) * p->c53));
            a199_ = p->r199 / (p->r195 + p->r199);
            kBurst_ = std::exp(-dt / p->nTau);
            burstN_ = (int)(p->trigMs * (real)0.001 * fs_ + (real)0.5);
            vca_.fill((real)-0.05, (real)0.05, p->qBf, p->qBr, p->vt, [](real x) { return x; });
        }
        void trigger()
        {
            ib_ = p_->nIb;
            left_ = burstN_;
            on_ = (int)(fs_ * (real)0.5); // the toms' ENV3 is out by then
        }
        bool idle() const { return on_ <= 0; }
        void process(real *__restrict out, int n)
        {
            if (on_ <= 0)
            {
                for (int i = 0; i < n; i++)
                    out[i] = (real)0;
                return;
            }
            on_ -= n;
            const TNParams &p = *p_;
            const real ir194 = (real)1 / p.r194;
            for (int i = 0; i < n; i++)
            {
                const real vn = amp_ * nz_.next();
                v161_ += (vn - v161_) * k161_;
                const real x = hp_.process(v161_);
                v53_ += (x - v53_) * k53_;
                real ie = (real)0;
                if (left_ > 0)
                {
                    left_--;
                    real f, l;
                    vca_.at2((x - v53_) * a199_, f, l);
                    ie = ib_ * f;
                    ib_ *= kBurst_;
                }
                out[i] = -p.r193 * (ie + x * ir194);
            }
        }

    private:
        const TNParams *p_ = nullptr;
        real fs_ = 48000, amp_ = 0, k161_ = 0, k53_ = 0, a199_ = 0, kBurst_ = 0, ib_ = 0, v161_ = 0, v53_ = 0;
        int burstN_ = 96, left_ = 0, on_ = 0;
        Biquad hp_;
        Noise nz_;
        VcaTable<32> vca_;
    };

    // a VCO's triangle: down at dDown a sample, up at dUp, turning at lo / hi; polyBLAMP at the corners (one
    // sample late)
    struct TomVco
    {
        real tri = 0, corr = 0;
        int dir = -1;
        real step(real dDown, real dUp, real lo, real hi)
        {
            real corrNext = (real)0;
            real t;
            if (dir > 0)
            {
                t = tri + dUp;
                if (t > hi)
                {
                    const real over = (t - hi) / dUp; // the share of the sample after the corner
                    t = hi - over * dDown;
                    dir = -1;
                    const real k = (dUp + dDown) * (real)(1.0 / 6.0), u = (real)1 - over;
                    corr -= k * over * over * over;
                    corrNext = -k * u * u * u;
                }
            }
            else
            {
                t = tri - dDown;
                if (t < lo)
                {
                    const real over = (lo - t) / dDown;
                    t = lo + over * dUp;
                    dir = 1;
                    const real k = (dUp + dDown) * (real)(1.0 / 6.0), u = (real)1 - over;
                    corr += k * over * over * over;
                    corrNext = k * u * u * u;
                }
            }
            const real out = tri + corr;
            tri = t;
            corr = corrNext;
            return out;
        }
        real reset(real target, real k) // while TP is high: towards target, falling after
        {
            tri = target + (tri - target) * k;
            corr = (real)0;
            dir = -1;
            return tri;
        }
    };

    // Q18's emitter current (the attack) with its base current ib, VCO-1 at x: its collector through R102 at x, its
    // emitter at the diodes' node a (x through R103 into them, R104 + R99 their load). Ebers-Moll with ib forced (as
    // SatVca, Q18's gains): ie / ib = -br + (bf + 1 + br) / (1 + e^-z), z = (vce - vt ln(bf / br)) / vt, a logistic
    // step in vce from -br ib to (bf + 1) ib around vt ln(bf / br) (0.1 V); vce = x - a - R102 (ie - ib). So no switch:
    // under its limit vce = vt ln(1 + ie / (br ib)) and more, at 2 uA of base current 29 mV for 20 uA (20 mV over
    // R102), 73 mV for 0.12 mA (SPICE the same); the knee towards the limit as soft. z by Newton within a bracket (the
    // node's KCL rises with z); at setup (TomTables)
    inline real q18Current(const TTParams &p, real ib, real x, real &z)
    {
        const real rl = p.r104 + p.r99, nvt = p.dN * p.vt, vt = p.vt;
        const real ir = p.br18 * ib, d = (p.bf18 + (real)1 + p.br18) * ib, v0 = vt * std::log(p.bf18 / p.br18);
        const real ir103 = (real)1 / p.r103, irl = (real)1 / rl, is = p.dIs;
        // the node's KCL at z (rising with it) and its slope
        auto kcl = [&](real zz, real &dh)
        {
            const real s = (real)1 / ((real)1 + std::exp(-zz)), ie = -ir + d * s, die = d * s * ((real)1 - s);
            const real a = x - v0 - vt * zz - p.r102 * (ie - ib);
            real e = a / nvt;
            e = e > (real)80 ? (real)80 : e < (real)-80 ? (real)-80 : e;
            const real ex = std::exp(e), emx = (real)1 / ex;
            dh = die + (vt + p.r102 * die) * (ir103 + is * (ex + emx) / nvt + irl);
            return ie + (x - a) * ir103 - is * (ex - emx) - a * irl;
        };
        real lo = (real)-30, hi = (real)30, dh;
        if (kcl(hi, dh) <= (real)0) // at its limit (vce some 0.9 V or more)
        {
            z = hi;
            return (p.bf18 + (real)1) * ib;
        }
        if (kcl(lo, dh) >= (real)0) // at its reverse limit
        {
            z = lo;
            return -ir;
        }
        // Newton, bisecting where its step leaves the bracket or does not halve the step before (the diodes' sinh
        // far from the root: tiny steps)
        if (!(z > lo && z < hi))
            z = (real)0;
        real step = hi - lo;
        for (int k = 0; k < 80; k++)
        {
            const real h = kcl(z, dh);
            if (h > (real)0)
                hi = z;
            else
                lo = z;
            const real dz = h / dh, zn = z - dz;
            const real last = step;
            if (!(zn > lo && zn < hi) || (dz + dz) * (dz + dz) > last * last)
            {
                step = (real)0.5 * (hi - lo);
                z = lo + step;
            }
            else
            {
                step = dz;
                z = zn;
            }
            if ((step < (real)1e-5 && step > (real)-1e-5) || hi - lo < (real)1e-5)
                break;
        }
        return -ir + d / ((real)1 + std::exp(-z));
    }

    // the tables the three toms share (their shared constants): the shaper (a triangle through R103 into the
    // diodes, R104 into R99: the collector's open voltage) and the VCA in one table over the triangle (VCO-2, VCO-1
    // once Q18 is off), the same over a companded axis (VCO-1 while Q18 drives it, up to some 80 V), the shaper alone
    // and the VCA over the collector (VCO-3: the tom noise comes in at its collector), Q18's current over the triangle
    // and its base current (q18Current)
    struct TomTables
    {
        static constexpr int kComp = 96;
        // Q18's rows: row 0 ib = 0, row k ib = bendIb1 2^(k - 1) up to twice C24's first base current (13.75 uA, the
        // circuit's), linear in ib between them (exact where Q18 is at its limit). Its knee moves with ib, the blend of
        // two rows softens it: within 3.6 % of the peak of the law solved at each x, 3.2 % of SPICE's (0.54 uA; the
        // law 0.6 ... 1.0 %, the switch before 15 %, fit/ttq18.py). Tried with the law in Python, not taken: rows of
        // 2^(1/2) (a quarter of that off for twice the bytes), more rows under bendIb1 (54 nA: nothing)
        static constexpr int kBendRows = 11, kBendX = 64;
        real ax = 4, xMax = 0;
        VcaTable<64> tri;
        VcaPoint comp[kComp];
        Table<64> shaper;
        VcaTable<48> node;
        real bendIb1 = 0, bendIIb1 = 0;
        real bend[kBendRows][kBendX]; // R103 times Q18's emitter current: the drive through R103 alone it adds
        void fill(const TTParams &p)
        {
            const real rl = p.r104 + p.r99, k = p.r99 / rl, nvt = p.dN * p.vt;
            const real r103 = p.r103, is = p.dIs;
            auto sh = [=](real x) { return diodeShaper(x, r103, rl, is, nvt) * k; };
            const real tmax = p.th * (real)1.3;
            xMax = tmax * ((real)1 + p.r103 / p.r102);
            tri.fill(-tmax, tmax, p.qBf, p.qBr, p.vt, sh);
            shaper.fill(-tmax, tmax, sh);
            // C24 at TP's level, its base current through R101 (Tom::process: Q18's base 0.55 V); the top row twice it
            const real ibTop = (real)2 * (p.vtpHi - p.vd - (real)0.55) / p.r101;
            bendIb1 = ibTop / (real)(1 << (kBendRows - 2));
            bendIIb1 = (real)1 / bendIb1;
            for (int i = 0; i < kBendX; i++)
                bend[0][i] = (real)0;
            for (int r = 1; r < kBendRows; r++)
            {
                const real ib = bendIb1 * (real)(1 << (r - 1));
                real z = (real)0;
                for (int i = 0; i < kBendX; i++)
                    bend[r][i] = p.r103 * q18Current(p, ib, shaper.lo + (real)i / shaper.scale, z);
            }
            SatVca sv;
            sv.init(p.qBf, p.qBr, p.vt);
            for (int i = 0; i < kComp; i++)
            {
                const real u = (real)2 * (real)i / (real)(kComp - 1) - (real)1; // u = x / (|x| + ax)
                const real uu = u > (real)0.999 ? (real)0.999 : u < (real)-0.999 ? (real)-0.999 : u;
                const real x = ax * uu / ((real)1 - (uu < (real)0 ? -uu : uu));
                vcaPoint(sv, p.vt, (real)1 / p.qBf, (real)1 / p.qBr, sh(x), (real)1e-4, comp[i]);
            }
            const real vmax = sh(xMax) * (real)1.3;
            node.fill(-vmax, vmax, p.qBf, p.qBr, p.vt, [](real v) { return v; });
        }
        void atComp(real x, VcaPoint &q) const
        {
            const real ux = x / ((x < (real)0 ? -x : x) + ax);
            const real f = (ux + (real)1) * (real)(0.5 * (kComp - 1));
            int i = (int)f;
            if (i > kComp - 2)
                i = kComp - 2;
            if (i < 0)
                i = 0;
            const real t = f - (real)i;
            const VcaPoint &a = comp[i], &b = comp[i + 1];
            q.f = a.f + t * (b.f - a.f);
            q.df = a.df + t * (b.df - a.df);
            q.l = a.l + t * (b.l - a.l);
            q.dl = a.dl + t * (b.dl - a.dl);
        }
        // the rows around Q18's base current ib and the share of the upper one (every 4 samples): ib / bendIb1 =
        // m 2^e, m in [1, 2): rows e + 1 and e + 2, the share m - 1
        void bendRow(real ib, int &row, real &f) const
        {
            const real u = ib * bendIIb1;
            if (!(u >= (real)1))
            {
                row = 0;
                f = u > (real)0 ? u : (real)0;
                return;
            }
            int e;
            real m;
            if constexpr (sizeof(real) == 4)
            {
                union
                {
                    float f;
                    int32_t i;
                } b = {(float)u};
                e = ((b.i >> 23) & 255) - 127;
                b.i = (b.i & 0x007FFFFF) | 0x3F800000;
                m = (real)b.f;
            }
            else
            {
                m = (real)2 * std::frexp(u, &e);
                e--;
            }
            row = e + 1;
            f = m - (real)1;
            if (row > kBendRows - 2)
                row = kBendRows - 2, f = (real)1;
        }
        // VCO-1 at x with Q18's current: the drive through R103 alone that makes the same node (atComp's axis)
        real bendDrive(real x, int row, real f) const
        {
            real v = (x - shaper.lo) * shaper.scale;
            if (v < (real)0)
                v = (real)0;
            if (v > (real)(kBendX - 1) - (real)0.001)
                v = (real)(kBendX - 1) - (real)0.001;
            const int k = (int)v;
            const real t = v - (real)k;
            const real *a = bend[row] + k, *b = bend[row + 1] + k;
            const real ya = a[0] + t * (a[1] - a[0]);
            const real yb = b[0] + t * (b[1] - b[0]);
            return x + ya + f * (yb - ya);
        }
    };

    class Tom
    {
    public:
        void init(real sampleRate, const TTParams *p, const TomTables *tables)
        {
            fs_ = sampleRate;
            dt_ = (real)1 / sampleRate;
            p_ = p;
            tab_ = tables;
            memset(&s_, 0, sizeof(s_));
            s_.idle = true;
            v1_ = v2_ = v3_ = TomVco();
            tune_ = decay_ = (real)-1;
            setKnobs((real)0.5, (real)0.5);
            prepare();
        }

        void setParams(const TTParams *p)
        {
            p_ = p;
            prepare();
        }

        // TUNE, DECAY 0..1
        void setKnobs(real tune, real decay)
        {
            if (tune != tune_ || decay != decay_)
            {
                tune_ = tune;
                decay_ = decay;
                dirty_ = true;
            }
        }

        // TRIG with the ACCENT line at acc volts
        void trigger(real acc)
        {
            const TTParams &p = *p_;
            real a = acc * p.accGain + p.accOff;
            if (a > p.vsat)
                a = p.vsat;
            ga_ = a - (real)0.05;
            s_.pulse = trigSamples_;
            s_.count = 0;
            s_.idle = false;
        }

        bool idle() const { return s_.idle; }
        // whether the next block reads the tom noise (VCO-3's VCA on, or a TRIG about to turn it on)
        bool wantsNoise() const { return !s_.idle && (s_.on21 || s_.pulse > 0); }

        // noise: the tom noise's output (TomNoise, volts) for these n samples, or nullptr
        void process(real *__restrict out, int n, const real *noise)
        {
            if (dirty_)
                knobCoeffs();
            if (s_.idle)
            {
                for (int i = 0; i < n; i++)
                    out[i] = (real)0;
                return;
            }
            const TTParams &p = *p_;
            const TomTables &tb = *tab_;
            State s = s_;
            TomVco v1 = v1_, v2 = v2_, v3 = v3_;
            BaseCurrent b19 = b19_, b20 = b20_, b21 = b21_;
            const real vt = p.vt, vd = p.vd;
            for (int i = 0; i < n; i++)
            {
                const uint32_t c = s.count++;
                const bool tp = s.pulse > 0;
                if (tp)
                    s.pulse--;

                // every 4 samples: the envelopes' capacitors, the VCAs' base currents, ENV4, the rails, the slopes,
                // the attack (their fastest after TP: C17 22 ms, C25 26 ms)
                if ((c & 3) == 0)
                {
                    if (tp)
                    {
                        s.v16 = s.v17 = s.v24 = vch_;
                        const real top = ga_ - vd;
                        if (top > s.v22)
                            s.v22 += (top - s.v22) * kGa22_;
                        if (top > s.v21)
                            s.v21 += (top - s.v21) * kGa21_;
                        if (top > s.v25)
                            s.v25 += (top - s.v25) * kGa25_;
                    }
                    else
                    {
                        s.v16 *= k16_;
                        s.v17 *= k17_;
                        real ib18 = (s.v24 - (real)0.55) * ir101_;
                        if (ib18 < (real)0)
                            ib18 = (real)0;
                        s.v24 -= ib18 * dtc24x4_;
                        s.q18 = p.bf18 * ib18; // what Q18 can pass
                        tb.bendRow(ib18, s.bendRow, s.bendF);
                    }
                    // Q19's base node: ENV1 (R107) and ENV2 (R108) meet there (C22 and C23 even out through it
                    // while Q19 is off); C21 into C23 through R100; DECAY empties C21
                    s.vth19 = (s.v22 * ir107_ + s.v23 * ir108_) * rth19_;
                    const real vb19 = s.vth19 - s.ib19 * rth19_;
                    const real i100 = (s.v21 - s.v23) * ir100_;
                    s.v21 -= (s.v21 * gDecay_ + i100) * dtc21x4_;
                    s.v22 -= (s.v22 - vb19) * ir107_ * dtc22x4_;
                    s.v23 += (i100 - (s.v23 - vb19) * ir108_ - s.ib20) * dtc23x4_;
                    s.v25 -= s.ib21 * dtc25x4_;
                    if ((c & 15) == 0)
                    {
                        b19.update(s.vth19, rth19_, vtB_, isB_);
                        b20.update(s.v23, p.r109, vtB_, isB_);
                        b21.update(s.v25, p.r110, vtB_, isB_);
                    }
                    else
                    {
                        b19.track(s.vth19, rth19_, vtB_);
                        b20.track(s.v23, p.r109, vtB_);
                        b21.track(s.v25, p.r110, vtB_);
                    }
                    // a VCA left out once its base current is 60 dB under a hit's and its envelope falls (ENV2
                    // rises for some 100 ms): the capacitors that only empty into the bases fall hyperbolically at
                    // the end (ib ~ C vt / t), 1 nA would take seconds
                    const bool early = tp || c < riseN_;
                    s.on19 = early || s.ib19 > (real)2e-8;
                    s.on20 = early || s.ib20 > (real)2e-8;
                    // VCO-3's VCA (the tom noise, ENV3: C25 empties into Q21's base only) already 40 dB under a hit's:
                    // -46 dB under the tom's loudest then (VCA19 and VCA20 -11 / -16 dB), after some 0.12 s (the low
                    // tom at DECAY's top) where 60 dB took 0.25 s; its VCO and the tom noise left out with it
                    s.on21 = tp || s.ib21 > (real)2e-7 || c < (uint32_t)(4 * trigSamples_);
                    if (!s.on21) // C25 empties into Q21's base on (the base current without the VCA's load)
                        s.ib21 = b21.ib;
                    real lower = -railUp_ * r64r66_ - (s.v16 * ir60_ + s.v17 * ir61_) * p.r64;
                    if (lower < -p.vsat)
                        lower = -p.vsat;
                    // the integrators' current: V / (R73 + rOn / V) = V^2 / (R73 V + rOn); the upper rail's at the knob
                    const real gUp = lower * lower / (-p.r73 * lower + rOn1_), gDown = gDown_;
                    s.up1 = gUp * dtc1_, s.dn1 = gDown * dtc1_;
                    s.up2 = gUp * dtc2_, s.dn2 = gDown * dtc2_;
                    s.up3 = gUp * dtc3_, s.dn3 = gDown * dtc3_;
                    // the comparators' thresholds, overshot by the slopes
                    s.h1 = th_ + s.up1 * delayN_, s.l1 = -th_ - s.dn1 * delayN_;
                    s.h2 = th_ + s.up2 * delayN_, s.l2 = -th_ - s.dn2 * delayN_;
                    s.h3 = th_ + s.up3 * delayN_, s.l3 = -th_ - s.dn3 * delayN_;
                    // Q18 still bends VCO-1 by more than 0.1 mV at the diodes (C24 empties into its base in some 7.5
                    // ms: for the first 100 ms or so); after that VCO-1 takes the plain table, its division saved
                    s.bend = s.on19 && s.q18 * p.r103 > (real)1e-4;
                }

                // the VCOs (reset while TP is high). They run on with their VCA off, as on the 909: the TP's reset
                // leaves a little of where they were (R71 C1 ... C3 up to 0.35 ms against its 2 ms), and a VCO stopped
                // there gave the next hit another phase (-30 dB rms of difference)
                real t1, t2, t3;
                if (tp)
                {
                    t1 = v1.reset(rst_, kRst1_);
                    t2 = v2.reset(rst_, kRst2_);
                    t3 = v3.reset(rst_, kRst3_);
                }
                else
                {
                    t1 = v1.step(s.dn1, s.up1, s.l1, s.h1);
                    t2 = v2.step(s.dn2, s.up2, s.l2, s.h2);
                    t3 = v3.step(s.dn3, s.up3, s.l3, s.h3);
                }

                VcaPoint q;
                real ie = (real)0;
                if (s.on19)
                {
                    // VCO-1 into its shaper, Q18 adding its current through R102 (its collector at VCO-1, its emitter
                    // at the diodes' node; Ebers-Moll with its base current forced, q18Current): the table over VCO-1
                    // and that base current gives the drive through R103 alone that makes the same node
                    if (s.bend)
                        tb.atComp(tb.bendDrive(t1, s.bendRow, s.bendF), q);
                    else
                        tb.tri.at(t1, q);
                    ie += vcaCurrent2(q, rn_, s.vth19, b19, s.ib19);
                }
                if (s.on20)
                {
                    tb.tri.at(t2, q);
                    ie += vcaCurrent2(q, rn_, s.v23, b20, s.ib20);
                }
                if (s.on21)
                {
                    if (noise)
                        tb.node.at(tb.shaper(t3) + noise[i] * aNoise_, q);
                    else
                        tb.tri.at(t3, q);
                    ie += vcaCurrent2(q, rn_, s.v25, b21, s.ib21);
                }
                // IC18a, its output at its swing at most
                const real y = -p.vr12 * ie;
                out[i] = y > p.vsat ? p.vsat : y < -p.vsat ? -p.vsat : y;
            }
            if (s.pulse == 0 && s.count > riseN_ && !s.on19 && !s.on20 && !s.on21 && s.v21 < vbe0_)
                s.idle = true;
            s_ = s;
            v1_ = v1, v2_ = v2, v3_ = v3;
            b19_ = b19, b20_ = b20, b21_ = b21;
        }

        real env1() const { return s_.v22; }
        real env2() const { return s_.v23; }
        real env3() const { return s_.v25; }
        real env4() const { return s_.v17; }
        real tri(int k) const { return k == 0 ? v1_.tri : k == 1 ? v2_.tri : v3_.tri; }

    private:
        void prepare()
        {
            const TTParams &p = *p_;
            const real dt = dt_;
            trigSamples_ = (int)(p.trigMs * (real)0.001 * fs_ + (real)0.5);
            vch_ = p.vtpHi - p.vd;
            k16_ = std::exp((real)-4 * dt / (p.r60 * p.c16));
            k17_ = std::exp((real)-4 * dt / (p.r61 * p.c17));
            ir60_ = (real)1 / p.r60;
            ir61_ = (real)1 / p.r61;
            r64r66_ = p.r64 / p.r66;
            rOn1_ = p.rOn;
            dtc1_ = dt / p.c1;
            dtc2_ = dt / p.c2;
            dtc3_ = dt / p.c3;
            th_ = p.th;
            delayN_ = p.cmpDelay / dt;
            kRst1_ = std::exp(-dt / (p.r71 * p.c1));
            kRst2_ = std::exp(-dt / (p.r71 * p.c2));
            kRst3_ = std::exp(-dt / (p.r71 * p.c3));
            ib13_ = (p.vtpHi - p.vd) / p.r72;
            ir101_ = (real)1 / p.r101;
            dtc24x4_ = (real)4 * dt / p.c24;
            isEff_ = p.qIs * ((real)1 / p.qBf + (real)1 / p.qBr);
            // the base's law: ib = isB exp(vbe / (qN vt)), through the ideal one's vbe at qIRef
            vtB_ = p.qN * p.vt;
            isB_ = p.qIRef * std::pow(isEff_ / p.qIRef, (real)1 / p.qN);
            rn_ = p.r104 * p.r99 / (p.r104 + p.r99);
            ir107_ = (real)1 / p.r107;
            ir108_ = (real)1 / p.r108;
            rth19_ = p.r107 * p.r108 / (p.r107 + p.r108);
            ir100_ = (real)1 / p.r100;
            dtc21x4_ = (real)4 * dt / p.c21;
            dtc22x4_ = (real)4 * dt / p.c22;
            dtc23x4_ = (real)4 * dt / p.c23;
            dtc25x4_ = (real)4 * dt / p.c25;
            kGa22_ = (real)1 - std::exp((real)-4 * dt / (p.rGa * p.c22));
            kGa21_ = (real)1 - std::exp((real)-4 * dt / (p.rGa * p.c21));
            kGa25_ = (real)1 - std::exp((real)-4 * dt / (p.rGa * p.c25));
            riseN_ = (uint32_t)((real)0.15 * fs_);
            vbe0_ = (real)0.45;
            aNoise_ = rn_ / p.r114; // the noise's current into the collector's node
            knobCoeffs();
        }

        void knobCoeffs()
        {
            const TTParams &p = *p_;
            dirty_ = false;
            const real tw = potTaper(tune_, p.tune3, p.tune7);
            railUp_ = p.vcc * (p.r112 + p.vr10 * tw) / (p.r113 + p.vr10 + p.r112);
            gDown_ = railUp_ * railUp_ / (p.r73 * railUp_ + p.rOn);
            // the reset: the integrator's output while Q13 shorts it through R71: -R71 (the upper rail's current - Q13's
            // base current)
            rst_ = -p.r71 * (railUp_ / p.r73 - ib13_);
            gDecay_ = (real)1 / (p.vr11 * potTaper(decay_, p.decay3, p.decay7) + p.r111);
        }

        struct State
        {
            int pulse;
            uint32_t count;
            bool idle;
            real v16, v17, v24, q18, bendF, v22, v21, v23, v25, vth19;
            int bendRow; // Q18's rows (TomTables::bendRow)
            bool on19, on20, on21;
            real up1, dn1, up2, dn2, up3, dn3;
            real h1, l1, h2, l2, h3, l3;
            bool bend;
            real ib19, ib20, ib21;
        } s_;

        const TTParams *p_ = nullptr;
        const TomTables *tab_ = nullptr;
        real fs_ = 48000, dt_ = (real)1 / 48000;
        real tune_ = -1, decay_ = -1, ga_ = 0;
        bool dirty_ = true;
        int trigSamples_ = 96;
        real vch_, k16_, k17_, ir60_, ir61_, r64r66_, rOn1_, dtc1_, dtc2_, dtc3_, th_, delayN_, kRst1_, kRst2_, kRst3_;
        real ib13_, ir101_, dtc24x4_, isEff_, isB_ = 0, vtB_ = (real)0.02585, rn_, ir107_, ir108_, rth19_;
        real ir100_, dtc21x4_, dtc22x4_, dtc23x4_, dtc25x4_;
        uint32_t riseN_ = 7200;
        real kGa22_, kGa21_, kGa25_, vbe0_, aNoise_, railUp_ = 2.5f, gDown_ = 0, rst_ = 0, gDecay_ = 0;
        TomVco v1_, v2_, v3_;
        BaseCurrent b19_, b20_, b21_;
    };
}
