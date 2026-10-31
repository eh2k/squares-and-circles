#pragma once

// SYNTH/XX-303's Drive. The voice through a gain (x 1 at 0+ to x 63, +36 dB, at 1; the folder x 1 + 15 Drive) into
// a curve, the type, then a one-pole lowpass (12 kHz at 0+ to 5 kHz at 1), the level made up (a voice's peak of REF
// stays where it was, minus 1.1 Drive^2 neper) and a DC blocker. 2x oversampled (a 39-tap halfband, flat to 19 kHz,
// -72 dB from 29 kHz), 19 samples (0.4 ms) late; 0 is off (nothing runs). The curves rise with slope 1 at 0:
//   DIODE  1 - e^-u, on the negative side a diode that clips at 0.75 of it: even harmonics too (the Drive before)
//   TANH   tanh(u): soft and symmetric, odd harmonics only
//   HARD   u clipped to +-1: straight up to the corner, then flat; buzzy
//   ASINH  asinh(u), unbounded: it compresses rather than clips, the wave keeps its shape and the notes their dynamics
//   FOLD   aanl.lib's sine wavefolder with t = 1, r = 1: u up to +-1, then sin(pi u / 2), folding back as the drive
//          rises (a voice's peak from Drive 0.2 on, twice at 1); metallic. Its level made up as if it clipped (it
//          stays within +-1)
// Each antialiased with first-order ADAA, aanl::ADAA1 (Faust's aanl.lib, lib/aanl): the difference quotient of the
// curve's antiderivative over two samples instead of the curve at one, half a 96 kHz sample later, 0.5 dB down at
// 10 kHz. Its threshold is 0.01 rather than aanl.lib's 0.001: in float the quotient of two close samples loses digits.
// A new type fades in over 5 ms, the old one running on beside it, so that the switch doesn't click.

#include <math.h>
#include "aanl/aanl.h"

class Drive
{
  public:
    enum
    {
        DIODE,
        TANH,
        HARD,
        ASINH,
        FOLD,
        TYPES
    };

    // the knob, 0...1, and the type
    void set(float knob, int type)
    {
        if (type < 0 || type >= TYPES)
            type = cur.t;
        if (knob != od)
        {
            if (od <= 0 && knob > 0)
                reset();
            od = knob;
            if (od > 0)
            {
                lp = 1 - expf(-6.283185307f * (12000.f - 7000.f * od) / 96000.f);
                cur.level(od);
            }
        }
        if (type != cur.t)
        {
            if (od > 0) // the old curve fades out
            {
                old = cur;
                fade = FADE;
            }
            cur.t = type;
            if (od > 0) // ADAA1 goes on with the new curve's antiderivative from the last sample
            {
                cur.level(od);
                cur.adaa.x1 = cur.g * x1;
                cur.adaa.F1x1 = F1(type, cur.adaa.x1);
            }
        }
    }

    // in place, 48 kHz
    void process(float *buf, int n)
    {
        if (od <= 0)
            return;
        for (int i = 0; i < n; i++)
        {
            // the histories: rings written twice, so that w[k] is the sample k back, k < HB_HALF
            pos = (pos == 0 ? HB_HALF : pos) - 1;
            // 2x up: the even 96 kHz sample from the halfband's taps, the odd one is the input 9 samples back
            xin[pos] = xin[pos + HB_HALF] = buf[i];
            const float *x = xin + pos;
            float e = 0;
            for (int k = 0; k < HB_HALF / 2; k++)
                e += HALFBAND[k] * (x[k] + x[HB_HALF - 1 - k]);
            const float v0 = voice(2 * e), v1 = voice(x[HB_HALF / 2 - 1]);
            // 2x down: the halfband over the 96 kHz output, its even taps on the odd samples
            veven[pos] = veven[pos + HB_HALF] = v0;
            vodd[pos] = vodd[pos + HB_HALF] = v1;
            const float *vo = vodd + pos;
            float y = 0;
            for (int k = 0; k < HB_HALF / 2; k++)
                y += HALFBAND[k] * (vo[k] + vo[HB_HALF - 1 - k]);
            y += 0.5f * veven[pos + HB_HALF / 2 - 1];
            // the DC blocker (the asymmetric curve's DC), 3.8 Hz
            dc = y - dcIn + 0.9995f * dc;
            dcIn = y;
            buf[i] = dc;
        }
    }

  private:
    static constexpr float REF = 0.25f; // a voice's peak at the app's level
    static constexpr float NEG = 0.75f; // the negative side's diode clips at 0.75 of the positive one's
    static constexpr int HB_HALF = 20;  // the taps of a polyphase branch (HALFBAND: half of them)
    // a halfband lowpass of 39 taps at 96 kHz: h[0], h[2], ..., h[18] (h[19] = 0.5, the other odd taps 0, symmetric)
    static constexpr float HALFBAND[HB_HALF / 2] = {-0.000527537252f, 0.0014429515f, -0.00325012875f, 0.00636456436f,
                                                    -0.0113833836f, 0.0192395026f, -0.031678866f, 0.0530722813f,
                                                    -0.0994315849f, 0.316031886f};
    static constexpr float ADAA_EPS = 0.01f; // ADAA1's threshold: closer samples take the curve at their middle
    static constexpr int FADE = 480;         // a type's fade in, 5 ms at 96 kHz

    // the curve of type t (not inlined: called from several places)
    __attribute__((noinline)) static float f(int t, float u)
    {
        switch (t)
        {
        case TANH:
            return aanl::tanh1::f(u);
        case HARD:
            return aanl::hardclip::f(u);
        case ASINH:
            return aanl::asinh1::f(u);
        case FOLD:
            return fabsf(u) <= 1 ? u : sinf(1.5707963268f * u);
        default:
            return u >= 0 ? 1 - expf(-u) : NEG * (expf(u / NEG) - 1);
        }
    }
    // its antiderivative, 0 at 0; the diode's from the curve's value: u + e^-u - 1 = u - f above 0,
    // NEG^2 (e^(u/NEG) - 1) - NEG u = NEG (f - u) below; the folder's 1/2 - 2/pi cos(pi |u| / 2) beyond +-1
    // (aanl.lib's sinF1 for r = 1)
    __attribute__((noinline)) static float F1(int t, float u)
    {
        switch (t)
        {
        case TANH:
            return aanl::tanh1::F1(u);
        case HARD:
            return aanl::hardclip::F1(u);
        case ASINH:
            return aanl::asinh1::F1(u);
        case FOLD:
            return fabsf(u) <= 1 ? 0.5f * u * u : 0.5f - 0.636619772f * cosf(1.5707963268f * fabsf(u));
        default:
        {
            const float y = f(DIODE, u);
            return u >= 0 ? u - y : NEG * (y - u);
        }
        }
    }

    // a curve with its gain, makeup and ADAA1
    struct Stage
    {
        int t = DIODE;
        float g = 0, makeup = 0;
        aanl::ADAA1 adaa;

        void level(float od)
        {
            g = t == FOLD ? 1 + 15 * od : expf(od * (36.f * 0.115129255f)); // +36 dB at 1; the folder linear
            makeup = REF / f(t == FOLD ? HARD : t, g * REF) * expf(-1.1f * od * od);
        }
        float operator()(float x)
        {
            const int c = t;
            return adaa(ADAA_EPS, [c](float v) { return f(c, v); }, [c](float v) { return F1(c, v); }, g * x) * makeup;
        }
    };

    float od = 0, lp = 0, s = 0, dc = 0, dcIn = 0, x1 = 0; // x1: the last input of the curves
    float xin[2 * HB_HALF] = {0}, veven[2 * HB_HALF] = {0}, vodd[2 * HB_HALF] = {0};
    int pos = 0, fade = 0;
    Stage cur, old; // the type, and the one it fades from

    // one 96 kHz sample (not inlined: process() takes two a sample)
    __attribute__((noinline)) float voice(float x)
    {
        float y = cur(x);
        if (fade > 0)
        {
            y += (old(x) - y) * (fade * (1.f / FADE));
            fade--;
        }
        x1 = x;
        s += lp * (y - s);
        return s;
    }
    void reset()
    {
        s = dc = dcIn = x1 = 0;
        for (int k = 0; k < 2 * HB_HALF; k++)
            xin[k] = veven[k] = vodd[k] = 0;
        cur.adaa = aanl::ADAA1();
        fade = 0;
    }
};
