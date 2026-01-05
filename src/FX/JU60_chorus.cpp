// Copyright (C)2022 - Eduard Heidt
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
//
// Credits to Andy Harman and JP Cimalando for Juno 60 chorus research, and Émilie Gillet for stmlib.
// https://github.com/pendragon-andyh/Juno60/blob/master/Chorus/README.md
// https://github.com/pendragon-andyh/Juno60/issues/2
// https://github.com/jpcima/rc-effect-playground/issues/2#issuecomment-541340615
// https://github.com/jpcima/string-machine/tree/master/sources/bbd

#include "../squares-and-circles-api.h"
#ifdef ONE_POLE
#undef ONE_POLE
#endif

#include <string.h>

#include "bbd/bbd_line.h"

#include "stmlib/dsp/filter.h"
#include "stmlib/dsp/delay_line.h"
#include "stmlib/dsp/dsp.h"

#include "bbd/bbd_line.cc"
#include "bbd/bbd_filter.cc"

struct Juno60_Chorus
{
    static constexpr size_t delay_size = (1 + SAMPLE_RATE * 0.0054); // max delay time
    stmlib::DelayLine<float, delay_size> delay_;
    stmlib::Svf pre_lpf, post_lpf_l, post_lpf_r;

    float phase_ = 0;

    float lfo_tri(float phase_inc_)
    {
        float t = -1.0f + (2.0f * phase_);

        phase_ += phase_inc_;
        if (phase_ > 1.0f)
            phase_ -= 1.0f;

        return fabsf(t);
    }

    struct Mode
    {
        float freq;
        float dry;
        float minDelayL;
        float maxDelayL;
        float minDelayR;
        float maxDelayR;
        bool stereo;
    };

    const Mode modes_[4] = {
        {0.513, 1.0, 0.00154, 0.00515, 0.00151, 0.0054, true},   // Off
        {0.513, 0.44, 0.00154, 0.00515, 0.00151, 0.0054, true},  // Mode I
        {0.863, 0.44, 0.00154, 0.00515, 0.00151, 0.0054, true},  // Mode II
        {9.75, 0.44, 0.00322, 0.00356, 0.00328, 0.00365, false}, // Mode I+II
    };

    Mode mode_ = modes_[1];

    float lmin, la;
    float rmin, ra;

    void setMode(const Mode &mode)
    {
        mode_ = mode;
        lmin = mode_.minDelayL * SAMPLE_RATE;
        la = (mode_.maxDelayL - mode_.minDelayL) * SAMPLE_RATE;
        rmin = mode_.minDelayR * SAMPLE_RATE;
        ra = (mode_.maxDelayR - mode_.minDelayR) * SAMPLE_RATE;
    }

    Juno60_Chorus()
    {
        delay_.Init();
        pre_lpf.Init();
        post_lpf_l.Init();
        post_lpf_r.Init();
        pre_lpf.set_f_q<stmlib::FREQUENCY_ACCURATE>(7237.f / SAMPLE_RATE, 1.f);
        post_lpf_l.set_f_q<stmlib::FREQUENCY_ACCURATE>(10644.f / SAMPLE_RATE, 1.f);
        post_lpf_r.set_f_q<stmlib::FREQUENCY_ACCURATE>(10644.f / SAMPLE_RATE, 1.f);
    }

    void process(float *inOut, float *outR, uint32_t len)
    {
        float wet = 1.f - mode_.dry;
        float f = mode_.freq / SAMPLE_RATE;
        while (len-- > 0)
        {
            float lfo_val = lfo_tri(f);

            *inOut = *outR = *inOut * mode_.dry;

            delay_.Write(pre_lpf.Process<stmlib::FILTER_MODE_LOW_PASS>(stmlib::SoftLimit(*inOut)));

            auto l = delay_.Read(lmin + la * lfo_val);
            *inOut++ += post_lpf_l.Process<stmlib::FILTER_MODE_LOW_PASS>(l * wet);

            auto r = delay_.Read(rmin + ra * (mode_.stereo ? (1.f - lfo_val) : lfo_val));
            *outR++ += post_lpf_r.Process<stmlib::FILTER_MODE_LOW_PASS>(r * wet);
        }
    }
};

struct Juno60_Chorus_BBD
{
    // https://github.com/jpcima/bbd-delay-experimental
    // Not sure if the emulation of the correct MN3009 BBD chips ?
    // But it sounds better than the naive emulation ;-)
    // For details - see also https://github.com/jpcima/bbd-delay-experimental/issues/1

    static constexpr int bbd_stages = 185; // the TCA-350-Y IC
    BBD_Line _delayL;
    BBD_Line _delayR;
    BBD_Filter_Coef fin;
    BBD_Filter_Coef fout;

    struct Mode
    {
        float freq;
        float dry;
        float minDelayL;
        float maxDelayL;
        float minDelayR;
        float maxDelayR;
        bool stereo;
    };

    // https://github.com/pendragon-andyh/Juno60/blob/master/Chorus/README.md
    const Mode modes_[4] = {
        {0.513, 1.0, 0.00154, 0.00515, 0.00151, 0.0054, true},   // Off
        {0.513, 0.44, 0.00154, 0.00515, 0.00151, 0.0054, true},  // Mode I
        {0.863, 0.44, 0.00154, 0.00515, 0.00151, 0.0054, true},  // Mode II
        {9.75, 0.44, 0.00322, 0.00356, 0.00328, 0.00365, false}, // Mode I+II
    };

    Mode mode_ = modes_[1];

    float minL, maxL, minR, maxR;

    void setMode(const Mode &mode)
    {
        mode_ = mode;
        minL = BBD_Line::hz_rate_for_delay(mode_.minDelayL, bbd_stages) / SAMPLE_RATE;
        maxL = BBD_Line::hz_rate_for_delay(mode_.maxDelayL, bbd_stages) / SAMPLE_RATE;
        minR = BBD_Line::hz_rate_for_delay(mode_.minDelayR, bbd_stages) / SAMPLE_RATE;
        maxR = BBD_Line::hz_rate_for_delay(mode_.maxDelayR, bbd_stages) / SAMPLE_RATE;
    }

    Juno60_Chorus_BBD()
    {
        const unsigned interp_size = 128;
        fin = BBD::compute_filter(SAMPLE_RATE, interp_size, bbd_fin_j60);
        fout = BBD::compute_filter(SAMPLE_RATE, interp_size, bbd_fout_j60);

        _delayL.setup(bbd_stages, fin, fout);
        _delayR.setup(bbd_stages, fin, fout);
        _delayL.clear();
        _delayR.clear();
    }

    float phase_ = 0;

    void lfo_tri(float phase_inc_, float *out)
    {
        for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            float t = -1.0f + (2.0f * phase_);
            *out++ = fabsf(t);

            phase_ += phase_inc_;
            if (phase_ > 1.0f)
                phase_ -= 1.0f;
        }
    }

    void process(float *inOut, float *outR, uint32_t len)
    {
        float wet = 1.f - mode_.dry;
        float f = mode_.freq / SAMPLE_RATE;
        
        float dry[FRAME_BUFFER_SIZE];
        float clockL[FRAME_BUFFER_SIZE];
        float clockR[FRAME_BUFFER_SIZE];

        memcpy(dry, inOut, len * sizeof(float));

        lfo_tri(f, clockL);

        for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            dry[i] = stmlib::SoftLimit(dry[i]);

            auto lfo = clockL[i];
            clockL[i] = minL + lfo * (maxL - minL);
            clockR[i] = minR + (mode_.stereo ? (1.f - lfo) : lfo) * (maxR - minR);
        }

        _delayL.process(FRAME_BUFFER_SIZE, dry, inOut, clockL);
        _delayR.process(FRAME_BUFFER_SIZE, dry, outR, clockR);

        for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            inOut[i] = inOut[i] * wet + dry[i] * mode_.dry;
            outR[i] = outR[i] * wet + dry[i] * mode_.dry;
        }
    }
};

static Juno60_Chorus_BBD _chorus = {};

static int32_t mode = 1;
static const char *modes[] = {
    "Off", "Mode I", "Mode II", "Mode I+II"};
static float raw = 1.f;

void engine::setup()
{
    engine::addParam("Amount", &raw);
    engine::addParam(">Modes", &mode, 0, 3, modes);
}

void set(float *target, const float *src, float amp)
{
    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        target[i] = src[i] * amp;
}

void mix(float *target, const float *src, float amp)
{
    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        target[i] += src[i] * amp;
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto inputR = engine::inputBuffer<1>();
    auto outputL = engine::outputBuffer<0>();
    auto outputR = engine::outputBuffer<1>();

    set(outputL, inputL, raw);
    set(outputR, inputR, raw);

    _chorus.setMode(_chorus.modes_[mode]);
    _chorus.process(outputL, outputR, FRAME_BUFFER_SIZE);

    mix(outputL, inputL, 1.f - raw);
    mix(outputR, inputR, 1.f - raw);
}

// #include "plaits/dsp/oscillator/oscillator.h"

// template <class T>
// struct Juno60Chorus_TEST : public Engine
// {
//     plaits::Oscillator _osc;
//     stmlib::DCBlocker _dc_blockerL;
//     stmlib::DCBlocker _dc_blockerR;
//     T _chorus;

//     float bufferL[FRAME_BUFFER_SIZE];
//     float bufferR[FRAME_BUFFER_SIZE];

//     uint8_t mode = 1;
//     float raw = 1.f;

//     Juno60Chorus_TEST() : Engine()
//     {
//         _dc_blockerL.Init(0.999f);
//         _dc_blockerR.Init(0.999f);
//         _osc.Init();
//         _chorus.setMode(_chorus.modes_[mode]);
//     }

//     void process(const ControlFrame &frame, OutputFrame &of) override
//     {
//         memset(bufferL, 0, sizeof(bufferL));
//         memset(bufferR, 0, sizeof(bufferL));
//         get_audio(AUX_L, bufferL, raw);
//         get_audio(AUX_R, bufferR, raw);

//         _osc.Render<plaits::OSCILLATOR_SHAPE_SQUARE>(111.11f / SAMPLE_RATE, 0.5f, bufferL, FRAME_BUFFER_SIZE);

//         for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
//         {
//             bufferL[i] *= 0.5f;
//         }

//         _chorus.process(bufferL, bufferR, FRAME_BUFFER_SIZE);

//         _dc_blockerL.Process(bufferL, FRAME_BUFFER_SIZE);
//         _dc_blockerR.Process(bufferR, FRAME_BUFFER_SIZE);

//         get_audio(AUX_L, bufferL, 1.f - raw);
//         get_audio(AUX_R, bufferR, 1.f - raw);

//         of.out = bufferL;
//         of.aux = bufferR;
//     }
// };