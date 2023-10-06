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

#include "machine.h"
#include "misc/nes_noise.hxx"
#include "stmlib/dsp/filter.h"
// #include "stmlib/dsp/dsp.h"
// #include "plaits/resources.h"

using namespace machine;

class SquareOscillator
{
public:
    inline void reset()
    {
        phase_ = 0.5f;
    }

    inline void freq(float freq)
    {
        phase_inc_ = freq / machine::SAMPLE_RATE;
    }

    inline void duty(float duty)
    {
        pw_ = duty;
    }

    // float fm = 2.f;
    // float fm_amp = 0.05f;
    // float fm_phase = 0.5f;

    inline void process(float &out2)
    {
        float out = phase_ < pw_ ? 1.f : -1.f;

        // float fm_ = stmlib::Interpolate(plaits::lut_sine, fm_phase, 1024.0f) * fm_amp;
        // fm_phase += phase_inc_ / fm;
        // if (fm_phase > 1.0f)
        //     fm_phase -= 1.00f;

        phase_ += phase_inc_; // * (1.f + fm_);
        if (phase_ > 1.0f)
            phase_ -= 1.00f;

        out2 += out;
    }

private:
    float phase_ = 0.5f;
    float phase_inc_ = 0;
    float pw_ = 0.5f;
};

struct MetallicOscEngine : public Engine
{
    SquareOscillator _osc[6];
    stmlib::DCBlocker _dc_blocker;
    float f = 1.f;
    float f0;
    float f1;
    float gain = 1.f;
    float duty = 0.5f;
    uint8_t n = 6;

    MetallicOscEngine() : Engine(VOCT_INPUT)
    {
        _dc_blocker.Init(0.999f);
        param[0].init("Level", &gain, gain);
        param[1].init_presets("@N", &n, n, 2, 6);
        param[2].init("F0", &f0, 540, 254.3f, 627.2f);
        param[3].init("F1", &f1, 800, 359.4f, 1149.9f);
        //param[1].value_changed = std::bind(&MetallicOscEngine::update, this);
        //param[2].value_changed = std::bind(&MetallicOscEngine::update, this);
        //param[3].value_changed = std::bind(&MetallicOscEngine::update, this);
        update();
    };

    void update()
    {
        // 119, 176, 214, 303, 363, 666
        // 205.3, 369.6, 404.4, 522.7, 359.4–1149.9, and 254.3–627.2 Hz
        constexpr float _808[] = {540, 800, 205.3f, 369.6f, 404.4f, 522.7f};
        const float *ff = _808;

        _osc[0].freq(f0 * f);
        _osc[1].freq(f1 * f);
        for (size_t i = 2; i < 6; i++)
            _osc[i].freq(ff[i] * f);

        // float r = (logf(f1) - logf(f0)) / (n);

        // for (size_t i = 0; i < n; i++)
        // {
        //     float f = i == 0 ? 1 : expf(r * i);
        //     _osc[i].freq(f0 * f);
        //     //printf("%d: %f\n", i, f0 * f);
        // }
        // exit(1);
    }

    float buffer[machine::FRAME_BUFFER_SIZE];

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        f = powf(2.f, (frame.qz_voltage(this->io, 0.f)));
        update();

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            buffer[i] = 0;
            for (size_t j = 0; j < n; j++)
            {
                this->_osc[j].process(buffer[i]);
            }
            buffer[i] *= gain / n;
        }

        _dc_blocker.Process(&buffer[0], FRAME_BUFFER_SIZE);

        of.push(buffer, machine::FRAME_BUFFER_SIZE);
    }
};

void init_808_squarenoise()
{
    machine::add<MetallicOscEngine>(machine::NOISE, "808_squares");
}

MACHINE_INIT(init_808_squarenoise);