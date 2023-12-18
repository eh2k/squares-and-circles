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

#include "machine.h"
#include "drumsynth/drumsynth.h"
#include "drumsynth/drumsynth_claps.h"
#include "misc/noise.hxx"

using namespace machine;

class ClapsEngine : public Engine
{
private:
    float tmp[FRAME_BUFFER_SIZE];
    float tmp2[FRAME_BUFFER_SIZE];
    float buffer[FRAME_BUFFER_SIZE];
    float bufferAux[FRAME_BUFFER_SIZE];
    static constexpr size_t n = 8;

    DrumSynth _instA = nullptr;
    DrumSynth _instB = nullptr;

    float pitch = 1.f;
    float stereo = 0.5f;
    float stretch = 1.f;

    DrumModel inst[32] = {};
    const size_t inst_count;
    uint8_t inst_selection = 0;

    DrumModel _cur_inst = {};
    bool _reload_inst = true;
    WhiteNoise r;
    uint32_t seed = 0;
    std::pair<uint32_t, float> seeds[7] = {
        {0x6d17ceb4, 1.f},
        {0x73623629, 1.f},
        {0x23cb4e73, 0.5f},
        {0x73C23C29, 0.5f},
        {0x42D1EC10, 1.3f},
        {0x5EFF9F17, 1.3f},
        {0x082087A1, 1.3f},
    };

public:
    ClapsEngine(const uint8_t *packed_drumKit) : Engine(TRIGGER_INPUT),
                                                 inst_count(packed_drumKit[0])
    {
        // unpack
        const uint8_t *p = packed_drumKit;
        p+=4;
        for (size_t i = 0; i < inst_count; i++)
        {
            inst[i].name = reinterpret_cast<const char *>(p);
            p += 12;
            inst[i].n = p[0];
            p+=4;
            inst[i].part = reinterpret_cast<const PartArgs *>(p);
            p += inst[i].n * sizeof(PartArgs);
        }

        param[0].init("Color", &pitch, pitch, 0.5f, 1.5f);
        param[1].init_presets("Clap", &inst_selection, 0, 0, inst_count + LEN_OF(seeds) - 1);
        param[1].value_changed = [&]()
        {
            _reload_inst = true;

            int n = inst_selection - inst_count;
            if (n >= 0 && n < (int)LEN_OF(seeds))
                r.seed = seed = seeds[n].first;
            else
                seed = r.seed;
        };
        param[1].print_value = [&](char *tmp)
        {
            if (inst_selection < 2)
                sprintf(tmp, ">%s", inst[inst_selection].name);
            else if (inst_selection < inst_count)
                sprintf(tmp, ">Clap%03X", inst_selection);
            else
                sprintf(tmp, ">%X", (unsigned int)seed);
        };
        param[2].init("Decay", &stretch, stretch, 0.1f, 2.0f);
        param[3].init("Stereo", &stereo, stereo);
        // param[0].init("Crispy", &crispy, crispy, -1.1f, 1.1f);
        // param[1].init("Decay", &decay, decay, 0, 2.f);
        //  TODO: BitCrusher + Filter + Distortion

        load_instrument(inst_selection);
    }

    ~ClapsEngine() override
    {
        free_instrument();
        machine::mfree(_instA);
        machine::mfree(_instB);
    }

    void free_instrument()
    {
        for (size_t i = 0; i < inst_count; i++)
            if (_cur_inst.part == inst[i].part)
                _cur_inst.part = nullptr;

        if (_cur_inst.part != nullptr)
            machine::mfree((void *)_cur_inst.part);
    }

    void load_instrument(uint8_t num)
    {
        _reload_inst = false;

        free_instrument();

        if (inst_selection < inst_count)
            _cur_inst = inst[num];
        else
        {
            seed = r.seed;

            float a = 1.f;
            for (auto &it : seeds)
                if (it.first == seed)
                    a = it.second;

            auto partArgs = (PartArgs *)machine::malloc(_cur_inst.n * sizeof(PartArgs));
            for (size_t i = 0; i < _cur_inst.n; i++)
            {
                size_t n = inst_count;
                DrumModel in = inst[r.next() % n];
                partArgs[i] = in.part[r.next() % in.n];

                in = inst[r.next() % n];
                auto &amp = in.part[r.next() % in.n].osc_amp;
                if (amp.n > 0)
                    partArgs[i].osc_amp = amp;

                in = inst[r.next() % n];
                partArgs[i].osc_pitch = in.part[r.next() % in.n].osc_pitch;

                in = inst[r.next() % n];
                auto &vca = in.part[r.next() % in.n].vca;
                if (vca.n > 0)
                    partArgs[i].vca = vca;

                in = inst[r.next() % n];
                partArgs[i].bq1 = in.part[r.next() % in.n].bq1;

                in = inst[r.next() % n];
                partArgs[i].bq2 = in.part[r.next() % in.n].bq2;
                partArgs[i].level *= a;
            }

            _cur_inst.part = partArgs;
        }

        machine::mfree(_instA);
        machine::mfree(_instB);
        _instA = drum_synth_init(&_cur_inst, machine::malloc);
        _instB = drum_synth_init(&_cur_inst, machine::malloc);
    }

    uint32_t t = UINT32_MAX;

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        memset(buffer, 0, sizeof(buffer));
        memset(bufferAux, 0, sizeof(bufferAux));

        if (frame.trigger)
        {
            if (_reload_inst)
                load_instrument(inst_selection);

            t = 0;

            drum_synth_reset(_instA);
            drum_synth_reset(_instB);
        }

        if (t < UINT32_MAX)
        {
            DrumParams params = {
                t : t,
                attack : 0,
                decay : stretch
            };

            float f = pitch; // powf(2.f, (frame.qz_voltage(this->io, 0)));
            float a = stereo;
            float b = 1.f - a;

            for (size_t k = 0; k < _cur_inst.n; k++)
            {
                if (stereo > 0.01f && _cur_inst.part[k].osc.type >= OSC_METALLIC)
                {
                    drum_synth_process_frame(_instA, k, (f - (f * 0.01f * stereo)), &params, tmp, machine::FRAME_BUFFER_SIZE);
                    drum_synth_process_frame(_instB, k, (f + (f * 0.01f * stereo)), &params, tmp2, machine::FRAME_BUFFER_SIZE);
                    if (_cur_inst.part[k].osc.type == OSC_METALLIC)
                    {
                        for (int i = 0; i < machine::FRAME_BUFFER_SIZE; i++)
                        {
                            buffer[i] += tmp[i];
                            bufferAux[i] += tmp2[i];
                        }
                    }
                    else
                        for (int i = 0; i < machine::FRAME_BUFFER_SIZE; i++)
                        {
                            buffer[i] += tmp[i];
                            bufferAux[i] += tmp[i] * b + tmp2[i] * a;
                        }
                }
                else
                {
                    drum_synth_process_frame(_instA, k, f, &params, tmp, machine::FRAME_BUFFER_SIZE);
                    for (int i = 0; i < machine::FRAME_BUFFER_SIZE; i++)
                    {
                        buffer[i] += tmp[i];
                        bufferAux[i] += tmp[i];
                    }
                }
                // bufferAux[i] += p._vca.value() * p._amp.value() * 0.99f;
            }

            t += machine::FRAME_BUFFER_SIZE;
        }

        of.out = buffer;
        of.aux = bufferAux;
    }
};

// #include "base/SampleEngine.hxx"
// #include "claps/cp909.h"
// #include "claps/cp808.h"

// struct TR909_CP : public SampleEngine
// {
//     const tsample_spec<int16_t> _sound = {"", (const int16_t *)cp909_raw, cp909_raw_len / 2, 44100, 0};

//     TR909_CP() : SampleEngine(&_sound, 0, 1) {}
// };

// struct TR808_CP : public SampleEngine
// {
//     const tsample_spec<int16_t> _sound = {"", (const int16_t *)cp808_raw, cp808_raw_len / 2, 44100, 0};

//     TR808_CP() : SampleEngine(&_sound, 0, 1) {}
// };

void init_claps()
{
    machine::add<ClapsEngine, const uint8_t *>(machine::DRUM, "Claps", drum_synth_claps);
}

MACHINE_INIT(init_claps);