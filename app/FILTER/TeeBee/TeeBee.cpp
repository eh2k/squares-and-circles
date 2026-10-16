// Copyright (C)2025 - E.Heidt
//
// Author: eh2k@gmx.de
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

// Adapted from: rosic_Open303.cpp/rosic_Open303.h f
// See also: https://www.timstinchcombe.co.uk/index.php?pge=diode2

#include "../../squares-and-circles-api.h"
#undef SAMPLE_RATE

#define protected public // OnePole::init() takes the coefficients
#include "open303/src/rosic_OnePoleFilter.cpp"
#undef protected

using namespace rosic;

// Float, not double (the original): it sounds the same (QEMU, 91..103 dB SNR, 1 LSB of 16 bits) and runs 2x faster
// on the M7. Except the envelope (in double, TB303_control) and the 7.5 Hz notch (as an SVF, Notch).
static_assert(sizeof(real_t) == sizeof(float), "real_t is float");

// 2^x for the cutoff, without the library's expf() per sample: 2^round(x) in the exponent bits, 2^f for
// f = -0.5 ... 0.5 by its Taylor series up to f^6 (relative error 1.2e-7, as float's precision)
static inline float exp2_(float x)
{
    float n = __builtin_roundf(x);
    float f = (x - n) * 0.69314718f; // ln 2
    float p = 1.0f + f * (1.0f + f * (1.0f / 2 + f * (1.0f / 6 + f * (1.0f / 24 + f * (1.0f / 120 + f * (1.0f / 720))))));
    union
    {
        float f;
        int32_t i;
    } u = {p};
    u.i += (int32_t)n << 23;
    return u.f;
}

// The envelope and the cutoff do not depend on the audio: computed once for both channels, with the ladder's
// coefficients (rosic::TeeBeeFilter in TB_303 mode, calculateCoefficientsApprox4() without the other modes' part).
struct TB303_control
{
    // rosic::DecayEnvelope and LeakyIntegrator rc2 in double: in float the envelope's y *= c drifts. rc1 has the
    // time constant 0, its output is its input.
    double env_y = 1.0, env_c = 1.0, env_yInit = 1.0;
    double rc2_y1 = 0.0, rc2_coeff = 0.0;

    real_t envScaler = 0;
    real_t envOffset = 0;

    // TeeBeeFilter's parameters and coefficients
    real_t cutoff = 1000.0;
    real_t resonanceSkewed = 0.0;
    real_t twoPiOverSampleRate = 2.0 * PI / (real_t)SAMPLE_RATE; // TeeBeeFilter::setSampleRate()
    real_t b0 = 0, k = 0, g = 1.0;

    // the values the exp() of the last frame came from
    float last_decay = -1, last_q = -1, last_cutoff = -1;
    float nominal_cutoff = 0;

    void init()
    {
        setDecayTimeConstant(200.0);
        rc2_coeff = exp(-1.0 / ((double)SAMPLE_RATE * 0.001 * 30.0)); // LeakyIntegrator::calculateCoefficient()
        calculateCoefficients();
    }

    void setDecayTimeConstant(double tau) // DecayEnvelope::setDecayTimeConstant(), calculateCoefficient()
    {
        env_c = exp(-1.0 / (0.001 * tau * (double)SAMPLE_RATE));
        env_yInit = 1.0 / env_c;
    }

    void calculateEnvModScalerAndOffset(real_t cutoff, real_t envMod) // Open303::calculateEnvModScalerAndOffset()
    {
        // define some constants that arise from the measurements:
        const real_t oF = 0.048292930943553;   // factor in line equation for offset
        const real_t oC = 0.294391201442418;   // constant in line equation for offset
        const real_t sLoF = 3.773996325111173; // factor in line eq. for scaler at low cutoff
        const real_t sLoC = 0.736965594166206; // constant in line eq. for scaler at low cutoff
        const real_t sHiF = 4.194548788411135; // factor in line eq. for scaler at high cutoff
        const real_t sHiC = 0.864344900642434; // constant in line eq. for scaler at high cutoff

        // do the calculation of the scaler and offset:
        real_t e = envMod; // linToLin(envMod, 0.0, 100.0, 0.0, 1.0);
        real_t c = cutoff; // expToLin(cutoff, c0, c1, 0.0, 1.0);
        real_t sLo = sLoF * e + sLoC;
        real_t sHi = sHiF * e + sHiC;
        envScaler = (1 - c) * sLo + c * sHi;
        envOffset = oF * c + oC;
    }

    void calculateCoefficients() // TeeBeeFilter::calculateCoefficientsApprox4(), TB_303
    {
        real_t wc = twoPiOverSampleRate * cutoff;
        real_t r = resonanceSkewed;
        real_t fx = wc * ONE_OVER_SQRT2 / (2 * PI);
        b0 = (0.00045522346 + 6.1922189 * fx) / (1.0 + 12.358354 * fx + 4.4156345 * (fx * fx));
        k = fx * (fx * (fx * (fx * (fx * (fx + 7198.6997) - 5837.7917) - 476.47308) + 614.95611) + 213.87126) +
            16.998792;
        g = k * 0.058823529411764705882352941176471; // 17 reciprocal
        g = (g - 1.0) * r + 1.0;                     // r is 0 to 1.0
        g = (g * (1.0 + r));
        k = k * r; // k is ready now
    }

    void setCutoff(real_t newCutoff) // TeeBeeFilter::setCutoff()
    {
        if (newCutoff != cutoff)
        {
            if (newCutoff < 200.0) // an absolute floor for the cutoff frequency - tweakable
                cutoff = 200.0;
            else if (newCutoff > 20000.0)
                cutoff = 20000.0;
            else
                cutoff = newCutoff;

            calculateCoefficients();
        }
    }

    // once per frame
    void update(bool trig, float _cutoff, float _q, float _envMod, float _decay)
    {
        if (_decay != last_decay)
        {
            last_decay = _decay;
            setDecayTimeConstant(rosic::linToExp(_decay, 0.0, 1.0, 200.0, 2000.0));
        }

        if (trig)
            env_y = env_yInit;

        calculateEnvModScalerAndOffset(_cutoff, _envMod);

        if (_cutoff != last_cutoff)
        {
            last_cutoff = _cutoff;
            nominal_cutoff = rosic::linToExp(_cutoff, 0.0, 1.0, 313.0, 2394.0);
        }

        if (_q != last_q) // TeeBeeFilter::setResonance(q)
        {
            last_q = _q;
            float q = 1.f + _q * 99.f;
            real_t resonanceRaw = 0.01 * (real_t)q;
            resonanceSkewed = (1.0 - expf(-3.0 * resonanceRaw)) / (1.0 - expf(-3.0));
            calculateCoefficients();
        }
    }

    // once per sample
    inline void next(float _accent)
    {
        env_y *= env_c;
        double in2 = _accent > 0.0f ? env_y : 0.0;
        rc2_y1 = in2 + rc2_coeff * (rc2_y1 - in2);

        real_t tmp1 = envScaler * ((real_t)env_y - envOffset); // seems not to work yet
        real_t tmp2 = _accent * (real_t)rc2_y1;

        real_t instCutoff = nominal_cutoff * exp2_(tmp1 + tmp2);

        setCutoff(instCutoff);
    }
};

// rosic::OnePoleFilter::getSample() with the coefficients it computed
struct OnePole
{
    float b0 = 0, b1 = 0, a1 = 0, x1 = 0, y1 = 0;
    void init(const rosic::OnePoleFilter &f)
    {
        b0 = f.b0, b1 = f.b1, a1 = f.a1;
    }
    inline float process(float in)
    {
        y1 = b0 * in + b1 * x1 + a1 * y1 + (float)TINY;
        x1 = in;
        return y1;
    }
};

// rosic::BiquadFilter's BANDREJECT (RBJ cookbook) as a state variable filter (Simper's trapezoidal SVF): the same
// transfer function, but a 7.5 Hz notch as a direct form biquad needs double, the SVF does not
struct Notch
{
    float k = 0, a1 = 0, a2 = 0, a3 = 0, ic1eq = 0, ic2eq = 0;
    void init(double frequency, double bandwidth, double sampleRate)
    {
        double w = 2 * PI * frequency / sampleRate;
        double s = sin(w);
        double alpha = s * sinh(0.5 * M_LN2 * bandwidth * w / s);
        double g = tan(w / 2);
        double kk = 2 * alpha / s; // 1 / Q
        double aa1 = 1 / (1 + g * (g + kk));
        k = kk;
        a1 = aa1;
        a2 = g * aa1;
        a3 = g * g * aa1;
    }
    inline float process(float v0)
    {
        float v3 = v0 - ic2eq;
        float v1 = a1 * ic1eq + a2 * v3;
        float v2 = ic2eq + a2 * ic1eq + a3 * v3;
        ic1eq = 2 * v1 - ic1eq;
        ic2eq = 2 * v2 - ic2eq;
        return v0 - k * v1;
    }
};

struct TB303_channel
{
    OnePole feedbackHighpass;             // the ladder's
    float y1 = 0, y2 = 0, y3 = 0, y4 = 0; // output signals of the 4 filter stages

    OnePole highpass1, highpass2, allpass;
    Notch notch;

    void init()
    {
        rosic::OnePoleFilter f;
        f.setSampleRate(SAMPLE_RATE);
        f.setMode(OnePoleFilter::HIGHPASS);
        f.setCutoff(150.0);
        feedbackHighpass.init(f);
        f.setCutoff(44.486);
        highpass1.init(f);
        f.setCutoff(24.167);
        highpass2.init(f);
        f.setMode(OnePoleFilter::ALLPASS);
        f.setCutoff(14.008);
        allpass.init(f);

        notch.init(7.5164, 4.7, SAMPLE_RATE);
    }

    inline float process(const TB303_control &c, float in)
    {
        const float b0 = c.b0, k = c.k, g = c.g;
        float tmp = in;
        tmp = highpass1.process(tmp);

        // TeeBeeFilter::getSample(), TB_303
        float y0 = tmp - feedbackHighpass.process(k * y4);
        y1 += 2 * b0 * (y0 - y1 + y2);
        y2 += b0 * (y1 - 2 * y2 + y3);
        y3 += b0 * (y2 - 2 * y3 + y4);
        y4 += b0 * (y3 - 2 * y4);
        tmp = 2 * g * y4;

        tmp = allpass.process(tmp);
        tmp = highpass2.process(tmp);
        tmp = notch.process(tmp);
        return tmp * 3.f;
    }
};

TB303_control _control;
TB303_channel _left, _right;

float _cutoff = 0.5f;
float _q = 0.5f;
float _envMod = 0.5f;
float _decay = 0.5f;
float _accent = 1.0f;

void engine::setup()
{
    _control.init();
    _left.init();
    _right.init();

    engine::addParam("Cutoff", &_cutoff);
    engine::addParam("Res", &_q);
    engine::addParam("EnvMode", &_envMod);
    engine::addParam("Decay", &_decay);
    engine::addParam("Accent", &_accent);
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto inputR = engine::inputBuffer<1>();
    auto outputL = engine::outputBuffer<0>();
    auto outputR = engine::outputBuffer<1>();

    _control.update(engine::trig(), _cutoff, _q, _envMod, _decay);

    for (int i = 0; i < FRAME_BUFFER_SIZE; ++i)
    {
        _control.next(_accent);
        outputL[i] = _left.process(_control, inputL[i]);
        outputR[i] = _right.process(_control, inputR[i]);
    }
}
