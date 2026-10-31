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

// Ducker: a trigger pulls the input down and lets it come back, the "pumping" of sidechain compression.
// Patch the kick's trigger (or play the track's MIDI channel) and insert the pad, bass or reverb.
//
// gain = 1 - Depth * duck, duck: attack 0 -> 1 (linear), hold, release 1 -> 0 along the Shape curve.
// A trigger during the release starts the attack from where the duck is.

#include "../../squares-and-circles-api.h"
#include <math.h>

static float _depth = 0.8f;
static float _release = 0.5f; // 10 ms ... 2 s
static float _attack = 0.37f; // 0.1 ms ... 50 ms
static float _shape = 0.75f;  // 0: fast return ... 0.5: linear ... 1: stays down, then returns fast
static float _hold = 0.2f;    // 0 ... 500 ms

enum
{
    IDLE,
    ATTACK,
    HOLD,
    RELEASE,
};

static int32_t _stage = IDLE;
static float _duck = 0.f;        // 0: no ducking ... 1: Depth
static float _phase = 0.f;       // release 0 ... 1
static int32_t _hold_left = 0;   // samples
static uint32_t _since = 0;      // samples since the last trigger, for the display
static float _depth_last = 0.8f; // Depth of the last frame, ramped from

static inline float attack_samples(float a)
{
    return 0.0001f * SAMPLE_RATE * exp2f(a * 8.96578428f); // 0.1 ms * 500^a
}

static inline float hold_samples(float h)
{
    return 0.5f * SAMPLE_RATE * h * h;
}

static inline float release_samples(float r)
{
    return 0.01f * SAMPLE_RATE * exp2f(r * 7.64385619f); // 10 ms * 200^r
}

// The release's way back, 0 -> 1: p / (1 + b (1 - p)). It starts with the slope 1 / (1 + b) and ends with 1 + b.
// Shape 0.5 is linear (b = 0); above, it stays down longer and comes back fast (the classic pump); below, it comes
// back fast first. 1 + b goes from 1/8 to 8, so even the shortest release doesn't click. One division a sample
// (a powf curve took ~350 cycles a sample on the DSM).
static inline float shape_b(float shape)
{
    return exp2f(6.f * shape - 3.f) - 1.f;
}

static inline float recovery(float p, float b)
{
    return p / (1.f + b * (1.f - p));
}

void engine::setup()
{
    engine::addParam("Depth", &_depth);
    engine::addParam("Release", &_release);
    engine::addParam("Attack", &_attack);
    engine::addParam("Shape", &_shape);
    engine::addParam("Hold", &_hold);
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto inputR = engine::inputBuffer<1>();
    auto outputL = engine::outputBuffer<0>();
    auto outputR = engine::outputBuffer<1>();

    if (engine::trig())
    {
        _stage = ATTACK;
        _since = 0;
    }

    const float attack_inc = 1.f / attack_samples(_attack);
    const int32_t hold = hold_samples(_hold);
    const float release_inc = 1.f / release_samples(_release);
    const float b = shape_b(_shape);
    const float ddepth = (_depth - _depth_last) * (1.f / FRAME_BUFFER_SIZE); // the knob moves without steps
    float depth = _depth_last;

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        switch (_stage)
        {
        case ATTACK:
            _duck += attack_inc;
            if (_duck >= 1.f)
            {
                _duck = 1.f;
                _stage = HOLD;
                _hold_left = hold;
            }
            break;
        case HOLD:
            if (--_hold_left <= 0)
            {
                _stage = RELEASE;
                _phase = 0.f;
            }
            break;
        case RELEASE:
            _phase += release_inc;
            if (_phase >= 1.f)
            {
                _duck = 0.f;
                _stage = IDLE;
            }
            else
                _duck = 1.f - recovery(_phase, b);
            break;
        }

        depth += ddepth;
        const float gain = 1.f - depth * _duck;
        outputL[i] = inputL[i] * gain;
        outputR[i] = inputR[i] * gain;
    }

    _depth_last = _depth;
    if (_since < UINT32_MAX - FRAME_BUFFER_SIZE)
        _since += FRAME_BUFFER_SIZE;
}

void engine::draw()
{
    // In the free slot at the bottom right: the gain over the time from the trigger to the end of the release
    // (top: unchanged, bottom: silent), and where the duck is now
    constexpr int x0 = 70, y0 = 44, w = 56, h = 13;

    const float depth = engine::getParamValueModulated_f32(&_depth);
    const float b = shape_b(engine::getParamValueModulated_f32(&_shape));
    const float a = attack_samples(engine::getParamValueModulated_f32(&_attack));
    const float hd = hold_samples(engine::getParamValueModulated_f32(&_hold));
    const float r = release_samples(engine::getParamValueModulated_f32(&_release));
    const float total = a + hd + r;

    int last_y = y0;
    for (int x = 0; x < w; x++)
    {
        const float t = total * x / (w - 1);
        float duck;
        if (t < a)
            duck = t / a;
        else if (t < a + hd)
            duck = 1.f;
        else
            duck = 1.f - recovery((t - a - hd) / r, b);

        const int y = y0 + (int)(h * depth * duck + 0.5f);
        gfx::drawLine(x0 + x - (x > 0), last_y, x0 + x, y);
        last_y = y;
    }

    if (_stage != IDLE && _since < total)
    {
        const int x = x0 + (int)((w - 1) * _since / total);
        gfx::drawLine(x, y0, x, y0 + h);
    }
}
