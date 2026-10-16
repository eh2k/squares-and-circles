// Author: AI generated (Claude Opus 5.5), prompted and reviewed by E.Heidt (eh2k@gmx.de)
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

// Drive: saturation, wave folder and bit crusher in one insert effect.
//
// Drive -> curve (2x oversampled) -> Crush (bits) -> Decim (sample rate) -> Tone (lowpass) -> DC blocker -> D/W
//
// The curves have slope 1 at zero and saturate at +-1 (TUBE: +1 / -0.5), scaled to kCeiling. They run at 96 kHz
// (halfband FIR up and down) with first-order antiderivative antialiasing (ADAA): y = (F(x) - F(x1)) / (x - x1),
// F the antiderivative of the curve. For each curve the quotient is rewritten so that x - x1 doesn't end up in a
// denominator where it could get small (ill-conditioned in float).

#include "../../squares-and-circles-api.h"
#include <math.h>

static float _drive = 0.35f;
static int32_t _mode = 0;
static float _tone = 0.8f;
static float _mix = 1.f;
static float _crush = 0.f;
static float _decim = 0.f;

static const char *_mode_names[] = {"SOFT", "TUBE", "HARD", "FOLD"};
enum
{
    SOFT,
    TUBE,
    HARD,
    FOLD,
};

// Halfband lowpass, 39 taps (Kaiser window, beta 7.9): flat to 18 kHz (-0.001 dB), -78 dB from 30 kHz at 96 kHz.
// Besides the center tap 0.5 only the odd taps are nonzero: kHalfband[k] is the tap at +-(2k + 1).
static constexpr int kTaps = 10;
static const float kHalfband[kTaps] = {
    0.315394734f, -0.0976084749f, 0.0503753044f, -0.0285423485f, 0.0161118433f,
    -0.00864279029f, 0.00424119559f, -0.00181660133f, 0.000625134329f, -0.000137997014f,
};

// ADAA averages two 96 kHz samples (-1.1 dB at 15 kHz): a 3-tap FIR at 48 kHz lifts it back, +-0.2 dB to 18 kHz
static constexpr float kLift = 0.05f;

// The wet path is 2 * kTaps + 0.25 samples late (upsampler kTaps, downsampler kTaps - 1, ADAA a quarter sample,
// lift 1): D/W mixes it with the input that many whole samples back.
static constexpr int kDryDelay = 2 * kTaps;

static constexpr float kTubeK2 = 4.f; // TUBE: the negative half saturates at -1 / sqrt(kTubeK2)

// The curves saturate at this level: the band-limited square waves of full drive overshoot it by up to 1.3x
static constexpr float kCeiling = 0.75f;

template <int N>
struct History // [0] is the newest sample; reads are contiguous because every sample is stored twice
{
    float buf[2 * N] = {};
    int pos = 0;

    inline void push(float x)
    {
        pos = (pos == 0 ? N : pos) - 1;
        buf[pos] = buf[pos + N] = x;
    }

    inline float operator[](int i) const
    {
        return buf[pos + i];
    }
};

struct Lowpass // 2-pole state variable filter, trapezoidal (stmlib::Svf's lowpass, but copyable)
{
    float s1 = 0.f, s2 = 0.f;

    inline float process(float in, float g, float r, float h)
    {
        const float hp = (in - r * s1 - g * s1 - s2) * h;
        const float bp = g * hp + s1;
        s1 = g * hp + bp;
        const float lp = g * bp + s2;
        s2 = g * bp + lp;
        return lp;
    }
};

struct Channel
{
    History<kDryDelay + 1> in; // 48 kHz input, for the upsampler and the dry signal
    History<2 * kTaps> odd;    // odd 96 kHz samples (the downsampler's side taps)
    History<kTaps> even;       // even 96 kHz samples, delayed to the downsampler's center tap
    float x1 = 0.f;            // ADAA: the last curve input
    float g1 = 1.f;            // ADAA: sqrt term of x1 (SOFT, TUBE)
    float y1 = 0.f, y2 = 0.f;  // lift
    float phase = 0.f;         // Decim
    float hold = 0.f;          // Decim
    float dc_x = 0.f;          // DC blocker
    float dc_y = 0.f;
    Lowpass tone;
};

static Channel _ch[2];
static float _gain_oct = 0.f; // Drive gain in octaves, smoothed
static float _tone_oct = 0.f; // Tone cutoff in octaves above 200 Hz, smoothed
static int32_t _last_mode = SOFT;
static bool _split = false; // the channels' states differ: a stereo input since they were last the same

// The channels' states are the same up to rounding (the History positions are equal anyway)
static bool same_state(const Channel &a, const Channel &b)
{
    const float *pa = reinterpret_cast<const float *>(&a);
    const float *pb = reinterpret_cast<const float *>(&b);
    for (size_t i = 0; i < sizeof(Channel) / sizeof(float); i++)
        if (fabsf(pa[i] - pb[i]) > 1e-6f)
            return false;
    return true;
}

// sin(x) for any x within the float range used here (|x| < 1000), error < 4e-6
static inline float sin_fast(float x)
{
    float t = x * 0.159154943f; // periods
    t -= floorf(t + 0.5f);      // -0.5 ... 0.5
    if (t > 0.25f)
        t = 0.5f - t;
    else if (t < -0.25f)
        t = -0.5f - t;
    const float a = t * 6.28318531f; // -pi/2 ... pi/2
    const float a2 = a * a;
    return a * (1.f - a2 * (1.f / 6.f) * (1.f - a2 * (1.f / 20.f) * (1.f - a2 * (1.f / 42.f) * (1.f - a2 * (1.f / 72.f)))));
}

static inline float sqrt_term(int mode, float x)
{
    if (mode == SOFT)
        return sqrtf(1.f + x * x);
    if (mode == TUBE)
        return sqrtf(1.f + (x < 0.f ? kTubeK2 : 1.f) * x * x);
    return 1.f;
}

// One 96 kHz sample through the curve, ADAA with the previous input x1
template <int mode>
static inline float curve(Channel &c, float x)
{
    float y;
    if (mode == SOFT)
    {
        // f = x / sqrt(1 + x^2), F = sqrt(1 + x^2): (F - F1) / (x - x1) = (x + x1) / (g + g1)
        const float g = sqrtf(1.f + x * x);
        y = (x + c.x1) / (g + c.g1);
        c.g1 = g;
    }
    else if (mode == TUBE)
    {
        // x >= 0 like SOFT, x < 0: f = x / sqrt(1 + k2 x^2), F = (sqrt(1 + k2 x^2) - 1) / k2.
        // Same sign: (x + x1) / (g + g1). Across zero, x - x1 is the distance to zero and back, never small:
        // F = x^2 / (g + 1) on both sides.
        const float g = sqrtf(1.f + (x < 0.f ? kTubeK2 : 1.f) * x * x);
        const float dx = x - c.x1;
        if ((x < 0.f) == (c.x1 < 0.f))
            y = (x + c.x1) / (g + c.g1);
        else if (fabsf(dx) > 1e-6f)
            y = (x * x / (g + 1.f) - c.x1 * c.x1 / (c.g1 + 1.f)) / dx;
        else
            y = 0.f;
        c.g1 = g;
    }
    else if (mode == HARD)
    {
        // f = clamp(x, -1, 1), F = x^2 / 2 inside, |x| - 1/2 outside
        const float ax = fabsf(x);
        const float ax1 = fabsf(c.x1);
        if (ax <= 1.f && ax1 <= 1.f)
            y = (x + c.x1) * 0.5f;
        else if (x > 1.f && c.x1 > 1.f)
            y = 1.f;
        else if (x < -1.f && c.x1 < -1.f)
            y = -1.f;
        else
        {
            // one of them outside: F is small near the knee, where x - x1 can be small
            const float dx = x - c.x1;
            if (fabsf(dx) > 1e-6f)
            {
                const float F = ax <= 1.f ? x * x * 0.5f : ax - 0.5f;
                const float F1 = ax1 <= 1.f ? c.x1 * c.x1 * 0.5f : ax1 - 0.5f;
                y = (F - F1) / dx;
            }
            else
                y = x > 0.f ? 1.f : -1.f;
        }
    }
    else // FOLD
    {
        // f = sin(x), F = -cos(x): (cos(x1) - cos(x)) / (x - x1) = sin(m) * sin(d) / d, m = (x + x1) / 2,
        // d = (x - x1) / 2
        const float m = (x + c.x1) * 0.5f;
        const float d = (x - c.x1) * 0.5f;
        const float sinc = fabsf(d) > 1e-3f ? sin_fast(d) / d : 1.f - d * d * (1.f / 6.f);
        y = sin_fast(m) * sinc;
    }
    c.x1 = x;
    return y;
}

// Input -> pre gain -> curve at 96 kHz -> post gain -> 48 kHz, and the input delayed like that
template <int mode>
static void render(Channel &c, const float *in, float *out, float *dry, const float *pre, const float *post)
{
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        c.in.push(in[i]);
        dry[i] = c.in[kDryDelay];

        // upsampler: the even sample is the input kTaps back, the odd one lies halfway to the next
        float odd = 0.f;
        for (int k = 0; k < kTaps; k++)
            odd += kHalfband[k] * (c.in[kTaps - 1 - k] + c.in[kTaps + k]);

        c.even.push(post[i] * curve<mode>(c, pre[i] * c.in[kTaps]));
        c.odd.push(post[i] * curve<mode>(c, pre[i] * 2.f * odd));

        // downsampler: center tap on the even sample, side taps on the odd ones
        float y = 0.5f * c.even[kTaps - 1];
        for (int k = 0; k < kTaps; k++)
            y += kHalfband[k] * (c.odd[kTaps - 1 - k] + c.odd[kTaps + k]);
        out[i] = (1.f + 2.f * kLift) * c.y1 - kLift * (y + c.y2);
        c.y2 = c.y1;
        c.y1 = y;
    }
}

void engine::setup()
{
    engine::addParam("D/W", &_mix);
    engine::addParam("Mode", &_mode, 0, LEN_OF(_mode_names) - 1, _mode_names);
    engine::addParam("Drive", &_drive);
    engine::addParam("Tone", &_tone);
    engine::addParam("Crush", &_crush);
    engine::addParam("Decim", &_decim);
}

void engine::process()
{
    float *inputs[2] = {engine::inputBuffer<0>(), engine::inputBuffer<1>()};
    float *outputs[2] = {engine::outputBuffer<0>(), engine::outputBuffer<1>()};

    int32_t mode = _mode;
    CONSTRAIN(mode, SOFT, FOLD);
    if (mode != _last_mode)
    {
        for (auto &c : _ch)
            c.g1 = sqrt_term(mode, c.x1);
        _last_mode = mode;
    }

    // Drive: -12 dB ... +40 dB (FOLD +24 dB, 5 folds of a full-scale input), smoothed over 2 ms. The ADAA averages
    // the curve between two samples: across a jump in gain it would get the new makeup gain on the old level
    const float max_db = mode == FOLD ? 24.f : 40.f;
    const float target_oct = (-12.f + (max_db + 12.f) * _drive) * 0.166096405f;
    float pre[FRAME_BUFFER_SIZE];
    float post[FRAME_BUFFER_SIZE];
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        _gain_oct += (target_oct - _gain_oct) * 0.0104f;
        const float gain = exp2f(_gain_oct);
        // below 0 dB the curve gets a quieter signal and its output is made up again, so the low end is clean
        pre[i] = gain * (1.f / kCeiling);
        post[i] = gain < 1.f ? kCeiling / gain : kCeiling;
    }

    // Crush: 16 bits at 0 ... 1 bit (-1, 0, 1) at 1. Decim: 48 kHz ... 375 Hz
    const float step = exp2f(15.f * _crush - 15.f);
    const float inv_step = 1.f / step;
    const float inc = exp2f(-7.f * _decim);

    // Tone: 2-pole lowpass 200 Hz ... 23.5 kHz, Q 0.5 (two real poles: no overshoot). At the top it lets everything
    // through (-0.02 dB at 15 kHz). Smoothed over 2 ms: big jumps of the cutoff from frame to frame make the filter
    // overshoot. tanf: the polynomial approximations (stmlib) get the cutoff wrong close to 24 kHz
    _tone_oct += (_tone * 6.87774f - _tone_oct) * 0.22f;
    const float tone_g = tanf(3.14159265f * 200.f * exp2f(_tone_oct) / SAMPLE_RATE);
    const float tone_r = 2.f; // 1 / Q
    const float tone_h = 1.f / (1.f + tone_r * tone_g + tone_g * tone_g);

    // A mono source (a mono engine on an insert, a single ADC) gives L == R to the sample: then the left channel is
    // computed and copied, at half the cost. After a stereo input, the right one is taken along again once its state
    // has come back to the left one's.
    bool same_input = true; // not memcmp: older firmwares don't resolve it
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        same_input &= inputs[0][i] == inputs[1][i];
    const int channels = same_input && !_split ? 1 : 2;

    for (int ch = 0; ch < channels; ch++)
    {
        Channel &c = _ch[ch];
        float *out = outputs[ch];
        float dry[FRAME_BUFFER_SIZE];

        switch (mode)
        {
        case SOFT:
            render<SOFT>(c, inputs[ch], out, dry, pre, post);
            break;
        case TUBE:
            render<TUBE>(c, inputs[ch], out, dry, pre, post);
            break;
        case HARD:
            render<HARD>(c, inputs[ch], out, dry, pre, post);
            break;
        default:
            render<FOLD>(c, inputs[ch], out, dry, pre, post);
            break;
        }

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            float y = out[i];

            if (_crush > 0.f)
                y = step * floorf(y * inv_step + 0.5f);

            c.phase += inc;
            if (c.phase >= 1.f)
            {
                c.phase -= 1.f;
                c.hold = y;
            }
            y = c.hold;

            y = c.tone.process(y, tone_g, tone_r, tone_h);

            // DC blocker (1.5 Hz): TUBE is asymmetric. Higher, it would tilt the clipped square waves past +-1
            c.dc_y = y - c.dc_x + 0.999804f * c.dc_y;
            c.dc_x = y;
            y = c.dc_y;

            out[i] = dry[i] + (y - dry[i]) * _mix;
        }
    }

    if (channels == 1)
    {
        _ch[1] = _ch[0];
        memcpy(outputs[1], outputs[0], sizeof(float) * FRAME_BUFFER_SIZE);
    }
    else if (same_input && same_state(_ch[0], _ch[1]))
        _split = false;
    else
        _split = true;
}
