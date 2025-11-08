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

#include "../squares-and-circles-api.h"
#undef SAMPLE_RATE
#define REAL_T_IS_DOUBLE

#include "open303/src/rosic_BiquadFilter.cpp"
#include "open303/src/rosic_DecayEnvelope.cpp"
#include "open303/src/rosic_EllipticQuarterBandFilter.cpp"
#include "open303/src/rosic_LeakyIntegrator.cpp"
#include "open303/src/rosic_OnePoleFilter.cpp"
#include "open303/src/rosic_TeeBeeFilter.cpp"

using namespace rosic;

#ifdef REAL_T_IS_DOUBLE
static_assert(sizeof(real_t) == sizeof(double), "real_t is double");
#else
static_assert(sizeof(real_t) == sizeof(float), "real_t is float");
#endif

struct TB303_filter
{
    rosic::TeeBeeFilter filter;
    rosic::LeakyIntegrator rc1;
    rosic::LeakyIntegrator rc2;
    rosic::DecayEnvelope mainEnv;
    rosic::EllipticQuarterBandFilter antiAliasFilter;

    rosic::OnePoleFilter highpass1, highpass2, allpass;
    rosic::BiquadFilter notch;

    real_t envScaler = 0;
    real_t envOffset = 0;
    int32_t oversampling = 1;

    void init(int32_t _oversampling = 1)
    {
        oversampling = _oversampling;
        highpass1.setSampleRate(SAMPLE_RATE * oversampling);
        highpass2.setSampleRate(SAMPLE_RATE);
        filter.setSampleRate(SAMPLE_RATE * oversampling);
        allpass.setSampleRate(SAMPLE_RATE);
        notch.setSampleRate(SAMPLE_RATE);

        mainEnv.setSampleRate(SAMPLE_RATE);
        rc1.setSampleRate(SAMPLE_RATE);
        rc2.setSampleRate(SAMPLE_RATE);

        filter.setMode(rosic::TeeBeeFilter::TB_303);
        filter.setFeedbackHighpassCutoff(150.0);

        highpass1.setMode(OnePoleFilter::HIGHPASS);
        highpass2.setMode(OnePoleFilter::HIGHPASS);
        allpass.setMode(OnePoleFilter::ALLPASS);
        notch.setMode(BiquadFilter::BANDREJECT);

        // tweakables:
        highpass1.setCutoff(44.486);
        highpass2.setCutoff(24.167);
        allpass.setCutoff(14.008);
        notch.setFrequency(7.5164);
        notch.setBandwidth(4.7);

        rc1.setTimeConstant(0.0);
        rc2.setTimeConstant(30.0);

        highpass1.reset();
        highpass2.reset();
        filter.reset();
        notch.reset();
        antiAliasFilter.reset();
    }

    void calculateEnvModScalerAndOffset(real_t cutoff, real_t envMod) // Open303::calculateEnvModScalerAndOffset()
    {
        // define some constants that arise from the measurements:
        const real_t c0 = 3.138152786059267e+002; // lowest nominal cutoff
        const real_t c1 = 2.394411986817546e+003; // highest nominal cutoff
        const real_t oF = 0.048292930943553;      // factor in line equation for offset
        const real_t oC = 0.294391201442418;      // constant in line equation for offset
        const real_t sLoF = 3.773996325111173;    // factor in line eq. for scaler at low cutoff
        const real_t sLoC = 0.736965594166206;    // constant in line eq. for scaler at low cutoff
        const real_t sHiF = 4.194548788411135;    // factor in line eq. for scaler at high cutoff
        const real_t sHiC = 0.864344900642434;    // constant in line eq. for scaler at high cutoff

        // do the calculation of the scaler and offset:
        real_t e = envMod; // linToLin(envMod, 0.0, 100.0, 0.0, 1.0);
        real_t c = cutoff; // expToLin(cutoff, c0, c1, 0.0, 1.0);
        real_t sLo = sLoF * e + sLoC;
        real_t sHi = sHiF * e + sHiC;
        envScaler = (1 - c) * sLo + c * sHi;
        envOffset = oF * c + oC;
    }

    void process(bool trig, float _cutoff, float _q, float _envMod, float _accent, float _decay,
                 const float *intput_frame, float *output_frame)
    {
        mainEnv.setDecayTimeConstant(rosic::linToExp(_decay, 0.0, 1.0, 200.0, 2000.0));

        if (trig)
            mainEnv.trigger();

        calculateEnvModScalerAndOffset(_cutoff, _envMod);

        float cutoff = rosic::linToExp(_cutoff, 0.0, 1.0, 313.0, 2394.0);
        float q = 1.f + _q * 99.f;
        filter.setResonance(q, false);
        filter.setDrive(8.f);

        real_t n2 =
            1.f; // LeakyIntegrator::getNormalizer(mainEnv.getDecayTimeConstant(), rc2.getTimeConstant(), SAMPLE_RATE);
        real_t n1 =
            1.f; // LeakyIntegrator::getNormalizer(mainEnv.getDecayTimeConstant(), rc1.getTimeConstant(), SAMPLE_RATE);

        for (size_t i = 0; i < FRAME_BUFFER_SIZE; ++i)
        {
            real_t mainEnvOut = mainEnv.getSample();
            real_t tmp1 = n1 * rc1.getSample(mainEnvOut);
            real_t tmp2 = 0.0;
            if (_accent > 0.0)
                tmp2 = mainEnvOut;

            tmp2 = n2 * rc2.getSample(tmp2);
            tmp1 = envScaler * (tmp1 - envOffset); // seems not to work yet
            tmp2 = _accent * tmp2;

            real_t instCutoff = cutoff * powf(2.0f, tmp1 + tmp2);

            filter.setCutoff(instCutoff);

            real_t tmp = intput_frame[i];

            for (int i = 1; i <= oversampling; i++)
            {
                tmp = highpass1.getSample(tmp);
                tmp = filter.getSample(tmp);
                //tmp = antiAliasFilter.getSample(tmp);
            }
            tmp = allpass.getSample(tmp);
            tmp = highpass2.getSample(tmp);
            tmp = notch.getSample(tmp);

            output_frame[i] = tmp * 3.f;
        }
    }
} _left, _right;

float _cutoff = 0.5f;
float _q = 0.5f;
float _envMod = 0.5f;
float _decay = 0.5f;
float _accent = 1.0f;
int32_t _oversampling = 1;

void engine::setup()
{
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

    _left.process(engine::trig(), _cutoff, _q, _envMod, _accent, _decay, inputL, outputL);
    _right.process(engine::trig(), _cutoff, _q, _envMod, _accent, _decay, inputR, outputR);
}