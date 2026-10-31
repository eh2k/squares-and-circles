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

// Compressor: feed-forward, stereo-linked, peak detector, soft knee, in the log domain
// (Giannoulis, Massberg, Reiss: "Digital Dynamic Range Compressor Design - A Tutorial and Analysis", 2012).
//
// level = log2 max(|L|, |R|) -> gain computer (threshold, ratio, 6 dB knee) -> smoothing (attack while the gain
// reduction grows, release while it shrinks) -> gain = 2^(reduction + makeup) -> D/W with the input
//
// Levels are in log2 units (6.02 dB) and computed with polynomial log2/exp2: a sample costs ~50 instructions, no libm.

#include "../../squares-and-circles-api.h"
#include <math.h>

static float _mix = 1.f;
static int32_t _ratio = 3;    // 4:1
static float _thresh = -12.f; // dB
static float _makeup = 0.f;   // dB
static float _attack = 0.6667f;  // 0.1 ms * 1000^x: 10 ms
static float _release = 0.4346f; // 10 ms * 200^x: 100 ms

static const char *_ratio_names[] = {"1.5:1", "2:1", "3:1", "4:1", "6:1", "8:1", "12:1", "20:1", "LIMIT"};
static const float _inv_ratios[] = {1 / 1.5f, 1 / 2.f, 1 / 3.f, 1 / 4.f, 1 / 6.f, 1 / 8.f, 1 / 12.f, 1 / 20.f, 0.f};

static char _attack_name[16] = "Attack\n10ms"; // written in draw(), no '%': a plain two-line label
static char _release_name[16] = "Release\n100ms";

static constexpr float kKnee = 6.f / 6.0206f; // 6 dB, in log2 units
static constexpr float kDbToLog2 = 1.f / 6.0206f;

static float _gr = 0.f;      // smoothed gain reduction, log2 units (<= 0)
static float _gr_peak = 0.f; // the largest reduction since the last draw(), dB (>= 0)

// log2(x) for normal x > 0, error < 1.1e-6: the mantissa m around 1 (0.71 ... 1.41), then
// log2(m) = 2 / ln2 * atanh(s), s = (m - 1) / (m + 1), as a series to s^7. log2(1) is exactly 0.
static inline float log2_fast(float x)
{
    union
    {
        float f;
        uint32_t i;
    } u = {x};
    float e = (float)((int32_t)(u.i >> 23) - 127);
    u.i = (u.i & 0x007FFFFF) | 0x3F800000; // 1 ... 2
    float m = u.f;
    if (m > 1.41421356f)
    {
        m *= 0.5f;
        e += 1.f;
    }
    const float s = (m - 1.f) / (m + 1.f);
    const float s2 = s * s;
    return e + s * (2.88539008f + s2 * (0.961796694f + s2 * (0.577078016f + s2 * 0.412198583f)));
}

// 2^x for x >= -126, relative error < 2e-6: the nearest integer as the exponent, 2^f for |f| <= 0.5 as
// e^(f ln2) to the 6th power. 2^0 is exactly 1.
static inline float exp2_fast(float x)
{
    if (x < -126.f)
        x = -126.f;
    const float fi = floorf(x + 0.5f);
    const float t = (x - fi) * 0.693147181f;
    union
    {
        float f;
        uint32_t i;
    } u;
    u.i = (uint32_t)((int32_t)fi + 127) << 23;
    return u.f * (1.f + t * (1.f + t * (0.5f + t * (1.f / 6 + t * (1.f / 24 + t * (1.f / 120 + t * (1.f / 720)))))));
}

static inline float attack_ms(float a)
{
    return 0.1f * exp2f(a * 9.96578428f); // 0.1 ms ... 100 ms
}

static inline float release_ms(float r)
{
    return 10.f * exp2f(r * 7.64385619f); // 10 ms ... 2 s
}

// the static curve: gain reduction (<= 0) for a level, both in log2 units
static inline float reduction(float level, float thresh, float slope)
{
    const float over = level - thresh;
    if (2.f * over <= -kKnee)
        return 0.f;
    if (2.f * over < kKnee)
    {
        const float k = over + 0.5f * kKnee;
        return -slope * k * k * (0.5f / kKnee);
    }
    return -slope * over;
}

void engine::setup()
{
    engine::addParam("D/W", &_mix);
    engine::addParam("Ratio", &_ratio, 0, LEN_OF(_ratio_names) - 1, _ratio_names);
    engine::addParam("Thresh\n%.0fdB", &_thresh, -40.f, 0.f);
    engine::addParam("Makeup\n%+.0fdB", &_makeup, 0.f, 24.f);
    engine::addParam(_attack_name, &_attack);
    engine::addParam(_release_name, &_release);
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto inputR = engine::inputBuffer<1>();
    auto outputL = engine::outputBuffer<0>();
    auto outputR = engine::outputBuffer<1>();

    int32_t ratio = _ratio;
    CONSTRAIN(ratio, 0, (int32_t)LEN_OF(_inv_ratios) - 1);
    const float slope = 1.f - _inv_ratios[ratio]; // LIMIT: 1
    const float thresh = _thresh * kDbToLog2;
    const float makeup = _makeup * kDbToLog2;
    const float a_coef = expf(-1000.f / (attack_ms(_attack) * SAMPLE_RATE));
    const float r_coef = expf(-1000.f / (release_ms(_release) * SAMPLE_RATE));

    float gr_max = 0.f;
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        const float peak = fmaxf(fabsf(inputL[i]), fabsf(inputR[i]));
        const float target = reduction(log2_fast(peak + 1e-9f), thresh, slope);

        // attack while the reduction grows, release while it shrinks
        _gr = target + (target < _gr ? a_coef : r_coef) * (_gr - target);
        gr_max = fminf(gr_max, _gr);

        const float wet = exp2_fast(_gr + makeup);
        const float gain = 1.f + _mix * (wet - 1.f);
        outputL[i] = inputL[i] * gain;
        outputR[i] = inputR[i] * gain;
    }

    _gr_peak = fmaxf(_gr_peak, -gr_max * 6.0206f);
}

void engine::draw()
{
    // the labels of the exponential knobs
    const float a = attack_ms(engine::getParamValueModulated_f32(&_attack));
    const float r = release_ms(engine::getParamValueModulated_f32(&_release));
    snprintf(_attack_name, sizeof(_attack_name), a < 10.f ? "Attack\n%.1fms" : "Attack\n%.0fms", a);
    if (r < 1000.f)
        snprintf(_release_name, sizeof(_release_name), "Release\n%.0fms", r);
    else
        snprintf(_release_name, sizeof(_release_name), "Release\n%.1fs", r / 1000.f);

    // gain reduction meter at the right edge, beside the knobs (whose labels end at x ~118): growing down from the
    // top, 24 dB over the 46 pixels inside
    constexpr int x0 = 121, y0 = 10, w = 6, h = 48;
    gfx::drawRect(x0, y0, w, h);
    int n = (int)(_gr_peak * (h - 2) / 24.f + 0.5f);
    if (n > h - 2)
        n = h - 2;
    if (n > 0)
        gfx::fillRect(x0 + 1, y0 + 1, w - 2, n);
    _gr_peak = 0.f;
}
