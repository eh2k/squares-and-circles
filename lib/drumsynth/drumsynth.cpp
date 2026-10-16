// Copyright (C)2021 - Eduard Heidt
//
// Author: Eduard Heidt (eh2k@gmx.de)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//

#define private public

#include "stmlib/dsp/dsp.h"
#include "stmlib/dsp/filter.h"
#include "plaits/dsp/oscillator/oscillator.h"
#include "stmlib/dsp/cosine_oscillator.h"
#include "plaits/resources.h"
#include "misc/noise.hxx"
#include "misc/Biquad.h"
#include "drumsynth.h"
#include "string.h"

#ifndef __SAMPLE_RATE
constexpr float __SAMPLE_RATE = 48000.f;
#endif

// skipping of silent parts; test builds (-DDRUMSYNTH_TEST_HOOKS) can switch it off to compare with the full render
#ifdef DRUMSYNTH_TEST_HOOKS
extern "C"
{
    bool drum_synth_skip_silent = true;
}
#else
static constexpr bool drum_synth_skip_silent = true;
#endif

#define CONSTRAIN_F(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#ifdef _MSC_VER
#define DRUMSYNTH_NOINLINE __declspec(noinline)
#else
#define DRUMSYNTH_NOINLINE __attribute__((noinline))
#endif

inline float dB2amp(float dB)
{
    return expf(dB * 0.11512925464970228420089957273422f); // return pow(10.0, (0.05*dB)); // naive, inefficient version
}

inline float amp2dB(float amp)
{
    return amp > 0.f ? 20.f * log10f(amp) : -120.f;
}

// the packed points of the drumkit (see drum_synth_load_models): a varint (LEB128, at most 5 bytes), a half float
// (IEEE 754 binary16) and an int16, little endian; the loader has checked that they are there
static inline uint32_t packed_varint(const uint8_t *&p)
{
    uint32_t v = 0;
    for (uint32_t shift = 0;; shift += 7)
    {
        const uint8_t b = *p++;
        v |= shift < 32 ? (uint32_t)(b & 0x7F) << shift : 0;
        if (!(b & 0x80))
            return v;
    }
}

static inline float half_to_float(uint16_t h)
{
    const uint32_t sign = (uint32_t)(h & 0x8000) << 16;
    const uint32_t exp = (h >> 10) & 0x1F, man = h & 0x3FF;
    if (exp == 0) // zero or subnormal: man * 2^-24
    {
        const float f = (float)man * 5.9604644775390625e-8f;
        return sign ? -f : f;
    }
    const uint32_t bits = sign | (exp == 31 ? 0x7F800000u : (exp + 112) << 23) | (man << 13);
    float f;
    memcpy(&f, &bits, sizeof(f));
    return f;
}

static inline float packed_half(const uint8_t *&p)
{
    const uint16_t h = (uint16_t)(p[0] | (p[1] << 8));
    p += 2;
    return half_to_float(h);
}

static inline int16_t packed_s16(const uint8_t *&p)
{
    const int16_t v = (int16_t)(uint16_t)(p[0] | (p[1] << 8));
    p += 2;
    return v;
}

// the envelope: exponential segments between its points, read from the drumkit buffer one after the other
class Envelope
{
    const EnvArgs *args_;

public:
    // not inlined: it runs once per envelope (drum_synth_init), four copies would only take flash
    DRUMSYNTH_NOINLINE void init(const EnvArgs *args)
    {
        args_ = args;
        value_ = 0.f;
        c_ = 1.0f;
        segment_ = 0;
        rise_ = -1;

        if (args_->n == 0)
        {
            value_ = 1.f;
            pos_ = 0;
            len_ = 0;
            return;
        }

        next_ = args_->points;
        const uint8_t *p = next_;
        const uint32_t t0 = packed_varint(p);
        const float v0 = packed_half(p);
        if (args_->n == 1)
        {
            value_ = v0;
            pos_ = 0;
            len_ = 0;
        }
        else if (t0 == 0)
            value_ = v0;

        // the last segment that rises, the last segment not counted: the decay stretches only the segments after it
        float v = v0;
        for (int32_t i = 1; i < args_->n - 1; i++)
        {
            packed_varint(p);
            const float w = packed_half(p);
            if (v < w)
                rise_ = i - 1;
            v = w;
        }
    }

    // e = s * c ^ l
    inline float calc_c(float start, float end, int len)
    {
        constexpr float e = 0.0001f;
        if (fabsf(start - end) < __FLT_EPSILON__)
            return 1.f;
        else
        {
            return powf(e + (end / start), 1.0f / len);
        }
    }

    // the next segment (not inlined: process stays small, it runs per sample and envelope)
    DRUMSYNTH_NOINLINE void next_segment(float stretch)
    {
        if (segment_ <= rise_)
            stretch = 1;

        const uint8_t *p = next_;
        packed_varint(p);
        value_ = packed_half(p); // the start point
        next_ = p;               // the end point, the start of the segment after this one
        const uint32_t dt = packed_varint(p);
        const float end = packed_half(p);

        pos_ = 0;
        len_ = dt * stretch;
        c_ = calc_c(value_, end, len_);
        segment_++;
    }

    inline void process(uint32_t t, float stretch)
    {
        if (t == 0) // restart
        {
            pos_ = 0;
            len_ = 0;
            segment_ = 0;
            next_ = args_->points;
        }

        if (pos_ < len_)
        {
            value_ *= c_;
            ++pos_;
        }
        else if (segment_ < (args_->n - 1))
            next_segment(stretch);
    }

    inline bool finished()
    {
        return pos_ != 0 && pos_ == len_;
    }

    // all segments done, value() stays constant from now on (until the next restart)
    inline bool ended()
    {
        if (args_->n <= 1)
            return args_->n == 1;
        return segment_ >= args_->n - 1 && pos_ >= len_;
    }

    inline float value()
    {
        return value_;
    }

private:
    int16_t segment_; // an envelope has at most INT16_MAX points (the loader)
    int16_t rise_;    // see init: up to this segment the decay does not stretch, -1: none
    uint32_t len_;
    uint32_t pos_;

    float value_;
    float c_;

    const uint8_t *next_; // the start point of the next segment
};

// cycles (per sample, or a duty cycle) 0..1 -> 32-bit fixed point phase (one cycle = 2^32), saturating
static inline uint32_t to_q32(float x)
{
#ifdef __ARM_FP
    // VCVT saturates in hardware (x < 0 -> 0, x >= 1 -> 0xFFFFFFFF), same result as below
    float q;
    asm("vcvt.u32.f32 %0, %1" : "=t"(q) : "t"(x * 4294967296.f));
    uint32_t u;
    memcpy(&u, &q, sizeof(u));
    return u;
#else
    return x <= 0.f ? 0u : x >= 1.f ? 0xFFFFFFFFu : (uint32_t)(x * 4294967296.f);
#endif
}

class Oscillator
{
public:
    void Init(float freq)
    {
        pw_ = 0.5f;
        phase_inc_ = freq / __SAMPLE_RATE;
        f_ = freq;
        f_sr_ = freq / __SAMPLE_RATE;
        osc.Init();
        metal_phase_ = kMetalStart;
        metal_inc_ = to_q32(f_sr_);
        metal_pw_ = to_q32(pw_);
    }

    inline void reset()
    {
        osc.Init();
        metal_phase_ = kMetalStart;
    }

    inline void pitch(float pitch)
    {
        phase_inc_ = f_ * pitch / __SAMPLE_RATE;
    }

    inline void duty(float duty)
    {
        pw_ = duty;
        metal_pw_ = to_q32(duty);
    }

    // metallic: square wave with a 32-bit fixed point phase, wraps by overflow
    static constexpr uint32_t kMetalStart = 0x80000000u; // phase 0.5, as plaits::Oscillator::Init

    inline void metal_pitch(float pitch)
    {
        metal_inc_ = to_q32(f_sr_ * pitch);
    }

    inline void Square(float &out)
    {
        this->osc.Render<plaits::OSCILLATOR_SHAPE_SQUARE>(phase_inc_, pw_, &out, 1);
    }

    inline void Saw(float &out)
    {
        this->osc.Render<plaits::OSCILLATOR_SHAPE_SAW>(phase_inc_, pw_, &out, 1);
    }

    inline void Tri(float &out)
    {
        this->osc.Render<plaits::OSCILLATOR_SHAPE_TRIANGLE>(phase_inc_, pw_, &out, 1);
    }

    // metallic: 1 while the square wave is high, then one sample on
    inline uint32_t Metallic()
    {
        uint32_t high = metal_phase_ < metal_pw_;
        metal_phase_ += metal_inc_;
        return high;
    }

    inline void Sine(float &out)
    {
        // #define PI_F 3.1415927410125732421875f
        // out = sinf((0.25f + osc.phase_) * PI_F * 2.f);

        // phase must stay < 1: Interpolate reads lut_sine[128 + phase * 512 + 1], lut_sine has 641 entries
        out = stmlib::Interpolate(plaits::lut_sine + 128, osc.phase_, 512.0f);

        osc.phase_ += phase_inc_;
        if (osc.phase_ >= 1.0f)
            osc.phase_ -= 1.0f;
    }

private:
    float phase_inc_, pw_, f_, amp2_;
    float f_sr_;                                // f_ / __SAMPLE_RATE
    uint32_t metal_phase_, metal_inc_, metal_pw_; // metallic, one cycle = 2^32
    plaits::Oscillator osc;
};

// oscillators a part needs: n for the metallic one, 1 for every other (for OSC_SAMPLE n is the sample's index)
static inline uint32_t part_osc_count(const PartArgs *part)
{
    return part->osc.type == OSC_SAMPLE ? 1 : std::max<uint32_t>(1, part->osc.n);
}

// a one-pole's coefficient for a cutoff at fc (cycles per sample)
static inline float one_pole_coefficient(float fc)
{
    return 1.f - expf(-6.2831853f * fc);
}

struct drum_synth_Part
{
    Oscillator* _osc = nullptr;
    stmlib::DCBlocker _dc_blocker;

    WhiteNoise noise = {};
    Envelope _amp = {};
    Envelope _pitch = {};
    Envelope _vca = {};
    Envelope _vcf_env = {};
    Biquad biquad1 = {};
    Biquad biquad2 = {};

    std::pair<float, float> biquadA[2] = {};
    std::pair<float, float> biquadB[2] = {};

    // Chamberlin state variable filter (lowpass), as in the original RC-808
    struct
    {
        float f, df;       // 2 * sin(pi * fc / fs), ramped linearly between control updates
        float q;           // 1 / resonance
        float vel_cutoff;  // Hz, per note
        uint32_t count;    // samples until next control update
        float low[2], band[2];
    } _vcf = {};

    const PartArgs *part;

    float amp = 0.4380016479995117f;

    // first order filters (BIQUAD_LP1 / BIQUAD_HP1): the one-pole coefficients of bq1 and bq2
    float fo1 = 0.f, fo2 = 0.f;

    // OSC_SAMPLE: the position, a sample index (negative before the start) and the fraction toward the next one
    int32_t smp_idx = 0;
    float smp_frac = 0.f;

    inline float sample_at(int32_t i) const
    {
        if (i < 0 || (uint32_t)i >= part->sample_len)
            return 0.f;
        switch (part->sample_fmt)
        {
        case SAMPLE_S8:
            return (float)(int8_t)part->sample[i] * (1.f / 128.f);
        case SAMPLE_S16LE:
            return (float)(int16_t)(part->sample[2 * i] | (part->sample[2 * i + 1] << 8)) * (1.f / 32768.f);
        default:
            return ((float)part->sample[i] - 128.f) * (1.f / 128.f);
        }
    }

    // the sample at the position, by the quadratic through it and its two neighbours (as the TR-909 hats are
    // read: h1 at t = 0, h0 at t = 1); the position then steps by inc samples
    inline float sample_next(float inc)
    {
        const float t = smp_frac;
        const float h2 = sample_at(smp_idx - 1), h1 = sample_at(smp_idx), h0 = sample_at(smp_idx + 1);
        const float t2 = t * t;
        const float y = (t2 * 0.5f - t * 0.5f) * h2 + (1.f - t2) * h1 + (t2 * 0.5f + t * 0.5f) * h0;
        smp_frac += inc;
        while (smp_frac >= 1.f)
        {
            smp_frac -= 1.f;
            if (smp_idx <= (int32_t)part->sample_len) // stops one past the end: sample_ended()
                smp_idx++;
        }
        return y;
    }

    inline bool sample_ended() const
    {
        return part->sample == nullptr || smp_idx > (int32_t)part->sample_len;
    }

    // metallic: sum of the square oscillators, their phase increments are only updated when the pitch changes
    float metal_pitch_ = -1.f; // pitch the increments were computed for (the pitch is never negative)

    inline float metallic(float pitch)
    {
        uint32_t high = 0; // oscillators at +1, the others are at -1
        if (pitch != metal_pitch_)
        {
            metal_pitch_ = pitch;
            for (size_t j = 0; j < part->osc.n; j++)
            {
                this->_osc[j].metal_pitch(pitch);
                high += this->_osc[j].Metallic();
            }
        }
        else
        {
            for (size_t j = 0; j < part->osc.n; j++)
                high += this->_osc[j].Metallic();
        }

        float osc = (float)(2 * (int32_t)high - (int32_t)part->osc.n);

        _dc_blocker.Process(&osc, 1);
        return osc;
    }

    void init(const PartArgs *part)
    {
        this->part = part;

        _amp.init(&part->osc_amp);
        _pitch.init(&part->osc_pitch);
        _vca.init(&part->vca);
        _vcf_env.init(&part->vcf_env);

        _dc_blocker.Init(0.99f);
        metal_pitch_ = -1.f;
        if (_ws_table)
            waveshaper_init();

        // g is linear (as in RC-808.exe), Biquad expects the peak/shelf gain in dB
        const auto &a = part->bq1;
        if (a.mode >= BIQUAD_LP1)
            fo1 = one_pole_coefficient(a.f / __SAMPLE_RATE);
        else if (a.mode)
            biquad1.setBiquad(a.mode - 1, a.f / __SAMPLE_RATE, a.q, amp2dB(a.g));
        const auto &b = part->bq2;
        if (b.mode >= BIQUAD_LP1)
            fo2 = one_pole_coefficient(b.f / __SAMPLE_RATE);
        else if (b.mode)
            biquad2.setBiquad(b.mode - 1, b.f / __SAMPLE_RATE, b.q, amp2dB(b.g));

        if (part->osc.type == OSC_METALLIC)
        {
            amp = dB2amp(-26.f);

            float r = (logf(part->osc.fb) - logf(part->osc.fa)) / (part->osc.n - 1);

            for (size_t i = 0; i < part->osc.n; i++)
            {
                float f = i == 0 ? 1 : expf(r * i);

                _osc[i].Init(part->osc.fa * f);
                _osc[i].duty(part->osc.duty);
            }
        }
        else if (part->osc.type == OSC_SAMPLE)
        {
            amp = 1.f; // the sample as it is, full scale +-1
            smp_idx = (int32_t)part->sample_len + 1; // plays from a trigger on
        }
        else
        {
            amp = 0.4380016479995117f;
            _osc[0].Init(part->osc.fa);
        }
    }

    // Waveshaper as in the original RC-808.exe: the control points snapped to the pixel grid of the waveshaper
    // dialog (by the converter: the drumkit has the pixels, core/rc8.go), connected by a parametric natural cubic
    // spline (chord length parameter), sampled at 100 points and resampled into a 101 entry table over x = -1..1
    // (linear interpolation).
    static constexpr int kWaveShaperGrid = 265; // default client size of the RC-808 waveshaper dialog
    static constexpr int kWaveShaperMaxPoints = 64;
    static constexpr int kWaveShaperTable = 101;

    float *_ws_table = nullptr; // kWaveShaperTable entries, only for a part with a waveshaper (see drum_synth_init)

    // slopes of the natural cubic spline through (t[i], y[i])
    static void natural_spline_slopes(const float *t, const float *y, int n, float *k)
    {
        float b[kWaveShaperMaxPoints], c[kWaveShaperMaxPoints];
        for (int i = 0; i < n; i++)
        {
            float a = 0.f;
            b[i] = 0.f;
            c[i] = 0.f;
            k[i] = 0.f;
            if (i > 0)
            {
                float h = t[i] - t[i - 1];
                a = 1.f / h;
                b[i] += 2.f / h;
                k[i] += 3.f * (y[i] - y[i - 1]) / (h * h);
            }
            if (i < n - 1)
            {
                float h = t[i + 1] - t[i];
                c[i] = 1.f / h;
                b[i] += 2.f / h;
                k[i] += 3.f * (y[i + 1] - y[i]) / (h * h);
            }
            if (i > 0) // forward elimination (Thomas algorithm)
            {
                float m = a / b[i - 1];
                b[i] -= m * c[i - 1];
                k[i] -= m * k[i - 1];
            }
        }
        k[n - 1] /= b[n - 1];
        for (int i = n - 2; i >= 0; i--)
            k[i] = (k[i] - c[i] * k[i + 1]) / b[i];
    }

    static float spline_eval(const float *t, const float *y, const float *k, int n, int &seg, float u)
    {
        while (seg < n - 2 && u > t[seg + 1])
            seg++;
        float h = t[seg + 1] - t[seg];
        float s = (u - t[seg]) / h;
        float dy = y[seg + 1] - y[seg];
        float a = k[seg] * h - dy;
        float b = -k[seg + 1] * h + dy;
        return (1.f - s) * y[seg] + s * y[seg + 1] + s * (1.f - s) * ((1.f - s) * a + s * b);
    }

    void waveshaper_init()
    {
        float px[kWaveShaperMaxPoints], py[kWaveShaperMaxPoints], s[kWaveShaperMaxPoints];
        int n = 0;

        const uint8_t *q = part->ws.points; // the pixels (the converter snaps the points as RC-808.exe, core/rc8.go)
        for (uint32_t i = 0; i < part->ws.n && n < kWaveShaperMaxPoints; i++)
        {
            const float x = packed_s16(q);
            const float y = packed_s16(q);
            if (n > 0 && x == px[n - 1] && y == py[n - 1])
                continue; // points on the same pixel would divide by zero
            px[n] = x;
            py[n] = y;
            s[n] = n == 0 ? 0.f : s[n - 1] + sqrtf((x - px[n - 1]) * (x - px[n - 1]) + (y - py[n - 1]) * (y - py[n - 1]));
            n++;
        }

        if (n < 2)
        {
            for (int j = 0; j <= 100; j++)
                _ws_table[j] = -1.f + 0.02f * j; // identity
            return;
        }

        float kx[kWaveShaperMaxPoints], ky[kWaveShaperMaxPoints];
        natural_spline_slopes(s, px, n, kx);
        natural_spline_slopes(s, py, n, ky);

        // sample the curve at 100 points, back to -1..1
        float X[101], Y[101];
        int seg_x = 0, seg_y = 0;
        float ds = s[n - 1] / 99.f;
        for (int k = 0; k < 100; k++)
        {
            float u = k * ds;
            X[k] = 2.f * spline_eval(s, px, kx, n, seg_x, u) / kWaveShaperGrid - 1.f;
            Y[k] = 1.f - 2.f * spline_eval(s, py, ky, n, seg_y, u) / kWaveShaperGrid;
        }
        X[100] = 1.f;
        Y[100] = Y[99];

        // resample to x = -1, -0.98, ... 1: first sample with X > x (like the original, so a curve that
        // bends back in x takes its first crossing)
        for (int j = 0; j <= 100; j++)
        {
            float x = (j - 50) * 0.02f;
            int i = 1;
            while (i < 100 && !(X[i] > x))
                i++;

            float dx = X[i] - X[i - 1];
            float fi = (i - 1) + (dx != 0.f ? (x - X[i - 1]) / dx : 0.f);
            fi = CONSTRAIN_F(fi, 0.f, 100.f); // the original reads out of bounds here
            int idx = (int)fi;

            float y = idx >= 100 ? Y[100] : Y[idx] + (Y[idx + 1] - Y[idx]) * (fi - idx);
            if (1.f - y < 1e-5f)
                y = 1.f;
            _ws_table[j] = y;
        }
    }

    inline float waveshaper_transform(float x)
    {
        if (x >= 1.f)
            return 1.f;
        if (x <= -1.f)
            return -1.f;

        float f = (x + 1.f) * 50.f;
        int i = (int)f;
        if (i >= 100)
            return 1.f;
        return _ws_table[i] + (_ws_table[i + 1] - _ws_table[i]) * (f - i);
    }

    inline float biquad(Biquad &bq, const BiquadArgs &args, float fo, float x, std::pair<float, float> &state)
    {
        if (args.mode >= BIQUAD_LP1) // one pole, its state in state.first
        {
            const float d = x - state.first;
            state.first += d * fo;
            return (args.mode == BIQUAD_LP1 ? state.first : d) * args.g;
        }
        x = bq.process(x, state.first, state.second);
        if (args.mode <= BIQUAD_NOTCH) // output gain for LP/HP/BP/NOTCH, as in RC-808.exe
            x *= args.g;
        return x;
    }

    // SERIAL:   bq1 -> waveshaper -> bq2
    // PARALLEL: (bq1 + bq2) -> waveshaper, a THRU filter passes the input unfiltered
    inline float filter(float x, std::pair<float, float> state[2])
    {
        if (part->flags & BIQUAD_SERIAL)
        {
            if (part->bq1.mode)
                x = biquad(biquad1, part->bq1, fo1, x, state[0]);

            if (part->ws.n)
                x = waveshaper_transform(x);

            if (part->bq2.mode)
                x = biquad(biquad2, part->bq2, fo2, x, state[1]);
        }
        else
        {
            float y1 = part->bq1.mode ? biquad(biquad1, part->bq1, fo1, x, state[0]) : x;
            float y2 = part->bq2.mode ? biquad(biquad2, part->bq2, fo2, x, state[1]) : x;
            x = y1 + y2;

            if (part->ws.n)
                x = waveshaper_transform(x);
        }

        return x;
    }

    inline void reset()
    {
        for (size_t j = 0; j < part_osc_count(this->part); j++)
            this->_osc[j].reset();
        smp_idx = (int32_t)part->sample_len + 1; // a sample plays from a trigger on
        smp_frac = 0.f;

        memset(&this->biquadA, 0, sizeof(this->biquadA));
        memset(&this->biquadB, 0, sizeof(this->biquadB));
        _vcf = {};
    }

    // VCF behaviour taken from the original RC-808.exe:
    // velocity cutoff is 0 up to velocity 40, then rises logarithmically from 1e-5Hz (velocity 40)
    // to 10^(-5 + log10(7000 / 1e-5) * velDepth) Hz (velocity 127), limited to 7kHz
    static float vcf_velocity_cutoff(float velocity, float vel_depth)
    {
        int vel = (int)(velocity * 127.f + 0.5f);
        if (vel <= 40)
            return 0.f;

        const float lo = -5.f;             // log10(1e-5)
        const float hi = 3.84509804f;      // log10(7000)
        float top = lo + (hi - lo) * vel_depth; // log10 of cutoff at velocity 127
        float cutoff = powf(10.f, lo + (top - lo) / 87.f * (vel - 40));
        return cutoff < 7000.f ? cutoff : 7000.f;
    }

    // control rate update every 16 samples, like the original
    inline void vcf_control(uint32_t t, float bp, float velocity, float f)
    {
        const VCFArgs *vcf = part->vcf;
        const bool trig = t == 0;

        if (trig)
        {
            _vcf = {};
            _vcf.vel_cutoff = vcf_velocity_cutoff(velocity, vcf->velDepth);
        }

        if (_vcf.count == 0)
        {
            _vcf.count = 16;

            // cutoff(Hz) = (1 + (BP - 1) * EnvDepth) * CutoffOffset + VelocityCutoff, max 7.3kHz
            float fc = ((bp - 1.f) * vcf->envDepth + 1.f) * vcf->cutoff + _vcf.vel_cutoff;
            fc = CONSTRAIN_F(fc * f, 0.f, 7300.f);

            float target = 2.f * sinf(3.14159265f * fc / __SAMPLE_RATE);
            _vcf.q = 1.f / (vcf->res > 0.7f ? vcf->res : 0.7f);

            if (trig)
                _vcf.f = target;

            // stability limit of the chamberlin filter
            if (_vcf.f + _vcf.q > 1.99f)
                _vcf.f = 1.99f - _vcf.q;
            if (target + _vcf.q > 1.99f)
                target = 1.99f - _vcf.q;

            _vcf.df = (target - _vcf.f) / 16.f;
        }

        _vcf.count--;
    }

    inline float vcf_process(float x, int ch)
    {
        _vcf.low[ch] += _vcf.f * _vcf.band[ch];
        float high = x - _vcf.low[ch] - _vcf.q * _vcf.band[ch];
        _vcf.band[ch] += _vcf.f * high;
        return _vcf.low[ch];
    }

    uint32_t last_f = 0;

    // a part is silent once its VCA envelope - or, if the filter chain applies it, its osc amp envelope - has
    // ended at a level below kSilence; its output would only be that residual level from then on
    static constexpr float kSilence = 1e-3f;

    inline bool silent()
    {
        if (part->osc.type == OSC_SAMPLE && sample_ended())
            return true;
        if (this->_vca.ended() && this->_vca.value() < kSilence)
            return true;
        return (part->flags & (BIQUAD_SERIAL | BIQUAD_PARALLEL)) && this->_amp.ended() && this->_amp.value() < kSilence;
    }

    // a trigger (t = 0) restarts the oscillators as after drum_synth_init: phase and state of all oscillators and
    // the metallic DC blocker. So a silent part has no oscillator state to keep running until the next trigger;
    // only the noise sequence continues.
    inline void restart_oscillators()
    {
        for (size_t j = 0; j < part_osc_count(part); j++)
            this->_osc[j].reset();
        _dc_blocker.Init(0.99f);
        // fb: where it starts, in samples; may be fractional and negative (a delay before the first sample)
        smp_idx = (int32_t)floorf(part->osc.fb);
        smp_frac = part->osc.fb - (float)smp_idx;
    }

    // returns false if the part is silent: nothing is rendered and outL/outR are not written
    inline bool process_frame(float f, const DrumParams *params, float *outL, float *outR, size_t size)
    {
        if (part->muted)
            return false;
        if (params->t == 0)
            restart_oscillators();
        else if (drum_synth_skip_silent && silent())
        {
            if (part->osc.type == OSC_NOISE1 || part->osc.type == OSC_NOISE2)
                noise.skip(2 * size); // as the two nextf() per sample: same noise after the next trigger
            return false;
        }

        uint32_t t = params->t;
        float osc = 0;
        float osc2 = 0;
        uint32_t ff = f * __SAMPLE_RATE;

        if (last_f != ff)
        {
            if (part->bq1.mode >= BIQUAD_LP1)
                fo1 = one_pole_coefficient(part->bq1.f / __SAMPLE_RATE * f);
            else if (part->bq1.mode)
                this->biquad1.setFc(part->bq1.f / __SAMPLE_RATE * f);
            if (part->bq2.mode >= BIQUAD_LP1)
                fo2 = one_pole_coefficient(part->bq2.f / __SAMPLE_RATE * f);
            else if (part->bq2.mode)
                this->biquad2.setFc(part->bq2.f / __SAMPLE_RATE * f);
        }

        // if (t > 0 && this->_amp.finished() && this->_vca.finished())
        // {
        //     return;
        // }

        while (size--)
        {
            this->_amp.process(t, params->decay);
            this->_vca.process(t, params->decay);
            this->_pitch.process(t, params->decay);
            if ((part->flags & VCF) && part->vcf)
            {
                this->_vcf_env.process(t, params->decay);
                vcf_control(t, this->_vcf_env.value(), params->velocity, f);
            }

            ++t;

            // if (this->_amp.value() < (1.f / INT16_MAX) || this->_vca.value() < (1.f / INT16_MAX))
            // {
            //     out++;
            //     continue;
            // }

            switch (part->osc.type)
            {
            case OSC_NOISE1:
            case OSC_NOISE2:
            {
                float a = this->noise.nextf(-1, 1);
                float b = this->noise.nextf(-1, 1);
                float st = 0.5f - (params->stereo / 2);
                osc = (a * (1 - st) + b * st);
                osc2 = (b * (1 - st) + a * st);
            }
            break;
            case OSC_METALLIC:
                osc = metallic(this->_pitch.value() * f);
                break;
            case OSC_SAMPLE:
                osc = sample_next(part->osc.fa / __SAMPLE_RATE * this->_pitch.value() * f);
                break;

            case OSC_SINE:

                this->_osc[0].pitch(this->_pitch.value() * f);
                this->_osc[0].Sine(osc);
                break;
            case OSC_SQUARE:

                this->_osc[0].pitch(this->_pitch.value() * f);
                this->_osc[0].Square(osc);
                break;
            case OSC_SAW:

                this->_osc[0].pitch(this->_pitch.value() * f);
                this->_osc[0].Saw(osc);
                break;
            case OSC_TRI:

                this->_osc[0].pitch(this->_pitch.value() * f);
                this->_osc[0].Tri(osc);
                break;
            default:
                // unsupported osc type: output silence (size+1 = current + remaining samples)
                memset(outL, 0, (size + 1) * sizeof(float));
                memset(outR, 0, (size + 1) * sizeof(float));
                return true;
            }

            // stereo noise runs through its own filter state, everything else is mono
            const bool stereo = (part->flags & (BIQUAD_SERIAL | BIQUAD_PARALLEL)) && params->stereo > 0 &&
                                (part->osc.type == OSC_NOISE1 || part->osc.type == OSC_NOISE2);

            if (part->flags & (BIQUAD_SERIAL | BIQUAD_PARALLEL))
            {
                osc = filter(osc * amp * this->_amp.value(), this->biquadA);

                if (stereo)
                    osc2 = filter(osc2 * amp * this->_amp.value(), this->biquadB);
            }

            if ((part->flags & VCF) && part->vcf)
            {
                osc = vcf_process(osc, 0);
                if (stereo)
                    osc2 = vcf_process(osc2, 1);

                _vcf.f += _vcf.df;
            }

            if (!stereo)
                osc2 = osc;

            *outL++ = osc * this->_vca.value() * part->level * params->levelL;
            *outR++ = osc2 * this->_vca.value() * part->level * params->levelR;
        }

        last_f = ff;
        return true;
    }
};

// DrumSynth memory, one block: uint32_t part count, then the parts, aligned for drum_synth_Part
// (offset 4 on 32-bit targets, 8 on 64-bit hosts), then the oscillators of all parts, then the waveshaper
// tables of the parts with a waveshaper
static constexpr size_t kPartsOffset = (sizeof(uint32_t) + alignof(drum_synth_Part) - 1) / alignof(drum_synth_Part) * alignof(drum_synth_Part);
static_assert(alignof(drum_synth_Part) % alignof(Oscillator) == 0, "oscillators follow the parts");
static_assert(alignof(Oscillator) % alignof(float) == 0, "waveshaper tables follow the oscillators");

static inline drum_synth_Part *drum_synth_parts(DrumSynth inst)
{
    return reinterpret_cast<drum_synth_Part *>(reinterpret_cast<uint8_t *>(inst) + kPartsOffset);
}

extern "C" DrumSynth drum_synth_init(const DrumModel *inst, void *(*malloc)(size_t size))
{
    if (malloc == nullptr)
        malloc = ::malloc;

    size_t osc_count = 0, ws_count = 0;
    for (size_t i = 0; i < inst->n; i++)
    {
        osc_count += part_osc_count(&inst->part[i]);
        ws_count += inst->part[i].ws.n ? 1 : 0;
    }

    const size_t osc_offset = kPartsOffset + sizeof(drum_synth_Part) * inst->n;
    const size_t ws_offset = osc_offset + sizeof(Oscillator) * osc_count;
    const size_t malloc_size = ws_offset + sizeof(float) * drum_synth_Part::kWaveShaperTable * ws_count;

    if (auto p = (DrumSynth)malloc(malloc_size))
    {
        p[0] = inst->n;
        auto _part = drum_synth_parts(p);
        auto osc = reinterpret_cast<Oscillator *>(reinterpret_cast<uint8_t *>(p) + osc_offset);
        auto ws = reinterpret_cast<float *>(reinterpret_cast<uint8_t *>(p) + ws_offset);

        for (size_t i = 0; i < inst->n; i++)
        {
            new (&_part[i]) drum_synth_Part();
            auto osc_n = part_osc_count(&inst->part[i]);
            _part[i]._osc = osc;
            for (uint32_t j = 0; j < osc_n; j++)
                new (osc++) Oscillator{};
            if (inst->part[i].ws.n)
            {
                _part[i]._ws_table = ws;
                ws += drum_synth_Part::kWaveShaperTable;
            }
            _part[i].init(&inst->part[i]);
        }
        return p;
    }

    return nullptr;
}

extern "C" void drum_synth_deinit(DrumSynth inst, void (*free)(void *ptr))
{
    if (free == nullptr)
        free = ::free;

    free(inst); // one block, see drum_synth_init
}

extern "C" void drum_synth_reset(DrumSynth inst)
{
    if (inst)
    {
        auto _part = drum_synth_parts(inst);
        for (size_t e = 0; e < inst[0]; e++)
        {
            _part[e].reset();
        }
    }
}
extern "C" float drum_synth_waveshaper(DrumSynth inst, int part, float x)
{
    if (inst == nullptr || part < 0 || (uint32_t)part >= inst[0])
        return x;

    auto &p = drum_synth_parts(inst)[part];
    return p.part->ws.n ? p.waveshaper_transform(x) : x;
}

extern "C" void drum_synth_process_frame(DrumSynth inst, int part, float freq, const DrumParams *params, float *outL, float *outR, size_t size)
{
    if (inst)
    {
        auto _part = drum_synth_parts(inst);
        if (part >= 0)
        {
            if (!_part[part].process_frame(freq, params, outL, outR, size))
            {
                memset(outL, 0, size * sizeof(float));
                memset(outR, 0, size * sizeof(float));
            }
        }
        else
        {
            float tmpL[size];
            float tmpR[size];
            size_t skip = -1;

            for (size_t part = 0; part < inst[0]; part++)
            {
                if (skip == part)
                    continue;

                bool active = _part[part].process_frame(freq, params, tmpL, tmpR, size);

                if (part == 0 && _part[part].part->amp_mod.dest != 0)
                {
                    skip = _part[part].part->amp_mod.dest;
                    float modL[size];
                    float modR[size];
                    bool mod_active = _part[_part[part].part->amp_mod.dest].process_frame(freq, params, modL, modR, size);

                    if (active && mod_active) // a silent carrier or modulator contributes nothing
                    {
                        for (size_t j = 0; j < size; j++)
                        {
                            outL[j] += tmpL[j] * modL[j];
                            outR[j] += tmpR[j] * modR[j];
                        }
                    }
                }
                else if (active)
                {
                    for (size_t j = 0; j < size; j++)
                    {
                        outL[j] += tmpL[j];
                        outR[j] += tmpR[j];
                    }
                }
            }
        }
    }
}

// the 4 bytes at p are tag; no memcmp: an older squares-and-circles firmware does not give it to the apps (the compiler
// inlines a memcmp of 4 bytes only sometimes)
static inline bool is_tag(const uint8_t *p, const char *tag)
{
    return p != nullptr && p[0] == tag[0] && p[1] == tag[1] && p[2] == tag[2] && p[3] == tag[3];
}

// bounds-checked cursor over the serialized drumkit
struct DrumKitReader
{
    const uint8_t *p;
    const uint8_t *end;

    const uint8_t *take(size_t count, size_t elem_size)
    {
        if (p == nullptr || count > (size_t)(end - p) / elem_size)
        {
            p = nullptr; // sticky error
            return nullptr;
        }
        const uint8_t *r = p;
        p += count * elem_size;
        return r;
    }

    template <typename T>
    bool read(T &out)
    {
        if (const uint8_t *s = take(1, sizeof(T)))
        {
            memcpy(&out, s, sizeof(T));
            return true;
        }
        return false;
    }

    bool fail()
    {
        p = nullptr;
        return false;
    }

    // a varint (LEB128, at most 5 bytes), a half float
    bool varint(uint32_t &out)
    {
        uint32_t v = 0;
        for (uint32_t i = 0; i < 5; i++)
        {
            const uint8_t *b = take(1, 1);
            if (b == nullptr)
                return false;
            v |= (uint32_t)(*b & 0x7F) << (7 * i);
            if (!(*b & 0x80))
            {
                out = v;
                return true;
            }
        }
        return fail();
    }

    bool half(float &out)
    {
        uint16_t h = 0;
        if (!read(h))
            return false;
        out = half_to_float(h);
        return true;
    }

    // a list of points, varint h: even: h / 2 points follow, odd: the list (with an even h) that starts
    // (h - 1) / 2 bytes before this one; a point is a varint and a half float (envelope) or two int16 (waveshaper);
    // not inlined (five calls per part, when loading)
    DRUMSYNTH_NOINLINE bool packed_points(const uint8_t *begin, bool ws, uint32_t &n, const uint8_t *&points)
    {
        const uint8_t *at = p;
        uint32_t h = 0;
        if (!varint(h))
            return false;
        const bool ref = h & 1;
        DrumKitReader list = *this;
        if (ref)
        {
            const size_t back = h >> 1;
            if (back == 0 || back > (size_t)(at - begin))
                return fail();
            list = {at - back, at}; // the list lies before this one
            if (!list.varint(h) || (h & 1))
                return fail();
        }
        if ((h >> 1) > INT32_MAX)
            return fail();
        n = h >> 1;
        points = list.p;
        uint32_t t;
        for (uint32_t i = 0; i < n; i++) // the points are there
            if (ws ? list.take(1, 4) == nullptr : !list.varint(t) || list.take(1, 2) == nullptr)
                return fail();
        if (!ref)
            p = list.p;
        return true;
    }

    bool packed_env(const uint8_t *begin, EnvArgs &env)
    {
        uint32_t n = 0;
        if (!packed_points(begin, false, n, env.points) || n > INT16_MAX) // Envelope: int16_t segments
            return fail();
        env.n = (int32_t)n;
        return true;
    }

    bool packed_biquad(BiquadArgs &bq)
    {
        uint32_t mode = 0;
        if (!varint(mode))
            return false;
        bq.mode = (BiquadMode)mode;
        return mode == BIQUAD_THRU || (read(bq.f) && read(bq.q) && read(bq.g));
    }
};

// a part (see README.md): varint flags, varint osc type, uint8 the osc fields there (1 fa, 2 fb, 4 duty, 8 n;
// the others 0), float fa, float fb, half duty, varint n, the envelopes osc_pitch, osc_amp, vca, the biquads (varint
// mode, unless THRU float f, q, g), the waveshaper points (pixels), with VCF: VCFArgs (4 byte aligned in
// the buffer, referenced) and vcf_env, varint amp_mod.dest (unless 0 half offset), half level
static bool read_part(DrumKitReader &r, const uint8_t *begin, PartArgs *part)
{
    uint32_t flags = 0, type = 0;
    uint8_t fields = 0;
    if (!r.varint(flags) || !r.varint(type) || !r.read(fields) || (fields & ~0xF))
        return r.fail();
    part->flags = (PartFlags)flags;
    part->osc.type = (OscType)type;
    if (((fields & 1) && !r.read(part->osc.fa)) || ((fields & 2) && !r.read(part->osc.fb)) ||
        ((fields & 4) && !r.half(part->osc.duty)) || ((fields & 8) && !r.varint(part->osc.n)) ||
        part->osc.n > DRUM_SYNTH_MAX_OSC)
        return r.fail();

    uint32_t ws_n = 0;
    if (!r.packed_env(begin, part->osc_pitch) || !r.packed_env(begin, part->osc_amp) || !r.packed_env(begin, part->vca) ||
        !r.packed_biquad(part->bq1) || !r.packed_biquad(part->bq2) || !r.packed_points(begin, true, ws_n, part->ws.points))
        return false;
    part->ws.n = ws_n;

    if (part->flags & VCF)
    {
        const size_t pad = (4 - (size_t)(r.p - begin) % 4) % 4;
        part->vcf = r.take(pad, 1) ? reinterpret_cast<const VCFArgs *>(r.take(1, sizeof(VCFArgs))) : nullptr;
        if (part->vcf == nullptr || ((uintptr_t)part->vcf % alignof(VCFArgs)) != 0 || !r.packed_env(begin, part->vcf_env))
            return r.fail();
    }

    return r.varint(part->amp_mod.dest) && (part->amp_mod.dest == 0 || r.half(part->amp_mod.offset)) &&
           r.half(part->level);
}

extern "C" int drum_synth_load_models(const uint8_t *drumkit, size_t len, DrumModel _instModel[DRUM_SYNTH_MAX_MODELS], void *(*malloc)(size_t size))
{
    return drum_synth_load_models_ex(drumkit, len, _instModel, malloc, nullptr);
}

// the kit's SMPL table, after the instruments
struct SampleEntry
{
    char name[16]; // null-padded
    uint32_t offset, length;
    SampleFormat format;
};
static_assert(sizeof(SampleEntry) == 28, "SMPL entries are packed");
static constexpr uint32_t kMaxSamples = DRUM_SYNTH_MAX_OSC; // the index is the part's osc.n

// OSC_SAMPLE parts get their data, or are muted; an instrument with a sample mutes its SAMPLE_FALLBACK parts
static void resolve_samples(DrumKitReader &r, DrumModel *models, int count, bool complete, DrumSynthSampleResolver resolve)
{
    const SampleEntry *table = nullptr;
    uint32_t entries = 0;
    const uint8_t *magic = complete ? r.take(4, 1) : nullptr;
    if (is_tag(magic, "SMPL") && r.read(entries) && entries <= kMaxSamples)
        table = reinterpret_cast<const SampleEntry *>(r.take(entries, sizeof(SampleEntry)));
    if (table == nullptr)
        entries = 0;

    for (int i = 0; i < count; i++)
    {
        PartArgs *parts = const_cast<PartArgs *>(models[i].part);
        bool has_sample = false;
        for (size_t j = 0; j < models[i].n; j++)
        {
            PartArgs &p = parts[j];
            if (p.osc.type != OSC_SAMPLE)
                continue;
            p.muted = true;
            if (resolve == nullptr || p.osc.n >= entries)
                continue;
            SampleEntry e;
            memcpy(&e, &table[p.osc.n], sizeof(e)); // the table need not be aligned
            char name[17] = {};
            memcpy(name, e.name, 16);
            size_t bytes = 0;
            const uint8_t *data = resolve(name, &bytes);
            const size_t width = e.format == SAMPLE_S16LE ? 2 : 1;
            if (data == nullptr || e.format > SAMPLE_S16LE || (size_t)e.offset > bytes / width)
                continue;
            const size_t avail = bytes / width - e.offset;
            p.sample = data + (size_t)e.offset * width;
            p.sample_len = (uint32_t)(e.length && e.length < avail ? e.length : avail);
            p.sample_fmt = e.format;
            p.muted = false;
            has_sample = true;
        }
        for (size_t j = 0; j < models[i].n; j++)
            if (parts[j].flags & SAMPLE_FALLBACK)
                parts[j].muted = has_sample;
    }
}

extern "C" int drum_synth_load_models_ex(const uint8_t *drumkit, size_t len, DrumModel _instModel[DRUM_SYNTH_MAX_MODELS], void *(*malloc)(size_t size),
                                         DrumSynthSampleResolver resolve)
{
    if (drumkit == nullptr)
        return 0;

    if (malloc == nullptr)
        malloc = ::malloc;

    DrumKitReader r = {drumkit, drumkit + len};

    const uint8_t *magic = r.take(4, 1);
    if (!is_tag(magic, "!RC8"))
        return 0;

    uint32_t count = 0;
    if (!r.read(count))
        return 0;

    const bool all = count <= DRUM_SYNTH_MAX_MODELS; // the SMPL table follows the last instrument
    if (count > DRUM_SYNTH_MAX_MODELS)
        count = DRUM_SYNTH_MAX_MODELS;

    int inst_count = 0;
    for (uint32_t i = 0; i < count; i++)
    {
        // name field: 11 chars (not null-terminated if 11 chars long), then the MIDI note number
        const uint8_t *name = r.take(12, 1);
        uint32_t n = 0;
        if (name == nullptr || !r.read(n) || n > (size_t)(r.end - r.p)) // cheap sanity check, each part needs > 1 byte
            break;

        // one block: the parts, then the null-terminated name
        const size_t parts_n = n ? n : 1;
        PartArgs *parts = (PartArgs *)malloc(sizeof(PartArgs) * parts_n + 12);
        if (parts == nullptr)
            break;

        char *name_z = reinterpret_cast<char *>(parts + parts_n);
        memcpy(name_z, name, 11);
        name_z[11] = 0;

        bool ok = true;
        for (uint32_t j = 0; ok && j < n; j++)
        {
            PartArgs *part = new (&parts[j]) PartArgs{};
            ok = read_part(r, drumkit, part);
            if (part->amp_mod.dest >= n)
                part->amp_mod.dest = 0;
        }

        if (!ok)
            break; // truncated/corrupt: keep the instruments loaded so far

        _instModel[i].name = name_z;
        _instModel[i].midi_note = name[11];
        _instModel[i].n = n;
        _instModel[i].part = parts;
        inst_count++;
    }

    resolve_samples(r, _instModel, inst_count, all && inst_count == (int)count, resolve);
    return inst_count;
}

extern "C" bool drum_synth_envelope_point(const EnvArgs *env, int32_t i, EnvXY *xy)
{
    if (env == nullptr || xy == nullptr || i < 0 || i >= env->n)
        return false;
    const uint8_t *p = env->points;
    EnvXY e;
    e.t = packed_varint(p);
    e.v = packed_half(p);
    for (int32_t k = 0; k < i; k++)
    {
        e.t += packed_varint(p);
        e.v = packed_half(p);
    }
    *xy = e;
    return true;
}

extern "C" void drum_synth_free_models(DrumModel *_instModel, int count, void (*free)(void *ptr))
{
    if (free == nullptr)
        free = ::free;

    for (int i = 0; i < count; i++)
    {
        free((void *)_instModel[i].part); // one block with the name, see drum_synth_load_models
        _instModel[i] = {};
    }
}
