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

// Tuner: note, cents and frequency of the input (an oscillator at the AUX input, or an engine on an insert). The
// input passes through unchanged. The left encoder sets the reference A4 (415 ... 466 Hz).
//
// Pitch: McLeod's normalised square difference function, NSDF(t) = 2 r(t) / m(t) ("A smarter way to find pitch",
// 2005). Coarse over the lags for 8 kHz ... 20 Hz: the short ones (above 857 Hz) at 48 kHz, the long ones at 12 kHz
// (after a 4th-order lowpass at 4 kHz); the first maximum within 85 % of the highest (no octave errors). Then at
// 48 kHz around that lag, and once more around a multiple of the period (up to 32 periods, up to 3600 samples): a lag
// n periods long measures the period n times more exactly. One analysis is done in pieces of ~2000 multiplications a
// frame over ~95 frames, a new one every 100 ms. The windows are read in place (no copies, energies slid from lag to
// lag): on the DSM the app's data is far from the core, and every byte it streams counts.

#include "../../squares-and-circles-api.h"
#include <math.h>

static int32_t _a4 = 25; // 415 + 25 = 440 Hz

static constexpr int kRing = 8192;   // 48 kHz
static constexpr int kCRing = 2048;  // 12 kHz
static constexpr int kW = 240;       // coarse window, 20 ms at 12 kHz (the NSDF needs no whole period)
static constexpr int kMinLag = 14;   // 857 Hz at 12 kHz: shorter lags at 48 kHz
static constexpr int kMaxLag = 600;  // 20 Hz
static constexpr int kWH = 512;      // the short lags' window, 11 ms at 48 kHz
static constexpr int kMinLagH = 6;   // 8 kHz
static constexpr int kMaxLagH = 4 * kMinLag;
static constexpr int kW2 = 1024;     // fine window, 21 ms at 48 kHz
static constexpr int kMaxLag2 = 3600;
static constexpr int kFine = kW2 + kMaxLag2 + 8; // the 48 kHz samples one analysis reads
// the work a frame, ~2000 multiplications: 10 coarse lags, 4 short ones or 2 fine ones
static constexpr int kLagsPerFrame = 10;
static constexpr int kLagsPerFrameH = 4;
static constexpr int kLagsPerFrameF = 2;

// the rings hold every sample twice (at i and i + size): any window of them is one piece of memory, read in place.
// An analysis is done before the writing comes round to its windows (~100 frames: 2400 of the 8192 - 4632, 600 of
// the 2048 - 841 samples)
static float _ring[2 * kRing];
static int32_t _rw = 0;
static float _cring[2 * kCRing];
static int32_t _cw = 0;
static int32_t _dec = 0;
static float _dc_x = 0.f, _dc_y = 0.f;
// the windows of the running analysis, set when it starts (an initialised pointer into .bss can't be relocated)
static const float *_fa = nullptr; // fine and short lags, 48 kHz
static const float *_ca = nullptr; // coarse, 12 kHz

struct Biquad
{
    float b0, b1, b2, a1, a2;
    float z1 = 0.f, z2 = 0.f;
    inline float process(float x)
    {
        const float y = b0 * x + z1;
        z1 = b1 * x - a1 * y + z2;
        z2 = b2 * x - a2 * y;
        return y;
    }
};
static Biquad _lp[2];

enum
{
    WAIT,
    COARSE_H, // short lags, 48 kHz
    COARSE,   // long lags, 12 kHz
    CURVE,    // both as one curve
    PICK,     // its peak
    FINE,
};
static int32_t _stage = WAIT;
static int32_t _wait = 0;
static float _nsdf[kMaxLag + 1];
static float _nsdf_h[kMaxLagH + 1];
// both as one curve over the lag in 48 kHz samples, for the peak picking
static constexpr int kCurve = (kMaxLagH - kMinLagH) + (kMaxLag - kMinLag + 1);
static float _curve_lag[kCurve];
static float _curve_val[kCurve];
static int32_t _lag = 0;
static float _e0 = 0.f; // energy of the window
static float _e1 = 0.f; // energy of the lagged window, slid on from lag to lag

// the fine search: up to two rounds of lags around a guess, one lag a frame
static int32_t _round = 0;
static int32_t _fine_lag[16];
static float _fine_val[16];
static int32_t _fine_n = 0;
static int32_t _fine_i = 0;
static int32_t _multiple = 1;
static float _tau = 0.f; // the period, 48 kHz samples
static bool _fine_e0 = false;    // the fine window's energy is to be computed (round 0; round 1 has the same window)
static int32_t _fine_lags = 0;   // of the analysis
static int32_t _fine_frames = 0; // the frames they took

// what draw() shows
static float _freq = 0.f; // smoothed, 0: no pitch
static int32_t _hold = 0; // analyses without a pitch

// sum of a[j] * b[j] for j < n (a multiple of 4), in this order: unrolled, the loop costs less
static inline float dot(const float *a, const float *b, int32_t n)
{
    float r = 0.f;
    for (int32_t j = 0; j < n; j += 4)
    {
        r += a[j] * b[j];
        r += a[j + 1] * b[j + 1];
        r += a[j + 2] * b[j + 2];
        r += a[j + 3] * b[j + 3];
    }
    return r;
}

// the vertex of the parabola through (-1, a), (0, b), (1, c)
static inline float parabola(float a, float b, float c)
{
    const float d = a - 2.f * b + c;
    return d < 0.f ? 0.5f * (a - c) / d : 0.f;
}

static void fine_energy()
{
    _e0 = dot(_fa, _fa, kW2); // the fine window's energy
}

static __attribute__((noinline)) void fine_round(int32_t round, float center, int32_t half)
{
    _round = round;
    if (round == 0)
        _fine_e0 = true; // the first step of FINE
    _fine_n = 0;
    _fine_i = 0;
    const int32_t c = (int32_t)(center + 0.5f);
    for (int32_t l = c - half; l <= c + half; l++)
        if (l > 1 && l <= kMaxLag2 + 8 - 1)
            _fine_lag[_fine_n++] = l;
    _stage = _fine_n > 2 ? FINE : WAIT;
}

static void no_pitch()
{
    _stage = WAIT;
    if (++_hold > 8) // ~0.5 s
        _freq = 0.f;
}

static void coarse_curve()
{
    int32_t n = 0;
    for (int32_t t = kMinLagH; t < kMaxLagH; t++, n++)
        _curve_lag[n] = t, _curve_val[n] = _nsdf_h[t];
    for (int32_t t = kMinLag; t <= kMaxLag; t++, n++)
        _curve_lag[n] = 4 * t, _curve_val[n] = _nsdf[t];
}

static void coarse_done()
{
    const int32_t n = kCurve;

    // the height of a maximum: the vertex of the parabola through it and its neighbours (a sharp peak falls
    // between two lags), where they are evenly spaced
    auto height = [&](int32_t i) {
        if (i == 0 || i == n - 1 || _curve_lag[i] - _curve_lag[i - 1] != _curve_lag[i + 1] - _curve_lag[i])
            return _curve_val[i];
        const float a = _curve_val[i - 1], b = _curve_val[i], c = _curve_val[i + 1];
        return b - 0.25f * (a - c) * parabola(a, b, c);
    };

    // key maxima: the highest in each positive stretch after the first zero crossing; the first one within 85 % of
    // the highest of them
    int32_t keys[64];
    int32_t nkeys = 0, best = -1;
    bool negative = false;
    for (int32_t i = 0; i < n && nkeys < 64; i++)
    {
        if (_curve_val[i] < 0.f)
        {
            if (best >= 0)
                keys[nkeys++] = best;
            negative = true;
            best = -1;
        }
        else if (negative && (best < 0 || _curve_val[i] > _curve_val[best]))
            best = i;
    }
    if (best >= 0 && nkeys < 64)
        keys[nkeys++] = best;

    float highest = 0.f;
    for (int32_t k = 0; k < nkeys; k++)
        if (height(keys[k]) > highest)
            highest = height(keys[k]);
    int32_t pick = -1;
    for (int32_t k = 0; k < nkeys && pick < 0; k++)
        if (height(keys[k]) >= 0.85f * highest)
            pick = keys[k];

    if (pick < 0 || height(pick) < 0.6f || _e0 < kW * 1e-6f) // no pitch, or quieter than -60 dBFS
        return no_pitch();

    float tc = _curve_lag[pick];
    const float step = pick > 0 ? _curve_lag[pick] - _curve_lag[pick - 1] : 1.f;
    if (pick > 0 && pick < n - 1 && _curve_lag[pick + 1] - _curve_lag[pick] == step)
        tc += step * parabola(_curve_val[pick - 1], _curve_val[pick], _curve_val[pick + 1]);
    fine_round(0, tc, step > 1.f ? 6 : 2);
}

// NSDF of the fine window at one lag; the lags of a round follow each other: the lagged window's energy slides
static __attribute__((noinline)) float nsdf_fine(int32_t lag, bool first)
{
    const float *a = _fa;
    const float *b = _fa + lag;
    if (first)
        _e1 = dot(b, b, kW2);
    else
        _e1 += b[kW2 - 1] * b[kW2 - 1] - b[-1] * b[-1];
    const float r = dot(a, b, kW2);
    const float m = _e0 + _e1;
    return m > 0.f ? 2.f * r / m : 0.f;
}

static __attribute__((noinline)) void fine_done()
{
    int32_t k = 0;
    for (int32_t i = 1; i < _fine_n; i++)
        if (_fine_val[i] > _fine_val[k])
            k = i;
    if (k == 0 || k == _fine_n - 1 || _fine_val[k] < 0.5f) // not a maximum inside: no pitch this time
        return no_pitch();
    const float peak = _fine_lag[k] + parabola(_fine_val[k - 1], _fine_val[k], _fine_val[k + 1]);

    if (_round == 0)
    {
        _tau = peak;
        _multiple = (int32_t)(kMaxLag2 / _tau);
        if (_multiple > 32) // its error grows with the multiple: 32 periods stay within the 4 lags around it
            _multiple = 32;
        if (_multiple >= 2)
            return fine_round(1, _multiple * _tau, 4);
    }
    else
        _tau = peak / _multiple;

    // smoothed, unless it moved by more than 10 cents
    const float f = SAMPLE_RATE / _tau;
    if (_freq > 0.f && fabsf(logf(f / _freq)) < 0.00578f)
        _freq += 0.5f * (f - _freq);
    else
        _freq = f;
    _hold = 0;
    _stage = WAIT;
}

void engine::setup()
{
    engine::addParam(".A4", &_a4, 0, 51); // not drawn: draw() shows it in the header

    // 4th-order Butterworth lowpass at 4 kHz before the 12 kHz ring: two biquads, Q 0.5412 and 1.3066
    const float w0 = 2.f * 3.14159265f * 4000.f / SAMPLE_RATE;
    const float q[2] = {0.5411961f, 1.3065630f};
    for (int s = 0; s < 2; s++)
    {
        const float alpha = sinf(w0) / (2.f * q[s]);
        const float cw = cosf(w0);
        const float a0 = 1.f + alpha;
        _lp[s].b0 = (1.f - cw) * 0.5f / a0;
        _lp[s].b1 = (1.f - cw) / a0;
        _lp[s].b2 = _lp[s].b0;
        _lp[s].a1 = -2.f * cw / a0;
        _lp[s].a2 = (1.f - alpha) / a0;
    }
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto inputR = engine::inputBuffer<1>();
    auto outputL = engine::outputBuffer<0>();
    auto outputR = engine::outputBuffer<1>();

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        outputL[i] = inputL[i];
        outputR[i] = inputR[i];

        // DC blocker (10 Hz), into the 48 kHz ring; lowpass and every 4th sample into the 12 kHz ring
        const float x = 0.5f * (inputL[i] + inputR[i]);
        _dc_y = x - _dc_x + 0.9987f * _dc_y;
        _dc_x = x;
        const int32_t w = _rw++ & (kRing - 1);
        _ring[w] = _ring[w + kRing] = _dc_y;
        const float lp = _lp[1].process(_lp[0].process(_dc_y));
        if (++_dec == 4)
        {
            _dec = 0;
            const int32_t c = _cw++ & (kCRing - 1);
            _cring[c] = _cring[c + kCRing] = lp;
        }
    }

    switch (_stage)
    {
    case WAIT:
        if (--_wait <= 0)
        {
            _wait = 105; // with the ~95 frames of an analysis: a new one every 100 ms
            _fa = &_ring[(_rw - kFine) & (kRing - 1)];
            _ca = &_cring[(_cw - (kW + kMaxLag + 1)) & (kCRing - 1)];
            _e0 = _e1 = 0.f; // the short lags' window, and the lagged one at the first lag
            for (int32_t j = 0; j < kWH; j++)
            {
                _e0 += _fa[j] * _fa[j];
                _e1 += _fa[j + kMinLagH] * _fa[j + kMinLagH];
            }
            _lag = kMinLagH;
            _fine_lags = _fine_frames = 0;
            _stage = COARSE_H;
        }
        break;
    case COARSE_H:
        for (int n = 0; n < kLagsPerFrameH && _lag < kMaxLagH; n++, _lag++)
        {
            const float *b = _fa + _lag;
            const float r = dot(_fa, b, kWH);
            const float m = _e0 + _e1;
            _nsdf_h[_lag] = m > 0.f ? 2.f * r / m : 0.f;
            _e1 += b[kWH] * b[kWH] - b[0] * b[0];
        }
        if (_lag >= kMaxLagH)
        {
            _e0 = _e1 = 0.f;
            for (int32_t j = 0; j < kW; j++)
            {
                _e0 += _ca[j] * _ca[j];
                _e1 += _ca[j + kMinLag] * _ca[j + kMinLag];
            }
            _lag = kMinLag;
            _stage = COARSE;
        }
        break;
    case COARSE:
        for (int n = 0; n < kLagsPerFrame && _lag <= kMaxLag; n++, _lag++)
        {
            const float *b = _ca + _lag;
            const float r = dot(_ca, b, kW);
            const float m = _e0 + _e1;
            _nsdf[_lag] = m > 0.f ? 2.f * r / m : 0.f;
            _e1 += b[kW] * b[kW] - b[0] * b[0]; // the lagged window one sample on
        }
        if (_lag > kMaxLag)
            _stage = CURVE;
        break;
    case CURVE:
        coarse_curve();
        _wait--; // a frame more than with the curve in PICK: the next analysis on the same samples
        _stage = PICK;
        break;
    case PICK:
        coarse_done();
        break;
    case FINE:
    {
        // steps of ~1000 multiplications, two a frame: the fine window's energy, a lag (the first of a round, with
        // its lagged window's energy: two)
        int32_t budget = kLagsPerFrameF;
        while (_stage == FINE)
        {
            if (_fine_e0)
            {
                fine_energy();
                _fine_e0 = false;
                budget -= 1;
                continue;
            }
            const int32_t cost = _fine_i == 0 ? 2 : 1;
            if (cost > budget)
                break;
            budget -= cost;
            _fine_val[_fine_i] = nsdf_fine(_fine_lag[_fine_i], _fine_i == 0);
            _fine_lags++;
            if (++_fine_i == _fine_n)
                fine_done();
        }
        _fine_frames++;
        // the next analysis starts when it did with two lags in every frame: on the same samples
        if (_stage != FINE)
            _wait -= _fine_frames - (_fine_lags + kLagsPerFrameF - 1) / kLagsPerFrameF;
        break;
    }
    }
}

// 7-segment digits and note names: bits a b c d e f g (top, top right, bottom right, bottom, bottom left, top left,
// middle)
static void seg7(int x, int y, int w, int h, int t, uint8_t mask)
{
    const int h2 = h / 2;
    if (mask & 0x01)
        gfx::fillRect(x + t, y, w - 2 * t, t);
    if (mask & 0x02)
        gfx::fillRect(x + w - t, y + t, t, h2 - t);
    if (mask & 0x04)
        gfx::fillRect(x + w - t, y + h2 + (t + 1) / 2, t, h - h2 - t - (t + 1) / 2);
    if (mask & 0x08)
        gfx::fillRect(x + t, y + h - t, w - 2 * t, t);
    if (mask & 0x10)
        gfx::fillRect(x, y + h2 + (t + 1) / 2, t, h - h2 - t - (t + 1) / 2);
    if (mask & 0x20)
        gfx::fillRect(x, y + t, t, h2 - t);
    if (mask & 0x40)
        gfx::fillRect(x + t, y + h2 - t / 2, w - 2 * t, t);
}

static const uint8_t kDigits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
//                                  C     C#    D     D#    E     F     F#    G     G#    A     A#    B
static const uint8_t kNotes[] = {0x39, 0x39, 0x5E, 0x5E, 0x79, 0x71, 0x71, 0x3D, 0x3D, 0x77, 0x77, 0x7C};
static const bool kSharp[] = {false, true, false, true, false, false, true, false, true, false, true, false};

void engine::draw()
{
    const int32_t a4 = 415 + engine::getParamValueModulated_i32(&_a4);
    char tmp[32];
    // the reference at the bottom left: below the scale, beside the in-tune box, above the track bar
    snprintf(tmp, sizeof(tmp), "A4 %dHz", (int)a4);
    gfx::drawString(4, 53, tmp, 0);

    // the cents scale: -50 ... +50 over x 4 ... 124
    constexpr int y0 = 42, x_mid = 64;
    gfx::drawLine(4, y0 + 8, 124, y0 + 8);
    for (int c = -50; c <= 50; c += 10)
    {
        const int x = x_mid + c * 6 / 5;
        const int l = c == 0 ? 7 : (c % 50 == 0 || c % 25 == 0 ? 4 : 2);
        gfx::drawLine(x, y0 + 8 - l, x, y0 + 8);
    }

    const float f = _freq;
    if (f <= 0.f)
    {
        seg7(4, 12, 14, 24, 3, 0x40); // "-"
        seg7(22, 12, 14, 24, 3, 0x40);
        return;
    }

    const float note = 69.f + 12.f * log2f(f / a4);
    const int32_t n = (int32_t)floorf(note + 0.5f);
    const float cents = (note - n) * 100.f;
    const int32_t name = ((n % 12) + 12) % 12;
    const int32_t octave = n / 12 - 1;

    seg7(4, 12, 14, 24, 3, kNotes[name]);
    if (kSharp[name])
        gfx::drawString(20, 11, "#", 1);
    if (octave >= 0 && octave <= 9)
        seg7(21, 23, 9, 13, 2, kDigits[octave]);

    snprintf(tmp, sizeof(tmp), "%+.1f", cents);
    gfx::drawString(44, 14, tmp, 1);
    gfx::drawString(44 + 6 * (int)strlen(tmp) + 2, 17, "cent", 0);
    snprintf(tmp, sizeof(tmp), f < 1000.f ? "%.2f Hz" : "%.1f Hz", f);
    gfx::drawString(44, 28, tmp, 0);

    // the needle; in tune (within 2 cents) the middle is a filled box
    const int x = x_mid + (int)(cents * 6.f / 5.f + (cents < 0.f ? -0.5f : 0.5f));
    gfx::fillRect(x - 1, y0 - 2, 3, 13);
    if (fabsf(cents) < 2.f)
        gfx::invertRect(x_mid - 6, y0 + 10, 13, 5);
}
