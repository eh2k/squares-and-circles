// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va909 -- the TR-909 hi-hat and cymbals: their ROMs as the circuit plays them. (c) 2026 E. Heidt.
//
// The voicing board of the Service Notes (JUN. 15, 1984, p. 11 and the "digital voice generators" of p. 5); MODEL.md
// names each part and where each constant comes from.
//
//   hi-hat    TRIG (5 V, 2 ms) resets the address counters (IC70 4520, IC71 4040) on its edge (C159 100p, R491 100k)
//             and holds C135 at its start through Q74 for its 2 ms. The clock (IC72c/d with R493, R492, C160, some
//             60 kHz, halved by IC73a) counts the ROM IC69 through: the open hat from 0x0000 to 0x5FFF, the closed
//             one (CLOSED/OPEN on the address lines through D196 ... D199) from 0x6000 to 0x7FFF; IC72a stops the
//             clock at the end. The data, latched (IC68 4174), make a resistor DAC (RA9, RM0621) that drives
//             C157 into R482 (22 ohm): Q85's collector, its emitter in IC67b's summing node (R483 47k), its base fed
//             by Q84 (a PNP pair in IC67a's loop: an exponential converter, the ACCENT line on one base through
//             R481 4.7k / R489 150) with the current C135 drives through R453 220k into IC67a. C135 discharges
//             through R453 and the DECAY pots: OH DECAY (R452 100k + VR23 1M A) always, CH DECAY (R451 10k + VR21
//             100k B, through Q73) while the hat is closed. One voice: a closed hat cuts the open one.
//   cymbals   the crash (IC61, IC58a, ROM IC62, IC63, RA10, Q71) and the ride (IC55, IC58b, IC54, IC53, RA12, Q69)
//             alike: their clocks (IC59 / IC57 4011UB, R443 / R441 6.8k, VR25 / VR27 10k B TUNE, C131 / C130 470p)
//             halved; the ROMs' top six address bits through RA11 / RA13 into IC52b / IC52a and Q70 / Q68 feed the
//             VCA's base: "the ROM addresses as the envelope data", the envelope the samples were compressed by.
//
// What the recordings (Rob Roy, fit/README.md) show and the model takes from them: the clocks (the ROMs' staircase
// correlated with them: 0.9), C135's decay times, the cymbals' envelope over the address (the same for every TUNE).
#pragma once
#include "va909.h"
#ifdef VA909_BLEP_HEADER
#include VA909_BLEP_HEADER
#else
#include "va909_blep.h"
#endif

namespace va909
{
    // A ROM played as the 909 plays it: a counter clocked at fclk addresses the ROM, a latch holds the data (6 bits:
    // the byte's top six, as the dumps hold them) for a resistor DAC: a staircase, 0 at code 32 (the ROMs' rest:
    // their first and last bytes; the DAC's coupling capacitor charged to it), -1..+1 for 31 codes. What follows the
    // DAC without memory (the VCA) maps each step's value when it is latched (map(v)): the circuit's VCA sees the
    // staircase, its output is one. Rendered at the output's rate band limited: each step the integral of a windowed
    // sinc cut at 20 kHz (va909_blep.h: its residual against the step, kBlepD samples either side, the step's time
    // to 1/256 sample: the 6-bit ROM's noise is above that jitter's), the output kBlepD samples late. The staircase's
    // images above 24 kHz (fclk - f, 2 fclk + f, ...: the cymbals' clock reaches 56 kHz) would fold back otherwise
    // (polyBLEP: the recordings 0.3 ... 0.6 dB further off, fit/README.md); what the ROM holds above 24 kHz itself
    // folds back, as little of it as it holds.
    // A block: step(i, ...) for each sample i of it, then finish(): the output and the later samples' share carried.
    struct RomStair
    {
        static constexpr int kDelay = kBlepD, kTaps = 2 * kBlepD, kMax = 32;
        const uint8_t *rom = nullptr;
        int pos = 0, end = 0, drain = 0;
        real phase = 0, cur = 0, lev = 0;
        float res[kMax + kTaps] = {}, dl[kMax + kTaps] = {}; // the residuals, the steps (from their slot on)
        bool running = false;
        void start(const uint8_t *r, int from, int to)
        {
            rom = r;
            pos = from;
            end = to;
            phase = (real)0;
            running = r != nullptr && from < to;
            drain = 2 * kTaps + kMax;
        }
        // stopped and the last step's response out: the voice's level from then on is its own (hold())
        bool settled() const { return !running && drain <= 0; }
        void hold(real v) { lev = cur = v; }
        real value() const { return (real)((int)(rom[pos] >> 2) - 32) * (real)(1.0 / 31.0); }
        // the steps of the block's sample i, ratio = fclk / fs, inv its inverse
        template <class Map>
        void step(int i, real ratio, real inv, const Map &map)
        {
            if (!running)
                return;
            phase += ratio;
            while (phase >= (real)1)
            {
                phase -= (real)1;
                const real v = map(value());
                if (++pos >= end)
                    running = false;
                const real d = v - cur;
                cur = v;
                // the step came t = phase / ratio samples before sample i: the slots i ... i + 2D - 1 (the output
                // D late) get its residual at j - D + t, the slot i + D on the step
                const float *s = kBlep[(int)(phase * inv * (real)kBlepP + (real)0.5)];
                float *r = res + i;
                for (int j = 0; j < kTaps; j++)
                    r[j] += d * s[j];
                dl[i + kBlepD] += d;
                if (!running)
                    break;
            }
        }
        void finish(real *out, int n)
        {
            real l = lev;
            for (int i = 0; i < n; i++)
            {
                l += dl[i];
                out[i] = l + res[i];
            }
            lev = l;
            for (int j = 0; j < kTaps; j++)
            {
                res[j] = res[n + j];
                dl[j] = dl[n + j];
            }
            for (int j = kTaps; j < n + kTaps; j++)
                res[j] = dl[j] = 0.f;
            if (!running)
                drain -= n;
        }
    };

    // The VCA of the ROM voices (Q85, Q71, Q69): a saturated transistor from the DAC's 22 ohm node (its collector) to
    // an op-amp's summing node (its emitter), its base current ib from a current source (an exponential converter's
    // output): a voltage controlled resistor that takes its share of the DAC's current from the 22 ohm, "a voltage
    // controlled potentiometer" (the Service Notes). Ebers-Moll with the base current given: ie = ib F(vce / vt),
    // F(w) = (a - u) / (b + c u), u = exp(-w), a = 1 + 1/bf, b = 1/bf, c = 1/br. The node: v0 the DAC's current through
    // the 22 ohm alone, v = v0 - 22 ohm (ie - ib), in vt units w + sig (F(w) - 1) = w0 with sig = 22 ohm ib / vt.
    struct RomVca
    {
        static constexpr int kN = 64;
        real a = 0, b = 0, c = 0, bac = 0, f0 = 0, d0 = 0, lo = 0, scale = 0;
        real fd[kN][2] = {}; // F and F' over w (linear between)
        // bf, br; w (vce / vt) the table covers: [-wmax, wmax]
        void init(real bf, real br, real wmax)
        {
            a = (real)1 + (real)1 / bf;
            b = (real)1 / bf;
            c = (real)1 / br;
            bac = b + a * c;
            f0 = b / (b + c);           // F(0)
            d0 = bac / ((b + c) * (b + c)); // F'(0)
            lo = -wmax;
            scale = (real)(kN - 1) / ((real)2 * wmax);
            for (int k = 0; k < kN; k++)
            {
                const real w = lo + (real)k / scale, u = std::exp(-w), id = (real)1 / (b + c * u);
                fd[k][0] = (a - u) * id;
                fd[k][1] = u * bac * id * id;
            }
        }
        // the rest (w0 0) to first order: the emitter's current over ib's scale, sig (f0 + sig d0) / (1 + sig d0)
        real rest(real sig) const { return sig * (f0 + sig * d0) / ((real)1 + sig * d0); }
        // 1 / (1 + sig F'(0)): divert()'s start, once a sample
        real inv(real sig) const { return (real)1 / ((real)1 + sig * d0); }
        // w0 - w: the share the transistor takes (times vt / 22 ohm: the emitter's current besides ib). F is convex for
        // w < ln(bf / br) (some 3.9): from the root of its tangent at 0 (above the root) Newton falls onto it from
        // above; one step leaves < 2.1e-3 for |w0| <= 1 (the fitted DACs' peaks: 0.6 ... 0.9), F and F' from the table
        // (fit/hhnative.cpp CHECK=1); inv = inv(sig)
        real divert(real w0, real sig, real inv) const
        {
            real w = (w0 + sig * ((real)1 - f0)) * inv;
            real x = (w - lo) * scale;
            x = x < (real)0 ? (real)0 : x > (real)(kN - 1) - (real)0.001 ? (real)(kN - 1) - (real)0.001 : x;
            const int k = (int)x;
            const real t = x - (real)k;
            const real F = fd[k][0] + t * (fd[k + 1][0] - fd[k][0]);
            const real dF = fd[k][1] + t * (fd[k + 1][1] - fd[k][1]);
            w -= (w + sig * (F - (real)1) - w0) / ((real)1 + sig * dF);
            return w0 - w;
        }
    };

    // the op-amp's output at its swing (an accent far over the normal one gets there)
    inline void clip(real *out, int n, real vsat)
    {
        for (int i = 0; i < n; i++)
            out[i] = out[i] > vsat ? vsat : out[i] < -vsat ? -vsat : out[i];
    }

#define VA909_HH_PARAMS(X)                                                                                             \
    X(fclk, 31570.0, "Hz, the ROM's clock: IC72c/d (R493 33k, R492 12k, C160 470p) halved; the recordings' (+-10 Hz)")  \
    X(vt, 0.02585, "V, the thermal voltage")                                                                           \
    X(vNode, 0.011, "V at R482 for the DAC's full swing (code 32 +- 31): its current through 22 ohm alone")            \
    X(r482, 22.0, "ohm, R482: the DAC's load, Q85's collector")                                                        \
    X(qBf, 250.0, "Q85's forward gain")                                                                                \
    X(qBr, 5.0, "Q85's reverse gain")                                                                                  \
    X(i0, 20e-6, "A, Q85's base current at the hit with the normal accent (C135's start through R453, Q84's ratio)")    \
    X(accK, 0.0290, "V/V, Q84's base per volt of ACCENT: R481 4.7k into R489 150 || R488 2.2k")                       \
    X(sigK, 0.0620, "V/V, the same base per volt at R482: R488 2.2k into R489 150 || R481 4.7k")                       \
    X(c135, 1e-6, "F, C135: the DECAY capacitor")                                                                      \
    X(r453, 220e3, "ohm, R453: C135 into IC67a's summing node (the reference current; C135's discharge)")              \
    X(c156, 1e-9, "F, C156: IC67a's integrator (with R453 the envelope's smoothing)")                                  \
    X(r451, 10e3, "ohm, R451 in series to CH DECAY")                                                                   \
    X(vr21, 100e3, "ohm, VR21 CH DECAY (100k B)")                                                                      \
    X(ch3, 0.3, "CH DECAY's share at 3 of 10 (a B taper)")                                                            \
    X(ch7, 0.7, "CH DECAY's share at 7 of 10")                                                                        \
    X(r452, 100e3, "ohm, R452 in series to OH DECAY")                                                                  \
    X(vr23, 1e6, "ohm, VR23 OH DECAY (1M A)")                                                                          \
    X(oh3, 0.04, "OH DECAY's share at 3 of 10 (an A taper)")                                                          \
    X(oh7, 0.25, "OH DECAY's share at 7 of 10")                                                                       \
    X(cPast, 0.01, "how far past 0 C135's discharge aims, over its start (ends after ln((1 + cPast) / cPast) taus)")   \
    X(tTrig, 0.002, "s, TRIG: C135 held at its start")                                                                 \
    X(r483, 47e3, "ohm, R483: IC67b's feedback")                                                                       \
    X(vsat, 13.5, "V, IC67b's output swing (15 V rails)")

    struct HHParams
    {
#define X(name, value, comment) real name;
        VA909_HH_PARAMS(X)
#undef X
    };

    static const HHParams hhCircuit = {
#define X(name, value, comment) (real)value,
        VA909_HH_PARAMS(X)
#undef X
    };

    static const char *const hhParamNames[] = {
#define X(name, value, comment) #name,
        VA909_HH_PARAMS(X)
#undef X
    };
    static constexpr int hhNumParams = sizeof(hhParamNames) / sizeof(hhParamNames[0]);

    inline real *hhParamByIndex(HHParams &p, int i) { return (real *)&p + i; }

    class HiHat
    {
    public:
        void init(real sampleRate, const HHParams *p, const uint8_t *rom, int romLen, real accNormal)
        {
            fs_ = sampleRate;
            rom_ = rom;
            romLen_ = romLen;
            accNormal_ = accNormal;
            stair_ = RomStair();
            c_ = ref_ = sig_ = (real)0;
            hold_ = 0;
            ch_ = oh_ = (real)-1;
            setParams(p);
        }

        void setParams(const HHParams *p)
        {
            p_ = p;
            vca_.init(p->qBf, p->qBr, p->vNode / p->vt * (real)1.1 + (real)0.4);
            kRef_ = (real)1 - std::exp((real)-1 / (p->r453 * p->c156 * fs_));
            const real ch = ch_, oh = oh_;
            ch_ = oh_ = (real)-1;
            setKnobs(ch < (real)0 ? (real)0.5 : ch, oh < (real)0 ? (real)0.5 : oh);
        }

        // CH DECAY, OH DECAY 0..1
        void setKnobs(real chDecay, real ohDecay)
        {
            if (chDecay == ch_ && ohDecay == oh_)
                return;
            ch_ = chDecay;
            oh_ = ohDecay;
            const HHParams &p = *p_;
            const real gOpen = (real)1 / p.r453 + (real)1 / (p.r452 + p.vr23 * potTaper(oh_, p.oh3, p.oh7));
            const real gClosed = gOpen + (real)1 / (p.r451 + p.vr21 * potTaper(ch_, p.ch3, p.ch7));
            kOpen_ = std::exp(-gOpen / (p.c135 * fs_));
            kClosed_ = std::exp(-gClosed / (p.c135 * fs_));
            kEnv_ = closed_ ? kClosed_ : kOpen_;
        }

        bool hasRom() const { return rom_ != nullptr && romLen_ >= 0x8000; }

        // TRIG: the closed hat or the open one, the ACCENT line in volts
        void trigger(bool closed, real acc)
        {
            if (!hasRom())
                return;
            const HHParams &p = *p_;
            closed_ = closed;
            kEnv_ = closed ? kClosed_ : kOpen_;
            stair_.start(rom_, closed ? 0x6000 : 0x0000, closed ? 0x8000 : 0x6000);
            // Q84: the reference current times exp(the base's volts / vt), in sig units (22 ohm ib / vt)
            ib0_ = p.i0 * p.r482 / p.vt * expFast(p.accK * (acc - accNormal_) / p.vt);
            c_ = (real)1;
            hold_ = (int)(p.tTrig * fs_);
        }

        bool idle() const { return !stair_.running && hold_ <= 0 && ref_ < (real)1e-5; }

        void process(real *__restrict out, int n)
        {
            for (; n > RomStair::kMax; n -= RomStair::kMax, out += RomStair::kMax)
                block(out, RomStair::kMax);
            block(out, n);
        }

        real env() const { return ref_; }

    private:
        void block(real *__restrict out, int n)
        {
            const HHParams &p = *p_;
            const real w0Scale = p.vNode / p.vt;
            const real outScale = -p.r483 * p.vt / p.r482;
            const real sigK = p.sigK, end = -p.cPast, ratio = p.fclk / fs_, rinv = fs_ / p.fclk;
            real c = c_, ref = ref_, sig = sig_;
            const real kEnv = kEnv_, kRef = kRef_;
            // the VCA on each step of the DAC: the emitter's current (ib's part and the share it takes of the DAC's),
            // R488 the node's voltage on Q84's base too
            real inv = vca_.inv(sig);
            // R488 (the node's voltage on Q84's base: a few % of the base current) as a factor on the output
            auto vca = [&](real x) {
                const real w0 = x * w0Scale;
                return outScale * (sig + vca_.divert(w0, sig, inv)) * ((real)1 + sigK * w0);
            };
            const bool rest = stair_.settled();
            for (int i = 0; i < n; i++)
            {
                if (hold_ > 0)
                    hold_--;
                else
                    c = end + (c - end) * kEnv;
                // IC67a: the current through R453 (none once C135 crosses 0: Q84 cannot take it back), smoothed
                const real cur = c > (real)0 ? c : (real)0;
                ref += (cur - ref) * kRef;
                sig = ib0_ * ref;
                if (rest)
                    out[i] = outScale * vca_.rest(sig); // the ROM at rest (code 32), the hat decaying
                else
                {
                    if ((i & 3) == 0)
                        inv = vca_.inv(sig); // sig moves slowly: the start's reciprocal every 4 samples
                    stair_.step(i, ratio, rinv, vca);
                }
            }
            if (rest)
                stair_.hold(out[n - 1]);
            else
                stair_.finish(out, n);
            clip(out, n, p.vsat);
            c_ = c;
            ref_ = ref;
            sig_ = sig;
        }

        const HHParams *p_ = nullptr;
        const uint8_t *rom_ = nullptr;
        int romLen_ = 0, hold_ = 0;
        real fs_ = 48000, accNormal_ = 2.5f, ch_ = -1, oh_ = -1;
        real c_ = 0, ref_ = 0, sig_ = 0, ib0_ = 0;
        real kEnv_ = 0, kOpen_ = 0, kClosed_ = 0, kRef_ = 0;
        bool closed_ = false;
        RomStair stair_;
        RomVca vca_;
    };

#define VA909_CY_PARAMS(X)                                                                                             \
    X(r443, 6.8e3, "ohm, R443 (R441 the ride's): the clock's timing resistance besides TUNE")                          \
    X(vr25, 10e3, "ohm, VR25 (VR27) TUNE, 10k B")                                                                     \
    X(c131, 470e-12, "F, C131 (C130): the clock's capacitor")                                                          \
    X(kOsc, 1.95, "the 4011UB oscillator's period over its R C (the recordings' clocks at TUNE 0 and 10)")             \
    X(tOsc, 2.7e-6, "s, the period's fixed part (the gates, R442): the recordings' clocks")                            \
    X(tune3, 0.3, "TUNE's share at 3 of 10")                                                                          \
    X(tune7, 0.7, "TUNE's share at 7 of 10")                                                                          \
    X(vt, 0.02585, "V, the thermal voltage")                                                                           \
    X(vNode, 0.011, "V at R431 (R438) for the DAC's full swing: its current through 22 ohm alone")                    \
    X(r431, 22.0, "ohm, R431 (R438)")                                                                                  \
    X(qBf, 250.0, "Q71's (Q69's) forward gain")                                                                        \
    X(qBr, 5.0, "Q71's (Q69's) reverse gain")                                                                          \
    X(i0, 20e-6, "A, Q71's base current at address 0 with the normal accent")                                         \
    X(aEnd, 64.0, "the address (its top six bits) where IC52b's reference current would reach 0")                      \
    X(aPow, 1.0, "the reference current's curve over the address: (1 - a / aEnd)^aPow")                               \
    X(accK, 0.0447, "V/V, Q70's (Q68's) base per volt of ACCENT: R426 4.7k into R428 220")                            \
    X(rRef, 270e3, "ohm, R432 (R439 470k): the address DAC into IC52b")                                                \
    X(cRef, 1e-9, "F, C126 (C125): IC52b's integrator")                                                                \
    X(r425, 47e3, "ohm, R425 (R424): IC51b's feedback")                                                                \
    X(vsat, 13.5, "V, IC51b's (IC51a's) output swing (15 V rails)")

    struct CYParams
    {
#define X(name, value, comment) real name;
        VA909_CY_PARAMS(X)
#undef X
    };

    static const CYParams cyCrashCircuit = {
#define X(name, value, comment) (real)value,
        VA909_CY_PARAMS(X)
#undef X
    };

    static const char *const cyParamNames[] = {
#define X(name, value, comment) #name,
        VA909_CY_PARAMS(X)
#undef X
    };
    static constexpr int cyNumParams = sizeof(cyParamNames) / sizeof(cyParamNames[0]);

    inline real *cyParamByIndex(CYParams &p, int i) { return (real *)&p + i; }

    // the ride: as the crash, R439 470k for R432 (the rest of its parts the same)
    inline CYParams cyRideCircuit()
    {
        CYParams p = cyCrashCircuit;
        p.rRef = (real)470e3;
        return p;
    }

    class Cymbal
    {
    public:
        void init(real sampleRate, const CYParams *p, const uint8_t *rom, int romLen, real accNormal)
        {
            fs_ = sampleRate;
            rom_ = rom;
            romLen_ = romLen;
            accNormal_ = accNormal;
            stair_ = RomStair();
            ref_ = (real)0;
            tune_ = (real)-1;
            setParams(p);
        }

        void setParams(const CYParams *p)
        {
            p_ = p;
            vca_.init(p->qBf, p->qBr, p->vNode / p->vt * (real)1.1 + (real)0.4);
            kRef_ = (real)1 - std::exp((real)-1 / (p->rRef * p->cRef * fs_));
            for (int a = 0; a < 64; a++)
            {
                const real s = (real)1 - (real)a / p->aEnd;
                env_[a] = envFit_[a] = s > (real)0 ? std::pow(s, p->aPow) : (real)0;
            }
            decay_ = (real)1;
            const real t = tune_;
            tune_ = (real)-1;
            setTune(t < (real)0 ? (real)0.5 : t);
        }

        // a decay (not the 909's, a mod): the envelope over the address ends at d of where it ends as fitted (0 < d <=
        // 1; 1: as fitted), its curve squeezed: the fitted table read at a / d, linear between its points
        void setDecay(real d)
        {
            d = d > (real)1 ? (real)1 : d < (real)0.05 ? (real)0.05 : d;
            if (d == decay_)
                return;
            decay_ = d;
            for (int a = 0; a < 64; a++)
            {
                const real x = (real)a / d;
                const int i = (int)x;
                env_[a] = i >= 63 ? (i == 63 && x == (real)63 ? envFit_[63] : (real)0)
                                  : envFit_[i] + (x - (real)i) * (envFit_[i + 1] - envFit_[i]);
            }
        }

        // TUNE 0..1
        void setTune(real tune)
        {
            if (tune == tune_)
                return;
            tune_ = tune;
            const CYParams &p = *p_;
            const real r = p.r443 + p.vr25 * ((real)1 - potTaper(tune, p.tune3, p.tune7));
            const real fclk = (real)0.5 / (p.kOsc * r * p.c131 + p.tOsc); // the oscillator halved
            ratio_ = fclk / fs_;
            inv_ = fs_ / fclk;
        }

        real clock() const { return ratio_ * fs_; }

        bool hasRom() const { return rom_ != nullptr && romLen_ >= 0x8000; }

        void trigger(real acc)
        {
            if (!hasRom())
                return;
            const CYParams &p = *p_;
            stair_.start(rom_, 0, 0x8000);
            ib0_ = p.i0 * p.r431 / p.vt * expFast(p.accK * (acc - accNormal_) / p.vt);
        }

        bool idle() const { return !stair_.running && ref_ < (real)1e-5; }

        void process(real *__restrict out, int n)
        {
            for (; n > RomStair::kMax; n -= RomStair::kMax, out += RomStair::kMax)
                block(out, RomStair::kMax);
            block(out, n);
        }

        real env() const { return ref_; }

    private:
        void block(real *__restrict out, int n)
        {
            const CYParams &p = *p_;
            const real w0Scale = p.vNode / p.vt;
            const real outScale = -p.r425 * p.vt / p.r431;
            const real kRef = kRef_;
            real ref = ref_, sig = 0;
            real inv = (real)1;
            auto vca = [&](real x) { return outScale * (sig + vca_.divert(x * w0Scale, sig, inv)); };
            const bool rest = stair_.settled();
            for (int i = 0; i < n; i++)
            {
                // the top six address bits; IC59d's stop at the end gates the VCA off
                const real e = stair_.running ? env_[(stair_.pos >> 9) & 63] : (real)0;
                ref += (e - ref) * kRef;
                sig = ib0_ * ref;
                if (rest)
                    out[i] = outScale * vca_.rest(sig);
                else
                {
                    if ((i & 3) == 0)
                        inv = vca_.inv(sig);
                    stair_.step(i, ratio_, inv_, vca);
                }
            }
            if (rest)
                stair_.hold(out[n - 1]);
            else
                stair_.finish(out, n);
            clip(out, n, p.vsat);
            ref_ = ref;
        }

        const CYParams *p_ = nullptr;
        const uint8_t *rom_ = nullptr;
        int romLen_ = 0;
        real fs_ = 48000, ratio_ = 0.7, inv_ = 1.4, tune_ = -1, accNormal_ = 2.5f, ib0_ = 0, ref_ = 0, kRef_ = 0;
        real env_[64] = {}, envFit_[64] = {}, decay_ = 1;
        RomStair stair_;
        RomVca vca_;
    };
}
