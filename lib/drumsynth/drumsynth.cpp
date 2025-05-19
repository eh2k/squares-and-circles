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

#define CONSTRAIN(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#include "stmlib/dsp/dsp.h"
#include "stmlib/dsp/filter.h"
#include "plaits/dsp/oscillator/oscillator.h"
#include "stmlib/dsp/cosine_oscillator.h"
#include "plaits/resources.h"
#include "misc/noise.hxx"
#include "misc/Biquad.h"
#include "drumsynth.h"
#include "string.h"
#include "misc/cubic_spline.hxx"

#ifndef __SAMPLE_RATE
constexpr float __SAMPLE_RATE = 48000.f;
#endif

inline float dB2amp(float dB)
{
    return expf(dB * 0.11512925464970228420089957273422f); // return pow(10.0, (0.05*dB)); // naive, inefficient version
}

class Envelope
{
    const EnvArgs *args_;

public:
    void init(const EnvArgs *args)
    {
        args_ = args;
        value_ = 0.f;
        e_ = 0.0001f;
        c_ = 1.0f;
        segment_ = 0;

        if (args_->n == 0)
        {
            value_ = 1.f;
            pos_ = 0;
            len_ = 0;
        }
        else if (args_->n == 1)
        {
            value_ = args_->xy[0].v;
            pos_ = 0;
            len_ = 0;
        }
        else
        {
            if (args_->xy[0].t == 0)
                value_ = args_->xy[0].v;
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

    inline bool finished()
    {
        return pos_ != 0 && pos_ == len_;
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
        phase_inc_ = freq / __SAMPLE_RATE;
        f_ = freq;
        osc.Init();
    }

    inline void reset()
    {
        osc.Init();
    }

    inline void pitch(float pitch)
    {
        phase_inc_ = f_ * pitch / __SAMPLE_RATE;
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
        // #define PI_F 3.1415927410125732421875f
        // out = sinf((0.25f + osc.phase_) * PI_F * 2.f);

        out = -stmlib::Interpolate(plaits::lut_sine + 128, osc.phase_, 512.0f);

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
    Oscillator *_osc = nullptr;
    stmlib::DCBlocker _dc_blocker;

    WhiteNoise noise = {};
    Envelope _amp = {};
    Envelope _pitch = {};
    Envelope _vca = {};
    Biquad biquad1 = {};
    Biquad biquad2 = {};

    std::pair<float, float> biquadA[2] = {};
    std::pair<float, float> biquadB[2] = {};

    const PartArgs *part;

    float amp = 1.f;
    cspline waveshaper = {};

    void init(const PartArgs *part)
    {
        this->part = part;

        _amp.init(&part->osc_amp);
        _pitch.init(&part->osc_pitch);
        _vca.init(&part->vca);

        _dc_blocker.Init(0.99f);

        const auto &a = part->bq1;
        if (a.mode)
            biquad1.setBiquad(a.mode - 1, a.f / __SAMPLE_RATE, a.q, a.g);
        const auto &b = part->bq2;
        if (b.mode)
            biquad2.setBiquad(b.mode - 1, b.f / __SAMPLE_RATE, b.q, b.g);

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
            amp = 1.f - __FLT_EPSILON__; // 0.4380016479995117f;
            _osc[0].Init(part->osc.fa);
        }

        if (part->ws.n)
        {
            cspline_init(&this->waveshaper, &part->ws.xy[0].x, &part->ws.xy[0].y, part->ws.n, 2);
        }
    }

    void free()
    {
        if (part->ws.n)
            cspline_free(&this->waveshaper);
    }

    float waveshaper_transform(float a)
    {
        CONSTRAIN(a, -1.f + __FLT_EPSILON__, 1.f - __FLT_EPSILON__);
        a = cspline_eval(&this->waveshaper, a);
        CONSTRAIN(a, -1.f + __FLT_EPSILON__, 1.f - __FLT_EPSILON__);
        return a;
    }

    inline void reset()
    {
        for (size_t j = 0; j < this->part->osc.n; j++)
            this->_osc[j].reset();

        memset(&this->biquadA, 0, sizeof(this->biquadA));
        memset(&this->biquadB, 0, sizeof(this->biquadB));
    }

    uint32_t last_f = 0;

    inline void process_frame(float f, const DrumParams *params, float *outL, float *outR, size_t size)
    {
        uint32_t t = params->t;
        float osc = 0;
        float osc2 = 0;
        uint32_t ff = f * __SAMPLE_RATE;

        // if (t > 0 && this->_amp.finished() && this->_vca.finished())
        // {
        //     return;
        // }

        while (size--)
        {
            this->_amp.process(t, params->decay);
            this->_vca.process(t, params->decay);
            this->_pitch.process(t, params->decay);

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
            {
                float a = this->noise.nextf(-1, 1);
                float b = this->noise.nextf(-1, 1);
                float st = 0.5f - (params->stereo / 2);
                osc = (a * (1 - st) + b * st);
                osc2 = (b * (1 - st) + a * st);
            }
            break;
            case OSC_METALLIC:

                osc = 0;
                for (size_t j = 0; j < part->osc.n; j++)
                {
                    // this->_osc[j].pitch(this->_pitch.value() * f);
                    this->_osc[j].pitch(f);
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
            default:
                return;
            }

            if (part->flags & BIQUAD_SERIAL)
            {
                osc *= amp;
                osc *= this->_amp.value();

                if (part->bq1.mode)
                {
                    if (last_f != ff)
                    {
                        this->biquad1.setFc(part->bq1.f / __SAMPLE_RATE * f);
                    }

                    osc = this->biquad1.process(osc, this->biquadA[0].first, this->biquadA[0].second);
                    if (part->bq1.mode < BIQUAD_NOTCH)
                        osc *= part->bq1.g;
                }

                if (part->ws.n)
                    osc = waveshaper_transform(osc);

                if (part->bq2.mode)
                {
                    if (last_f != ff)
                    {
                        this->biquad2.setFc(part->bq2.f / __SAMPLE_RATE * f);
                    }

                    osc = this->biquad2.process(osc, this->biquadA[1].first, this->biquadA[1].second);
                    if (part->bq2.mode < BIQUAD_NOTCH)
                        osc *= part->bq2.g;
                }

                if (params->stereo > 0 && (part->osc.type == OSC_NOISE1 || part->osc.type == OSC_NOISE2))
                {
                    osc2 *= amp;
                    osc2 *= this->_amp.value();

                    if (part->bq1.mode)
                    {
                        osc2 = this->biquad1.process(osc2, this->biquadB[0].first, this->biquadB[0].second);
                        if (part->bq1.mode < BIQUAD_NOTCH)
                            osc2 *= part->bq1.g;
                    }

                    if (part->ws.n)
                        osc2 = waveshaper_transform(osc2);

                    if (part->bq2.mode)
                    {
                        osc2 = this->biquad2.process(osc2, this->biquadB[1].first, this->biquadB[1].second);
                        if (part->bq2.mode < BIQUAD_NOTCH)
                            osc2 *= part->bq2.g;
                    }
                }
                else
                {
                    osc2 = osc;
                }
            }
            else if (part->flags & BIQUAD_PARALLEL)
            {
                if (part->bq1.mode)
                    osc = this->biquad1.process(osc) * part->bq1.g;
                if (part->bq2.mode)
                    osc = this->biquad2.process(osc) * part->bq2.g;

                if (part->ws.n)
                    osc = waveshaper_transform(osc);

                osc2 = osc;
            }

            *outL++ = osc * this->_vca.value() * part->level * params->levelL * 0.8f;
            *outR++ = osc2 * this->_vca.value() * part->level * params->levelR * 0.8f;
            CONSTRAIN(*(outL - 1), -1.f, 1.f);
            CONSTRAIN(*(outR - 1), -1.f, 1.f);
        }

        last_f = ff;
    }
};

extern "C" DrumSynth drum_synth_init(const DrumModel *inst, void *(*malloc)(size_t size))
{
    if (malloc == nullptr)
        malloc = ::malloc;

    const size_t malloc_size = (sizeof(inst->n) + (sizeof(drum_synth_Part) * inst->n));

    if (auto p = (DrumSynth)malloc(malloc_size))
    {
        p[0] = inst->n;
        auto _part = (drum_synth_Part *)&p[1];

        for (size_t i = 0; i < inst->n; i++)
        {
            new (&_part[i]) drum_synth_Part();
            auto osc_n = std::max<uint32_t>(1, inst->part[i].osc.n);
            _part[i]._osc = new (malloc(sizeof(Oscillator) * osc_n)) Oscillator[osc_n]{};
            _part[i].init(&inst->part[i]);
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
extern "C" void drum_synth_process_frame(DrumSynth inst, int part, float freq, const DrumParams *params, float *outL, float *outR, size_t size)
{
    if (inst)
    {
        auto _part = (drum_synth_Part *)&inst[1];
        if (part >= 0)
        {
            _part[part].process_frame(freq, params, outL, outR, size);
        }
        else
        {
            float tmpL[size];
            float tmpR[size];
            size_t skip = -1;

            for (size_t part = 0; part < inst[0]; part++)
            {
                if (skip == part)
                    continue;

                _part[part].process_frame(freq, params, tmpL, tmpR, size);

                if (part == 0 && _part[part].part->amp_mod.dest != 0)
                {
                    skip = _part[part].part->amp_mod.dest;
                    float modL[size];
                    float modR[size];
                    _part[_part[part].part->amp_mod.dest].process_frame(freq, params, modL, modR, size);

                    for (size_t j = 0; j < size; j++)
                    {
                        outL[j] += tmpL[j] * modL[j];
                        outR[j] += tmpR[j] * modR[j];
                    }
                }
                else
                {
                    for (size_t j = 0; j < size; j++)
                    {
                        outL[j] += tmpL[j];
                        outR[j] += tmpR[j];
                    }
                }
            }
        }
    }
}

extern "C" int drum_synth_load_models(const uint8_t *drumkit, DrumModel _instModel[16], void *(*malloc)(size_t size))
{
    if (drumkit == nullptr)
        return 0;

    if (drumkit[0] == '!' && drumkit[1] == 'R' && drumkit[2] == 'C' && drumkit[3] == '8')
        drumkit += 4;
    else
        return 0;

    int inst_count = 0;
    const uint8_t *p = drumkit;
    p += 4;
    for (size_t i = 0; i < drumkit[0]; i++)
    {
        _instModel[i].name = reinterpret_cast<const char *>(p);
        p += 12;

        _instModel[i].n = *reinterpret_cast<const size_t *>(p);
        p += sizeof(_instModel[i].n);

        PartArgs *part = new (malloc(sizeof(PartArgs) * _instModel[i].n)) PartArgs[_instModel[i].n]{};
        _instModel[i].part = part;

        for (size_t j = 0; j < _instModel[i].n; j++)
        {
            part->flags = *reinterpret_cast<const PartFlags *>(p);
            p += sizeof(part->flags);

            part->osc = *reinterpret_cast<const OscArgs *>(p);
            p += sizeof(part->osc);

            part->osc_pitch.n = *reinterpret_cast<const int32_t *>(p);
            p += sizeof(part->osc_pitch.n);
            part->osc_pitch.xy = reinterpret_cast<const EnvXY *>(p);
            if (part->osc_pitch.xy[part->osc_pitch.n - 1].t > 0)
            {
                // OK
            }
            int k = part->osc_pitch.n;
            p += sizeof(EnvXY) * k;

            part->osc_amp.n = *reinterpret_cast<const int32_t *>(p);
            p += sizeof(part->osc_amp.n);
            part->osc_amp.xy = reinterpret_cast<const EnvXY *>(p);
            if (part->osc_amp.xy[part->osc_amp.n - 1].t > 0)
            {
                // OK
            }
            k = part->osc_amp.n;
            p += sizeof(EnvXY) * k;

            part->vca.n = *reinterpret_cast<const int32_t *>(p);
            p += sizeof(part->vca.n);
            part->vca.xy = reinterpret_cast<const EnvXY *>(p);
            if (part->vca.xy[part->vca.n - 1].t > 0)
            {
                // OK
            }
            k = part->vca.n;
            p += sizeof(EnvXY) * k;

            part->bq1 = *reinterpret_cast<const BiquadArgs *>(p);
            p += sizeof(BiquadArgs);

            part->bq2 = *reinterpret_cast<const BiquadArgs *>(p);
            p += sizeof(BiquadArgs);

            part->ws.n = *reinterpret_cast<const uint32_t *>(p);
            p += sizeof(part->ws.n);
            part->ws.xy = reinterpret_cast<const WS_XY *>(p);
            if (part->ws.xy[part->ws.n - 1].x > 0)
            {
                // OK
            }
            k = part->ws.n;
            p += sizeof(WS_XY) * k;

            if (part->flags & VCF)
            {
                part->vcf = reinterpret_cast<const VCFArgs *>(p);
                p += sizeof(*part->vcf);

                part->vcf_env.n = *reinterpret_cast<const uint32_t *>(p);
                p += sizeof(part->vcf_env.n);
                part->vcf_env.xy = reinterpret_cast<const EnvXY *>(p);
                if (part->vcf_env.xy[part->vcf_env.n - 1].t > 0)
                {
                    // OK
                }
                k = part->vcf_env.n;
                p += sizeof(EnvXY) * k;
            }

            part->amp_mod.dest = *reinterpret_cast<const uint32_t *>(p);
            p += sizeof(part->amp_mod.dest);
            part->amp_mod.offset = *reinterpret_cast<const float *>(p);
            p += sizeof(part->amp_mod.offset);

            part->level = *reinterpret_cast<const float *>(p);
            p += sizeof(part->level);
            part++;
        }
        inst_count++;
    }

    return inst_count;
}

float drum_synth_process_ws(DrumSynth inst, int part, float x)
{
    auto _part = (drum_synth_Part *)&inst[1];
    if (part >= 0 && _part[part].part->ws.n)
    {
        return _part[part].waveshaper_transform(x);
    }

    return x;
}