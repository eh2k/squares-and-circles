// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the TR-909 bass drum modelled from its circuit. (c) 2026 E. Heidt.
//
// The voicing board of the Service Notes (JUN. 15, 1984, p. 11, PCB 2291084903); MODEL.md names each part. Every
// state is a capacitor's voltage, every path the schematic's, so accent, rolls and flams follow from the circuit:
//
//   trigger   TRIG (5 V, 2 ms) turns on Q1, Q1 turns on Q2: the trigger pulse TP (+15 V) for 2 ms
//   ENV3      TP charges C9 through D7, R21; R57 + TUNE (VR2) and R26 discharge it: the pitch envelope
//   VCO       IC12a sums ENV3 (R26) and +15 V (R27, the lowest pitch); IC12b passes it with +-1, Q10 switched by
//             the Schmitt trigger IC13b; the integrator IC13a (R59, C14): a triangle between IC13b's thresholds.
//             While TRIG is high Q7 shorts C14 (Q11, R51) and forces IC13b high: the triangle starts at 0, rising
//   shaper    the triangle through R52 into D10/D11: rounded towards a sine
//   VCA       Q12 in saturation, its collector at R43 (47 ohm, a few mV of the shaped wave), its emitter in IC11a's
//             summing node, its base fed from ENV1 through R54: the current follows ENV1 above Q12's Vbe
//   ENV1      the accent (IC11b, x3.13) through Q4, R35, D9 into C8 while Q3 holds Q4 on (C7, charged by TP,
//             discharged through R18 into Q3's base); D8 into R36/R22 bends the charge; DECAY (VR5) + R58 and R54
//             discharge it
//   click     Q8 (off during TP) lets C5 fall through R14 and steps it back at TP's end; C2, R7, R15 and the
//             Sallen-Key low pass of Q9 (R16, C3, C4) make it a ringing click, C11 and R44 put it into the mix node
//   noise     the noise bus (0/15 V, a clocked shift register) through R10/R9, C15, R46 / C13 (low pass), R45
//   ATTACK    the mix node (R56) through VR3 into R55 (47 ohm) and the VCA Q6, its base fed from ENV2 (C12,
//             charged by the accent through Q5 and R41 while TRIG is high, discharged through R42)
//   output    IC11a, LEVEL (VR4) its feedback: out = -VR4 (ie12 + ie6)
//
// The accent is the ACCENT line's voltage (the CPU's 6 bit DAC, 0..5 V), held from one trigger to the next. The
// voice runs at the sample rate given (48 kHz), every state integrated per sample; the triangle's corners are band
// limited (polyBLAMP), the shaper and Q12's gain are one table over the triangle's voltage.
#pragma once
#include "va909.h"

namespace va909
{
    // the constants: name, default (the part's value or the circuit's), comment; MODEL.md has the sources
#define VA909_BD_PARAMS(X)                                                                                             \
    /* supplies, op amps (M5218L), diodes (1SS133), transistors (2SC2603-F, 2SA1115-F) */                              \
    X(vcc, 15.0, "V, the positive supply")                                                                             \
    X(vsat, 13.5, "V, the op amps' output swing (IC13b's levels)")                                                     \
    X(vd, 0.6, "V, a diode's drop at the charge currents")                                                             \
    X(vt, 0.02585, "V, the thermal voltage (27 C)")                                                                    \
    /* the trigger */                                                                                                  \
    X(trigMs, 2.0, "ms, the TRIG pulse (Service Notes: 2 ms)")                                                         \
    X(vtpHi, 14.9, "V, TP while TRIG is high (Q2 saturated)")                                                          \
    X(vtpLo, 0.31, "V, TP at rest (Q8's base current through R5 into R19)")                                            \
    /* ENV3, the pitch envelope */                                                                                     \
    X(r21, 2.2e3, "ohm, R21: C9's charge")                                                                             \
    X(c9, 0.33e-6, "F, C9 (0.22 uF before SN 381500)")                                                                 \
    X(r57, 22e3, "ohm, R57 in series with TUNE")                                                                       \
    X(vr2, 100e3, "ohm, VR2 TUNE (100k A)")                                                                            \
    X(tune3, 0.030, "TUNE's resistance at 3 of 10, of VR2 (an A taper: two straight tracks, 0..3..7..10 linear)")        \
    X(tune7, 0.244, "TUNE's resistance at 7 of 10, of VR2")                                                          \
    X(r26, 330e3, "ohm, R26: ENV3 into IC12a")                                                                         \
    X(r27, 1.5e6, "ohm, R27: +15 V into IC12a (the lowest pitch)")                                                     \
    X(r28, 150e3, "ohm, R28: IC12a's feedback")                                                                        \
    /* the VCO */                                                                                                      \
    X(r59, 47e3, "ohm, R59: the integrator's input")                                                                   \
    X(c14, 0.033e-6, "F, C14: the integrator's capacitor")                                                             \
    X(r49, 100e3, "ohm, R49: IC13b's feedback")                                                                        \
    X(r50, 47e3, "ohm, R50: IC13b's (+) input to ground (thresholds +-vsat R50 / (R49 + R50))")                        \
    X(r51, 2.2e3, "ohm, R51: the reset's resistance (Q11)")                                                            \
    /* the shaper and the VCA Q12 */                                                                                   \
    X(r52, 10e3, "ohm, R52: the triangle into D10/D11")                                                               \
    X(dIs, 2.5e-9, "A, D10/D11's saturation current")                                                                  \
    X(dN, 1.75, "D10/D11's emission coefficient")                                                                      \
    X(r53, 2.2e3, "ohm, R53")                                                                                          \
    X(r43, 47.0, "ohm, R43: Q12's collector to ground")                                                                \
    X(qBf, 250.0, "the VCAs' (Q12, Q6) forward current gain")                                                          \
    X(qBr, 5.0, "their reverse current gain (sets the gain in saturation and the negative half's limit)")              \
    X(qIs, 1e-14, "A, their saturation current")                                                                       \
    X(r54, 470e3, "ohm, R54: ENV1 into Q12's base")                                                                    \
    /* ENV1 */                                                                                                         \
    X(accGain, 3.128, "IC11b's gain, 1 + R37 / R38 (10k / 4.7k)")                                                      \
    X(r35, 1.8e3, "ohm, R35: C8's charge")                                                                             \
    X(c8, 0.33e-6, "F, C8")                                                                                            \
    X(r58, 47e3, "ohm, R58 in series with DECAY")                                                                      \
    X(vr5, 1e6, "ohm, VR5 DECAY (1M A)")                                                                               \
    X(decay3, 0.030, "DECAY's resistance at 3 of 10, of VR5")                                                        \
    X(decay7, 0.244, "DECAY's resistance at 7 of 10, of VR5")                                                        \
    X(r36, 47e3, "ohm, R36: +15 V to D8's divider")                                                                    \
    X(r22, 10e3, "ohm, R22: D8's divider to ground")                                                                   \
    X(vce4, 0.1, "V, Q4 saturated")                                                                                    \
    X(r20, 6.8e3, "ohm, R20: C7's charge")                                                                             \
    X(c7, 0.033e-6, "F, C7: the hold")                                                                                 \
    X(r18, 470e3, "ohm, R18: C7 into Q3's base")                                                                       \
    X(r17, 470e3, "ohm, R17: Q3's base to ground")                                                                     \
    X(vbe3, 0.55, "V, Q3's base at the currents of the hold's end")                                                    \
    X(r32, 47e3, "ohm, R32: Q4's base into Q3")                                                                        \
    X(r33, 1e6, "ohm, R33: Q4's base to its emitter")                                                                  \
    X(bf3, 250.0, "Q3's current gain")                                                                                 \
    X(bf4, 200.0, "Q4's current gain")                                                                                 \
    /* ENV2 */                                                                                                         \
    X(r41, 22e3, "ohm, R41: C12's charge")                                                                             \
    X(c12, 0.033e-6, "F, C12")                                                                                         \
    X(r42, 470e3, "ohm, R42: ENV2 into Q6's base")                                                                     \
    X(r495, 0.0, "ohm, R495: Q6's base to ground (150k on units up to SN 371499; 0: none)")                            \
    X(accMin, 1.3, "V, IC11b's output below which Q5 stays off (D5 and two Vbe)")                                      \
    /* the click */                                                                                                    \
    X(vq8, 14.9, "V, Q8's collector while it is on")                                                                   \
    X(c5, 6.8e-9, "F, C5")                                                                                             \
    X(r14, 22e3, "ohm, R14")                                                                                           \
    X(c2, 3.3e-9, "F, C2")                                                                                             \
    X(r7, 100e3, "ohm, R7")                                                                                            \
    X(r15, 100e3, "ohm, R15")                                                                                          \
    X(r16, 39e3, "ohm, R16")                                                                                           \
    X(c4, 100e-12, "F, C4")                                                                                            \
    X(c3, 4.7e-9, "F, C3")                                                                                             \
    X(c11, 0.47e-6, "F, C11")                                                                                          \
    X(r44, 22e3, "ohm, R44: the click into the mix node")                                                              \
    /* the noise */                                                                                                    \
    X(noiseClk, 450e3, "Hz, the noise generator's clock (IC31a/b, R184 10k, C47 100p); 0: no noise")                                \
    X(r10, 12e3, "ohm, R10")                                                                                           \
    X(r9, 4.7e3, "ohm, R9")                                                                                            \
    X(c15, 0.47e-6, "F, C15")                                                                                          \
    X(r46, 4.7e3, "ohm, R46")                                                                                          \
    X(c13, 0.1e-6, "F, C13")                                                                                           \
    X(r45, 4.7e3, "ohm, R45: the noise into the mix node")                                                             \
    /* the mix node, ATTACK, the output */                                                                             \
    X(r56, 330.0, "ohm, R56: the mix node to ground")                                                                  \
    X(vr3, 500.0, "ohm, VR3 ATTACK (500 B), in series to R55")                                                         \
    X(r55, 47.0, "ohm, R55: Q6's collector to ground")                                                                 \
    X(vr4, 100e3, "ohm, VR4 LEVEL (100k B) at its top")

    struct BDParams
    {
#define X(name, value, comment) real name;
        VA909_BD_PARAMS(X)
#undef X
    };

    static const BDParams bdCircuit = {
#define X(name, value, comment) (real)value,
        VA909_BD_PARAMS(X)
#undef X
    };

    static const char *const bdParamNames[] = {
#define X(name, value, comment) #name,
        VA909_BD_PARAMS(X)
#undef X
    };
    static constexpr int bdNumParams = sizeof(bdParamNames) / sizeof(bdParamNames[0]);

    inline real *bdParamByIndex(BDParams &p, int i) { return (real *)&p + i; }

    class BassDrum
    {
    public:
        static constexpr int kTable = 128;

        void init(real sampleRate, const BDParams *p)
        {
            fs_ = sampleRate;
            dt_ = (real)1 / sampleRate;
            kLevel_ = (real)1 - std::exp(-dt_ / (real)0.001);
            p_ = p;
            memset(&s_, 0, sizeof(s_));
            s_.dir = 1;
            ib12_.ib = ib6_.ib = (real)1e-12;
            setKnobs((real)0.5, (real)0.5, (real)0.5);
            prepare();
        }

        // the panel's knobs, 0..1
        void setKnobs(real tune, real attack, real decay)
        {
            if (tune != tune_ || attack != attack_ || decay != decay_)
            {
                tune_ = tune;
                attack_ = attack;
                decay_ = decay;
                dirty_ = true;
            }
        }

        // tables false: the scalar coefficients only, for parts the tables do not depend on (the shaper's and the VCAs' parts, the click's network: r7 r14 r15 r16 c2 ... c5, r43 r52 r53, dIs dN, qBf qBr vt, vsat r49 r50)
        void setParams(const BDParams *p, bool tables = true)
        {
            p_ = p;
            if (tables)
                prepare<true>();
            else
                prepare<false>();
        }

        // a drive (not the 909's, a mod as the 909's Drive mods: the wave from the shaper's sine towards a square): the
        // triangle into D10/D11 times g (a gain before R52), 1 as fitted. Not in process() (the voice computes as
        // before without it): driveStep() fills the shaper's table again towards it, `points` of its 64 pairs a call
        void setDrive(real g) { driveTarget_ = g < (real)1 ? (real)1 : g; }

        // the shaper's parts of the params changed (setParams(p, false) with other dIs / dN, other diodes for D10/D11:
        // a mod): driveStep() fills its table again
        void shaperChanged() { shaperDirty_ = true; }

        // the body's level kept at that of the diodes is, n for D10/D11 (a mod with other diodes there: they change
        // the wave, not how loud it is; 0: none, the circuit's). Q12's current is its base current times its gain F
        // (SatVca) at the shaped triangle, and the triangle spends as long at each of its voltages: the body's level
        // is the AC part of F over the table's points within the triangle's range. That of these diodes at a drive
        // first (driveStep(), half the points: the shaper is odd, F at -u from the same point; some 4 ms, once a
        // drive), the table waits for it; then while the table fills, in every call, the table's as it is (old and new
        // points), Q12's current (not the click: ATTACK, Q6) times the ratio, so the level follows the table point by
        // point (within 1 ms). 1 exactly while every point is of these diodes (the circuit's samples)
        void setLevelRef(real is, real n) { refIs_ = is, refN_ = n; }

        // some points of the shaper's table towards the drive set (and the shaper's parts); false while it is at them
        bool driveStep(int points = 8)
        {
            if (drivePos_ < 0)
            {
                if (driveTarget_ == drive_ && !shaperDirty_)
                    return false;
                drivePass_ = driveTarget_;
                shaperDirty_ = false; // changed again while the pass runs: another one after it
                drivePos_ = 0;
            }
            const BDParams &p = *p_;
            const real k43 = p.r43 / (p.r53 + p.r43), g = drivePass_;
            const bool other = refIs_ > (real)0 && (refIs_ != p.dIs || refN_ != p.dN); // p's diodes are not those
            SatVca s;
            s.init(p.qBf, p.qBr, p.vt);
            if ((other || otherAny()) && refDrive_ != g) // the reference's level at this drive before the table moves
            {
                if (refFor_ != g)
                    refFor_ = g, refK_ = kTable / 2, refS_[0] = refS_[1] = refS_[2] = (real)0;
                for (int n = 0; n < points && refK_ < kTable; refK_ += 2)
                {
                    const real v = q12_.lo + (real)refK_ / q12_.scale;
                    if (v > vth_)
                        continue;
                    const real u = diodeShaper(v * g, p.r52, p.r53 + p.r43, refIs_, refN_ * p.vt) * k43;
                    const real f0 = s.gain(u), f1 = s.gain(-u);
                    refS_[0] += (real)2, refS_[1] += f0 + f1, refS_[2] += f0 * f0 + f1 * f1;
                    n++;
                }
                if (refK_ >= kTable)
                    refDrive_ = g, refRms_ = acRms(refS_);
                return true;
            }
            // the points in pairs k, 127 - k (the triangle's voltage v and -v) from the ends in: the shaper is odd
            // (diodeShaper() exactly so), one shaper for both, and the table stays odd while it fills. From the ends:
            // the peaks are new first, the level falls with them (from the middle a 1N4148 -> LED at Drive 0.5 peaked
            // x1.9 for some ms, from the ends x1.14)
            for (int n = 0; n < points && drivePos_ < kTable / 2; n++, drivePos_++)
            {
                const int k = kTable - 1 - drivePos_, km = kTable - 1 - k;
                const real v = q12_.lo + (real)k / q12_.scale; // the triangle's voltage of the point (> 0)
                const real u = shaper(v * g) * k43;
                vcaPoint(s, p.vt, (real)1 / p.qBf, (real)1 / p.qBr, u, (real)1e-4, q12_.y[k]); // as VcaTable::fill()
                vcaPoint(s, p.vt, (real)1 / p.qBf, (real)1 / p.qBr, -u, (real)1e-4, q12_.y[km]);
                for (int i : {k, km})
                {
                    const uint32_t bit = (uint32_t)1 << (i & 31);
                    if (other)
                        other_[i >> 5] |= bit;
                    else
                        other_[i >> 5] &= ~bit;
                }
            }
            if (otherAny())
            {
                // the table's points as the reference's: k >= 64 even, k < 64 odd (their mirrors), within the range
                real t[3] = {(real)0, (real)0, (real)0};
                for (int k = 0; k < kTable; k++)
                {
                    const real v = q12_.lo + (real)k / q12_.scale;
                    if (((k & 1) == 0) != (k >= kTable / 2) || v < -vth_ || v > vth_)
                        continue;
                    const real f = q12_.y[k].f;
                    t[0] += (real)1, t[1] += f, t[2] += f * f;
                }
                const real r = acRms(t);
                level_ = r > (real)0 ? refRms_ / r : (real)1;
            }
            else
                level_ = (real)1;
            if (drivePos_ >= kTable / 2)
                drive_ = drivePass_, drivePos_ = -1;
            return true;
        }

        // the body's factor of setLevelRef() (1: none, or every point of the table is of those diodes)
        real level() const { return level_; }

        // TRIG with the ACCENT line at acc volts (0..5): the line holds this voltage until the next trigger
        void trigger(real acc)
        {
            s_.pulse = trigSamples_;
            accb_ = acc * p_->accGain;
            if (accb_ > p_->vsat)
                accb_ = p_->vsat;
            if (accb_ < (real)0)
                accb_ = (real)0;
            ic3sat_ = (accb_ - p_->vd - (real)0.1) * ir32_;
            s_.idle = false;
        }

        bool idle() const { return s_.idle; }

        // n samples into out (written, not added)
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
            const BDParams &p = *p_;
            // the states as locals for the loop (nothing the loop stores can then alias them or the constants)
            State s = s_;
            BaseCurrent ib12 = ib12_, ib6 = ib6_;
            Noise nz = noise_;
            const real vd = p.vd, vbe3 = p.vbe3, bf3 = p.bf3, bf4 = p.bf4, vce4 = p.vce4, accMin = p.accMin;
            const real vq8 = p.vq8, vr4 = p.vr4, r54 = p.r54, r42 = p.r42, vt = p.vt;
            const real lvTo = level_, kLv = kLevel_;
            real lv = levelNow_;
            for (int i = 0; i < n; i++)
            {
                const uint32_t cnt = s.count++;
                if ((cnt & 15) == 0) // Q12's and Q6's Vbe every 16 samples, sooner where their capacitor moves
                {
                    ib12.update(s.v8, r54, vt, isEff_);
                    if (s.clickOn || s.pulse > 0)
                        ib6.update(s.v12, r42, vt, isEff_);
                }
                else if ((cnt & 1) == 0) // between, every 2 samples a step along the slopes (C12 / C8 move fast at the hit)
                {
                    ib12.track(s.v8, r54, vt);
                    if (s.clickOn || s.pulse > 0)
                        ib6.track(s.v12, r42, vt);
                }
                const bool tp = s.pulse > 0;
                if (tp)
                    s.pulse--;

                // ENV3 (C9): TP through D7, R21; R57 + TUNE and R26 discharge it
                if (tp && s.v9 < vch9_)
                    s.v9 = vch9_ + (s.v9 - vch9_) * k9c_;
                else
                    s.v9 *= k9d_;

                // C7, the hold: TP through D6, R20; R18 into Q3's base (clamped at vbe3) and R17. ENV1 (C8): Q4
                // (current limited by Q3 and Q4's gains) through R35, D9; D8 into R36/R22; only while C7 holds Q3 on,
                // else C8 just falls
                real ich8 = (real)0;
                if (tp || s.v7 > v7on_)
                {
                    if (tp)
                        s.v7 = vch7_ + (s.v7 - vch7_) * k7c_;
                    else
                        s.v7 = vbe3 + (s.v7 - vbe3) * k7a_;
                    real ib3 = s.v7 > v7on_ ? (s.v7 - vbe3) * ir18_ - vbe3 * ir17_ : (real)0;
                    real ic3 = ic3sat_; // Q3 saturated: Q4's base through R32
                    const real ic3max = bf3 * ib3;
                    if (ic3 > ic3max)
                        ic3 = ic3max;
                    const real ib4 = ic3 - vd * ir33_;
                    if (ib4 > (real)0)
                    {
                        real ich = (accb_ - vce4 - vd - s.v8) * ir35_;
                        const real lim = bf4 * ib4;
                        if (ich > lim)
                            ich = lim;
                        if (ich > (real)0)
                        {
                            if (s.v8 > x0_)
                                ich -= (s.v8 - x0_) * irx_;
                            if (ich > (real)0)
                                ich8 = ich;
                        }
                    }
                }
                else if (s.v7 > (real)0)
                    s.v7 *= k7b_;
                s.v8 += dtc8_ * (ich8 - s.v8 * gdec_ - s.ib12); // Q12's base current of the last sample
                if (s.v8 < (real)0)
                    s.v8 = (real)0;

                // ENV2 (C12): the accent through Q5, R41 while TRIG is high; R42 into Q6's base, R495
                {
                    real ich = (real)0;
                    if (tp && accb_ > accMin)
                    {
                        ich = (accb_ - (real)0.1 - s.v12) * ir41_;
                        if (ich < (real)0)
                            ich = (real)0;
                    }
                    s.v12 += dtc12_ * (ich - s.ib6 - s.v12 * ir495_);
                    if (s.v12 < (real)0)
                        s.v12 = (real)0;
                }

                // the VCO: IC12's |V1| sets the integrator's slope; Q11 resets it while TRIG is high
                const real v1 = g26_ * s.v9 + v1rest_;
                real tri = s.tri;
                real corrNext = (real)0;
                if (tp)
                {
                    const real vrst = v1 * rRst_;
                    tri = vrst + (tri - vrst) * kRst_;
                    s.dir = 1;
                }
                else
                {
                    // a corner at a fraction 'over' of the sample before this one: polyBLAMP, (1 - |t|)^3 / 6 of
                    // the slope's change on the sample before the corner and the one after (the output is the
                    // triangle one sample late, so both are still to come)
                    const real d = v1 * slope_; // per sample
                    tri += s.dir > 0 ? d : -d;
                    if (tri > vth_)
                    {
                        const real over = (tri - vth_) / d;
                        tri = vth_ + vth_ - tri;
                        s.dir = -1;
                        s.corr += -(real)2 * d * over * over * over * (real)(1.0 / 6.0);
                        const real u = (real)1 - over;
                        corrNext = -(real)2 * d * u * u * u * (real)(1.0 / 6.0);
                    }
                    else if (tri < -vth_)
                    {
                        const real over = (-vth_ - tri) / d;
                        tri = -vth_ - vth_ - tri;
                        s.dir = 1;
                        s.corr += (real)2 * d * over * over * over * (real)(1.0 / 6.0);
                        const real u = (real)1 - over;
                        corrNext = (real)2 * d * u * u * u * (real)(1.0 / 6.0);
                    }
                }
                const real triOut = s.tri + s.corr; // one sample late, corrected
                s.tri = tri;
                s.corr = corrNext;

                // the shaper (a table over the triangle's voltage: R43's open circuit voltage) and Q12
                VcaPoint q;
                q12_.at(triOut, q);
                real ie12 = vcaCurrent(q, rn12_, s.v8, ib12, s.ib12);

                // the click: Q8 off while TP is high; the 4-state network C5, C2, C3, C4 (exact per sample)
                real ie6 = (real)0;
                if (s.clickOn || tp)
                {
                    real ip = (real)0;
                    if (s.ringOn || tp)
                    {
                        const real *A = tp ? aOff_ : aOn_;
                        if (!tp && s.wasTp)
                            s.x[0] = vq8; // Q8 turns on: C5 is recharged at once
                        const real x0 = s.x[0], x1 = s.x[1], x2 = s.x[2], x3 = s.x[3];
                        if (tp) // with Q8 on, C5 is held (row 0 the identity)
                            s.x[0] = A[0] * x0 + A[1] * x1 + A[2] * x2 + A[3] * x3;
                        s.x[1] = A[4] * x0 + A[5] * x1 + A[6] * x2 + A[7] * x3;
                        s.x[2] = A[8] * x0 + A[9] * x1 + A[10] * x2 + A[11] * x3;
                        s.x[3] = A[12] * x0 + A[13] * x1 + A[14] * x2 + A[15] * x3;
                        // the network rings some 3 ms after each edge; then C11 alone
                        s.ringOn = tp || std::fabs(s.x[1] - s.x[0]) + std::fabs(s.x[2]) + std::fabs(s.x[3]) > (real)1e-4;
                    }
                    // Q9's emitter (C4's voltage less a Vbe, which C11 blocks), C11 and R44 into the mix node
                    ip = (s.x[3] - s.vc11) * ir44_;
                    s.vc11 += dtc11_ * ip;
                    // the noise: white at the density of the bus after R10/R9, C15 and R46 into C13, R45 out
                    const real vn = noiseAmp_ * nz.next();
                    const real is = (vn - s.vc15 - s.vc13) * irn_;
                    s.vc15 += dtc15_ * is;
                    s.vc13 += dtc13_ * (is - s.vc13 * ir45_);
                    const real in = s.vc13 * ir45_;
                    const real vq6 = (ip + in) * rMix_;
                    s.vq6 = vq6;
                    VcaPoint q6;
                    q6_.at(vq6, q6);
                    ie6 = vcaCurrent(q6, rn6_, s.v12, ib6, s.ib6);
                    // the click and the noise matter while Q6 conducts (C12 keeps some 0.4 V, Q6's base takes no
                    // more: down to ibClick_) and while the click rings
                    s.clickOn = tp || s.ringOn || s.ib6 > ibClick_;
                }
                else
                    s.ib6 = (real)0; // asleep: Q6's base current below the threshold
                s.wasTp = tp;

                lv += (lvTo - lv) * kLv; // setLevelRef(): 1 and staying there without other diodes
                out[i] = -vr4 * (ie12 * lv + ie6);
            }
            levelNow_ = lv;
            // asleep once Q12's base current is below ibIdle_, the click is over, the hold is over and no trigger
            // is pending; the capacitors keep their charge (the next hit starts from it, as in the circuit, where they
            // fall further only through the VCAs' bases, the pots and leaks)
            if (s.pulse == 0 && !s.clickOn && s.ib12 < ibIdle_ && s.v7 < v7on_)
            {
                s.idle = true;
                s.v9 = (real)0;
            }
            s_ = s;
            ib12_ = ib12;
            ib6_ = ib6;
            noise_ = nz;
        }

        // the states, for the fit's probes
        real env1() const { return s_.v8; }
        real env2() const { return s_.v12; }
        real env3() const { return s_.v9; }
        real hold() const { return s_.v7; }
        real triangle() const { return s_.tri; }
        real click() const { return s_.x[3]; }  // Q9's base, less its rest
        real attackNode() const { return s_.vq6; } // Q6's collector

    private:
        // the shaper's node: (v - u) / R52 = 2 Is sinh(u / (N vt)) + u / (R53 + R43)
        real shaper(real v) const
        {
            const BDParams &p = *p_;
            return diodeShaper(v, p.r52, p.r53 + p.r43, p.dIs, p.dN * p.vt);
        }

        // C = expm(M dt) of a 4x4 by scaling and squaring a Taylor series
        static void expm4(const real *M, real dt, real *C)
        {
            real norm = 0;
            for (int i = 0; i < 16; i++)
                norm = std::fmax(norm, std::fabs(M[i] * dt));
            int s = 0;
            while (norm * (real)4 > (real)0.5 && s < 30)
            {
                norm *= (real)0.5;
                s++;
            }
            const real h = dt / std::exp2((real)s);
            real T[16], P[16], Q[16];
            for (int i = 0; i < 16; i++)
            {
                T[i] = (i % 5 == 0) ? (real)1 : (real)0;
                P[i] = T[i];
            }
            for (int k = 1; k <= 12; k++)
            {
                for (int r = 0; r < 4; r++)
                    for (int c = 0; c < 4; c++)
                    {
                        real a = 0;
                        for (int j = 0; j < 4; j++)
                            a += P[r * 4 + j] * M[j * 4 + c] * h;
                        Q[r * 4 + c] = a / (real)k;
                    }
                for (int i = 0; i < 16; i++)
                {
                    P[i] = Q[i];
                    T[i] += P[i];
                }
            }
            for (int q = 0; q < s; q++)
            {
                for (int r = 0; r < 4; r++)
                    for (int c = 0; c < 4; c++)
                    {
                        real a = 0;
                        for (int j = 0; j < 4; j++)
                            a += T[r * 4 + j] * T[j * 4 + c];
                        Q[r * 4 + c] = a;
                    }
                for (int i = 0; i < 16; i++)
                    T[i] = Q[i];
            }
            for (int i = 0; i < 16; i++)
                C[i] = T[i];
        }

        // the click's network, states x = (v5, v2, v3, v4): C5 (Q8's collector), C2 (q8c - c2b), C3 (sa - q9e),
        // C4 (Q9's base); sa = v3 + v4 - vbe; Q9 a follower. With Q8 on, v5 is held (its row zero).
        void clickMatrix(bool q8on, real *M) const
        {
            const BDParams &p = *p_;
            // i2 = (v5 - v2 - sa) / R7, sa = v3 + v4 (- vbe, a constant offset: the matrix is of the deviations)
            const real g7 = (real)1 / p.r7, g15 = (real)1 / p.r15, g16 = (real)1 / p.r16, g14 = (real)1 / p.r14;
            for (int i = 0; i < 16; i++)
                M[i] = (real)0;
            // d v5/dt = (-v5 g14 - i2) / C5
            if (!q8on)
            {
                M[0] = (-g14 - g7) / p.c5;
                M[1] = g7 / p.c5;
                M[2] = g7 / p.c5;
                M[3] = g7 / p.c5;
            }
            // d v2/dt = i2 / C2
            M[4] = g7 / p.c2;
            M[5] = -g7 / p.c2;
            M[6] = -g7 / p.c2;
            M[7] = -g7 / p.c2;
            // d v3/dt = (i2 - sa g15 - (sa - v4) g16) / C3 = (i2 - (v3 + v4) g15 - v3 g16) / C3
            M[8] = g7 / p.c3;
            M[9] = -g7 / p.c3;
            M[10] = (-g7 - g15 - g16) / p.c3;
            M[11] = (-g7 - g15) / p.c3;
            // d v4/dt = (sa - v4) g16 / C4 = v3 g16 / C4
            M[14] = g16 / p.c4;
        }

        // a template: the tables' branch is gone at compile time, prepare<true>() compiles as it did without it
        template <bool tables = true>
        void prepare()
        {
            const BDParams &p = *p_;
            trigSamples_ = (int)(p.trigMs * (real)0.001 * fs_ + (real)0.5);
            // ENV3's charge target and coefficient with the discharge in parallel are set with the knobs
            vch7_ = p.vtpHi - p.vd;
            k7c_ = std::exp(-dt_ / (p.r20 * p.c7));
            k7a_ = std::exp(-dt_ / (p.r18 * p.c7));
            k7b_ = std::exp(-dt_ / ((p.r18 + p.r17) * p.c7));
            v7on_ = p.vbe3 * (p.r17 + p.r18) / p.r17;
            ir18_ = (real)1 / p.r18;
            ir17_ = (real)1 / p.r17;
            ir32_ = (real)1 / p.r32;
            ir33_ = (real)1 / p.r33;
            ir35_ = (real)1 / p.r35;
            x0_ = p.vcc * p.r22 / (p.r36 + p.r22);
            irx_ = (p.r36 + p.r22) / (p.r36 * p.r22);
            dtc8_ = dt_ / p.c8;
            ir41_ = (real)1 / p.r41;
            dtc12_ = dt_ / p.c12;
            ir495_ = p.r495 > (real)0 ? (real)1 / p.r495 : (real)0;
            isEff_ = p.qIs * ((real)1 / p.qBf + (real)1 / p.qBr);
            ir54_ = (real)1 / p.r54;
            ir42_ = (real)1 / p.r42;
            ibIdle_ = (real)1e-9;   // A: Q12's base current, some -80 dB of a hit's 10 uA: the voice sleeps
            ibClick_ = (real)1e-7;  // A: Q6's, -40 dB of a hit's: the noise's thump (some 25 dB under the click) stops
            if (tables)
                q6_.fill((real)-0.25, (real)0.25, p.qBf, p.qBr, p.vt, [](real x) { return x; });
            rn12_ = p.r43 * p.r53 / (p.r43 + p.r53);
            // the VCO
            g26_ = p.r28 / p.r26;
            v1rest_ = p.vcc * p.r28 / p.r27;
            slope_ = dt_ / (p.r59 * p.c14);
            vth_ = p.vsat * p.r50 / (p.r49 + p.r50);
            rRst_ = p.r51 / p.r59;
            kRst_ = std::exp(-dt_ / (p.r51 * p.c14));
            // the shaper and Q12 over the triangle's range: R43's open circuit voltage and the VCA's F, L there
            if (tables)
            {
                const real lo = -vth_ * (real)1.1, hi = vth_ * (real)1.1;
                const real k43 = p.r43 / (p.r53 + p.r43);
                q12_.fill(lo, hi, p.qBf, p.qBr, p.vt, [this, k43](real v) { return shaper(v) * k43; });
                drive_ = (real)1, drivePos_ = -1, shaperDirty_ = false; // the shaper of p: driveStep() fills it
                                                                        // again for a drive set
                // other diodes than setLevelRef()'s: a pass measures their level
                const bool other = refIs_ > (real)0 && (refIs_ != p.dIs || refN_ != p.dN);
                for (uint32_t &w : other_)
                    w = other ? ~(uint32_t)0 : (uint32_t)0;
                level_ = levelNow_ = (real)1, refDrive_ = refFor_ = (real)0;
                if (other)
                    shaperDirty_ = true;
            }
            // the click
            if (tables)
            {
                real M[16];
                clickMatrix(false, M);
                expm4(M, dt_, aOff_);
                clickMatrix(true, M);
                expm4(M, dt_, aOn_);
            }
            ir44_ = (real)1 / p.r44;
            dtc11_ = dt_ / p.c11;
            // the noise: the bus's density after R10/R9 as white noise at fs, uniform (variance 1/3)
            const real a = (real)0.5 * p.vcc * p.r9 / (p.r10 + p.r9);
            noiseAmp_ = p.noiseClk > (real)0 ? a * std::sqrt(fs_ / p.noiseClk) * (real)1.7320508 : (real)0;
            irn_ = (real)1 / (p.r10 * p.r9 / (p.r10 + p.r9) + p.r46);
            dtc15_ = dt_ / p.c15;
            dtc13_ = dt_ / p.c13;
            ir45_ = (real)1 / p.r45;
            if (s_.x[0] == (real)0) // the rest: Q8 on, C2 charged to its collector's voltage
                s_.x[0] = s_.x[1] = p.vq8;
            knobCoeffs();
        }

        void knobCoeffs()
        {
            const BDParams &p = *p_;
            dirty_ = false;
            const real rt = p.r57 + p.vr2 * potTaper(tune_, p.tune3, p.tune7);
            const real g9 = (real)1 / rt + (real)1 / p.r26;
            const real gc = (real)1 / p.r21;
            vch9_ = (p.vtpHi - p.vd) * gc / (gc + g9);
            k9c_ = std::exp(-dt_ * (gc + g9) / p.c9);
            k9d_ = std::exp(-dt_ * g9 / p.c9);
            gdec_ = (real)1 / (p.r58 + p.vr5 * potTaper(decay_, p.decay3, p.decay7));
            // ATTACK: VR3 in series to R55, clockwise less; the mix node's shunt R56 || (VR3 + R55)
            const real r3 = p.vr3 * ((real)1 - attack_);
            const real rpath = r3 + p.r55;
            rMix_ = p.r56 * rpath / (p.r56 + rpath) * p.r55 / rpath;
            rn6_ = p.r55 * (r3 + p.r56) / (p.r55 + r3 + p.r56); // Q6's node: R55 || (VR3 + R56)
        }

        struct State
        {
            int pulse;
            uint32_t count;
            bool idle, clickOn, ringOn, wasTp;
            int dir;
            real v9, v7, v8, v12, ib12, ib6;
            real tri, corr;
            real x[4], vc11, vc15, vc13, vq6;
        } s_;

        const BDParams *p_ = nullptr;
        real fs_ = 48000, dt_ = (real)1 / 48000;
        real tune_ = 0.5, attack_ = 0.5, decay_ = 0.5;
        bool dirty_ = true;
        real drive_ = 1, driveTarget_ = 1, drivePass_ = 1; // the shaper table's drive, the one set, the one being filled
        int drivePos_ = -1;                               // the next point of the table to fill (-1: none)
        bool shaperDirty_ = false;                        // the shaper's parts changed (shaperChanged())
        real refIs_ = 0, refN_ = 0;                       // setLevelRef()'s diodes
        real refDrive_ = 0, refFor_ = 0, refRms_ = 1;     // their level known at a drive, being measured at one
        real refS_[3] = {};                               // its points, F's sum and sum of squares
        int refK_ = 0;                                    // its next point
        uint32_t other_[kTable / 32] = {};                // the table's points of other diodes than those
        real level_ = 1, levelNow_ = 1, kLevel_ = 0;      // the body's factor, where it is, its step a sample
        bool otherAny() const
        {
            uint32_t a = 0;
            for (uint32_t w : other_)
                a |= w;
            return a != 0;
        }
        // the AC part's RMS of points (count, sum, sum of squares)
        static real acRms(const real *t)
        {
            const real v = t[0] > (real)0 ? t[2] / t[0] - (t[1] / t[0]) * (t[1] / t[0]) : (real)0;
            return v > (real)0 ? std::sqrt(v) : (real)0;
        }
        int trigSamples_ = 96;
        real accb_ = 0, ic3sat_ = 0;
        real vch9_, k9c_, k9d_;
        real vch7_, k7c_, k7a_, k7b_, v7on_, ir18_, ir17_, ir32_, ir33_, ir35_, x0_, irx_, dtc8_, gdec_;
        real ir41_, dtc12_, ir495_, isEff_, ir54_, ir42_, ibIdle_, ibClick_, rn12_, rn6_;
        real g26_, v1rest_, slope_, vth_, rRst_, kRst_;
        real aOff_[16], aOn_[16], ir44_, dtc11_;
        real noiseAmp_, irn_, dtc15_, dtc13_, ir45_, rMix_;
        VcaTable<64> q6_;
        VcaTable<kTable> q12_;
        BaseCurrent ib12_, ib6_;
        Noise noise_;
    };
}
