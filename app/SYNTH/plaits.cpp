// Copyright (C)2022 - E.Heidt
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

// ENGINE_NAME: M-OSC/Virt.Analog
// ENGINE_NAME: M-OSC/Waveshaping
// ENGINE_NAME: M-OSC/2-OP-FM
// ENGINE_NAME: M-OSC/Formant/PD
// ENGINE_NAME: M-OSC/Harmonic
// ENGINE_NAME: M-OSC/Wavetable
// ENGINE_NAME: M-OSC/Chord
// ENGINE_NAME: DRUMS/Analog BD
// ENGINE_NAME: DRUMS/Analog SD
// ENGINE_NAME: DRUMS/Analog HH2
// ENGINE_NAME: DRUMS/Analog HH
// ENGINE_NAME: DRUMS/909ish-BD
// ENGINE_NAME: DRUMS/909ish-SD
// ENGINE_NAME: SYNTH/ClassicVAVCF

#include "../squares-and-circles-api.h"

#include "stmlib/stmlib.h"
#define private public // ;-)
#include "plaits/dsp/voice.h"
#include "stmlib/dsp/dsp.h"

using namespace machine;

plaits::Modulations modulations;
plaits::Voice voice;
plaits::Patch patch;

float bufferOut[FRAME_BUFFER_SIZE];
float bufferAux[FRAME_BUFFER_SIZE];
float out_aux_mix = 0.5f;
float _pitch = 0;
float _base_pitch = DEFAULT_NOTE;

struct
{
    int engine;
    int output; // output=0 -> out, output=1 -> aux, output=3 -> stereo
} _mode = {};

uint8_t *_buffer = nullptr;
plaits::Engine *_plaitsEngine = nullptr;

constexpr int WAVETABLE_ENGINE = 5;
constexpr int CHORD_ENGINE = 6;
constexpr int CLASSIC_VAVCF_ENGINE = 16;

void engine_free()
{
    free(_plaitsEngine);
    free(_buffer);
}

template <class T>
void alloc_engine(size_t mem = 48)
{
    _plaitsEngine = new (malloc(sizeof(T))) T();
    _buffer = (uint8_t *)malloc(mem * sizeof(float));

    stmlib::BufferAllocator allocator;
    allocator.Init(_buffer, mem * sizeof(float));
    _plaitsEngine->Init(&allocator);
}

float harmonics, timbre, morph;

bool is_drum()
{
    return _mode.engine == 13 || _mode.engine == 14 || _mode.engine == 15;
}

void init_params(float hh, float tt, float mm, const plaits::PostProcessingSettings &settings)
{
    patch.harmonics = harmonics = hh;
    patch.timbre = timbre = tt;
    patch.morph = morph = mm;
    if (_plaitsEngine)
        _plaitsEngine->post_processing_settings = settings;
}

void engine::setup()
{
    const char *name = engine::name();

    while (*name++ != '/')
        ;

    if (!strcmp(name, "Virt.Analog"))
        _mode = {0, 0};
    else if (!strcmp(name, "Waveshaping"))
        _mode = {1, 0};
    else if (!strcmp(name, "2-OP-FM"))
        _mode = {2, 0};
    else if (!strcmp(name, "Formant/PD"))
        _mode = {3, 2};
    else if (!strcmp(name, "Harmonic"))
        _mode = {4, 2};
    else if (!strcmp(name, "Wavetable"))
        _mode = {5, 2};
    else if (!strcmp(name, "Chord"))
        _mode = {6, 0};
    else if (!strcmp(name, "Analog BD"))
        _mode = {13, 0};
    else if (!strcmp(name, "Analog SD"))
        _mode = {14, 0};
    else if (!strcmp(name, "Analog HH2"))
        _mode = {15, 1};
    else if (!strcmp(name, "Analog HH"))
        _mode = {15, 0};
    else if (!strcmp(name, "909ish-BD"))
        _mode = {13, 1};
    else if (!strcmp(name, "909ish-SD"))
        _mode = {14, 1};
    else if (!strcmp(name, "ClassicVAVCF"))
        _mode = {16, 2};
    else
        _mode = {0, 0};

    voice.Init();
    patch.engine = 0;
    memset(&patch, 0, sizeof(patch));
    patch.note = DEFAULT_NOTE + _pitch * 12.f;

    modulations.timbre_patched = true;
    patch.timbre_modulation_amount = 0;

    modulations.frequency_patched = true;
    patch.frequency_modulation_amount = 0;

    modulations.morph_patched = true;
    patch.morph_modulation_amount = 0;

    memset(&modulations, 0, sizeof(modulations));

    patch.lpg_colour = 0.5;
    patch.decay = 0.5;
    modulations.trigger_patched = true; // trigger;
    // modulations.level_patched = true;
    // modulations.level = 1;

    switch (_mode.engine)
    {
    case 0:
        alloc_engine<plaits::VirtualAnalogEngine>();
        init_params(0.5f, 0.5f, 0.5f, {0.8f, 0.8f, false});
        engine::addParam(V_OCT, &_pitch);
        engine::addParam("Detune", &harmonics);
        engine::addParam("Square", &timbre);
        engine::addParam("CSAW", &morph);
        engine::addParam("Decay", &patch.decay);
        if (_mode.output == 2)
            engine::addParam("AuxMix", &out_aux_mix, out_aux_mix);
        else
            engine::addParam("Color", &patch.lpg_colour);
        break;
    case 1:
        alloc_engine<plaits::WaveshapingEngine>();
        init_params(0.8f, 0.8f, 0.75f, {0.7f, 0.6f, false});
        engine::addParam(V_OCT, &_pitch);
        engine::addParam("Waveform", &harmonics);
        engine::addParam("Fold", &timbre);
        engine::addParam("Asym", &morph);
        engine::addParam("Decay", &patch.decay);
        if (_mode.output == 2)
            engine::addParam("AuxMix", &out_aux_mix, out_aux_mix);
        else
            engine::addParam("Color", &patch.lpg_colour);
        break;
    case 2:
        alloc_engine<plaits::FMEngine>();
        init_params(0.8f, 0.8f, 0.75f, {0.6f, 0.6f, false});
        engine::addParam(V_OCT, &_pitch);
        engine::addParam("Ratio", &harmonics);
        engine::addParam("Mod", &timbre);
        engine::addParam("Feedb.", &morph);
        engine::addParam("Decay", &patch.decay);
        if (_mode.output == 2)
            engine::addParam("AuxMix", &out_aux_mix, out_aux_mix);
        else
            engine::addParam("Color", &patch.lpg_colour);
        break;
    case 3:
        alloc_engine<plaits::GrainEngine>();
        init_params(0.8f, 0.8f, 0.75f, {0.7f, 0.6f, false});
        engine::addParam(V_OCT, &_pitch);
        engine::addParam("Ratio", &harmonics);
        engine::addParam("Frm/Fq.", &timbre);
        engine::addParam("Width", &morph);
        engine::addParam("Decay", &patch.decay);
        engine::addParam("PD-Mix", &out_aux_mix, out_aux_mix);
        break;
    case 4:
        alloc_engine<plaits::AdditiveEngine>();
        init_params(0.8f, 0.8f, 0.75f, {0.8f, 0.8f, false});
        engine::addParam(V_OCT, &_pitch);
        engine::addParam("Bump", &harmonics);
        engine::addParam("Peak", &timbre);
        engine::addParam("Shape", &morph);
        engine::addParam("Decay", &patch.decay);
        if (_mode.output == 2)
            engine::addParam("AuxMix", &out_aux_mix, out_aux_mix);
        else
            engine::addParam("Color", &patch.lpg_colour);
        break;
    case WAVETABLE_ENGINE:
        alloc_engine<plaits::WavetableEngine>(64 * sizeof(const int16_t *));
        init_params(0.f, 0.8f, 0.75f, {0.6f, 0.6f, false});
        engine::addParam(V_OCT, &_pitch);
        engine::addParam("Bank", &harmonics, 0.f, 0.5f);
        engine::addParam("Row", &timbre);
        engine::addParam("Column", &morph);
        engine::addParam("Decay", &patch.decay);
        if (_mode.output == 2)
            engine::addParam("AuxMix", &out_aux_mix, out_aux_mix);
        else
            engine::addParam("Color", &patch.lpg_colour);
        _plaitsEngine->LoadUserData(nullptr);
        break;
    case CHORD_ENGINE:
    {
        alloc_engine<plaits::ChordEngine>(plaits::kChordNumChords * plaits::kChordNumNotes + plaits::kChordNumChords + plaits::kChordNumNotes);
        init_params(0.5f, 0.5f, 0.5f, {0.8f, 0.8f, false});
        engine::addParam(V_OCT, &_pitch);

        int32_t *pchord = (int32_t *)&static_cast<plaits::ChordEngine *>(_plaitsEngine)->chords_.chord_index_quantizer_.quantized_value_;
        *pchord = 8;
        engine::addParam("Chord", pchord, 0,
                         static_cast<plaits::ChordEngine *>(_plaitsEngine)->chords_.chord_index_quantizer_.num_steps() - 1,
                         (const char **)plaits::chord_names);

        engine::addParam("Inv.", &timbre);
        engine::addParam("Shape", &morph);
        engine::addParam("Decay", &patch.decay);
        if (_mode.output == 2)
            engine::addParam("AuxMix", &out_aux_mix, out_aux_mix);
        else
            engine::addParam("Color", &patch.lpg_colour);
    }
    break;
    // case 15: // speech_engine_
    //     alloc_engine<plaits::SpeechEngine>(false, 0.8f, 0.8f);
    //     break;
    // case 8: // swarm_engine_
    //     alloc_engine<plaits::SwarmEngine>(false, -3.0f, 1.0f);
    //     break;
    // case 9: // noise_engine_
    //     alloc_engine<plaits::NoiseEngine>(false, -1.0f, -1.0f);
    //     break;
    // case 10: // particle_engine_
    //     alloc_engine<plaits::ParticleEngine>(false, -2.0f, 1.0f);
    //     break;
    // case 11: // string_engine_
    //     alloc_engine<plaits::StringEngine>(true, -1.0f, 0.8f);
    //     break;
    // case 12: // modal_engine_
    //     alloc_engine<plaits::ModalEngine>(true, -0.5f, 0.8f);
    //     break;
    case 13:
        _base_pitch += -24.f;
        alloc_engine<plaits::BassDrumEngine>();
        init_params(0.8f, 0.5f, 0.5f, {0.8f, 0.8f, true});
        engine::addParam("Pitch", &_pitch, -1.f, 1.f);
        engine::addParam(_mode.output == 0 ? "Drive" : "Punch", &harmonics);
        engine::addParam("Tone", &timbre);
        engine::addParam("Decay", &morph);
        break;
    case 14:
        alloc_engine<plaits::SnareDrumEngine>();
        init_params(0.5f, 0.5f, 0.5f, {0.8f, 0.8f, true});
        engine::addParam("Pitch", &_pitch, -1.f, 1.f);
        engine::addParam("Snappy", &harmonics);
        engine::addParam("Tone", &timbre);
        engine::addParam("Decay", &morph);
        break;
    case 15:
        alloc_engine<plaits::HiHatEngine>();
        init_params(0.5f, 0.9f, 0.6f, {0.8f, 0.8f, true});
        engine::addParam("Pitch", &_pitch, -1.f, 1.f);
        engine::addParam("Noise", &harmonics);
        engine::addParam("Tone", &timbre);
        engine::addParam("Decay", &morph);
        break;
    // engines 2
    case CLASSIC_VAVCF_ENGINE:
        alloc_engine<plaits::VirtualAnalogVCFEngine>();
        init_params(0.5f, 0.5f, 0.5f, {1.f, 1.f, false});
        engine::addParam(V_OCT, &_pitch);
        engine::addParam("Morph", &morph);
        engine::addParam("Cutoff", &timbre);
        engine::addParam("Harsh", &harmonics);
        out_aux_mix = 0;
        modulations.timbre_patched = false;
        patch.timbre_modulation_amount = 0;
        engine::addParam("EnvMod", &patch.timbre_modulation_amount, -1.f, 1.f);
        patch.decay = 0.5f;
        engine::addParam("Decay", &patch.decay, 0.f, 0.99f);
        break;
#if 0 //TODO....
        case 17:
            alloc_engine<plaits::PhaseDistortionEngine>(plaits::kMaxBlockSize * 4);
            _plaitsEngine->post_processing_settings = {0.7f, 0.7f, false};
            break;
        case 18:
        case 19:
        case 20:
            alloc_engine<plaits::SixOpEngine>(
                plaits::kMaxBlockSize * 4 + plaits::kMaxBlockSize * plaits::kNumSixOpVoices + sizeof(plaits::fm::Patch) * 96 / sizeof(float));
            init_params("", 0.0f, "Mod", 0.5f, "Env", 0.5f, {1.f, 1.f, false});
            param[1].init_presets("Preset", &static_cast<plaits::SixOpEngine *>(_plaitsEngine)->patch_index, 0, 0, 95);
            param[1].print_value = [&](char *name)
            {
                auto i = static_cast<plaits::SixOpEngine *>(_plaitsEngine)->patch_index;
                sprintf(name, ">%.10s", static_cast<plaits::SixOpEngine *>(_plaitsEngine)->patches_[i].name);
            };
            _plaitsEngine->LoadUserData(plaits::fm_patches_table[0]);
            modulations.morph_patched = false;
            param[5].init("EnvMod", &patch.morph_modulation_amount, 0.0f, -1.f, 1.f);

            modulations.timbre_patched = false;
            param[4].init("ModMod", &patch.timbre_modulation_amount, 0.0f, -1.f, 1.f);
            break;
            case 21:
                alloc_engine<plaits::WaveTerrainEngine>(plaits::kMaxBlockSize * 4);
                _plaitsEngine->post_processing_settings = {0.7f, 0.7f, false};
                patch.engine = (engine - 16);
                break;
            case 22:
                alloc_engine<plaits::StringEngine>(16 + 3 * (1024 + 265));
                _plaitsEngine->post_processing_settings = {0.8f, 0.8f, false};
                patch.engine = (engine - 16);
                break;
            case 23:
                alloc_engine<plaits::ChiptuneEngine>(plaits::kChordNumChords * plaits::kChordNumNotes + plaits::kChordNumChords + plaits::kChordNumNotes);
                _plaitsEngine->post_processing_settings = {0.5f, 0.5f, false};
                patch.engine = (engine - 16);
                break;
#endif
    }

    if (_mode.engine >= 16)
        patch.engine = (_mode.engine - 16);

    _plaitsEngine->Reset();
}

void engine::process()
{
    float a = bufferOut[0] / 256.f;
    ONE_POLE(patch.harmonics, harmonics + a, 0.1f);
    ONE_POLE(patch.timbre, timbre + a, 0.1f);
    ONE_POLE(patch.morph, morph + a, 0.1f);

    modulations.level_patched = false;
    modulations.level = 1.f;

    patch.note = _base_pitch + engine::cv() * 12;

    float last_decay = patch.decay;
    float last_morph = patch.morph;
    // if (!frame.trigger && frame.gate)
    // {
    //     if (is_drum())
    //         patch.morph = 1;
    //     else
    //         patch.decay = 1;

    //     modulations.level = 1.f;
    //     // modulations.level_patched = true;
    // }
    modulations.engine = patch.engine;
    modulations.trigger = engine::trig() ? 1 : 0;

    if (!is_drum())
        modulations.trigger_patched = patch.decay < 1.f;

    if (!__io->tr)
    {
        modulations.trigger_patched = false;
        modulations.level_patched = true;
        modulations.level = patch.decay;
        patch.decay = 0.001f;
    }
    else
    {
        modulations.trigger_patched = true;
    }

    modulations.note = 0;
    voice.Render(_plaitsEngine, patch, modulations, bufferOut, bufferAux, FRAME_BUFFER_SIZE);

    patch.decay = last_decay;
    patch.morph = last_morph;

    switch (_mode.output)
    {
    case 0:
        std::copy_n(bufferOut, FRAME_BUFFER_SIZE, engine::outputBuffer<0>());
        std::copy_n(bufferOut, FRAME_BUFFER_SIZE, engine::outputBuffer<1>());
        break;
    case 1:
        std::copy_n(bufferAux, FRAME_BUFFER_SIZE, engine::outputBuffer<0>());
        std::copy_n(bufferAux, FRAME_BUFFER_SIZE, engine::outputBuffer<1>());
        break;
    case 2:
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
            bufferOut[i] = stmlib::Crossfade(bufferOut[i], bufferAux[i], out_aux_mix);
        std::copy_n(bufferOut, FRAME_BUFFER_SIZE, engine::outputBuffer<0>());
        std::copy_n(bufferOut, FRAME_BUFFER_SIZE, engine::outputBuffer<1>());
        break;
    case 3:
        std::copy_n(bufferOut, FRAME_BUFFER_SIZE, engine::outputBuffer<0>());
        std::copy_n(bufferAux, FRAME_BUFFER_SIZE, engine::outputBuffer<1>());
        break;
    }
}

void engine::draw()
{
    engine::setParamName(&patch.decay, __io->tr ? "Decay" : "Level");
}

#include "stmlib/utils/random.cc"
#include "lib/stmlib/dsp/units.cc"

//${SCRIPT_PATH}/main.cc $(find lib/plaits/ -type f -name "*.cc")

#include "lib/plaits/resources.cc"
// lib/plaits/dsp/speech/sam_speech_synth.cc
// lib/plaits/dsp/speech/lpc_speech_synth_controller.cc
// lib/plaits/dsp/speech/lpc_speech_synth_words.cc
// lib/plaits/dsp/speech/lpc_speech_synth_phonemes.cc
// lib/plaits/dsp/speech/naive_speech_synth.cc
// lib/plaits/dsp/speech/lpc_speech_synth.cc
#include "lib/plaits/dsp/engine/string_engine.cc"
#include "lib/plaits/dsp/engine/swarm_engine.cc"
#include "lib/plaits/dsp/engine/chord_engine.cc"
#include "lib/plaits/dsp/engine/waveshaping_engine.cc"
#include "lib/plaits/dsp/engine/modal_engine.cc"
#include "lib/plaits/dsp/engine/fm_engine.cc"
#include "lib/plaits/dsp/engine/snare_drum_engine.cc"
// #include "lib/plaits/dsp/engine/speech_engine.cc"
#include "lib/plaits/dsp/engine/grain_engine.cc"
#include "lib/plaits/dsp/engine/virtual_analog_engine.cc"
#include "lib/plaits/dsp/engine/wavetable_engine.cc"
#include "lib/plaits/dsp/engine/additive_engine.cc"
#include "lib/plaits/dsp/engine/noise_engine.cc"
#include "lib/plaits/dsp/engine/hi_hat_engine.cc"
#include "lib/plaits/dsp/engine/particle_engine.cc"
#include "lib/plaits/dsp/engine/bass_drum_engine.cc"
// lib/plaits/dsp/engine2/chiptune_engine.cc
// lib/plaits/dsp/engine2/six_op_engine.cc
// lib/plaits/dsp/engine2/wave_terrain_engine.cc
// lib/plaits/dsp/engine2/string_machine_engine.cc
// lib/plaits/dsp/engine2/phase_distortion_engine.cc
#include "lib/plaits/dsp/engine2/virtual_analog_vcf_engine.cc"
#include "lib/plaits/dsp/chords/chord_bank.cc"
#include "lib/plaits/dsp/physical_modelling/string_voice.cc"
#include "lib/plaits/dsp/physical_modelling/string.cc"
#include "lib/plaits/dsp/physical_modelling/modal_voice.cc"
#include "lib/plaits/dsp/physical_modelling/resonator.cc"
#include "lib/plaits/dsp/fm/algorithms.cc"
#include "lib/plaits/dsp/fm/dx_units.cc"
#include "lib/plaits/dsp/voice.cc"
