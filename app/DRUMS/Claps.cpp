// Copyright (C)2021 - E.Heidt
//
// Author: E.Heidt (eh2k@gmx.de)
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

#include "../squares-and-circles-api.h"
#include "lib/drumsynth/drumsynth.h"
#include "lib/drumsynth/drumsynth_claps.h"
#include "lib/misc/noise.hxx"

static constexpr size_t n = 8;

DrumSynth _inst = nullptr;

float pitch = 1.f;
float stereo = 0.5f;
float stretch = 1.f;

DrumModel inst[32] = {};
const uint8_t *packed_drumKit = drum_synth_claps;
const size_t inst_count = 10; // packed_drumKit[0];
int32_t inst_selection = 0;

DrumModel _cur_inst = {};
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

void load_instrument(uint8_t num);

int sprint_inst_name(char *tmp, int inst_selection)
{
    if (inst_selection < 2)
        return sprintf(tmp, ">%s", inst[inst_selection].name);
    else if (inst_selection < inst_count)
        return sprintf(tmp, ">Clap%03X", inst_selection);
    else if ((inst_selection - inst_count) < (int)LEN_OF(seeds))
        return sprintf(tmp, ">%X", seeds[inst_selection - inst_count].first);
    else
        return sprintf(tmp, ">%X", (unsigned int)seed);
}

char inst_name_buff[256] = {};
const char *inst_names[inst_count + LEN_OF(seeds)] = {};

void engine::setup()
{
    // unpack
    const uint8_t *p = packed_drumKit;
    p += 4;
    for (size_t i = 0; i < packed_drumKit[0]; i++)
    {
        inst[i].name = reinterpret_cast<const char *>(p);
        p += 12;

        inst[i].n = *reinterpret_cast<const size_t *>(p);
        p += sizeof(inst[i].n);

        PartArgs *part = new PartArgs[inst[i].n]{};
        inst[i].part = part;

        for (int j = 0; j < inst[i].n; j++)
        {
            part->flags = *reinterpret_cast<const PartFlags *>(p);
            p += sizeof(part->flags);

            part->osc = *reinterpret_cast<const OscArgs *>(p);
            p += sizeof(part->osc);

            part->osc_pitch.n = *reinterpret_cast<const int32_t *>(p);
            p += sizeof(part->osc_pitch.n);
            part->osc_pitch.xy = reinterpret_cast<const EnvXY *>(p);
            p += sizeof(EnvXY) * 16;

            part->osc_amp.n = *reinterpret_cast<const int32_t *>(p);
            p += sizeof(part->osc_amp.n);
            part->osc_amp.xy = reinterpret_cast<const EnvXY *>(p);
            p += sizeof(EnvXY) * 16;

            part->vca.n = *reinterpret_cast<const int32_t *>(p);
            p += sizeof(part->vca.n);
            part->vca.xy = reinterpret_cast<const EnvXY *>(p);
            p += sizeof(EnvXY) * 16;

            part->bq1 = *reinterpret_cast<const BiquadArgs *>(p);
            p += sizeof(BiquadArgs);

            part->bq2 = *reinterpret_cast<const BiquadArgs *>(p);
            p += sizeof(BiquadArgs);

            part->ws.n = *reinterpret_cast<const uint32_t *>(p);
            p += sizeof(part->ws.n);
            part->ws.xy = reinterpret_cast<const WS_XY *>(p);
            p += sizeof(WS_XY) * 8;

            part->level = *reinterpret_cast<const float *>(p);
            p += sizeof(part->level);
            part++;
        }
    }

    engine::addParam("Color", &pitch, 0.5f, 1.5f);

    char *tmp = inst_name_buff;
    for (int i = 0; i < (inst_count + LEN_OF(seeds)); i++)
    {
        inst_names[i] = tmp;
        tmp += sprint_inst_name(tmp, i) + 1;
    }
    engine::addParam("Clap", &inst_selection, 0, (inst_count + LEN_OF(seeds)) - 1, inst_names);
    engine::addParam("Decay", &stretch, 0.1f, 2.0f);
    engine::addParam("Stereo", &stereo);
    // param[0].init("Crispy", &crispy, crispy, -1.1f, 1.1f);
    // param[1].init("Decay", &decay, decay, 0, 2.f);
    //  TODO: BitCrusher + Filter + Distortion
    load_instrument(inst_selection);
}

void free_instrument()
{
    for (size_t i = 0; i < inst_count; i++)
        if (_cur_inst.part == inst[i].part)
            _cur_inst.part = nullptr;

    if (_cur_inst.part != nullptr)
        ::free((void *)_cur_inst.part);
}

void engine_free()
{
    free_instrument();
    drum_synth_deinit(_inst, ::free);
}

void load_instrument(uint8_t num)
{
    int n = num - inst_count;
    if (n >= 0 && n < (int)LEN_OF(seeds))
        r.seed = seed = seeds[n].first;
    else
        seed = r.seed;

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

        auto partArgs = (PartArgs *)::malloc(_cur_inst.n * sizeof(PartArgs));
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

    drum_synth_deinit(_inst, ::free);
    _inst = drum_synth_init(&_cur_inst, ::malloc);
}

uint32_t _t = UINT32_MAX;
int32_t last_inst_selection = -1;

void engine::process()
{
    auto buffer = engine::outputBuffer<0>();
    auto bufferAux = engine::outputBuffer<1>();
    memset(buffer, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    memset(bufferAux, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    float tmpL[FRAME_BUFFER_SIZE];
    float tmpR[FRAME_BUFFER_SIZE];

    if (engine::trig())
    {
        if (last_inst_selection != inst_selection)
        {
            last_inst_selection = inst_selection;
            load_instrument(inst_selection);
        }

        _t = 0;

        drum_synth_reset(_inst);
    }

    if (_t < UINT32_MAX)
    {
        DrumParams params = {_t, 0, stretch, stereo, 1.f, 1.f};

        float f = pitch; // powf(2.f, (frame.qz_voltage(this->io, 0)));
        float a = stereo;
        float b = 1.f - a;

        for (size_t k = 0; k < _cur_inst.n; k++)
        {
            {
                drum_synth_process_frame(_inst, k, f, &params, tmpL, tmpR, FRAME_BUFFER_SIZE);
                for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
                {
                    buffer[i] += tmpL[i];
                    bufferAux[i] += tmpR[i];
                }
            }
            // bufferAux[i] += p._vca.value() * p._amp.value() * 0.99f;
        }

        _t += FRAME_BUFFER_SIZE;
    }
}

#include "lib/drumsynth/drumsynth.cpp"
#include "lib/misc/Biquad.cpp"
#include "lib/plaits/resources.cc"
