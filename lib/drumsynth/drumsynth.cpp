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

constexpr float SAMPLE_RATE = 48000.f;

inline float dB2amp(float dB)
{
    return expf(dB * 0.11512925464970228420089957273422f); // return pow(10.0, (0.05*dB)); // naive, inefficient version
}

class Envelope
{
    const EnvArgs *args_;

public:
    void init(const EnvArgs &args)
    {
        args_ = &args;
        value_ = 0.f;
        e_ = 0.0001f;
        c_ = 1.0f;
        segment_ = 0;

        if (args.n == 0)
        {
            value_ = 1.f;
            pos_ = 0;
            len_ = 0;
        }
    }

    // e = s * c ^ l
    inline float calc_c(float start, float end, int len)
    {
        if (fabsf(start - end) < __FLT_EPSILON__)
            return 1.f;
        else
        {
            return powf(e_ + (end / start), 1.0f / len);
        }
    }

    inline void process(uint32_t t, float stretch)
    {
        if (t == 0) // restart
        {
            pos_ = 0;
            len_ = 0;
            segment_ = 0;
        }

        if (pos_ < len_)
        {
            value_ *= c_;
            ++pos_;
        }
        else if (segment_ < (args_->n - 1))
        {
            int32_t i = segment_;
            while (i++ < (args_->n - 2))
                if (args_->xy[i - 1].v < args_->xy[i].v)
                {
                    stretch = 1;
                    break;
                }

            pos_ = 0;
            len_ = (args_->xy[segment_ + 1].t - args_->xy[segment_].t) * stretch;
            value_ = args_->xy[segment_].v;
            c_ = calc_c(value_, args_->xy[segment_ + 1].v, len_);
            segment_++;
        }
    }

    inline float value()
    {
        return value_;
    }

private:
    int32_t segment_;
    uint32_t len_;
    uint32_t pos_;

    float value_;
    float c_;
    float e_;
};

class Oscillator
{
public:
    void Init(float freq)
    {
        pw_ = 0.5f;
        phase_inc_ = freq / SAMPLE_RATE;
        f_ = freq;
        osc.Init();
    }

    inline void reset()
    {
        osc.Init();
    }

    inline void pitch(float pitch)
    {
        phase_inc_ = f_ * pitch / SAMPLE_RATE;
    }

    inline void duty(float duty)
    {
        pw_ = duty;
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

    // float fm = 2.f;
    // float fm_amp = 0.05f;
    // float fm_phase = 0.5f;
    inline void Metallic(float &out2)
    {
        float out = osc.phase_ < pw_ ? 1.f : -1.f;

        // float fm_ = stmlib::Interpolate(plaits::lut_sine, fm_phase, 1024.0f) * fm_amp;
        // fm_phase += phase_inc_ / fm;
        // if (fm_phase > 1.0f)
        //     fm_phase -= 1.00f;

        osc.phase_ += phase_inc_; // * (1.f + fm_);
        if (osc.phase_ > 1.0f)
            osc.phase_ -= 1.00f;

        out2 += out;
    }

    inline void Sine(float &out)
    {
        out = stmlib::Interpolate(plaits::lut_sine + 256, osc.phase_, 1024.0f);

        osc.phase_ += phase_inc_;
        if (osc.phase_ > 1.0f)
            osc.phase_ -= 1.0f;
    }

private:
    float phase_inc_, pw_, f_, amp2_;
    plaits::Oscillator osc;
};

struct drum_synth_Part
{
    Oscillator _osc[6] = {};
    stmlib::DCBlocker _dc_blocker;

    WhiteNoise noise = {};
    Envelope _amp = {};
    Envelope _pitch = {};
    Envelope _vca = {};
    Biquad biquad1 = {};
    Biquad biquad2 = {};

    const PartArgs *part;

    float amp = 0.4380016479995117f;

    void init(const PartArgs *part)
    {
        this->part = part;

        _amp.init(part->osc_amp);
        _pitch.init(part->osc_pitch);
        _vca.init(part->vca);

        _dc_blocker.Init(0.99f);

        const auto &a = part->bq1;
        if (a.mode)
            biquad1.setBiquad(a.mode - 1, a.f / SAMPLE_RATE, a.q, a.g);
        const auto &b = part->bq2;
        if (b.mode)
            biquad2.setBiquad(b.mode - 1, b.f / SAMPLE_RATE, b.q, b.g);

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
        else
        {
            amp = 0.4380016479995117f;
            _osc[0].Init(part->osc.fa);
        }
    }

    float cubicInterpolation(float x, float x0, float y0, float x1, float y1)
    {
        float t = (x - x0) / (x1 - x0);
        float t2 = t * t;
        float t3 = t2 * t;
        float a = 2 * t3 - 3 * t2 + 1;
        float b = t3 - 2 * t2 + t;
        float c = -2 * t3 + 3 * t2;
        float d = t3 - t2;
        return a * y0 + b * (x1 - x0) * y0 + c * y1 + d * (x1 - x0) * y1;
    }

    float waveshaper_transform(float a)
    {
        CONSTRAIN(a, -0.999f, 0.999f);

        for (size_t j = 0; j < part->ws.n - 1; j++)
        {
            if (part->ws.xy[j + 0].x <= a && a < part->ws.xy[j + 1].x)
            {
                return cubicInterpolation(a, part->ws.xy[j + 0].x, part->ws.xy[j + 0].y, part->ws.xy[j + 1].x, part->ws.xy[j + 1].y);
            }
        }

        return a;
    }

    inline void reset()
    {
        for (size_t j = 0; j < this->part->osc.n; j++)
            this->_osc[j].reset();
    }

    uint32_t last_f = 0;

    inline void process_frame(float f, uint32_t t, float stretch, float *out, size_t size)
    {
        float osc = 0;
        uint32_t ff = f * SAMPLE_RATE;

        while (size--)
        {
            this->_amp.process(t, stretch);
            this->_vca.process(t, stretch);
            this->_pitch.process(t, stretch);

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
                osc = this->noise.nextf(-1, 1);
                break;
            case OSC_METALLIC:

                osc = 0;
                for (size_t j = 0; j < part->osc.n; j++)
                {
                    this->_osc[j].pitch(this->_pitch.value() * f);
                    this->_osc[j].Metallic(osc);
                }

                _dc_blocker.Process(&osc, 1);
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
            }

            if (part->flags & BIQUAD_SERIAL)
            {
                osc *= amp;
                osc *= this->_amp.value();

                if (part->bq1.mode)
                {
                    if (last_f != ff)
                    {
                        this->biquad1.setFc(part->bq1.f / SAMPLE_RATE * f);
                    }

                    osc = this->biquad1.process(osc);
                    if (part->bq1.mode < BIQUAD_NOTCH)
                        osc *= part->bq1.g;
                }

                if (part->ws.n)
                    osc = waveshaper_transform(osc);

                if (part->bq2.mode)
                {
                    if (last_f != ff)
                    {
                        this->biquad2.setFc(part->bq2.f / SAMPLE_RATE * f);
                    }

                    osc = this->biquad2.process(osc);
                    if (part->bq2.mode < BIQUAD_NOTCH)
                        osc *= part->bq2.g;
                }
            }
            else if (part->flags & BIQUAD_PARALLEL)
            {
                if (part->bq1.mode)
                    osc = this->biquad1.process(osc) * part->bq1.g;
                if (part->bq2.mode)
                    osc = this->biquad2.process(osc) * part->bq2.g;
            }

            *out++ = osc * this->_vca.value() * part->level;
        }

        last_f = ff;
    }
};

extern "C" DrumSynth drum_synth_init(const DrumModel *inst, void *(*malloc)(size_t size))
{
    if (malloc == nullptr)
        malloc = ::malloc;

    const size_t malloc_size = (4 + (sizeof(drum_synth_Part) * inst->n));

    if (auto p = (DrumSynth)malloc(malloc_size))
    {
        p[0] = inst->n;
        auto _part = (drum_synth_Part *)&p[1];

        for (size_t i = 0; i < inst->n; i++)
        {
            (new (&_part[i]) drum_synth_Part())->init(&inst->part[i]);
        }
        return p;
    }

    return nullptr;
}

extern "C" void drum_synth_reset(DrumSynth inst)
{
    if (inst)
    {
        auto _part = (drum_synth_Part *)&inst[1];
        for (size_t e = 0; e < inst[0]; e++)
        {
            _part[e].reset();
        }
    }
}
extern "C" void drum_synth_process_frame(DrumSynth inst, int part, float freq, const DrumParams *params, float *out, size_t size)
{
    if (inst)
    {
        auto _part = (drum_synth_Part *)&inst[1];
        _part[part].process_frame(freq, params->t, params->decay, out, size);
    }
}