// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the TR-909 hand clap modelled from its circuit. (c) 2026 E. Heidt.
//
// The voicing board of the Service Notes (JUN. 15, 1984, p. 11); MODEL.md names each part.
//
//   trigger   TRIG turns on Q35, Q35 Q36: the accent (IC26a, x3.13) gated (ga) into R213/R218 and D52
//   bursts    IC29 (AN6912, open collectors on 0 / -15 V, the ladder R219/R217/R214: -5 V, -10 V): a pulls C62 to
//             -15 V while TRIG is high, R229 lets it back (27 ms); b sinks while C62 is under -5 V (some 30 ms) and
//             sets d's threshold (R227/R228: -10 V); c sinks while b doesn't, else lets R220 charge C65 through D54;
//             d (C63 its positive feedback, R225 from C65) turns Q40 on whenever C65 has risen past the threshold:
//             Q40 empties C65 through R223 (330 ohm) at once: a sawtooth on C65 every some 10 ms while b sinks, a
//             long one after
//   OTA       C65 (through R223, or -15 V while Q40 is on) on Q38's left base (R224/R238), TM on its right
//             (R494/R230): the pair steers the accent's current (R235) into the BA662's bias; the band passed noise
//             through C56, R202/R201 into it; its output into R237 (82k), clipping at its swing, C58 into IC28a
//   tail      the band passed noise through C55, R204 / C57 (a low pass) and R205 into Q37 at R203 (47 ohm); ENV2:
//             ga charges C59 through D52, R233 into C61, R234 to Q37's base
//   noise     the noise bus through IC26b's band pass (R208, R209, R207, C42, C43: some 960 Hz, Q 2.1)
//   output    IC28a, LEVEL (VR20) its feedback: out = -VR20 (i58 + ie37)
#pragma once
#include "va909.h"
#include "sd.h" // Biquad

namespace va909
{
#define VA909_CP_PARAMS(X)                                                                                             \
    X(vee, 15.0, "V, the negative supply (IC29's lower rail)")                                                         \
    X(vsat, 13.5, "V, the op amps' swing; the OTA's output clips there too")                                           \
    X(vd, 0.6, "V, a diode's drop")                                                                                    \
    X(vt, 0.02585, "V, the thermal voltage")                                                                           \
    X(trigMs, 2.0, "ms, the TRIG pulse")                                                                               \
    X(accGain, 3.128, "IC26a's gain, 1 + R212 / R206 (10k / 4.7k)")                                                    \
    /* the bursts */                                                                                                   \
    X(r229, 1e6, "ohm, R229: C62 back to 0 V")                                                                         \
    X(c62, 0.027e-6, "F, C62: how long the bursts go on")                                                              \
    X(thB, -5.0, "V, the ladder's tap at b's input (R219 / R217 / R214, 0 / -15 V)")                                  \
    X(thD, -10.0, "V, d's threshold while b sinks (R227 2.7k / R228 5.6k of -14.8 V)")                                 \
    X(r220, 82e3, "ohm, R220: C65's charge (through D54) while c lets it")                                             \
    X(c65, 0.47e-6, "F, C65: the bursts' sawtooth (ENV1)")                                                             \
    X(r223, 330.0, "ohm, R223: Q40 empties C65 through it")                                                            \
    X(r224, 470e3, "ohm, R224: C65 into Q38's left base")                                                              \
    X(r238, 15e3, "ohm, R238: Q38's left base to ground")                                                              \
    X(tauPd, 0.22e-3, "s, d's input following C65: R225 (1M) times C63 (220p)")                                        \
    X(jumpPd, 9.7, "V, C63's kick on d's input when d switches (its output between -14.85 and some -5 V)")             \
    /* the OTA */                                                                                                      \
    X(vtm, -0.33, "V, Q38's right base: the TM trimmer (factory set: 2 Vpp at the multi out, LEVEL at noon)")          \
    X(r235, 15e3, "ohm, R235: the accent's current into Q38's emitters")                                               \
    X(otaK, 1.0, "the BA662's output current over its bias current (an OTA: 1; its data sheet unknown)")             \
    X(r202, 39e3, "ohm, R202: the noise into the OTA")                                                                 \
    X(r201, 1e3, "ohm, R201: the OTA's input to ground")                                                               \
    X(c56, 0.022e-6, "F, C56")                                                                                         \
    X(r237, 82e3, "ohm, R237: the OTA's load")                                                                         \
    X(c58, 1e-9, "F, C58: the OTA into IC28a's summing node")                                                          \
    /* the tail */                                                                                                     \
    X(c55, 0.47e-6, "F, C55")                                                                                          \
    X(r204, 1e3, "ohm, R204")                                                                                          \
    X(c57, 0.47e-6, "F, C57: the tail's low pass")                                                                     \
    X(r205, 3.3e3, "ohm, R205")                                                                                        \
    X(r203, 47.0, "ohm, R203: Q37's collector")                                                                        \
    X(c59, 0.1e-6, "F, C59: ENV2")                                                                                     \
    X(r233, 470e3, "ohm, R233")                                                                                        \
    X(c61, 0.1e-6, "F, C61")                                                                                           \
    X(r234, 470e3, "ohm, R234: into Q37's base")                                                                       \
    X(rGa, 200.0, "ohm, the gated accent's source into C59")                                                           \
    X(qBf, 250.0, "Q37's forward gain")                                                                                \
    X(qBr, 5.0, "Q37's reverse gain")                                                                                  \
    X(qIs, 1e-14, "A, Q37's saturation current")                                                                       \
    X(v61rest, 0.55, "V, C59 and C61 at rest (Q37's base takes them no lower; D52's leak only in minutes)")            \
    /* the noise */                                                                                                    \
    X(noiseClk, 450e3, "Hz, the noise generator's clock")                                                              \
    X(r208, 47e3, "ohm, R208: the noise into IC26b")                                                                   \
    X(r209, 10e3, "ohm, R209")                                                                                         \
    X(r207, 150e3, "ohm, R207: IC26b's feedback")                                                                      \
    X(c42, 4.7e-9, "F, C42 / C43")                                                                                     \
    X(vr20, 50e3, "ohm, VR20 LEVEL (50k B) at its top")

    struct CPParams
    {
#define X(name, value, comment) real name;
        VA909_CP_PARAMS(X)
#undef X
    };

    static const CPParams cpCircuit = {
#define X(name, value, comment) (real)value,
        VA909_CP_PARAMS(X)
#undef X
    };

    static const char *const cpParamNames[] = {
#define X(name, value, comment) #name,
        VA909_CP_PARAMS(X)
#undef X
    };
    static constexpr int cpNumParams = sizeof(cpParamNames) / sizeof(cpParamNames[0]);

    inline real *cpParamByIndex(CPParams &p, int i) { return (real *)&p + i; }

    class HandClap
    {
    public:
        void init(real sampleRate, const CPParams *p)
        {
            fs_ = sampleRate;
            dt_ = (real)1 / sampleRate;
            p_ = p;
            memset(&s_, 0, sizeof(s_));
            s_.v59 = s_.v61 = p->v61rest;
            s_.dSink = true;
            prepare();
            s_.idle = true;
        }

        // tables false: the scalar coefficients only, for parts the tables do not depend on (Q37's qBf qBr vt; the logistic and tanh curves have none)
        void setParams(const CPParams *p, bool tables = true)
        {
            p_ = p;
            if (tables)
                prepare<true>();
            else
                prepare<false>();
        }

        void trigger(real acc)
        {
            s_.pulse = trigSamples_;
            accb_ = acc * p_->accGain;
            if (accb_ > p_->vsat)
                accb_ = p_->vsat;
            if (accb_ < (real)0)
                accb_ = (real)0;
            iTail_ = p_->otaK * (accb_ - (real)0.2) / p_->r235;
            if (iTail_ < (real)0)
                iTail_ = (real)0;
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
            const CPParams &p = *p_;
            State s = s_;
            BaseCurrent ib37 = ib37_;
            Noise nz = noise_;
            Biquad bp = bp_;
            const real vee = p.vee, vr20 = p.vr20;
            for (int i = 0; i < n; i++)
            {
                if ((s.count++ & 15) == 0) // Q37's Vbe every 16 samples, sooner where C61 moves
                    ib37.update(s.v61, p.r234, p.vt, isEff_);
                else
                    ib37.track(s.v61, p.r234, p.vt);
                const bool tp = s.pulse > 0;
                if (tp)
                    s.pulse--;

                // a and C62; b while C62 is under the tap; c the other way round
                if (tp)
                    s.v62 = -vee;
                else
                    s.v62 *= k62_;
                const bool b = s.v62 < p.thB;
                const real nd = b ? p.thD : (real)0;

                // C65: charged through D54 / R220 while c lets it (b sinks), emptied by Q40, R224 towards 0 V
                real v65 = s.v65;
                if (b && v65 < -p.vd)
                    v65 = -p.vd + (v65 + p.vd) * k220_;
                if (!s.dSink) // d off: Q40 on, C65 through R223 towards -15 V
                    v65 = -vee + (v65 + vee) * k223_;
                v65 *= k224_;
                s.v65 = v65;

                // d: its input follows C65 (R225, C63), kicked by C63 when it switches
                s.pd = v65 + (s.pd - v65) * kPd_;
                if (s.dSink && s.pd > nd)
                {
                    s.dSink = false;
                    s.pd += p.jumpPd;
                }
                else if (!s.dSink && s.pd < nd)
                {
                    s.dSink = true;
                    s.pd -= p.jumpPd;
                }

                // the noise, IC26b's band pass
                const real x = bp.process(noiseAmp_ * nz.next());

                // the OTA: Q38 steers the accent's current by C65 (or -15 V through Q40) against TM
                const real m = s.dSink ? v65 : -vee;
                const real vl = m * kL_;
                const real e = (vl - p.vtm) * ivt_; // the left (OTA) side takes 1 / (1 + exp(e)) (a table)
                // C56 / (R202 + R201) a high pass, R202 / R201 a divider
                s.v56 += (x - s.v56) * k56_;
                real vinf = (real)0;
                if (e < (real)12) // else under 1e-5 of the tail current: the OTA's output nothing
                {
                    const real iabc = iTail_ * logistic_(-e);
                    // the OTA's tanh (a (3,2) rational form, 1 with zero slope at 3; a table)
                    const real th = tanh_((x - s.v56) * kIn_ * ihvt_);
                    vinf = iabc * th * p.r237;
                }
                // its output node: R237 to ground, C58 into the summing node; clipped at the swing
                real vo = vinf + (s.vo - vinf) * k58_;
                if (vo > p.vsat)
                    vo = p.vsat;
                if (vo < -p.vsat)
                    vo = -p.vsat;
                const real i58 = (vo - s.vo) * c58dt_;
                s.vo = vo;

                // the tail: C55, R204 into C57, R205 to R203; Q37 from C61
                const real it = (x - s.v55 - s.v57) * ir204_;
                s.v55 += it * dtc55_;
                s.v57 += (it - s.v57 * ir205_) * dtc57_;
                if (tp && accb_ - p.vd > s.v59)
                    s.v59 += (accb_ - p.vd - s.v59) * kGa59_;
                const real i233 = (s.v59 - s.v61) * ir233_;
                s.v59 -= i233 * dtc59_;
                s.v61 += (i233 - s.ib37) * dtc61_;
                VcaPoint q;
                q37_.at(s.v57 * k203_, q);
                const real ie37 = vcaCurrent(q, rn37_, s.v61, ib37, s.ib37);

                out[i] = -vr20 * (i58 + ie37);
            }
            // asleep once the bursts are over, Q37's base takes nothing and the OTA's bias is gone (C65 risen so far
            // that Q38 steers less than 1e-4 of its current there)
            if (s.pulse == 0 && !b(s) && s.ib37 < (real)1e-8 && (s.v65 * kL_ - p_->vtm) * ivt_ > (real)9.2 &&
                s.vo < (real)1e-3 && s.vo > (real)-1e-3)
                s.idle = true;
            s_ = s;
            ib37_ = ib37;
            noise_ = nz;
            bp_ = bp;
        }

        real env1() const { return s_.v65; }
        real env2() const { return s_.v61; }
        real burst() const { return s_.v62; }

    private:
        struct State
        {
            int pulse;
            uint32_t count;
            bool idle, dSink;
            real v62, v65, pd, vo, v56, v55, v57, v59, v61, ib37;
        } s_;

        bool b(const State &s) const { return s.v62 < p_->thB; }

        // a template: the tables' branch is gone at compile time, prepare<true>() compiles as it did without it
        template <bool tables = true>
        void prepare()
        {
            const CPParams &p = *p_;
            trigSamples_ = (int)(p.trigMs * (real)0.001 * fs_ + (real)0.5);
            k62_ = std::exp(-dt_ / (p.r229 * p.c62));
            k220_ = std::exp(-dt_ / (p.r220 * p.c65));
            k223_ = std::exp(-dt_ / (p.r223 * p.c65));
            k224_ = std::exp(-dt_ / ((p.r224 + p.r238) * p.c65));
            kPd_ = std::exp(-dt_ / p.tauPd);
            kL_ = p.r238 / (p.r224 + p.r238);
            ivt_ = (real)1 / p.vt;
            if (tables)
            {
                logistic_.fill((real)-12, (real)20, [](real e) { return (real)1 / ((real)1 + std::exp(-e)); });
                tanh_.fill((real)-3, (real)3, [](real u) { return u * ((real)27 + u * u) / ((real)27 + (real)9 * u * u); });
            }
            ihvt_ = (real)1 / ((real)2 * p.vt);
            k56_ = (real)1 - std::exp(-dt_ / ((p.r202 + p.r201) * p.c56));
            kIn_ = p.r201 / (p.r202 + p.r201);
            k58_ = std::exp(-dt_ / (p.r237 * p.c58));
            c58dt_ = p.c58 / dt_;
            ir204_ = (real)1 / p.r204;
            ir205_ = (real)1 / (p.r205 + p.r203);
            dtc55_ = dt_ / p.c55;
            dtc57_ = dt_ / p.c57;
            k203_ = p.r203 / (p.r205 + p.r203);
            rn37_ = p.r203 * p.r205 / (p.r203 + p.r205);
            kGa59_ = (real)1 - std::exp(-dt_ / (p.rGa * p.c59));
            ir233_ = (real)1 / p.r233;
            dtc59_ = dt_ / p.c59;
            dtc61_ = dt_ / p.c61;
            isEff_ = p.qIs * ((real)1 / p.qBf + (real)1 / p.qBr);
            if (tables)
                q37_.fill((real)-0.25, (real)0.25, p.qBf, p.qBr, p.vt, [](real v) { return v; });
            const real a = (real)0.5 * (real)15 * (real)1; // the noise bus's +-7.5 V
            noiseAmp_ = p.noiseClk > (real)0 ? a * std::sqrt(fs_ / p.noiseClk) * (real)1.7320508 : (real)0;
            // IC26b: the multiple-feedback band pass, its centre gain -R207 / (2 R208)
            const real r12 = p.r208 * p.r209 / (p.r208 + p.r209);
            const real f0 = (real)1 / ((real)6.2831853 * p.c42 * std::sqrt(r12 * p.r207));
            const real q = (real)0.5 * std::sqrt(p.r207 / r12);
            const real g = p.r207 / ((real)2 * p.r208);
            const real w = std::tan((real)3.14159265 * f0 / fs_);
            const real nn = (real)1 / ((real)1 + w / q + w * w);
            bp_.b0 = -g * (w / q) * nn;
            bp_.b1 = (real)0;
            bp_.b2 = g * (w / q) * nn;
            bp_.a1 = (real)2 * (w * w - (real)1) * nn;
            bp_.a2 = ((real)1 - w / q + w * w) * nn;
            if (accb_ > (real)0)
                iTail_ = p.otaK * (accb_ - (real)0.2) / p.r235;
        }

        const CPParams *p_ = nullptr;
        real fs_ = 48000, dt_ = (real)1 / 48000;
        int trigSamples_ = 96;
        real accb_ = 0, iTail_ = 0;
        real k62_, k220_, k223_, k224_, kPd_, kL_, ivt_, ihvt_, k56_, kIn_, k58_, c58dt_;
        real ir204_, ir205_, dtc55_, dtc57_, k203_, rn37_, kGa59_, ir233_, dtc59_, dtc61_, isEff_, noiseAmp_;
        VcaTable<32> q37_;
        BaseCurrent ib37_;
        Noise noise_;
        Biquad bp_;
        Table<128> logistic_; // 1 / (1 + exp(-e)) over -12 ... 20, held beyond
        Table<64> tanh_;      // the (3,2) rational tanh over -3 ... 3, held beyond
    };
}
