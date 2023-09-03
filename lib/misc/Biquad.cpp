//
//  Biquad.cpp
//
//  Created by Nigel Redmon on 11/24/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the Biquad code:
//  http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code
//  for your own purposes, free or commercial.
//
//  [eh2k] perforcance optimizations..

#include <math.h>
#include "Biquad.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

#define M_PI_F float(M_PI)
#define M_PI_POW_2 M_PI * M_PI
#define M_PI_POW_3 M_PI_POW_2 * M_PI
#define M_PI_POW_5 M_PI_POW_3 * M_PI_POW_2

inline float tan_pi_f(float f) // stmlib::OnePole::tan<FREQUENCY_FAST>(Fc)
{
    // The usual tangent approximation uses 3.1755e-01 and 2.033e-01, but
    // the coefficients used here are optimized to minimize error for the
    // 16Hz to 16kHz range, with a sample rate of 48kHz.
    const float a = 3.260e-01f * M_PI_POW_3;
    const float b = 1.823e-01f * M_PI_POW_5;
    float f2 = f * f;
    return f * (M_PI_F + f2 * (a + b * f2));
}

Biquad::Biquad()
{
    type = bq_type_lowpass;
    a0 = 1.0;
    a1 = a2 = b1 = b2 = 0.0;
    Fc = 0.50;
    Q = 0.707;
    peakGain = 1.0f;
    // z1 = z2 = 0.0;
}

Biquad::Biquad(int type, float Fc, float Q, float peakGainDB)
{
    setBiquad(type, Fc, Q, peakGainDB);
    // z1 = z2 = 0.0;
}

Biquad::~Biquad()
{
}

void Biquad::setType(int type)
{
    this->type = type;
    calcBiquad();
}

void Biquad::setQ(float Q)
{
    this->Q = Q;
    calcBiquad();
}

void Biquad::setFc(float Fc)
{
    this->Fc = Fc;
    calcBiquad();
}

void Biquad::setPeakGain(float peakGainDB)
{
    this->peakGain = expf(peakGainDB * 0.11512925464970228420089957273422f); // dB2amp(peakGainDB);
    calcBiquad();
}

void Biquad::setBiquad(int type, float Fc, float Q, float peakGainDB)
{
    this->type = type;
    this->Q = Q;
    this->Fc = Fc;
    setPeakGain(peakGainDB);
}

void Biquad::calcBiquad(void)
{
    float norm;
    float V = peakGain;     // powf(10, fabsf(peakGain) / 20.0f);
    float K = tan_pi_f(Fc); // tanf(M_PI * Fc)
    switch (this->type)
    {
    case bq_type_lowpass:
        norm = 1 / (1 + K / Q + K * K);
        a0 = K * K * norm;
        a1 = 2 * a0;
        a2 = a0;
        b1 = 2 * (K * K - 1) * norm;
        b2 = (1 - K / Q + K * K) * norm;
        break;

    case bq_type_highpass:
        norm = 1 / (1 + K / Q + K * K);
        a0 = 1 * norm;
        a1 = -2 * a0;
        a2 = a0;
        b1 = 2 * (K * K - 1) * norm;
        b2 = (1 - K / Q + K * K) * norm;
        break;

    case bq_type_bandpass:
        norm = 1 / (1 + K / Q + K * K);
        a0 = K / Q * norm;
        a1 = 0;
        a2 = -a0;
        b1 = 2 * (K * K - 1) * norm;
        b2 = (1 - K / Q + K * K) * norm;
        break;

    case bq_type_notch:
        norm = 1 / (1 + K / Q + K * K);
        a0 = (1 + K * K) * norm;
        a1 = 2 * (K * K - 1) * norm;
        a2 = a0;
        b1 = a1;
        b2 = (1 - K / Q + K * K) * norm;
        break;

    case bq_type_peak:
        if (peakGain >= 1)
        { // boost
            norm = 1 / (1 + 1 / Q * K + K * K);
            a0 = (1 + V / Q * K + K * K) * norm;
            a1 = 2 * (K * K - 1) * norm;
            a2 = (1 - V / Q * K + K * K) * norm;
            b1 = a1;
            b2 = (1 - 1 / Q * K + K * K) * norm;
        }
        else
        { // cut
            norm = 1 / (1 + V / Q * K + K * K);
            a0 = (1 + 1 / Q * K + K * K) * norm;
            a1 = 2 * (K * K - 1) * norm;
            a2 = (1 - 1 / Q * K + K * K) * norm;
            b1 = a1;
            b2 = (1 - V / Q * K + K * K) * norm;
        }
        break;
    case bq_type_lowshelf:
        if (peakGain >= 1)
        { // boost
            norm = 1 / (1 + sqrtf(2) * K + K * K);
            a0 = (1 + sqrtf(2 * V) * K + V * K * K) * norm;
            a1 = 2 * (V * K * K - 1) * norm;
            a2 = (1 - sqrtf(2 * V) * K + V * K * K) * norm;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - sqrtf(2) * K + K * K) * norm;
        }
        else
        { // cut
            norm = 1 / (1 + sqrtf(2 * V) * K + V * K * K);
            a0 = (1 + sqrtf(2) * K + K * K) * norm;
            a1 = 2 * (K * K - 1) * norm;
            a2 = (1 - sqrtf(2) * K + K * K) * norm;
            b1 = 2 * (V * K * K - 1) * norm;
            b2 = (1 - sqrtf(2 * V) * K + V * K * K) * norm;
        }
        break;
    case bq_type_highshelf:
        if (peakGain >= 1)
        { // boost
            norm = 1 / (1 + sqrtf(2) * K + K * K);
            a0 = (V + sqrtf(2 * V) * K + K * K) * norm;
            a1 = 2 * (K * K - V) * norm;
            a2 = (V - sqrtf(2 * V) * K + K * K) * norm;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - sqrtf(2) * K + K * K) * norm;
        }
        else
        { // cut
            norm = 1 / (V + sqrtf(2 * V) * K + K * K);
            a0 = (1 + sqrtf(2) * K + K * K) * norm;
            a1 = 2 * (K * K - 1) * norm;
            a2 = (1 - sqrtf(2) * K + K * K) * norm;
            b1 = 2 * (K * K - V) * norm;
            b2 = (V - sqrtf(2 * V) * K + K * K) * norm;
        }
        break;
    }

    return;
}
