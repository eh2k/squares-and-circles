// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// va303 -- a TB-303 voice modelled from its circuit. (c) 2026 E. Heidt.
//
// The structure follows the schematic (Roland's service notes as Kevin Lightner redrew them) and Robin Whittle's
// measurements (the Devil Fish manual, "Investigating some unique aspects of the TB-303's sound"); every constant is a
// part's value or was fitted to recordings of a TB-303 (MODEL.md lists each with its source). No code of the TB-03,
// of ReBirth or of the ABL3 is in it: those were only measured.
//
//   pitch     the CV in octaves, through the slide's RC only while a slide is on (the 303 switches it in), and the
//             4 ms the 303's CPU takes to start a note
//   VCO       a saw (PolyBLEP); the square as the 303's squarer makes it: the saw through a coupling capacitor into
//             a comparator (so its width follows the pitch), its top sagging through the next capacitor, its rising
//             edge slower than its falling one
//   VCF       the four-stage diode ladder (Q12 ... Q22, C18 0.018 uF and three 0.033 uF): an input pair whose
//             differential voltage is the signal less the resonance feedback, the feedback through a coupling
//             capacitor (a highpass), every diode pair a conductance that falls with the voltage across it
//             (tanh(v)/v), solved without a delay in the loop (TPT/ZDF); the cutoff from the exponential converter's
//             summing node: the Cutoff pot, the Env Mod pot's share of the filter envelope, the Accent Sweep Circuit
//   MEG       a capacitor charged at the gate and discharged through the Decay pot's series resistance (200 ms to
//             2.5 s to a tenth; accents short the pot: 200 ms)
//   accent    the envelope through a diode and 47k into the second section of the Resonance pot with a 1 uF
//             capacitor at its other end, the wiper into the cutoff (the sweep; at high resonance it comes from the
//             capacitor, which keeps its charge from one accent to the next); the envelope also into the amp
//   VEG/VCA   the amp's envelope (1 uF through 1.5 M: a held note fades), the gate held 8 ms after the gate and
//             then a linear fall of 8 ms, a leak while closed, the OTA's saturation
//
// The core runs VA303_OS times oversampled (2x in the engine) and is decimated with a halfband; VA303_ITER > 0 makes
// the conductances converge by iteration (the reference model, 8x and double), 0 takes them from the sample before
// (semi-implicit, the engine). VA303_REAL is float (the engine) or double (the reference).
#pragma once
#include <cmath>
#include <cstdint>
#include <cstring>

#ifndef VA303_REAL
#define VA303_REAL float
#endif
#ifndef VA303_OS
#define VA303_OS 2
#endif
#ifndef VA303_ITER
#define VA303_ITER 0
#endif

namespace va303
{
    typedef VA303_REAL real;

    // the constants: name, default, comment (the parts in MODEL.md); a parameter file of the fit sets them by name
#define VA303_PARAMS(X)                                                                                                \
    /* the oscillator */                                                                                               \
    X(sawSign, -1.0, "the saw's polarity into the filter (the 303's falls)")                                           \
    X(hp1, 44.5, "Hz, the coupling into the filter (measured on a 303 by Open303)")                                    \
    X(sqWidth, 0.51, "the pulse's width at C2 (65.4 Hz)")                                                              \
    X(sqWidthK, 0.10, "its change with the pitch: width = sqWidth + sqWidthK (1 - 65.4 Hz / f), fitted to the TB-03's") \
    X(sqWidthOct, 0.0, "its change per octave, + sqWidthOct log2(f / 65.4 Hz): the units measured, about -0.05 (0: none)") \
    X(sqGain, 0.7, "the pulse's amplitude relative to the saw's (the recordings' square is as loud as their saw)")      \
    X(sqSag, 25.0, "Hz, the pulse's coupling: its top sags")                                                           \
    X(sqRise, 6000.0, "Hz, the pulse's rising edge (one-pole); it falls at once")                                      \
    /* the filter */                                                                                                   \
    X(cutLow, 6.32, "log2 Hz of the converter's part of the cutoff with Cutoff at 0, no envelope, no accent (the 303's held notes: 80 Hz)") \
    X(cutRange, 4.4, "octaves over the Cutoff pot (linear pot into the exponential converter; measured: 185 ... 1629 Hz over 25 ... 100 %)") \
    X(cutTop, 0.0, "Hz, the converter's soft top (0: none)")                                                           \
    X(cutPotTop, 0.0, "the Cutoff pot's right stop on its line: its last fifth flatter (0: straight)")                 \
    X(cutFloor, 185.0, "Hz, the ladder's bias apart from the converter: the cutoff with the Cutoff pot down (0: none)") \
    X(envFloor, 0.0, "the envelope's share of that bias per unit of the Env Mod pot, linear in Hz (0: none)")          \
    X(accFloor, 0.0, "the accent sweep's share of that bias per unit of the sweep, linear in Hz, as envFloor (0: none)")\
    X(cap1, 0.545, "the bottom capacitor relative to the others (C18 0.018 uF / 0.033 uF)")                            \
    X(topG, 1.0, "the top pair's conductance (a single diode an arm) relative to the ladder's pairs")                  \
    X(topLeak, 0.0, "kOhm an arm, the top node's load: R113/R114 100k behind C27/C25 (0: none)")                  \
    X(driveIn, 1.0, "the input pair: the saw's amplitude in units of its linear range")                                \
    X(driveLadder, 1.0, "the diode pairs: the stages' voltages in units of their linear range")                        \
    X(resoMax, 1.1, "the feedback at Reso 1 relative to the linear ladder's oscillation threshold")                    \
    X(resoCurve, 0.0, "k = kOsc resoMax (q + resoCurve q (q - 1)); 0: linear pot")                                    \
    X(resoS, 0.0, "the law's S: + resoS q (q - 1) (2 q - 1), k held at 0 (< 0: flat at both ends, steep between)")     \
    X(resoCut, 0.0, "octaves the cutoff moves per unit of Reso (the pot's load on the bias; 0: none)")                 \
    X(resoCutHz, 0.0, "Hz: resoCut grows with the cutoff, fc^2 / (fc^2 + resoCutHz^2) of it (0: the same everywhere)") \
    X(fbHp, 150.0, "Hz, the feedback's coupling capacitor (Open303's 150 Hz; the recordings' level stays with Reso)")   \
    X(fbHp2, 0.0, "Hz, the feedback's second coupling (C15 and C23 are both in its way; 0: none)")                    \
    X(hp2, 24.0, "Hz, the coupling out of the filter (Open303's)")                                                     \
    X(gain, 0.0, "dB, the voice's level at the amp's level 1")                                                         \
    X(resoGain, 1.0, "the output's gain with the resonance, 1 + resoGain k (1: the passband's loss 1/(1+k) made up)")  \
    /* the filter envelope */                                                                                          \
    X(megAttack, 3.0, "ms, the capacitor's charge at the gate")                                                        \
    X(megRs, 90.0, "kOhm in series with the Decay pot (R138 68k and the diodes' end at D26: some 75 with 1 uF)")       \
    X(megRpot, 1000.0, "kOhm, the Decay pot VR6 (1M A)")                                                               \
    X(megC, 1.0, "uF, the envelope's capacitor C62")                                                                   \
    X(decayTaper, 2.0, "the Decay pot's audio taper: (10^(k d) - 1) / (10^k - 1); 0 linear")                           \
    X(envDepth, 5.2, "octaves at Env Mod 1 for the envelope's peak (9.8 V through 220k into 2.2k)")                    \
    X(envTaper, 2.0, "the Env Mod pot's audio taper (VR5 50k A)")                                                      \
    X(envR61, 0.0, "R61 10k under VR5 relative to the pot: the envelope's share at Env Mod 0 (0.2; 0: none)")        \
    X(envRest, -0.3, "the envelope's resting level, of its peak: Env Mod moves the base cutoff")                       \
    /* the accent */                                                                                                   \
    X(accDiode, 0.06, "the diode's drop, of the envelope's peak (0.6 V of 9.8 V)")                                     \
    X(accR1, 0.94, "R46 47k relative to the Resonance pot's second section (50k)")                                     \
    X(accRm, 2.0, "R72 100k relative to the pot")                                                                      \
    X(accC, 50.0, "ms, the pot (50k) times C13 (1 uF)")                                                                \
    X(accDepth, 12.0, "octaves per unit of the wiper's voltage (the envelope's peak; 9.8 V through 100k into 2.2k)")   \
    X(accVca, 4.0, "the accent's share of the amp's level per unit of Accent times the envelope above the threshold")  \
    X(accVcaThr, 0.45, "the threshold (the diode D27) on Accent times the envelope, of the envelope's peak")           \
    X(accVcaKnee, 0.0, "D27's soft knee: the width of its turning on, of the envelope's peak (0: a switch)")           \
    X(accHp, 0.0, "Hz, the coupling of the accent into the amp (0: none)")                                             \
    X(accC36, 0.0, "ms, the accent into the amp through R119 47k into C36 0.033 uF, out by R120 (0: none)")       \
    X(accVcaTau, 0.0, "ms, the fall of the accent's drive into the amp, measured slower than the MEG (0: the MEG's)")   \
    /* the amp */                                                                                                      \
    X(vegAttack, 3.0, "ms, the amp's attack")                                                                          \
    X(vegTau, 1500.0, "ms, the VEG's fall while the gate is held (C42 1 uF, R123 1.5 M)")                              \
    X(vegTarget, -0.13, "what it falls towards: the VCA's current ends before the capacitor is full")                  \
    X(gateHold, 8.0, "ms the amp stays open after the gate")                                                           \
    X(gateFall, 8.0, "ms of the linear fall after the hold")                                                           \
    X(bleed, -62.0, "dB, the VCA's leak while it is closed (the recordings' floor after a note)")                       \
    X(vcaSat, 0.0, "the output's saturation level, of the voice's level (0: none)")                                    \
    X(satAsym, 0.0, "the negative side's limit relative to the positive one's (0: alike)")                         \
    /* the pitch */                                                                                                    \
    X(slideTau, 22.0, "ms, the slide's RC (100k into 0.22 uF)")

    struct Params
    {
#define X(name, value, comment) real name;
        VA303_PARAMS(X)
#undef X
    };

    static const Params defaultParams = {
#define X(name, value, comment) (real)value,
        VA303_PARAMS(X)
#undef X
    };

    static const char *const paramNames[] = {
#define X(name, value, comment) #name,
        VA303_PARAMS(X)
#undef X
    };
    static constexpr int numParams = sizeof(paramNames) / sizeof(paramNames[0]);

    inline real *paramByIndex(Params &p, int i) { return (real *)&p + i; }

    // tanh(x)/x as the Pade (3,2) approximant: exact to 2e-3 below |x| = 3, clamped above
    inline real tanhOverX(real x)
    {
        const real a = std::fabs(x);
        if (a >= (real)3)
            return (real)1 / a;
        const real x2 = x * x;
        return ((real)27 + x2) / ((real)27 + (real)9 * x2);
    }

    inline real polyblep(real t, real dt)
    {
        if (t < dt)
        {
            t /= dt;
            return t + t - t * t - (real)1;
        }
        if (t > (real)1 - dt)
        {
            t = (t - (real)1) / dt;
            return t * t + t + t + (real)1;
        }
        return (real)0;
    }

    // one-pole coefficient for a time constant in ms at a rate (0: at once)
    inline real coeffMs(real ms, real fs) { return ms > (real)0 ? (real)1 - std::exp((real)-1 / ((real)0.001 * ms * fs)) : (real)1; }
    // one-pole coefficient for a corner in Hz
    inline real coeffHz(real hz, real fs) { return hz > (real)0 ? (real)1 - std::exp((real)-6.283185307 * hz / fs) : (real)0; }
    // an audio taper pot: (10^(k x) - 1) / (10^k - 1), linear with k 0
    inline real taper(real x, real k) { return k > (real)0.001 ? (std::exp2((real)3.321928095 * k * x) - (real)1) / (std::exp2((real)3.321928095 * k) - (real)1) : x; }

    // The engine's approximations (VA303_FAST: the semi-implicit float build; the reference takes the library's):
    // 2^x with the integer part in the exponent and the fraction by a minimax polynomial of degree 5 (9e-8
    // relative), tan(x) on 0..0.94 as the Pade (5,4) approximant (1e-7 relative)
#ifndef VA303_FAST
#define VA303_FAST (VA303_ITER == 0)
#endif
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
    inline real tanFast(real x)
    {
        const real x2 = x * x;
        return x * ((real)945 - (real)105 * x2 + x2 * x2) / ((real)945 - (real)420 * x2 + (real)15 * x2 * x2);
    }
#if VA303_FAST
    inline real fastExp2(real x) { return exp2Fast(x); }
    inline real fastTan(real x) { return tanFast(x); }
#else
    inline real fastExp2(real x) { return std::exp2(x); }
    inline real fastTan(real x) { return std::tan(x); }
#endif

    class Voice
    {
      public:
        static constexpr int OS = VA303_OS;
        static constexpr int CTRL = VA303_FAST ? 4 : 1;

        // fs: the output rate; blockSize: the samples a block() call renders (the note delay is in blocks)
        void init(real fs, int blockSize, const Params *params)
        {
            p = params;
            pending = nullptr;
            this->fs = fs;
            fsCore = fs * OS;
            this->blockSize = blockSize;
            std::memset(&st, 0, sizeof(st));
            std::memset(fifo, 0, sizeof(fifo));
            fifoPos = 0;
            delayBlocks = 0;
            settleBlocks = settleLeft = 0;
            wave = 0;
            knob.c = knob.q = knob.m = knob.d = knob.a = (real)0.5;
            knobsChanged = true;
            calcConstants();
            initDecimator();
        }

        // another set of constants (a character): at once while nothing sounds, else with the next note (in the
        // middle of one the amp and the filter would jump); back to the one playing before the next note: none pending
        // (the knob turned away and back within a note played the other one for a note)
        void setParams(const Params *params)
        {
            if (params == p)
                pending = nullptr;
            if (params == p || params == pending)
                return;
            if (!st.gateOn && st.gateLevel <= 0 && st.holdLeft <= 0)
                applyParams(params);
            else
                pending = params;
        }
        const Params *params() const { return p; }

        void setWave(int w) { wave = w; } // 0 saw, 1 square, 2 the external input

        // the 4 ms the 303's CPU takes from the step to the note (Whittle; the engine: 4), a FIFO of blocks; not one of
        // the constants: the CPU's, not the circuit's, and the fit renders without it (the recordings are cut at the
        // note). Here, not in calcConstants(): a character's change would move the FIFO's read index under a note.
        void setNoteDelay(real ms)
        {
            const int d = (int)(ms * (real)0.001 * fs / blockSize + (real)0.5);
            delayBlocks = d < 0 ? 0 : d > MAX_DELAY ? MAX_DELAY : d;
        }

        // the time after a new note in which the CV jumps instead of sliding: a CV that reaches the voice through an
        // input's averaging (the firmware: 2 ms, the OCT41 4 ms) is only partly at the new note in the trigger's
        // block, and its rest, a CV change while the gate is held, would slide (22 ms: a fifth up 253 cents flat after
        // 24 ms, 57 after 56). Not the 303's: its CPU sets the CV before the gate. 0 (the default, the fit): none.
        void setCvSettle(real ms)
        {
            const int b = (int)(ms * (real)0.001 * fs / blockSize + (real)0.5);
            settleBlocks = b < 0 ? 0 : b;
        }

        // the Devil Fish's Cutoff pot (Robin Whittle): its range "increased so it goes higher and lower", the filter's
        // resonance at its right stop "approximately 5 kHz. This is an octave above the typical maximum for the
        // TB-303", the 303's range "between about 10.30 and 3 o'clock" (0.35 ... 0.8 of the turn; the manual, pp. 6,
        // 8). On: the Cutoff knob is that pot, the character's range between 0.35 and 0.8, beyond it the cutoff scaled:
        // above by DF_OCT octaves a turn, an octave over the last 0.2 (an octave above the character's own top: the
        // units' tops differ, the x0xb0x about 1 kHz, acidvoice.com's TB-303 3.3), below at the 303's slope (cutRange
        // over 0.45 of the turn: about 5 octaves at the left stop, where the notes fall silent, "the filter passes no
        // audio at the frequencies the VCO is producing"). Reso's pull is the one at the 303's stop: its law, fitted
        // within the 303's range, would eat the octave (the converter an octave higher, TB-303h resonates lower). The
        // clock positions are approximate (at the 303's slope 3 o'clock would leave about three octaves above it, not
        // one): the top's figure is held. Off: the 303's pot.
        void setDfCutoff(bool on)
        {
            if (on != dfCutoff)
            {
                dfCutoff = on;
                knobsChanged = true;
            }
        }

        // the Devil Fish's Filter FM pot: "the audio output of the VCA" (the Muffler in it), AC coupled, into the
        // filter's frequency: "edginess and complexity dependent on the signal level coming out of the VCA - so it is
        // stronger on accented notes. At higher settings, the results approach chaos" (the manual, p. 5). Linear in Hz
        // (the manual has its Filter Tracking and the FM jack so, the pot's law not), at the control's rate: the
        // output's mean over its period (CTRL core samples; per sample at the core's rate the code took 560 B more,
        // the core inlined twice), coupled at FM_HP, into the cutoff before its clamps (the bias not below 1 Hz, as
        // the transistor's current not below 0). The depth at the pot's right stop FM_HZ per unit of the voice's
        // output (a plain note at the knobs' middle peaks at 0.15), the pot's square below: chosen, not fitted (no
        // recording with a known setting; MODEL.md). 0 (the default): none.
        void setFilterFm(real pot)
        {
            pot = pot < 0 ? (real)0 : pot > 1 ? (real)1 : pot;
            fmHz = FM_HZ * pot * pot;
        }

        // the Devil Fish's Overdrive (Robin Whittle, 1993): the oscillator's level into the filter, which on the 303 is
        // fixed by R62 220k into the input pair's base; 1 the 303's, up to 66.6 (R62 3.3k), 0 none. The input pair and
        // the diode pairs saturate as they do at the 303's level, only harder: the filter "operates under duress", its
        // resonance quenched at the waveform's extremes (the Devil Fish manual). The external input (wave 2) as well.
        void setOverdrive(real g) { inGain = g; }

        // the Devil Fish's Muffler: a three-position switch on the VCA's output, "two types of muted clipping" that
        // soften "the loudest extremes of the sound" and introduce "a moderate level square wave clipping buzz, whilst
        // allowing the bass to pass largely unaffected"; it works on what is "louder than usual", the accents, the
        // Overdrive, a loud external input (the Devil Fish manual, p. 6). Its circuit is not published. Here: diodes
        // from the output to ground behind a capacitor, whose voltage follows the bass (a one-pole at MUF_HZ), so what
        // is above the bass is clipped, linear up to the diodes' knee, then soft (tanh) towards their limit; two diodes
        // in series (1) or one (2), at the voice's level, where the plain notes stay below the knee. 0: none.
        void setMuffler(int m) { muffler = m < 0 ? 0 : m > 2 ? 2 : m; }

        // the bass mod of the x0xb0x (C20 and C21 0.1 uF for 0.01, the couplings into the VCA): the amp's input coupling
        // (R121 220k, C21) at 7.2 Hz for 72, +3 dB at 72 Hz, +7.6 dB at 32 Hz, +20 dB at the lowest. In front of the
        // amp, so the output's limit (vcaSat) and the Muffler see the bass. The Devil Fish's bass is alike (the manual:
        // "In the TB-303, 32 Hz was down by 5 dB. Now it is down by 1 dB").
        void setBassMod(bool on) { bassMod = on; }

        void setKnobs(real cutoff, real reso, real envmod, real decay, real accent)
        {
            if (cutoff == knob.c && reso == knob.q && envmod == knob.m && decay == knob.d && accent == knob.a)
                return; // the engine calls it every block: calcControl() only on a change
            knob.c = cutoff;
            knob.q = reso;
            knob.m = envmod;
            knob.d = decay;
            knob.a = accent;
            knobsChanged = true;
        }

        // a block of the sequencer's signals (trig: a note starts; gate; accent at the trigger; note: MIDI, fractional),
        // delayed by the 303's note delay, rendered into out (and, with wave 2, from in). tie: a trigger while the gate
        // is still on slides (the 303's overlapping steps, legato MIDI notes); false: it starts a new note anyway (a
        // gate the caller holds on after a bare trigger pulse)
        void block(real note, bool trig, bool gate, bool accent, real *out, const real *in = nullptr, bool tie = true)
        {
            Input &w = fifo[fifoPos];
            w.note = note;
            w.trig = trig;
            w.gate = gate;
            w.accent = accent;
            w.tie = tie;
            const Input r = fifo[(fifoPos + MAX_DELAY + 1 - delayBlocks) % (MAX_DELAY + 1)];
            fifoPos = (fifoPos + 1) % (MAX_DELAY + 1);
            if (r.trig ? !r.tie : !r.gate) // the gate's end, or an untied trigger: the note before ends first
                noteOff();
            if (r.trig)
                noteOn(r.note, r.accent);
            else if (r.gate && settleLeft > 0) // the CV still settling after a new note (setCvSettle): it jumps
            {
                settleLeft--;
                st.pitchTarget = st.pitch = r.note;
            }
            else if (r.gate)
                setNote(r.note);
            render(out, blockSize, in);
        }

        // the events as the block() delivers them
        void noteOn(real note, bool accent)
        {
            if (st.gateOn) // a note while the gate is still on: a slide, as the 303 plays overlapping steps
            {
                st.pitchTarget = note;
                st.sliding = true;
                settleLeft = 0; // a slide's CV glides from its trigger on
                if (accent != st.accented) // the accent is the step's, a slid one's too (the x0xb0x: +8 dB on it)
                {
                    st.accented = accent;
                    knobsChanged = true; // the decay: accents short the pot
                }
                return;
            }
            if (pending) // a new character starts with a new note (not with a slide: the filter and amp would jump)
                applyParams(pending);
            st.pitchTarget = st.pitch = note;
            st.sliding = false;
            settleLeft = settleBlocks;
            st.gateOn = true;
            st.accented = accent;
            st.megAttacking = true;
            st.vegAttacking = true;
            st.gateFalling = false;
            st.holdLeft = 0;
            knobsChanged = true; // the decay for this note
        }

        void noteOff()
        {
            if (!st.gateOn)
                return;
            st.gateOn = false; // the slide's RC runs on (the 303's slide switch holds to the next step)
            settleLeft = 0;
            st.holdLeft = (int)(p->gateHold * (real)0.001 * fsCtrl);
            // the accent ends with the gate, as the x0xb0x's MIDI firmware drops it (its sweep, its share of the amp,
            // the short decay); otherwise it sounded on into the rest, 14 dB under the note (the 303's own sequencer
            // holds it to the step's end, which a voice without steps doesn't know)
            if (st.accented)
            {
                st.accented = false;
                knobsChanged = true;
            }
        }

        void setNote(real note) // the CV moves while the gate is held: a slide
        {
            if (note != st.pitchTarget)
            {
                st.pitchTarget = note;
                st.sliding = true;
            }
        }

        void render(real *out, int n, const real *in)
        {
            if (knobsChanged)
                calcControl();
            for (int i = 0; i < n; i++)
            {
                real acc = 0;
                const real xi = in ? in[i] : (real)0;
                for (int k = 0; k < OS; k++)
                {
                    // the external input linear between its samples (held, its images above fs / 2 would fold back
                    // through the ladder's nonlinearity)
                    const real x = in ? st.auxPrev + (xi - st.auxPrev) * ((real)(k + 1) / (real)OS) : (real)0;
                    const real y = core(x, in != nullptr);
                    acc = decimate(y, k, acc);
                }
                st.auxPrev = xi;
                out[i] = acc;
            }
        }

        // what the model derives once (the fit reads them)
        real kOsc() const { return kosc; }

      private:
        static constexpr int MAX_DELAY = 16;
        static constexpr real C36_R119 = 47, C36_R120 = 22; // kOhm, the accent's network into the amp
        struct Input
        {
            real note;
            bool trig, gate, accent, tie;
        };
        struct Knobs
        {
            real c, q, m, d, a;
        } knob;
        struct State
        {
            // the pitch
            real pitch, pitchTarget;
            bool sliding, gateOn, accented;
            real phase, dt, dtInc;
            // the control's rate: the cutoff's tan and the pitch's increment every CTRL core samples, ramped between
            int ctrl;
            real g, gInc;
            // the squarer
            real sqSagLp, sqSlew, sqW;
            // the envelopes and the amp's level
            real meg, veg, gateLevel, accLp, c36, accEnv, amp;
            bool megAttacking, vegAttacking, gateFalling;
            int holdLeft;
            // the accent sweep
            real sweepCap;
            // the filter
            real s[4], g0, gam[4], fbLp, fbLp2, hp1Lp, hp2Lp;
            // the bass mod's and the Muffler's capacitors
            real bassLp, mufLp;
            // the Filter FM: the output's sum over the control's period, its coupling's capacitor
            real fmAcc, fmLp;
            // the external input's sample before (render())
            real auxPrev;
            // the decimator
            real dec[OS == 2 ? 80 : 256];
            int decPos;
        } st;
        const Params *p, *pending;
        real fs, fsCore, fsCtrl;

        void applyParams(const Params *params)
        {
            p = params;
            pending = nullptr;
            calcConstants();
            knobsChanged = true;
        }
        int blockSize, delayBlocks, fifoPos, wave;
        int settleBlocks, settleLeft; // setCvSettle(): the blocks after a new note in which the CV jumps, those left
        real inGain = (real)1; // the Overdrive (setOverdrive)
        bool dfCutoff = false; // the Devil Fish's Cutoff range (setDfCutoff)
        real cutScale = (real)1; // its factor on the cutoff beyond the 303's range (calcControl())
        real fmHz = (real)0;   // the Filter FM's depth (setFilterFm)
        int muffler = 0;       // the Muffler (setMuffler)
        bool bassMod = false;  // the bass mod (setBassMod)
        // the bass mod: the coupling's corners; the Muffler: the capacitor's corner, the diodes' knee and limit per
        // position, at the voice's level, where a plain note at the knobs' middle peaks at 0.15 above the bass, at an
        // open cutoff 0.42, an accent at Accent 100 % 0.45 ... 0.6, the Overdrive at 8 times 0.38. 1 (two diodes in
        // series, soft): an accent's peaks 1 ... 2 dB lower; 2 (one, hard): 4 ... 5 dB, the mids lower, the clipping's
        // buzz above 2.4 kHz up to +13 dB at 8 times the Overdrive; the bass below 150 Hz within 1.1 dB. MUF_HZ from
        // the Devil Fish's own switch 0 -> 1 (MODEL.md: rw11): 40 ... 60 Hz fit it best, 100 Hz worse.
        static constexpr real BASS_HZ = 72, BASS_MOD_HZ = 7.2, MUF_HZ = 50;
        // the Devil Fish's Cutoff pot: the 303's stops on it, octaves a turn above (setDfCutoff); the Filter FM: Hz per
        // unit of the output at its right stop, the coupling's corner (setFilterFm)
        static constexpr real DF_LO = 0.35, DF_HI = 0.8, DF_OCT = 5, FM_HZ = 10000, FM_HP = 10;
        static constexpr real MUF_KNEE[3] = {0, (real)0.2, (real)0.15}, MUF_LIM[3] = {0, (real)0.45, (real)0.22};
        real aBass, aMuf, aFmHp;
        Input fifo[MAX_DELAY + 1];
        bool knobsChanged;
        // the constants of the rate
        real aMegAtt, aVegAtt, aVeg, aSlide, aHp1, aHp2, aSqSag, aSqRise, aAccHp, gFb, gFb2, fallStep, bleedLin, gainLin;
        real leak, aC36Off, aC36On, aAccVca, satP, satN, satInvP, satInvN;
        real kosc, capInv[4], cutTopInv, fbHa1, fbHa2;
        // the control's terms (per note and knobs)
        real cutBase, cutEnv, envTaperM, cutAcc, kRes, aMeg, tauMegMs, resoGainLin;
        real swOn, swIn, swS, swC, swCoeffOn, swCoeffOff; // the accent sweep's coefficients at this resonance

        void calcConstants()
        {
            fsCtrl = fsCore / CTRL;
            aMegAtt = coeffMs(p->megAttack, fsCtrl);
            aVegAtt = coeffMs(p->vegAttack, fsCtrl);
            aVeg = coeffMs(p->vegTau, fsCtrl);
            aSlide = coeffMs(p->slideTau, fsCtrl);
            aAccHp = coeffHz(p->accHp, fsCtrl);
            fallStep = p->gateFall > 0 ? (real)1 / (p->gateFall * (real)0.001 * fsCtrl) : (real)1;
            aHp1 = coeffHz(p->hp1, fsCore);
            aHp2 = coeffHz(p->hp2, fsCore);
            aSqSag = coeffHz(p->sqSag, fsCore);
            aSqRise = coeffHz(p->sqRise, fsCore);
            aBass = coeffHz(BASS_MOD_HZ, fsCore);
            aMuf = coeffHz(MUF_HZ, fsCore);
            aFmHp = coeffHz(FM_HP, fsCtrl);
            gFb = std::tan((real)3.14159265 * p->fbHp / fsCore);
            gFb2 = p->fbHp2 > 0 ? std::tan((real)3.14159265 * p->fbHp2 / fsCore) : (real)0;
            fbHa1 = (real)1 / ((real)1 + gFb);
            fbHa2 = (real)1 / ((real)1 + gFb2);
            cutTopInv = p->cutTop > 0 ? (real)1 / p->cutTop : (real)0;
            bleedLin = std::exp2(p->bleed / (real)6.0206);
            gainLin = std::exp2(p->gain / (real)6.0206);
            // the top node's load: each arm through C27/C25 (a short at the ring's frequencies) into R113/R114, across
            // C26 0.033 uF a differential 2R: it drains the node at 1/(2 R C26) a second, whatever the cutoff (the
            // ladder's conductances are the pairs', which scale with it). In the trapezoidal step: that rate times T/2.
            leak = p->topLeak > 0 ? (real)1 / ((real)2 * p->topLeak * (real)1e3 * (real)0.033e-6) / ((real)2 * fsCore) : (real)0;
            // C36: charged through R119 (tau = accC36), discharged through R120 (22k / 47k of it) and D27 while above
            // the threshold; exact steps of the linear equation either way
            aC36Off = coeffMs(p->accC36, fsCtrl);
            aC36On = coeffMs(p->accC36 / ((real)1 + C36_R119 / C36_R120), fsCtrl);
            aAccVca = coeffMs(p->accVcaTau, fsCtrl);
            // the output's saturation: the positive side at vcaSat, the negative at satAsym of it
            satP = p->vcaSat * gainLin;
            satN = satP * (p->satAsym > 0 ? p->satAsym : (real)1);
            satInvP = satP > 0 ? (real)1 / satP : (real)0;
            satInvN = satN > 0 ? (real)1 / satN : (real)0;
            capInv[0] = (real)1 / p->cap1;
            capInv[1] = capInv[2] = capInv[3] = (real)1;
            kosc = oscillationThreshold();
        }

        // the linear ladder's feedback at which it starts to oscillate: the loop's phase at 180 degrees, 1 over the
        // gain there. The input pair's current into the bottom node, the top pair a conductance topG to the bias:
        // H(s) = v4 / I = 1 / (c1 c2 c3 c4 det(sI - M)) in units of the cutoff, M the nodes' matrix (its poles are
        // Stinchcombe's: with the bottom capacitor half, s^4 + 8 s^3 + 20 s^2 + 16 s + 2).
        real oscillationThreshold() const
        {
            // the characteristic polynomial of M by Faddeev-LeVerrier (in real: the engine has no double)
            const real c1 = capInv[0], c2 = capInv[1], c3 = capInv[2], c4 = capInv[3], gt = p->topG;
            const real M[4][4] = {{-c1, c1, 0, 0},
                                  {c2, -2 * c2, c2, 0},
                                  {0, c3, -2 * c3, c3},
                                  {0, 0, c4, -(1 + gt) * c4}};
            real c[5] = {1, 0, 0, 0, 0}; // det(sI - M) = c0 s^4 + c1 s^3 + ... + c4
            real N[4][4] = {{0}};
            for (int k = 1; k <= 4; k++)
            {
                real T[4][4];
                for (int i = 0; i < 4; i++)
                    for (int j = 0; j < 4; j++)
                    {
                        real s = 0;
                        for (int l = 0; l < 4; l++)
                            s += M[i][l] * N[l][j];
                        T[i][j] = s + (i == j ? c[k - 1] : (real)0);
                    }
                real tr = 0;
                for (int i = 0; i < 4; i++)
                    for (int l = 0; l < 4; l++)
                        tr += M[i][l] * T[l][i];
                c[k] = -tr / k;
                std::memcpy(N, T, sizeof(T));
            }
            const real prod = (real)1 / (capInv[0] * capInv[1] * capInv[2] * capInv[3]);
            // the phase of det(jw I - M) rises from 0 towards 360 degrees (the poles are negative and real): find w
            // where it passes 180 (the real part negative, the imaginary part turned negative), by bisection on a log
            // grid; there the loop's phase is -180 and the gain 1/|H| = prod(c) |det|
            auto det = [&](real w, real &re, real &im) {
                // (jw)^4 = w^4, (jw)^3 = -j w^3, (jw)^2 = -w^2, jw
                re = c[0] * w * w * w * w - c[2] * w * w + c[4];
                im = -c[1] * w * w * w + c[3] * w;
            };
            real lo = (real)0.05, hi = (real)50;
            real re, im;
            const real step = std::exp2(std::log2(hi / lo) / (real)200);
            for (int i = 0; i < 200; i++)
            {
                const real w = lo * step;
                det(w, re, im);
                if (re < 0 && im < 0) // past 180 degrees
                {
                    hi = w;
                    break;
                }
                lo = w;
            }
            for (int i = 0; i < 40; i++)
            {
                const real w = std::sqrt(lo * hi);
                det(w, re, im);
                if (re < 0 && im < 0)
                    hi = w;
                else
                    lo = w;
            }
            det(std::sqrt(lo * hi), re, im);
            return prod * std::sqrt(re * re + im * im);
        }

        void calcControl()
        {
            knobsChanged = false;
            const Knobs &k = knob;
            const real q = k.q < 0 ? 0 : k.q > 1 ? 1 : k.q;
            // the Cutoff pot: a straight line in octaves, its last fifth flatter with cutPotTop (both units' cutoff
            // rises less from three quarters of the turn to the right stop than below: dinsync's 303 0.88, the
            // x0xb0x 0.865 of the line)
            real cp = k.c;
            if (dfCutoff) // the Devil Fish's pot (setDfCutoff): the 303's range between DF_LO and DF_HI, scaled beyond
            {
                const real u = (cp - DF_LO) * ((real)1 / (DF_HI - DF_LO)); // the 303's pot, below 0 and above 1 beyond
                cutScale = std::exp2(u < 0 ? u * p->cutRange : cp > DF_HI ? (cp - DF_HI) * DF_OCT : (real)0);
                cp = u < 0 ? (real)0 : u > 1 ? (real)1 : u;
            }
            else
                cutScale = (real)1;
            if (p->cutPotTop > 0 && cp > (real)0.8)
                cp = (real)0.8 + (cp - (real)0.8) * (real)5 * (p->cutPotTop - (real)0.8);
            cutBase = p->cutLow + p->cutRange * cp;
            {
                // Reso lowers the cutoff; at a low cutoff hardly (the x0xb0x's and dinsync's Reso at Cutoff 0), with
                // resoCutHz the more the higher the cutoff the knobs set
                real rc = p->resoCut * q;
                if (p->resoCutHz > 0)
                {
                    const real f0 = std::exp2(cutBase) + (p->cutFloor > 0 ? p->cutFloor : (real)0);
                    rc *= f0 * f0 / (f0 * f0 + p->resoCutHz * p->resoCutHz);
                }
                cutBase += rc;
            }
            envTaperM = taper(k.m, p->envTaper);
            if (p->envR61 > 0) // VR5's end goes to ground through R61: its wiper keeps R61 / (R61 + pot) at Env Mod 0
                envTaperM = (p->envR61 + envTaperM) / (p->envR61 + (real)1);
            cutEnv = p->envDepth * envTaperM;
            cutAcc = p->accDepth;
            real law = q + p->resoCurve * q * (q - (real)1);
            if (p->resoS != 0) // the pot's S (dinsync's TB-303 and the x0xb0x: little at 1/4 and 1/2, most of it at 3/4)
            {
                law += p->resoS * q * (q - (real)1) * ((real)2 * q - (real)1);
                law = law > 0 ? law : (real)0;
            }
            kRes = kosc * p->resoMax * law;
            resoGainLin = (real)1 + p->resoGain * kRes;
            // the decay for the note sounding: accents short the pot
            tauMegMs = p->megC * (p->megRs + (st.accented ? (real)0 : p->megRpot * taper(k.d, p->decayTaper)));
            aMeg = coeffMs(tauMegMs, fsCtrl);
            // the accent sweep's coefficients at this resonance: rw from the wiper to the capacitor, r1 from the diode
            // to the pot's end, rm from the wiper into the cutoff (all relative to the pot), the capacitor's time
            // constant accC. Node equations with the diode conducting (V_s at the diode's cathode):
            //   V_A: (V_s - V_A)/r1 = (V_A - V_W)/rq,   V_W: (V_A - V_W)/rq + (V_C - V_W)/rw = V_W/rm
            // and with it blocking (V_A = V_W): (V_C - V_W)/rw = V_W/rm. rq = q (the wiper to the diode's end).
            const real rq = q, rw = (real)1 - q, r1 = p->accR1, rm = p->accRm;
            // conducting: V_W = swS V_s + swC V_C; the capacitor charges towards swIn V_s ... derived below
            // Let gA = 1/(r1 + rq) (the diode's end to the wiper as one resistor), gw = 1/rw, gm = 1/rm:
            //   V_W (gA + gw + gm) = gA V_s + gw V_C
            // the capacitor: C dV_C/dt = (V_W - V_C) gw = gw (gA V_s + gw V_C)/(gA + gw + gm) - gw V_C
            //   = gw gA/(gA+gw+gm) V_s - gw (gA + gm)/(gA+gw+gm) V_C  -> towards swIn V_s with tau = accC (gA+gw+gm)/(gw (gA+gm))
            // the diode conducts while V_s > V_A, V_A = V_W + (V_s - V_W) r1/(r1 + rq) ... i.e. while V_s > V_W,
            // which with V_W = swS V_s + swC V_C means V_s (1 - swS) > swC V_C: V_s > swOn V_C, swOn = swC/(1 - swS).
            // blocking: V_W = V_C gw/(gw + gm) = swOff V_C, the capacitor drains with tau = accC (gw + gm)/(gw gm).
            const real gA = (real)1 / (r1 + rq), gw = rw > (real)1e-6 ? (real)1 / rw : (real)1e6, gm = (real)1 / rm;
            const real den = gA + gw + gm;
            swS = gA / den;
            swC = gw / den;
            swIn = gA / (gA + gm);
            swOn = swC / ((real)1 - swS);
            const real tauOnSamples = p->accC * (real)0.001 * fsCtrl * den / (gw * (gA + gm));
            const real tauOffSamples = p->accC * (real)0.001 * fsCtrl * (gw + gm) / (gw * gm);
            swCoeffOn = (real)1 / ((real)1 + tauOnSamples);
            swCoeffOff = (real)1 / ((real)1 + tauOffSamples);
            swOffWiper = gw / (gw + gm);
        }
        real swOffWiper;

        // The control, every CTRL core samples (the reference: every one): the pitch's slide, the filter envelope,
        // the accent sweep, the cutoff through the exponential converter into the filter's coefficient and the pitch
        // into the oscillator's increment (both ramped until the next call), and the amp: the VEG, the gate's hold
        // and fall, the accent's share, the leak. Its time constants are in control samples (fsCtrl).
        inline void control()
        {
            State &s = st;
            // the pitch: the slide's RC while sliding, else the target at once
            if (s.sliding)
                s.pitch += (s.pitchTarget - s.pitch) * aSlide;
            else
                s.pitch = s.pitchTarget;

            // the filter envelope: charging at the gate, then the RC's fall
            if (s.megAttacking)
            {
                s.meg += ((real)1 - s.meg) * aMegAtt;
                if (s.meg > (real)0.995)
                    s.megAttacking = false;
            }
            else
                s.meg -= s.meg * aMeg;

            // the accent sweep circuit, fed by the envelope on accented notes
            const real vs = (s.accented ? s.meg * knob.a : (real)0) - p->accDiode;
            real sweep;
            if (vs > swOn * s.sweepCap)
            {
                s.sweepCap += (swIn * vs - s.sweepCap) * swCoeffOn;
                sweep = swS * vs + swC * s.sweepCap;
            }
            else
            {
                s.sweepCap -= s.sweepCap * swCoeffOff;
                sweep = swOffWiper * s.sweepCap;
            }

            // the cutoff: the summing node in octaves through the exponential converter, plus the ladder's bias
            // apart from it (the floor, which the envelope scales linearly)
            const real envLevel = s.meg + p->envRest;
            const real oct = cutBase + cutEnv * envLevel + cutAcc * sweep;
            real fc = fastExp2(oct);
            if (p->cutFloor > 0)
            {
                real fl = p->cutFloor * ((real)1 + p->envFloor * envTaperM * envLevel);
                if (p->accFloor > 0) // the sweep into the same summing node lifts that bias too, as the envelope
                    fl += p->cutFloor * p->accFloor * sweep;
                const real flMin = p->cutFloor * (real)0.05;
                fc += fl > flMin ? fl : flMin;
            }
            fc *= cutScale; // the Devil Fish's Cutoff range beyond the 303's (1 within it and without)
            if (p->cutTop > 0)
            {
                const real u = fc * cutTopInv; // the Pade's tanh (to 2e-3 below |u| 3; fc / cutTop stays under 1.8)
                fc = p->cutTop * u * tanhOverX(u);
            }
            // the Filter FM (setFilterFm): the output's mean over the control's period, AC coupled, in Hz into the bias
            const real fmIn = s.fmAcc * (real)(1.0 / CTRL);
            s.fmAcc = 0;
            if (fmHz > 0)
            {
                s.fmLp += (fmIn - s.fmLp) * aFmHp;
                fc += fmHz * (fmIn - s.fmLp);
            }
            const real fmax = fsCore * (real)0.28;
            if (fc > fmax)
                fc = fmax;
            if (fc < (real)1)
                fc = (real)1;
            s.gInc = (fastTan((real)3.14159265 * fc / fsCore) - s.g) * (real)(1.0 / CTRL);
            const real f = (real)440 * fastExp2((s.pitch - (real)69) * (real)(1.0 / 12.0));
            s.dtInc = (f / fsCore - s.dt) * (real)(1.0 / CTRL);
            if (wave == 1)
            {
                // the pulse's width follows the pitch (the squarer is coupled by a capacitor; the law is fitted)
                real w = p->sqWidth + p->sqWidthK * ((real)1 - (real)65.406 / f);
                if (p->sqWidthOct != 0) // the measured law: the same an octave; log2(f / 65.4 Hz) is (pitch - 36) / 12
                    w += p->sqWidthOct * (s.pitch - (real)36) * (real)(1.0 / 12.0);
                s.sqW = w < (real)0.05 ? (real)0.05 : w > (real)0.95 ? (real)0.95 : w;
            }

            // the amp: the VEG, the gate's hold and fall, the accent's share, the leak
            if (s.vegAttacking)
            {
                s.veg += ((real)1 - s.veg) * aVegAtt;
                if (s.veg > (real)0.995)
                    s.vegAttacking = false;
            }
            else
                s.veg += (p->vegTarget - s.veg) * aVeg;
            if (s.gateOn)
                s.gateLevel += ((real)1 - s.gateLevel) * aVegAtt;
            else if (s.holdLeft > 0)
                s.holdLeft--;
            else if (s.gateLevel > 0)
            {
                s.gateLevel -= fallStep;
                if (s.gateLevel < 0)
                    s.gateLevel = 0;
            }
            // the envelope that drives the accent into the amp: the MEG, or with accVcaTau the MEG held at its peak and
            // falling with accVcaTau (the recordings' accent at an open cutoff falls with 106 ms where the MEG that
            // sweeps the filter falls with 60: MODEL.md)
            real accDrive = s.meg;
            if (p->accVcaTau > 0)
            {
                s.accEnv -= s.accEnv * aAccVca;
                if (s.accEnv < s.meg)
                    s.accEnv = s.meg;
                accDrive = s.accEnv;
            }
            real accAmp;
            if (p->accC36 > 0)
            {
                // the Accent pot's share of the envelope (on accents) into C36 through R119; out of it through R120 and
                // D27 into the VCA's control while above the threshold. Scaled so that a slow source gives the same
                // share as without C36: (V - thr) (1 + R120/R119) / (R120/R119) = source - thr
                const real src = s.accented ? accDrive * knob.a : (real)0;
                const real r = C36_R120 / C36_R119;
                if (p->accVcaKnee > 0)
                {
                    // D27 as a diode, not a switch: it turns on over the knee (the softplus of the voltage over its
                    // threshold; its slope, how far it conducts, blends C36's charge and discharge)
                    const real x = (s.c36 - p->accVcaThr) / p->accVcaKnee;
                    const real e = x < (real)20 ? std::exp(x < (real)-20 ? (real)-20 : x) : (real)0;
                    const real on = x < (real)20 ? e / ((real)1 + e) : (real)1;
                    const real vInf = (src + p->accVcaThr / r) / ((real)1 + (real)1 / r);
                    s.c36 += on * (vInf - s.c36) * aC36On + ((real)1 - on) * (src - s.c36) * aC36Off;
                    accAmp = (x < (real)20 ? p->accVcaKnee * std::log((real)1 + e) : s.c36 - p->accVcaThr) * ((real)1 + r) / r;
                }
                else
                {
                    if (s.c36 > p->accVcaThr)
                    {
                        const real vInf = (src + p->accVcaThr / r) / ((real)1 + (real)1 / r);
                        s.c36 += (vInf - s.c36) * aC36On;
                    }
                    else
                        s.c36 += (src - s.c36) * aC36Off;
                    accAmp = s.c36 > p->accVcaThr ? (s.c36 - p->accVcaThr) * ((real)1 + r) / r : (real)0;
                }
            }
            else
            {
                accAmp = s.accented ? accDrive * knob.a - p->accVcaThr : (real)0; // above the diode's threshold
                if (p->accVcaKnee > 0 && s.accented)
                {
                    const real x = accAmp / p->accVcaKnee;
                    accAmp = x < (real)20 ? p->accVcaKnee * std::log((real)1 + std::exp(x < (real)-20 ? (real)-20 : x)) : accAmp;
                }
                if (accAmp < 0)
                    accAmp = 0;
            }
            if (p->accHp > 0)
            {
                s.accLp += (accAmp - s.accLp) * aAccHp;
                accAmp -= s.accLp;
            }
            const real veg = s.veg > 0 ? s.veg : (real)0;
            s.amp = veg * s.gateLevel + p->accVca * accAmp + bleedLin;
        }

        inline real core(real xin, bool external)
        {
            State &s = st;
            if (--s.ctrl <= 0)
            {
                s.ctrl = CTRL;
                control();
            }
            s.g += s.gInc;
            s.dt += s.dtInc;
            const real g = s.g;

            // the oscillator
            real x;
            if (external)
                x = xin;
            else
            {
                s.phase += s.dt;
                if (s.phase >= (real)1)
                    s.phase -= (real)1;
                if (wave == 1)
                {
                    // the squarer's pulse: its width follows the pitch (control(): sqW), band-limited at both edges
                    // (PolyBLEP), then the top sags through the next coupling and the rising edge is slower than the
                    // falling one
                    const real w = s.sqW;
                    real ph2 = s.phase - w;
                    if (ph2 < 0)
                        ph2 += (real)1;
                    real pulse = s.phase < w ? (real)1 : (real)-1;
                    pulse += polyblep(s.phase, s.dt) - polyblep(ph2, s.dt);
                    pulse *= p->sqGain;
                    s.sqSagLp += (pulse - s.sqSagLp) * aSqSag;
                    const real sag = pulse - s.sqSagLp;
                    s.sqSlew += (sag - s.sqSlew) * (sag > s.sqSlew ? aSqRise : (real)1);
                    x = s.sqSlew;
                }
                else
                    x = ((real)2 * s.phase - (real)1 - polyblep(s.phase, s.dt)) * p->sawSign;
            }

            if (inGain != (real)1) // the Overdrive: the level into the filter
                x *= inGain;

            // the coupling into the filter
            s.hp1Lp += (x - s.hp1Lp) * aHp1;
            x -= s.hp1Lp;

            // the ladder
            const real y4 = ladder(x, g);

            // the coupling out of the filter, the amp, the OTA's saturation
            s.hp2Lp += (y4 - s.hp2Lp) * aHp2;
            real y = (y4 - s.hp2Lp) * resoGainLin * gainLin;
            if (bassMod) // the coupling into the amp at 7.2 Hz for 72: (s + w72) / (s + w7.2) = 1 + 9 w7.2 / (s + w7.2)
            {
                s.bassLp += (y - s.bassLp) * aBass;
                y += (BASS_HZ / BASS_MOD_HZ - (real)1) * s.bassLp;
            }
            y *= s.amp;
            if (p->vcaSat > 0) // a soft limit, each side its own (u tanh(u)/u: the Pade's, flat from |u| = 3)
            {
                const bool pos = y > 0;
                const real u = y * (pos ? satInvP : satInvN);
                y = (pos ? satP : satN) * u * tanhOverX(u);
            }
            if (muffler) // the Muffler: what is above the bass, linear to the knee, then soft towards the limit
            {
                s.mufLp += (y - s.mufLp) * aMuf;
                const real hi = y - s.mufLp, a = std::fabs(hi), knee = MUF_KNEE[muffler];
                if (a > knee)
                {
                    const real w = MUF_LIM[muffler] - knee, u = (a - knee) / w;
                    const real c = knee + w * u * tanhOverX(u);
                    y = s.mufLp + (hi < 0 ? -c : c);
                }
            }
            s.fmAcc += y; // the Filter FM's input (control())
            return y;
        }

        // One step of the ladder at the core rate: x the input, g the cutoff's tan; returns the top node. The nodes
        // v1 (bottom, C18) ... v4 (top, C26); the input pair's current I = G0 u into v1, u = x - k hp(v4) its
        // differential voltage; the pairs G1 (v1-v2), G2 (v2-v3), G3 (v3-v4) and the top pair G4 (v4 to the bias);
        // each conductance tanh(v)/v of the voltage across it, from the sample before or the iteration before.
        // Trapezoidal integrators v_i = s_i + h_i (currents in - out), h_i = g / c_i, solved from the top down.
        inline real ladder(real x, real g)
        {
            State &s = st;
            const real k = kRes;
            const real ha1 = fbHa1, hb1 = -s.fbLp * ha1; // the feedback's highpass: ha v4 + hb
            // with a second one after it (fbHp2), still linear in v4: hp2(ha1 v4 + hb1) = ha2 ha1 v4 + ha2 hb1 + hb2
            const real ha2 = fbHa2;
            const real ha = p->fbHp2 > 0 ? ha2 * ha1 : ha1;
            const real hb = p->fbHp2 > 0 ? ha2 * hb1 - s.fbLp2 * ha2 : hb1;
            real v1 = 0, v2 = 0, v3 = 0, v4 = 0;
            for (int it = 0; it <= VA303_ITER; it++)
            {
                const real h1 = g * capInv[0], h2 = g * capInv[1], h3 = g * capInv[2], h4 = g * capInv[3];
                const real G0 = s.g0, G1 = s.gam[0], G2 = s.gam[1], G3 = s.gam[2], G4 = s.gam[3] * p->topG;
                // v4 = a4 + b4 v3, v3 = a3 + b3 v2, v2 = a2 + b2 v1
                const real i4 = (real)1 / ((real)1 + h4 * (G3 + G4) + leak);
                const real a4 = s.s[3] * i4, b4 = h4 * G3 * i4;
                const real i3 = (real)1 / ((real)1 + h3 * (G2 + G3 * ((real)1 - b4)));
                const real a3 = (s.s[2] + h3 * G3 * a4) * i3, b3 = h3 * G2 * i3;
                const real i2 = (real)1 / ((real)1 + h2 * (G1 + G2 * ((real)1 - b3)));
                const real a2 = (s.s[1] + h2 * G2 * a3) * i2, b2 = h2 * G1 * i2;
                // v4 = A + B v1; the bottom node with the input pair's current and the feedback in it
                const real A = a4 + b4 * (a3 + b3 * a2), B = b4 * b3 * b2;
                const real gin = h1 * G0;
                const real num = s.s[0] + h1 * G1 * a2 + gin * (x - k * hb - k * ha * A);
                const real den = (real)1 + h1 * G1 * ((real)1 - b2) + gin * k * ha * B;
                v1 = num / den;
                v2 = a2 + b2 * v1;
                v3 = a3 + b3 * v2;
                v4 = a4 + b4 * v3;
                const real u = x - k * (ha * v4 + hb);
                // the conductances at these voltages: the next iteration's, or the next sample's
                s.g0 = tanhOverX(p->driveIn * u);
                s.gam[0] = tanhOverX(p->driveLadder * (v1 - v2));
                s.gam[1] = tanhOverX(p->driveLadder * (v2 - v3));
                s.gam[2] = tanhOverX(p->driveLadder * (v3 - v4));
                s.gam[3] = tanhOverX(p->driveLadder * v4);
            }
            s.s[0] = (real)2 * v1 - s.s[0];
            s.s[1] = (real)2 * v2 - s.s[1];
            s.s[2] = (real)2 * v3 - s.s[2];
            s.s[3] = (real)2 * v4 - s.s[3];
            const real lp = (gFb * v4 + s.fbLp) * ha1;
            if (p->fbHp2 > 0)
            {
                const real w = v4 - lp; // the first highpass's output into the second
                const real lp2 = (gFb2 * w + s.fbLp2) * ha2;
                s.fbLp2 = (real)2 * lp2 - s.fbLp2;
            }
            s.fbLp = (real)2 * lp - s.fbLp;
            return v4;
        }

        // the decimator: 2x a halfband of 39 taps (the Drive's), 8x a windowed sinc of 255 taps
        static constexpr int HB_HALF = 20;
        static constexpr real HALFBAND[HB_HALF / 2] = {-0.000527537252, 0.0014429515, -0.00325012875, 0.00636456436,
                                                       -0.0113833836, 0.0192395026, -0.031678866, 0.0530722813,
                                                       -0.0994315849, 0.316031886};
        static constexpr int SINC_TAPS = 255;
        real sinc[OS == 2 ? 1 : SINC_TAPS];

        void initDecimator()
        {
#if VA303_OS != 2
            // a Blackman-windowed sinc at 0.45 of the output rate's Nyquist (the reference's builds only)
            const double fc = 0.45 * 0.5 / OS;
            double sum = 0;
            for (int i = 0; i < SINC_TAPS; i++)
            {
                const double m = i - 0.5 * (SINC_TAPS - 1);
                const double xx = 2 * 3.14159265358979 * fc * m;
                const double sn = m == 0 ? 1 : std::sin(xx) / xx;
                const double w = 0.42 - 0.5 * std::cos(2 * 3.14159265358979 * i / (SINC_TAPS - 1)) + 0.08 * std::cos(4 * 3.14159265358979 * i / (SINC_TAPS - 1));
                sinc[i] = (real)(sn * w);
                sum += sn * w;
            }
            for (int i = 0; i < SINC_TAPS; i++)
                sinc[i] = (real)(sinc[i] / sum);
#endif
        }

        // feeds one core sample (k of OS in this output sample); returns the output sample when k == OS - 1, else acc
        inline real decimate(real y, int k, real acc)
        {
            State &s = st;
            if (OS == 2)
            {
                // the rings written twice, so that w[j] is the sample j back: the odd (second) core samples through the
                // halfband's odd taps, the even one at its centre
                if (k == 0)
                {
                    s.decPos = (s.decPos == 0 ? HB_HALF : s.decPos) - 1;
                    s.dec[s.decPos] = s.dec[s.decPos + HB_HALF] = y; // even
                    return acc;
                }
                s.dec[2 * HB_HALF + s.decPos] = s.dec[3 * HB_HALF + s.decPos] = y; // odd
                const real *vo = s.dec + 2 * HB_HALF + s.decPos;
                real out = 0;
                for (int j = 0; j < HB_HALF / 2; j++)
                    out += HALFBAND[j] * (vo[j] + vo[HB_HALF - 1 - j]);
                out += (real)0.5 * s.dec[s.decPos + HB_HALF / 2 - 1];
                return out;
            }
            else
            {
                s.dec[s.decPos] = y;
                s.decPos = (s.decPos + 1) % SINC_TAPS;
                if (k < OS - 1)
                    return acc;
                real out = 0;
                int pos = s.decPos;
                for (int j = 0; j < SINC_TAPS; j++)
                {
                    pos = pos == 0 ? SINC_TAPS - 1 : pos - 1;
                    out += sinc[j] * s.dec[pos];
                }
                return out;
            }
        }
    };

} // namespace va303
