//
//  Biquad.h
//
//  Created by Nigel Redmon on 11/24/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the Biquad code:
//  http://www.earlevel.com/main/2012/11/25/biquad-c-source-code/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code
//  for your own purposes, free or commercial.
//

#ifndef Biquad_h
#define Biquad_h

enum
{
    bq_type_lowpass = 0,
    bq_type_highpass,
    bq_type_bandpass,
    bq_type_notch,
    bq_type_peak,
    bq_type_lowshelf,
    bq_type_highshelf
};

class Biquad
{
public:
    Biquad();
    Biquad(int type, float Fc, float Q, float peakGainDB);
    ~Biquad();
    void setType(int type);
    void setQ(float Q);
    void setFc(float Fc);
    void setPeakGain(float peakGainDB);
    void setBiquad(int type, float Fc, float Q, float peakGainDB);
    float process(float in);

protected:
    void calcBiquad(void);

    int type;
    float a0, a1, a2, b0, b1, b2;
    float Fc, Q, peakGain;
    float z1, z2;
    // float x1; // input delayed by 1 sample
    // float x2; // input delayed by 2 samples
    // float y1; // output delayed by 1 sample
    // float y2; // output delayed by 2 samples
};

inline float Biquad::process(float in)
{
    float out = in * a0 + z1;
    z1 = in * a1 + z2 - b1 * out;
    z2 = in * a2 - b2 * out;
    return out;

    // float x = in;
    // float y = a0 * x + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;

    // x2 = x1;
    // x1 = x;
    // y2 = y1;
    // y1 = y;

    // return y;
}

#endif // Biquad_h
