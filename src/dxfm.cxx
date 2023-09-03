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

#include "machine.h"
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

using namespace machine;

template <int INSTANCE>
struct DxFMEngine : public MidiEngine
{
    struct dxfm
    {
        float note = 0;
        int key_down = 0;
        Lfo lfo;
        FmCore fm_core;
        Dx7Note dx7_note;
        stmlib::RingBuffer<int32_t, N * 3> buffer;
    } voices[2];
    Controllers controllers;

    float _pitch;
    uint8_t _prog;
    float _hold;
    float _rate;

    char patch[128] = {// MARIMBA
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

    uint8_t data[156];

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
#define NAME() (const char *)&data[6 * 21 + 19]

    void initControllers()
    {
        controllers.values_[kControllerPitch] = 0x2000;
        controllers.values_[kControllerPitchRange] = 0;
        controllers.values_[kControllerPitchStep] = 0;
        controllers.values_[kControllerPortamentoGlissando] = 0;

        controllers.modwheel_cc = 0;
        controllers.foot_cc = 0;
        controllers.breath_cc = 0;
        controllers.aftertouch_cc = 0;
        controllers.portamento_enable_cc = false;
        controllers.portamento_cc = 0;
        controllers.refresh();
    }

    void applyRate(float rate)
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

    DxFMEngine() : MidiEngine(MIDI_ENGINE | TRIGGER_INPUT | VOCT_INPUT | PRESETS_ENGINE | STEREOLIZED)
    {
        Exp2::init();
        Tanh::init();
        Sin::init();

        Freqlut::init(machine::SAMPLE_RATE);
        Lfo::init(machine::SAMPLE_RATE);
        PitchEnv::init(machine::SAMPLE_RATE);
        Env::init_sr(machine::SAMPLE_RATE);
        Porta::init_sr(machine::SAMPLE_RATE);

        initControllers();
        controllers.masterTune = 0;
        controllers.opSwitch = 0x3f; // all operators

        param[0].init_v_oct("Freq", &_pitch);
        param[1].init(">", &_prog, 1, 0, 32);
        param[1].value_changed = [&]()
        {
            loadPatch = true;
        };
        param[2].init("Rate", &_rate, -1.0f, -1.5f, -0.5f);
        param[2].value_changed = [&]()
        {
            applyRate(_rate); // * ((float)rand() / INT32_MAX * 0.1f));
        };
        param[3].init("Hold", &_hold, 0, 0, SAMPLE_RATE / FRAME_BUFFER_SIZE);

        UnpackPatch(patch, (char *)data);
        loadDXPatch(1); // If patch available, select first
    }

    const char DXFM_PATCH[8] = {'D', 'X', 'F', 'M', 'S', 'Y', 'X', '0' + INSTANCE};

    void loadDXPatch(uint8_t prog)
    {
        if (const uint8_t *sysexData = machine::flash_read(DXFM_PATCH))
        {
            bool valid = false;
            for (int i = 0; i < 128; i++)
                valid |= sysexData[i] != 0 && sysexData[i] != 0xFF;

            if (valid)
            {
                memcpy(patch, (const char *)sysexData + ((prog - 1) * 128), sizeof(patch));
                UnpackPatch(patch, (char *)data);
                loadPatch = false;
                _prog = prog;
                return;
            }
        }

        _prog = 0;
        return;
    }

    void onMidiNote(uint8_t key, uint8_t velocity) override
    {
        // Not yet implemented...
    }
    void onMidiPitchbend(int16_t pitch) override
    {
        // Not yet implemented...
    }
    void onMidiCC(uint8_t ccc, uint8_t value) override
    {
        // Not yet implemented...
    }

    char name[16];

    void display() override
    {
        if (_prog == 0)
        {
            char tmp[64];
            auto bak = param[0].name;
            param[0].name = nullptr;
            gfx::drawEngine(this);
            param[0].name = bak;

            gfx::drawRect(4, 16, 120, 38);
            gfx::drawRect(5, 17, 118, 36);
            machine::get_io_info(1, 0, name);
            sprintf(tmp, "  Listening on [%s]", name);
            gfx::drawString(0, 22, tmp, 1);
            sprintf(tmp, "  Data %.4d of 4096", dmod_received);
            gfx::drawString(0, 32, tmp, 1);

            // level = std::max(level, fabsf(machine::get_cv<float>(dmod_port) * 51));
            gfx::drawRect(13, 42, 102, 6);
            gfx::fillRect(13, 42, ((float)dmod_received / 4096) * 101, 6);
            return;
        }

        gfx::drawEngine(this);

        gfx::setColor(0);
        gfx::fillRect(64, 20, 64, 10);
        gfx::setColor(1);

        sprintf(name, "%02d:", _prog - 1);
        gfx::drawString(69, 16, name, 0);
        name[0] = param[1].flags & Parameter::IS_SELECTED ? '>' : ' ';
        sprintf(&name[1], "%.10s", NAME());
        gfx::drawString(62, 22, name, 1);

        // gfx::setColor(trig);
        // gfx::fillRect(124, 12, 4, 4);
        // gfx::setColor(1);
    }

    float bufferL[machine::FRAME_BUFFER_SIZE];
    float bufferR[machine::FRAME_BUFFER_SIZE];

    uint8_t trig = 0;
    bool loadPatch = true;

    int16_t dmod_samples[machine::FRAME_BUFFER_SIZE];
    float dmod_in[machine::FRAME_BUFFER_SIZE];
    const int dmod_port = 0;
    size_t dmod_received = 0;

    void dmod_listen(OutputFrame &of)
    {
        if (loadPatch)
        {
            loadPatch = false;
            machine::dmod_init_reception(DXFM_PATCH, 4096);
        }

        memset(dmod_in, 0, sizeof(dmod_in));
        machine::get_audio(dmod_port, dmod_in, 1.f);

        for (size_t i = 0; i < machine::FRAME_BUFFER_SIZE; i++)
            dmod_samples[i] = dmod_in[i] * INT16_MAX;

        if (machine::dmod_process(dmod_samples, dmod_received))
        {
            machine::message("..loading patch.");

            _prog = 1;
            loadPatch = true;
        }

        of.push(dmod_samples, machine::FRAME_BUFFER_SIZE);
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        if (_prog == 0)
        {
            dmod_listen(of);
            return;
        }

        int velo = 127;
        int porta = -1;

        if (frame.trigger)
        {
            trig |= 1;
            int key_down = INT32_MAX;
            auto next = &voices[0];
            for (auto &voice : voices)
                if (voice.key_down < key_down)
                {
                    key_down = voice.key_down;
                    next = &voice;
                }
            next->key_down = 0;
        }
        else if (frame.gate)
        {
            trig |= 2;
        }
        else
        {
            for (auto &voice : voices)
                if (voice.key_down > 0)
                    --voice.key_down;
        }

        if (voices[0].buffer.readable() < N)
        {
            if (loadPatch)
            {
                loadDXPatch(_prog);
                loadPatch = false;

                for (auto &voice : voices)
                {
                    voice.lfo.reset(&LFO_RATE());
                    voice.dx7_note.update(data, voice.note, velo, porta, &controllers);
                }
            }

            // see midinote_to_logfreq
            controllers.masterTune = (frame.qz_voltage(this->io, _pitch)) * (1 << 24);

            for (auto &voice : voices)
            {
                if ((trig & 1) && voice.key_down == 0)
                {
                    voice.key_down = _hold;
                    voice.note = (float)machine::DEFAULT_NOTE;
                    voice.lfo.keydown();
                    voice.dx7_note.keyup();
                    voice.dx7_note.init(data, voice.note, velo, voice.note, porta, &controllers);

                    if (OSC_SYNC())
                        voice.dx7_note.oscSync();

                    trig = 0;
                }
                else if (voice.key_down == 1)
                {
                    voice.dx7_note.keyup();
                }

                int32_t lfovalue = voice.lfo.getsample();
                int32_t lfodelay = voice.lfo.getdelay();

                auto p = voice.buffer.OverwritePtr(N);
                memset(p, 0, sizeof(int32_t) * N);
                voice.dx7_note.compute(p, &voice.fm_core, lfovalue, lfodelay, &controllers);
            }

            trig = 0;
        }

        memset(bufferL, 0, sizeof(bufferL));
        memset(bufferR, 0, sizeof(bufferL));

        int v = 0;
        for (auto &voice : voices)
        {
            auto p = voice.buffer.ImmediateReadPtr(machine::FRAME_BUFFER_SIZE);

            if (v++ % 2 == 0)
            {
                for (int i = 0; i < machine::FRAME_BUFFER_SIZE; i++)
                {
                    float s = (float)*p++ / (INT32_MAX / 8); // signed_saturate_rshift(*p++ >> 4, 24, 9) / 32768.0f;
                    bufferL[i] += s * (1.f - 1.f / 256.f * this->io->stereo);
                    bufferR[i] += s;
                }
            }
            else
            {
                for (int i = 0; i < machine::FRAME_BUFFER_SIZE; i++)
                {
                    float s = (float)*p++ / (INT32_MAX / 8); // signed_saturate_rshift(*p++ >> 4, 24, 9) / 32768.0f;
                    bufferL[i] += s;
                    bufferR[i] += s * (1.f - 1.f / 256.f * this->io->stereo);
                }
            }
        }

        of.out = bufferL;
        of.aux = bufferR;
    }
};

void init_dxfm()
{
    machine::add<DxFMEngine<0>>(SYNTH, "DxFM");
    // machine::add<DxFMEngine<0>>(SYNTH, "DxFM_2");
    // machine::add<DxFMEngine<2>>(SYNTH, "DxFM_3");
}

MACHINE_INIT(init_dxfm);