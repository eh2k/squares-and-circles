/*
 * Copyright (C)2023 - E.Heidt
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


// ENGINE_NAME:SYNTH/DxFM;SYNTH/DxFM_BNK1-3

#include "../squares-and-circles-api.h"
#include "stmlib/utils/ring_buffer.h"
#include "misc/dspinst.h"

#include "msfa/controllers.h"
#include "msfa/dx7note.h"
#include "msfa/lfo.h"
#include "msfa/synth.h"
#include "msfa/fm_core.h"

#include "msfa/synth.h"
#include "msfa/exp2.h"
#include "msfa/sin.h"
#include "msfa/freqlut.h"
#include "msfa/porta.h"
#include "msfa/patch.h"

#include <unistd.h>
#include <limits.h>
#include "plaits/resources.h"

struct dxfm
{
    int note = 0;
    int key_down = 0;
    Lfo lfo;
    FmCore fm_core;
    Dx7Note dx7_note;
    stmlib::RingBuffer<int32_t, N * 3> buffer;
};

static dxfm voices[2] = {};
static Controllers controllers = {};

static float _pitch = 0;
static int32_t _prog = 0;
static char _progName[16] = {};
static int32_t _progSelect = 0;
static float _hold = 0;
static float _rate = 0;

static float _stereolize = 0;

static dxfm *active_voice = nullptr;
static dxfm *other_voice = nullptr;

const uint8_t *fm_patches_table[4] = {};

static char patch[128] = { // MARIMBA
    0x00, 0x3f, 0x37, 0x00, 0x4e, 0x4e, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00,
    0x38, 0x08, 0x63, 0x08, 0x0d, 0x63, 0x4b, 0x00, 0x08, 0x52, 0x30, 0x00,
    0x00, 0x36, 0x00, 0x2e, 0x00, 0x38, 0x08, 0x5d, 0x00, 0x32, 0x63, 0x4b,
    0x00, 0x52, 0x52, 0x30, 0x00, 0x00, 0x36, 0x00, 0x2e, 0x00, 0x38, 0x08,
    0x55, 0x0a, 0x00, 0x5f, 0x21, 0x31, 0x29, 0x63, 0x5c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3b, 0x04, 0x63, 0x00, 0x00, 0x63, 0x48, 0x00, 0x00,
    0x52, 0x30, 0x00, 0x00, 0x36, 0x00, 0x2e, 0x00, 0x38, 0x08, 0x60, 0x06,
    0x00, 0x5f, 0x28, 0x31, 0x37, 0x63, 0x5c, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x3b, 0x00, 0x5f, 0x00, 0x00, 0x5e, 0x43, 0x5f, 0x3c, 0x32, 0x32,
    0x32, 0x32, 0x06, 0x08, 0x23, 0x00, 0x00, 0x00, 0x31, 0x18, 0x4d, 0x41,
    0x52, 0x49, 0x4d, 0x42, 0x41, 0x20, 0x20, 0x20};

static uint8_t data[156] = {};

#define OP_ENV_RATE(op, i) data[op * 21 + i]
#define OP_ENV_LEVEL(op, i) data[op * 21 + 4 + i]
#define OP_kbd_lev_scl_brk_pt(op) data[op * 21 + 8]
#define OP_kbd_lev_scl_lft_depth(op) data[op * 21 + 9]
#define OP_kbd_lev_scl_rht_depth(op) data[op * 21 + 10]
#define OP_kbd_lev_scl_lft_curve(op) data[op * 21 + 11]
#define OP_kbd_lev_scl_rht_curve(op) data[op * 21 + 12]
#define OP_kbd_rate_scaling(op) data[op * 21 + 13]
#define OP_amp_mod_sensitivity(op) data[op * 21 + 14]
#define OP_key_vel_sensitivity(op) data[op * 21 + 15]
#define OP_output_level(op) data[op * 21 + 16]
#define OP_osc_mode(op) data[op * 21 + 17]
#define OP_osc_freq_coarse(op) data[op * 21 + 18]
#define OP_osc_freq_fine(op) data[op * 21 + 19]
#define OP_osc_detune(op) data[op * 21 + 20]
#define PE_rate(i) data[6 * 21 + i]
#define PE_level(i) data[6 * 21 + 4 + i]
#define ALGORITHM() data[6 * 21 + 8]
#define FEEDBACK() data[6 * 21 + 9]
#define OSC_SYNC() data[6 * 21 + 10]
#define LFO_RATE() data[(6 * 21 + 11)]
#define LFO_DELAY() data[6 * 21 + 12]
#define LFO_PITCH_MOD_DEPTH() data[6 * 21 + 13]
#define LFO_AMP_MOD_DEPTH() data[6 * 21 + 14]
#define LFO_SYNC() data[6 * 21 + 15]
#define LFO_WAVEFORM() data[6 * 21 + 16]
#define PITCH_MOD_SENSITIVITY() data[6 * 21 + 17]
#define TRANSPOSE() data[6 * 21 + 18]
#define NAME() ((const char *)&data[6 * 21 + 19])

static void initControllers()
{
    controllers.values_[kControllerPitch] = 0x2000;
    controllers.values_[kControllerPitchRange] = 0;
    controllers.values_[kControllerPitchStep] = 0;

    controllers.modwheel_cc = 0;
    controllers.foot_cc = 0;
    controllers.breath_cc = 0;
    controllers.aftertouch_cc = 0;
    controllers.refresh();
}

static uint8_t loadDXPatch(uint8_t prog)
{
    uint8_t load = prog;
    const uint8_t *sysexData = fm_patches_table[prog / 32];
    prog = prog % 32;

    if (sysexData)
    {
        bool invalid = false;
        for (int i = 0; i < sizeof(patch); i++)
            invalid |= sysexData[i + prog * 128] & 0x80;

        if (!invalid)
        {
            memcpy(patch, (const char *)sysexData + (prog * 128), sizeof(patch));
            UnpackPatch(patch, (char *)data);

            char *name = _progName;
            memset(_progName, 0, sizeof(_progName));
            for (int i = 0; i < 10; i++)
            {
                *name = NAME()[i];
                if (*name >= ' ' && *name <= '~')
                {
                    if (i == 0 || *name != ' ' || *(name - 1) != *name)
                        name++;
                }
                else
                {
                    *name = 0;
                    break;
                }
            }
            return load;
        }
    }

    return 0;
}

static void applyRate(float rate)
{
    for (int op = 0; op < 6; op++)
    {
        auto opData = &patch[op * 17];
        for (int j = 1; j < 4; j++)
        {
            auto v = min(opData[j] & 0x7f, 99);
            OP_ENV_RATE(op, j) = min((int)(v * -rate), 99);
        }

        // for (int j = 1; j < 4; j++)
        // {
        //     auto v = min(opData[j + 4] & 0x7f, 99);
        //     if (_rate < 0)
        //         OP_ENV_LEVEL(op, j) = v - (99 - v) * -_rate;
        //     else
        //         OP_ENV_LEVEL(op, j) = v - v * -_rate;
        // }
    }
}

void engine::setup()
{
    UnpackPatch(patch, (char *)data);

    uint8_t nprogs = 0;

    if (!strcmp(engine::name(), "SYNTH/DxFM_BNK1-3"))
    {
        if (fm_patches_table[nprogs / 32] = plaits::fm_patches_table[0])
            nprogs += 32;
        if (fm_patches_table[nprogs / 32] = plaits::fm_patches_table[1])
            nprogs += 32;
        if (fm_patches_table[nprogs / 32] = plaits::fm_patches_table[2])
            nprogs += 32;
    }
    else
    {
        if (fm_patches_table[nprogs / 32] = machine::fs_read("DXFMSYX0"))
            nprogs += 32;
    }

    if (nprogs > 96)
        _progSelect = 96;

    for (size_t prog = 0; prog < nprogs; prog++)
    {
        const uint8_t *sysexData = fm_patches_table[prog / 32];
    }

    engine::addParam(IO_STEREOLIZE, &_stereolize);
    engine::addParam(V_OCT, &_pitch);

    if (nprogs == 0)
    {
        nprogs = 1;
    }

    snprintf(_progName, 12, NAME());
    engine::addParam(_progName, &_progSelect, 0, nprogs - 1);

    _rate = -1.0f;
    engine::addParam("Rate", &_rate, -1.5f, -0.5f);

    _hold = 0;
    engine::addParam("Hold", &_hold, 0, SAMPLE_RATE / FRAME_BUFFER_SIZE);

    Exp2::init();
    Tanh::init();
    Sin::init();

    Freqlut::init(SAMPLE_RATE);
    Lfo::init(SAMPLE_RATE);
    PitchEnv::init(SAMPLE_RATE);
    Env::init_sr(SAMPLE_RATE);
    Porta::init_sr(SAMPLE_RATE);

    initControllers();
    controllers.masterTune = 0;
    controllers.opSwitch = 0x3f; // all operators
    active_voice = &voices[0];
    other_voice = &voices[1];

    for (auto &voice : voices)
        voice.buffer.Init();

    loadDXPatch(0); // If patch available, select first
}

static uint8_t _trig = 0;

void engine::process()
{
    applyRate(_rate); // * ((float)rand() / INT32_MAX * 0.1f));

    int velo = 127;

    if (engine::trig())
    {
        _trig |= 1;
    }
    else if (engine::gate())
    {
        // keep key down
    }
    else
    {
        for (auto &voice : voices)
            if (voice.key_down > 1)
                --voice.key_down;
    }

    if (voices[0].buffer.readable() < N)
    {
        // see midinote_to_logfreq
        float note = (2.f + engine::cv()) * 12.f + DEFAULT_NOTE;

        if (_progSelect != _prog)
        {
            _prog = loadDXPatch(_progSelect);

            for (auto &voice : voices)
            {
                voice.dx7_note.keyup();
                voice.lfo.reset(&LFO_RATE());
                voice.dx7_note.update(data, voice.note, velo);
            }
        }

        for (auto &voice : voices)
        {
            if ((_trig & 1) && &voice == other_voice)
            {
                voice.key_down = 2 + _hold;
                voice.note = note;
                voice.lfo.keydown();
                voice.dx7_note.keyup();

                float r = _rate + 1.5f; // 0..1
                if (r < 0.5f)
                    velo = 64 + 64 * (r * 2);

                voice.dx7_note.init(data, voice.note, velo);

                if (OSC_SYNC())
                    voice.dx7_note.oscSync();

                _trig = 0;

                // if (other_voice && active_voice->key_down)
                // {
                //     voice.dx7_note.transferSignal(active_voice->dx7_note);
                //     voice.dx7_note.transferState(active_voice->dx7_note);
                // }

                std::swap(active_voice, other_voice);
            }
            else if (voice.key_down == 1)
            {
                voice.key_down = 0;
                voice.dx7_note.keyup();
            }

            int32_t lfovalue = voice.lfo.getsample();
            int32_t lfodelay = voice.lfo.getdelay();

            auto p = voice.buffer.OverwritePtr(N);
            memset(p, 0, sizeof(int32_t) * N);

            float diff_note = (note - voice.note) / 12.f;
            controllers.masterTune = diff_note * (1 << 24);
            voice.dx7_note.compute(p, &voice.fm_core, lfovalue, lfodelay, &controllers);
        }

        _trig = 0;
    }

    auto bufferL = engine::outputBuffer<0>();
    auto bufferR = engine::outputBuffer<1>();

    int v = 0;
    float stereo = (1.f - _stereolize);
    for (auto &voice : voices)
    {
        auto p = voice.buffer.ImmediateReadPtr(FRAME_BUFFER_SIZE);

        if (v++ % 2 == 0)
        {
            for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
            {
                float s = (float)*p++ / INT32_MAX * 8;
                bufferL[i] = s * stereo;
                bufferR[i] = s;
            }
        }
        else
        {
            for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
            {
                float s = (float)*p++ / INT32_MAX * 8;
                bufferL[i] += s;
                bufferR[i] += s * stereo;
            }
        }
    }
}

void engine::draw()
{
    if (fm_patches_table[0] == nullptr)
    {
        gfx::clearRect(4, 16, 120, 38);
        gfx::drawRect(4, 16, 120, 38);
        gfx::drawRect(5, 17, 118, 36);
        gfx::drawStringCenter(64, 26, "NO DXFM.SYX FOUND!", 1);
        gfx::drawString(10, 38, "Please load a syx file", 0);
        gfx::drawString(10, 38 + 7, " with the Webflasher! ", 0);
    }
}

#include "msfa/sin.cc"
#include "msfa/pitchenv.cc"
#include "msfa/fm_op_kernel.cc"
#include "msfa/fm_core.cc"
#include "msfa/lfo.cc"
#include "msfa/dx7note.cc"
#include "msfa/freqlut.cc"
#include "msfa/patch.cc"
#include "msfa/porta.cc"
#include "msfa/env.cc"
#include "msfa/exp2.cc"
#include "plaits/resources.cc"