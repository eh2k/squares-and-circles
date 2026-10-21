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

#include "../../squares-and-circles-api.h"
#include "stmlib/algorithms/voice_allocator.h"

// Engines that are left out (0) or built (1), here or with -DPLAITS_...=1 in a build_flags line. build.sh and
// sync_readme.sh leave out an ENGINE_NAME the #if leaves out
// SPEECH/Plaits, Plaits' speech engine (formant, SAM and LPC vowels, the LPC words): 21 KB more code; the words are
// SPEECH/LPC
#ifndef PLAITS_SPEECH
#define PLAITS_SPEECH 0
#endif
// SYNTH/6-OP-FM, Plaits' DX7 engine with its 96 patches (SYNTH/DxFM_BNK1-3 plays them with msfa): 30 KB more code
#ifndef PLAITS_SIX_OP
#define PLAITS_SIX_OP 0
#endif

ENGINE_NAME("M-OSC/Virt.Analog");
ENGINE_NAME("M-OSC/Waveshaping");
ENGINE_NAME("M-OSC/2-OP-FM");
ENGINE_NAME("M-OSC/Grain");
ENGINE_NAME("M-OSC/Harmonic");
ENGINE_NAME("M-OSC/Wavetable");
ENGINE_NAME("M-OSC/Chord");
ENGINE_NAME("DRUMS/Analog BD");
ENGINE_NAME("DRUMS/Analog SD");
ENGINE_NAME("DRUMS/Analog HH2");
ENGINE_NAME("DRUMS/Analog HH");
ENGINE_NAME("DRUMS/909ish-BD");
ENGINE_NAME("DRUMS/909ish-SD");
ENGINE_NAME("SYNTH/ClassicVAVCF");
ENGINE_NAME("M-OSC/Swarm");
ENGINE_NAME("NOISE/Filtered");
ENGINE_NAME("NOISE/Particle");
ENGINE_NAME("SYNTH/String");
ENGINE_NAME("SYNTH/Modal");
#if PLAITS_SPEECH
ENGINE_NAME("SPEECH/Plaits");
#endif
ENGINE_NAME("M-OSC/PhaseDist");
ENGINE_NAME("M-OSC/WaveTerrain");
ENGINE_NAME("SYNTH/Str.Machine");
ENGINE_NAME("M-OSC/Chiptune");
#if PLAITS_SIX_OP
ENGINE_NAME("SYNTH/6-OP-FM");
#endif

// The oscillator engines are polyphonic over MIDI: the Trig/CV voice and four MIDI voices, each a plaits::Voice
// (LPG, decay envelope) with an engine of its own. The drums and the speech engine (PLAITS_SPEECH) are the Trig/CV
// voice alone and no MIDI engine.

#define private public // ;-)
#include "plaits/dsp/voice.h"
#include "stmlib/dsp/dsp.h"

using namespace machine;

struct poly_voice
{
    plaits::Voice voice;
    plaits::Engine *engine = nullptr;
    float note = DEFAULT_NOTE;
    float gain = 1.f;     // MIDI velocity
    bool trigger = false; // MIDI voices: triggered in the last frame
    bool idle = true;     // silent: not computed until the next trigger
    uint8_t quiet = 0;    // frames in a row below kQuiet
    bool held = false;      // after its ping the LPG stays open while the key (Trig/CV: the gate) is held
    // MIDI voices: set by the handlers, taken over by process()
    uint8_t key = 0;
    uint8_t velocity = 0;
    bool pending = false;
    bool sustained = false; // released while the sustain pedal is down
};

// Trig/CV: one voice, retriggered like Plaits' own (its LPG goes on from where it is, nothing is cut off); two
// voices in turn would sound the same note twice, out of phase, when the sequence repeats a note
constexpr int CV_VOICES = 1;
constexpr int MIDI_VOICES = 4; // ENGINE_MODE_MIDI_IN: polyphonic (on the DSM a voice is 16-22k of 240k cycles a frame)
constexpr float kQuiet = 1.f / 16384; // -84 dB
constexpr uint8_t kQuietFrames = 16;  // 8 ms

static poly_voice _voices[CV_VOICES + MIDI_VOICES];
static poly_voice &_cv_voice = _voices[0];
static poly_voice *const _midi_voices = &_voices[CV_VOICES];
static stmlib::VoiceAllocator<MIDI_VOICES> _allocator;

plaits::Modulations modulations;
plaits::Patch patch;

float bufferOut[FRAME_BUFFER_SIZE];
float bufferAux[FRAME_BUFFER_SIZE];
float out_aux_mix = 0.5f;
float _pitch = 0;
float _v_oct = 0;
float _base_pitch = DEFAULT_NOTE;
int32_t _chord = 8;

static float _last_out = 0;       // first sample of the last frame: dithers the knobs
static float _bend = 0;           // semitones, +-12 like the firmware's pitch bend for V/OCT
static bool _sustain = false;     // CC 64
static uint8_t _midi_noteons = 0; // Note Ons since the last process()
static bool _midi_seen = false;   // a MIDI note came in: no drone any more

struct
{
    int engine;
    int output; // output=0 -> out, output=1 -> aux, output=2 -> out/aux crossfade (mono)
} _mode = {};

// _mode.engine: the original Plaits' engines 0 ... 15 (green and red bank), those of Plaits 1.2 16 ... 23 (orange
// bank, dsp/engine2; 6-OP FM 18 with the patches of all three of Plaits' banks, 19 and 20 not used)
constexpr int WAVETABLE_ENGINE = 5;
constexpr int CHORD_ENGINE = 6;
constexpr int SPEECH_ENGINE = 7;
constexpr int SWARM_ENGINE = 8;
constexpr int NOISE_ENGINE = 9;
constexpr int PARTICLE_ENGINE = 10;
constexpr int STRING_ENGINE = 11;
constexpr int MODAL_ENGINE = 12;
constexpr int CLASSIC_VAVCF_ENGINE = 16;
constexpr int PHASE_DISTORTION_ENGINE = 17;
constexpr int SIX_OP_ENGINE = 18;
constexpr int WAVE_TERRAIN_ENGINE = 21;
constexpr int STRING_MACHINE_ENGINE = 22;
constexpr int CHIPTUNE_ENGINE = 23;

// One string of plaits::StringEngine, which rings three in turn: a MIDI voice is one string (5 KB, not 15 KB)
struct SingleStringEngine : public plaits::Engine
{
    plaits::StringVoice voice_;
    float *temp_buffer_;

    void Init(stmlib::BufferAllocator *allocator) override
    {
        temp_buffer_ = allocator->Allocate<float>(plaits::kMaxBlockSize);
        voice_.Init(allocator);
    }
    void Reset() override { voice_.Reset(); }
    void LoadUserData(const uint8_t *user_data) override {}
    void Render(const plaits::EngineParameters &parameters, float *out, float *aux, size_t size,
                bool *already_enveloped) override
    {
        std::fill(&out[0], &out[size], 0.0f);
        std::fill(&aux[0], &aux[size], 0.0f);
        voice_.Render(parameters.trigger & plaits::TRIGGER_UNPATCHED, parameters.trigger & plaits::TRIGGER_RISING_EDGE,
                      parameters.accent, plaits::NoteToFrequency(parameters.note), parameters.harmonics,
                      parameters.timbre * parameters.timbre, parameters.morph, temp_buffer_, out, aux, size);
    }
};

// The MIDI voices of the chiptune engine play the chord through the LPG, as the other engines' voices do: Plaits'
// arpeggiator, clocked by the Trig, with its own envelope and no LPG, is the Trig/CV voice's
struct ChiptuneChordEngine : public plaits::ChiptuneEngine
{
    void Render(const plaits::EngineParameters &parameters, float *out, float *aux, size_t size,
                bool *already_enveloped) override
    {
        plaits::EngineParameters p = parameters;
        p.trigger = plaits::TriggerState(p.trigger | plaits::TRIGGER_UNPATCHED);
        set_envelope_shape(NO_ENVELOPE);
        plaits::ChiptuneEngine::Render(p, out, aux, size, already_enveloped);
    }
};

#if PLAITS_SIX_OP
// Plaits' 6-OP FM with a gate of its own: the DX envelopes hold while the key (or the Trig/CV voice's gate) is held, in
// Plaits while TRIG is high; here a Trig is one frame. Two FM voices in turn, as in Plaits, so a release rings on under
// the next note. The MIDI voices play the 96 patches of the Trig/CV voice's engine (patches_), 15 KB once
struct SixOpGateEngine : public plaits::SixOpEngine
{
    bool gate_ = false;     // key or gate held (render())
    bool armed_ = false;    // the gate holds from the note on: the Trig of the Trig/CV voice arrives 7 frames late
    int short_gate_ = 0;    // the gate stays high 4 frames (2 ms) after a note on at least: the two FM voices are
                            // computed in turn, each every other frame (Plaits' staggered rendering), and the first
                            // render with a new patch only sets it up (fm::Voice::Render()); a shorter gate is missed
    float velocity_ = -1.f; // MIDI voices: the key's velocity (DX: each operator by its sensitivity), else the accent

    void Render(const plaits::EngineParameters &parameters, float *out, float *aux, size_t size,
                bool *already_enveloped) override
    {
        plaits::EngineParameters p = parameters;
        if (!(p.trigger & plaits::TRIGGER_UNPATCHED))
        {
            const bool rising = p.trigger & plaits::TRIGGER_RISING_EDGE;
            armed_ = gate_ && (armed_ || rising);
            short_gate_ = rising ? 2 * plaits::kNumSixOpVoices : std::max(short_gate_ - 1, 0);
            const bool gate = armed_ || short_gate_ > 0; // a short Trig or note: 2 ms, as a pulse at Plaits' TRIG
            p.trigger = plaits::TriggerState((p.trigger & ~plaits::TRIGGER_HIGH) | (gate ? plaits::TRIGGER_HIGH : 0));
            if (velocity_ >= 0)
                p.accent = velocity_;
        }
        plaits::SixOpEngine::Render(p, out, aux, size, already_enveloped);
    }
};

static int32_t _patch = 0;
static const char **_patch_names = nullptr; // the knob's labels: the patches' names
#endif

// buffer for plaits::Engine::Init(), in floats
template <class T>
constexpr size_t mem_floats(size_t n = 1)
{
    return (n * sizeof(T) + sizeof(float) - 1) / sizeof(float);
}

constexpr size_t kChordBankMem = mem_floats<float>(plaits::kChordNumChords * plaits::kChordNumNotes) +
                                 mem_floats<int>(plaits::kChordNumChords) + mem_floats<float>(plaits::kChordNumNotes);

// an engine for the voices first ... n - 1 (the drums: the Trig/CV voice)
template <class T>
void alloc_engines(size_t mem = 48, int n = CV_VOICES + MIDI_VOICES, int first = 0)
{
    for (int i = first; i < n; i++)
    {
        auto &v = _voices[i];
        void *engine_mem = malloc(sizeof(T));
        uint8_t *buffer = (uint8_t *)malloc(mem * sizeof(float));
        if (engine_mem == nullptr || buffer == nullptr)
        {
            gfx::message("OUT OF MEMORY"); // the voices from here on stay nullptr: silent
            return;
        }
        v.engine = new (engine_mem) T();

        stmlib::BufferAllocator allocator;
        allocator.Init(buffer, mem * sizeof(float));
        v.engine->Init(&allocator);
    }
}

float harmonics, timbre, morph;

bool is_drum()
{
    return _mode.engine == 13 || _mode.engine == 14 || _mode.engine == 15;
}

// a MIDI engine with MIDI voices; the drums and the speech engine (its LPC word bank is 14 KB) are one voice
bool is_poly()
{
    return !is_drum() && _mode.engine != SPEECH_ENGINE;
}

// the Chord knob of the engines that take the chord from HARMONICS (ChordBank::set_chord(), quantizer centre)
static bool _chord_knob = false;

void init_params(float hh, float tt, float mm, const plaits::PostProcessingSettings &settings)
{
    patch.harmonics = harmonics = hh;
    patch.timbre = timbre = tt;
    patch.morph = morph = mm;
    for (auto &v : _voices)
        if (v.engine)
            v.engine->post_processing_settings = settings;
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
    else if (!strcmp(name, "Grain"))
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
    else if (!strcmp(name, "Swarm"))
        _mode = {SWARM_ENGINE, 2};
    else if (!strcmp(name, "Filtered"))
        _mode = {NOISE_ENGINE, 2};
    else if (!strcmp(name, "Particle"))
        _mode = {PARTICLE_ENGINE, 2};
    else if (!strcmp(name, "String"))
        _mode = {STRING_ENGINE, 0};
    else if (!strcmp(name, "Modal"))
        _mode = {MODAL_ENGINE, 0};
#if PLAITS_SPEECH
    else if (!strcmp(name, "Plaits"))
        _mode = {SPEECH_ENGINE, 2};
#endif
    else if (!strcmp(name, "PhaseDist"))
        _mode = {PHASE_DISTORTION_ENGINE, 2};
    else if (!strcmp(name, "WaveTerrain"))
        _mode = {WAVE_TERRAIN_ENGINE, 2};
    else if (!strcmp(name, "Str.Machine"))
        _mode = {STRING_MACHINE_ENGINE, 2};
    else if (!strcmp(name, "Chiptune"))
        _mode = {CHIPTUNE_ENGINE, 2};
#if PLAITS_SIX_OP
    else if (!strcmp(name, "6-OP-FM"))
        _mode = {SIX_OP_ENGINE, 0};
#endif
    else
        _mode = {0, 0};

    for (auto &v : _voices)
        v.voice.Init();

    memset(&patch, 0, sizeof(patch));
    patch.note = DEFAULT_NOTE;
    memset(&modulations, 0, sizeof(modulations));

    patch.lpg_colour = 0.5;
    patch.decay = 0.5;

    switch (_mode.engine)
    {
    case 0:
        alloc_engines<plaits::VirtualAnalogEngine>();
        init_params(0.5f, 0.5f, 0.5f, {0.8f, 0.8f, false});
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Detune", &harmonics);
        engine::addParam("Square", &timbre);
        engine::addParam("CSAW", &morph);
        engine::addParam("Decay", &patch.decay);
        if (_mode.output == 2)
            engine::addParam("AuxMix", &out_aux_mix);
        else
            engine::addParam("Color", &patch.lpg_colour);
        break;
    case 1:
        alloc_engines<plaits::WaveshapingEngine>();
        init_params(0.8f, 0.8f, 0.75f, {0.7f, 0.6f, false});
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Waveform", &harmonics);
        engine::addParam("Fold", &timbre);
        engine::addParam("Asym", &morph);
        engine::addParam("Decay", &patch.decay);
        if (_mode.output == 2)
            engine::addParam("AuxMix", &out_aux_mix);
        else
            engine::addParam("Color", &patch.lpg_colour);
        break;
    case 2:
        alloc_engines<plaits::FMEngine>();
        init_params(0.8f, 0.8f, 0.75f, {0.6f, 0.6f, false});
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Ratio", &harmonics);
        engine::addParam("Mod", &timbre);
        engine::addParam("Feedb.", &morph);
        engine::addParam("Decay", &patch.decay);
        if (_mode.output == 2)
            engine::addParam("AuxMix", &out_aux_mix);
        else
            engine::addParam("Color", &patch.lpg_colour);
        break;
    case 3:
        alloc_engines<plaits::GrainEngine>();
        init_params(0.8f, 0.8f, 0.75f, {0.7f, 0.6f, false});
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Ratio", &harmonics);
        engine::addParam("Frm/Fq.", &timbre);
        engine::addParam("Width", &morph);
        engine::addParam("Decay", &patch.decay);
        engine::addParam("PD-Mix", &out_aux_mix);
        break;
    case 4:
        alloc_engines<plaits::AdditiveEngine>();
        init_params(0.8f, 0.8f, 0.75f, {0.8f, 0.8f, false});
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Bump", &harmonics);
        engine::addParam("Peak", &timbre);
        engine::addParam("Shape", &morph);
        engine::addParam("Decay", &patch.decay);
        if (_mode.output == 2)
            engine::addParam("AuxMix", &out_aux_mix);
        else
            engine::addParam("Color", &patch.lpg_colour);
        break;
    case WAVETABLE_ENGINE:
        alloc_engines<plaits::WavetableEngine>(64 * sizeof(const int16_t *));
        init_params(0.f, 0.8f, 0.75f, {0.6f, 0.6f, false});
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Bank", &harmonics, 0.f, 0.5f);
        engine::addParam("Row", &timbre);
        engine::addParam("Column", &morph);
        engine::addParam("Decay", &patch.decay);
        if (_mode.output == 2)
            engine::addParam("AuxMix", &out_aux_mix);
        else
            engine::addParam("Color", &patch.lpg_colour);
        for (auto &v : _voices)
            if (v.engine)
                v.engine->LoadUserData(nullptr);
        break;
    case CHORD_ENGINE:
        alloc_engines<plaits::ChordEngine>(plaits::kChordNumChords * plaits::kChordNumNotes + plaits::kChordNumChords + plaits::kChordNumNotes);
        init_params(0.5f, 0.5f, 0.5f, {0.8f, 0.8f, false});
        engine::addParam(V_OCT, &_v_oct);
        // render() copies the knob into the quantizer of every voice's engine
        _chord = 8;
        engine::addParam("Chord", &_chord, 0, plaits::kChordNumChords - 1, (const char **)plaits::chord_names);
        engine::addParam("Inv.", &timbre);
        engine::addParam("Shape", &morph);
        engine::addParam("Decay", &patch.decay);
        if (_mode.output == 2)
            engine::addParam("AuxMix", &out_aux_mix);
        else
            engine::addParam("Color", &patch.lpg_colour);
        break;
#if PLAITS_SPEECH
    case SPEECH_ENGINE:
        alloc_engines<plaits::SpeechEngine>(mem_floats<plaits::LPCSpeechSynth::Frame>(plaits::kLPCSpeechSynthMaxFrames) +
                                                mem_floats<int>(plaits::kLPCSpeechSynthMaxWords) + 2 * plaits::kMaxBlockSize,
                                            CV_VOICES);
        init_params(0.5f, 0.5f, 0.5f, {-0.7f, 0.8f, false});
        out_aux_mix = 0.f;
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Model", &harmonics);
        engine::addParam("Species", &timbre);
        engine::addParam("Phon.", &morph);
        engine::addParam("Decay", &patch.decay);
        engine::addParam("AuxMix", &out_aux_mix);
        break;
#endif
    case SWARM_ENGINE:
        alloc_engines<plaits::SwarmEngine>(mem_floats<plaits::SwarmVoice>(plaits::kNumSwarmVoices));
        init_params(0.5f, 0.5f, 0.5f, {-3.0f, 1.0f, false});
        out_aux_mix = 0.f;
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Spread", &harmonics);
        engine::addParam("Density", &timbre);
        engine::addParam("Overlap", &morph);
        engine::addParam("Decay", &patch.decay);
        engine::addParam("AuxMix", &out_aux_mix);
        break;
    case NOISE_ENGINE:
        alloc_engines<plaits::NoiseEngine>(plaits::kMaxBlockSize);
        init_params(0.5f, 0.9f, 0.5f, {-1.0f, -1.0f, false}); // clocked at 5.5 kHz
        out_aux_mix = 0.f;
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Filter", &harmonics);
        engine::addParam("Clock", &timbre);
        engine::addParam("Reson.", &morph);
        engine::addParam("Decay", &patch.decay);
        engine::addParam("AuxMix", &out_aux_mix);
        break;
    case PARTICLE_ENGINE:
        alloc_engines<plaits::ParticleEngine>(mem_floats<uint16_t>(8192));
        // at 0.5 a crackle now and then. OUT x3 without Plaits' limiter (-2 in Plaits): its peak follower, slow to
        // let go, held the spiky impulses down 10 dB below the oscillators; process()'s softLimit() rounds the peaks
        init_params(0.5f, 0.65f, 0.7f, {3.0f, 1.0f, false});
        out_aux_mix = 0.f;
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Spread", &harmonics);
        engine::addParam("Density", &timbre);
        engine::addParam("Filter", &morph);
        engine::addParam("Decay", &patch.decay);
        engine::addParam("AuxMix", &out_aux_mix);
        break;
    case STRING_ENGINE:
        alloc_engines<plaits::StringEngine>(plaits::kMaxBlockSize + 16 +
                                                plaits::kNumStrings * (plaits::kDelayLineSize + plaits::kDelayLineSize / 4),
                                            CV_VOICES);
        alloc_engines<SingleStringEngine>(plaits::kMaxBlockSize + plaits::kDelayLineSize + plaits::kDelayLineSize / 4,
                                          CV_VOICES + MIDI_VOICES, CV_VOICES);
        // Plaits' limiter with a gain of 5 (1 in Plaits): the string is 14 dB below the oscillators else
        init_params(0.5f, 0.5f, 0.5f, {-5.0f, 0.8f, true});
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Inharm.", &harmonics);
        engine::addParam("Bright", &timbre);
        engine::addParam("Decay", &morph);
        break;
    case MODAL_ENGINE:
        alloc_engines<plaits::ModalEngine>(plaits::kMaxBlockSize);
        init_params(0.5f, 0.5f, 0.5f, {-1.0f, 0.8f, true});
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Struct.", &harmonics);
        engine::addParam("Bright", &timbre);
        engine::addParam("Decay", &morph);
        break;
    case 13:
        _base_pitch += -24.f;
        alloc_engines<plaits::BassDrumEngine>(48, CV_VOICES);
        init_params(0.8f, 0.5f, 0.5f, {0.8f, 0.8f, true});
        engine::addParam("Pitch", &_pitch, -1.f, 1.f);
        engine::addParam(_mode.output == 0 ? "Drive" : "Punch", &harmonics);
        engine::addParam("Tone", &timbre);
        engine::addParam("Decay", &morph);
        break;
    case 14:
        alloc_engines<plaits::SnareDrumEngine>(48, CV_VOICES);
        init_params(0.5f, 0.5f, 0.5f, {0.8f, 0.8f, true});
        engine::addParam("Pitch", &_pitch, -1.f, 1.f);
        engine::addParam("Snappy", &harmonics);
        engine::addParam("Tone", &timbre);
        engine::addParam("Decay", &morph);
        break;
    case 15:
        alloc_engines<plaits::HiHatEngine>(48, CV_VOICES);
        init_params(0.5f, 0.9f, 0.6f, {0.8f, 0.8f, true});
        engine::addParam("Pitch", &_pitch, -1.f, 1.f);
        engine::addParam("Noise", &harmonics);
        engine::addParam("Tone", &timbre);
        engine::addParam("Decay", &morph);
        break;
    // engines 2
    case CLASSIC_VAVCF_ENGINE:
        alloc_engines<plaits::VirtualAnalogVCFEngine>();
        init_params(0.5f, 0.5f, 0.5f, {1.f, 1.f, false});
        engine::addParam(V_OCT, &_v_oct);
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
    case PHASE_DISTORTION_ENGINE:
        alloc_engines<plaits::PhaseDistortionEngine>(plaits::kMaxBlockSize * 4);
        init_params(0.5f, 0.5f, 0.5f, {0.7f, 0.7f, false});
        out_aux_mix = 0.f;
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Ratio", &harmonics);
        engine::addParam("Amount", &timbre);
        engine::addParam("Asym", &morph);
        engine::addParam("Decay", &patch.decay);
        engine::addParam("AuxMix", &out_aux_mix);
        break;
    case WAVE_TERRAIN_ENGINE:
        alloc_engines<plaits::WaveTerrainEngine>(plaits::kMaxBlockSize * 4);
        init_params(0.5f, 0.5f, 0.5f, {0.7f, 0.7f, false});
        out_aux_mix = 0.f;
        engine::addParam(V_OCT, &_v_oct);
        engine::addParam("Terrain", &harmonics);
        engine::addParam("Radius", &timbre);
        engine::addParam("Offset", &morph);
        engine::addParam("Decay", &patch.decay);
        engine::addParam("AuxMix", &out_aux_mix);
        break;
    case STRING_MACHINE_ENGINE:
        alloc_engines<plaits::StringMachineEngine>(kChordBankMem + mem_floats<plaits::Ensemble::E::T>(1024));
        init_params(0.5f, 0.5f, 0.5f, {0.8f, 0.8f, false});
        engine::addParam(V_OCT, &_v_oct);
        // process() sets HARMONICS to the middle of the chord's step
        _chord_knob = true;
        engine::addParam("Chord", &_chord, 0, plaits::kChordNumChords - 1, (const char **)plaits::chord_names);
        engine::addParam("Filter", &timbre);
        engine::addParam("Regist.", &morph);
        engine::addParam("Decay", &patch.decay);
        engine::addParam("AuxMix", &out_aux_mix); // 0.5: OUT and AUX, Plaits' stereo pair, in mono
        break;
    case CHIPTUNE_ENGINE:
        alloc_engines<plaits::ChiptuneEngine>(kChordBankMem, CV_VOICES);
        alloc_engines<ChiptuneChordEngine>(kChordBankMem, CV_VOICES + MIDI_VOICES, CV_VOICES);
        init_params(0.5f, 0.5f, 0.5f, {0.5f, 0.5f, false});
        engine::addParam(V_OCT, &_v_oct);
        _chord_knob = true;
        engine::addParam("Chord", &_chord, 0, plaits::kChordNumChords - 1, (const char **)plaits::chord_names);
        engine::addParam("Arp", &timbre);
        engine::addParam("Shape", &morph);
        engine::addParam("Decay", &patch.decay); // the arpeggiator: process() makes it the chiptune's envelope
        engine::addParam("AuxMix", &out_aux_mix);
        patch.lpg_colour = 1.f; // the MIDI voices and the drone: the LPG as a VCA, the squares stay square
        break;
#if PLAITS_SIX_OP
    case SIX_OP_ENGINE:
    {
        // the Trig/CV voice's engine unpacks the 96 patches (fm_patches_table[0]: Plaits' three banks one after the
        // other); the buffers of the MIDI voices have no room for them (patches_ stays NULL), they take that one's
        constexpr int kPatches = 96; // kNumPatchesPerBank
        constexpr size_t mem = plaits::kMaxBlockSize * (4 + plaits::kNumSixOpVoices);
        alloc_engines<SixOpGateEngine>(mem + mem_floats<plaits::fm::Patch>(kPatches), CV_VOICES);
        alloc_engines<SixOpGateEngine>(mem, CV_VOICES + MIDI_VOICES, CV_VOICES);
        auto *cv = static_cast<SixOpGateEngine *>(_cv_voice.engine);
        if (cv)
            cv->LoadUserData(plaits::fm_patches_table[0]);
        for (int i = CV_VOICES; i < CV_VOICES + MIDI_VOICES; i++)
            if (_voices[i].engine)
            {
                if (cv)
                    static_cast<SixOpGateEngine *>(_voices[i].engine)->patches_ = cv->patches_;
                else
                    _voices[i].engine = nullptr; // no patches: silent
            }
        init_params(0.5f, 0.5f, 0.5f, {1.0f, 1.0f, true});
        engine::addParam(V_OCT, &_v_oct);
        // the knob's labels: the names of the patches, without the blanks at the end
        char *names = (char *)malloc(kPatches * 11);
        _patch_names = (const char **)malloc(kPatches * sizeof(const char *));
        if (cv && names && _patch_names)
        {
            for (int i = 0; i < kPatches; i++)
            {
                char *n = names + i * 11;
                memcpy(n, cv->patches_[i].name, 10);
                n[10] = 0;
                for (int j = 9; j >= 0 && n[j] == ' '; j--)
                    n[j] = 0;
                _patch_names[i] = n;
            }
            engine::addParam("Patch", &_patch, 0, kPatches - 1, _patch_names);
        }
        else
            engine::addParam("Patch", &_patch, 0, kPatches - 1);
        engine::addParam("Bright", &timbre);
        engine::addParam("Env", &morph);
        break;
    }
#endif
    }

    // what plaits::Voice::Render() takes as Plaits' engine index: speech (15) and chiptune (7) get extra settings there
    if (_mode.engine >= 16)
        patch.engine = (_mode.engine - 16);
    else if (_mode.engine == SPEECH_ENGINE)
        patch.engine = 15;

    for (auto &v : _voices)
        if (v.engine)
            v.engine->Reset();

    // computed from its first Trig on (the drone of an unpatched Trig input sounds at once): an engine that bypasses
    // the LPG in Trig mode does not sound before it (the chiptune's bass on AUX, the speech engine's scan mode)
    _cv_voice.idle = true;

    if (is_poly())
    {
        _allocator.Init();
        _allocator.set_size(MIDI_VOICES);
        engine::setMode(ENGINE_MODE_MIDI_IN);
    }
}

// With the Trig input set to "-" and no MIDI note so far, the Trig/CV voice drones at the Level of the Decay knob
static bool drone()
{
    return !engine::hasTrig() && !_midi_seen;
}

// plaits::Voice::Render() writes the trigger into its delay line and reads the value written kMaxTriggerDelay
// frames ago (Read(0) after Write()): a Trig reaches the LPG 4 ms later. A MIDI voice puts its trigger into the
// slot read in this frame instead, and Render() writes a 0.
static void trigger_now(plaits::Voice &voice)
{
    auto &delay = voice.trigger_delay_;
    delay.line_[(delay.write_ptr_ + plaits::kMaxTriggerDelay - 1) % plaits::kMaxTriggerDelay] = 1.f;
}

// Renders a voice into bufferOut/bufferAux and adds it to out
static void render(poly_voice &v, bool trigger, float *out)
{
    if (v.engine == nullptr)
        return;

    patch.note = v.note;
    modulations.trigger = trigger ? 1.f : 0.f;
    modulations.hold = v.held;
    if (trigger)
        v.quiet = 0; // silent until the trigger is through the delay line

    if (_mode.engine == CHORD_ENGINE)
        static_cast<plaits::ChordEngine *>(v.engine)->chords_.chord_index_quantizer_.quantized_value_ = _chord;
#if PLAITS_SIX_OP
    if (_mode.engine == SIX_OP_ENGINE)
    {
        auto *e = static_cast<SixOpGateEngine *>(v.engine);
        e->patch_index = _patch;
        e->gate_ = v.held;
    }
#endif

    v.voice.Render(v.engine, patch, modulations, bufferOut, bufferAux, FRAME_BUFFER_SIZE);

    float peak = 0;
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        const float o = _mode.output == 2   ? stmlib::Crossfade(bufferOut[i], bufferAux[i], out_aux_mix)
                        : _mode.output == 1 ? bufferAux[i]
                                            : bufferOut[i];
        const float s = o * v.gain;
        out[i] += s;
        peak = std::max(peak, fabsf(s));
    }

    // silent, the LPG no longer rising and not held: not computed until the next trigger
    v.quiet = (peak < kQuiet && !v.voice.lpg_envelope_.ramp_up_) ? v.quiet + 1 : 0;
    v.idle = v.quiet >= kQuietFrames && !v.held;
}

// Up to KNEE as it is (single notes of the engines peak below 0.8 at their defaults), above it rounded off
// softly, 1.0 at KNEE + 3 * (1 - KNEE) = 1.3: chords sum their notes
static inline float softLimit(float x)
{
    constexpr float KNEE = 0.85f;
    if (x > KNEE)
        return KNEE + (1.f - KNEE) * stmlib::SoftClip((x - KNEE) / (1.f - KNEE));
    if (x < -KNEE)
        return -KNEE - (1.f - KNEE) * stmlib::SoftClip((-x - KNEE) / (1.f - KNEE));
    return x;
}

void engine::process()
{
    float *out = engine::outputBuffer<0>();
    memset(out, 0, sizeof(float) * FRAME_BUFFER_SIZE);

    float a = _last_out / 256.f;
    ONE_POLE(patch.harmonics, harmonics + a, 0.1f);
    ONE_POLE(patch.timbre, timbre + a, 0.1f);
    ONE_POLE(patch.morph, morph + a, 0.1f);
    if (_chord_knob) // * 1.02f * kChordNumChords - 0.5f in the quantizer: the chord, no glide through the others
        patch.harmonics = (_chord + 0.5f) / (1.02f * plaits::kChordNumChords);

    // On its MIDI channel the firmware also makes a Trig of each Note On (midi.cpp process()), in this frame: that
    // one the MIDI voices play already
    const bool midi_trig = _midi_noteons > 0;
    _midi_noteons = 0;

    const float decay = patch.decay;
    // the chiptune's envelope (Plaits: the TIMBRE attenuverter) on the arpeggiator of the Trig/CV voice, which
    // bypasses the LPG: Decay 1 none (the attenuverter at noon), the lower the shorter, OUT and the bass on AUX
    if (_mode.engine == CHIPTUNE_ENGINE)
        patch.timbre_modulation_amount = 1.f - decay;
    modulations.engine = patch.engine;
    modulations.note = 0;

    // Trig/CV voice: follows V/OCT all the time
    const bool trig = engine::trig() && !midi_trig;
    _cv_voice.note = _base_pitch + (_pitch + engine::cv()) * 12.f;

    // the chiptune's drone is its chord; in Trig mode the arpeggiator is silent until a Trig, its bass not at Decay 1
    // (no envelope): the voice stops when the drone ends (Trig patched, or the first MIDI note)
    static bool was_drone = false;
    if (was_drone && !drone() && _mode.engine == CHIPTUNE_ENGINE)
        _cv_voice.idle = true;
    was_drone = drone();

    if (drone())
    {
        modulations.trigger_patched = false;
        modulations.level_patched = true;
        modulations.level = decay;
        patch.decay = 0.001f;
        render(_cv_voice, trig, out);
        _cv_voice.idle = false; // Level 0 is silent, but the knob can turn it up again
        patch.decay = decay;
    }
    else
    {
        modulations.trigger_patched = true;
        modulations.level_patched = false;
        modulations.level = 1.f;

        // held from its Trig while the gate stays high (a drum is struck, never held)
        _cv_voice.held = !is_drum() && (trig || (_cv_voice.held && engine::gate()));

        // a drum is computed all the time, as when it was the only voice (its noise and random state go on)
        if (trig || is_drum())
            _cv_voice.idle = false;
        if (!_cv_voice.idle)
            render(_cv_voice, trig, out);
    }

    // MIDI voices: the key plus the V_OCT knob plus pitch bend
    const float midi_offset = _v_oct * 12.f + _bend;
    modulations.trigger_patched = true;
    modulations.level_patched = false;
    modulations.level = 1.f;

    // a MIDI voice sounds: the sum is limited. The engines added later (speech ... modal, phase distortion ...
    // chiptune) always: Plaits' limiter of Modal or Swarm lets the attack of a Trig through, up to 1.35
    bool limit = _mode.engine >= SPEECH_ENGINE && !is_drum() && _mode.engine != CLASSIC_VAVCF_ENGINE;
    for (int i = 0; i < MIDI_VOICES; i++)
    {
        auto &v = _midi_voices[i];

        // a trigger needs a frame low before it: a Note On right after one on the same voice waits a frame
        const bool trigger = v.pending && !v.trigger;
        if (trigger)
        {
            v.pending = false;
            v.gain = v.velocity / 127.f;
#if PLAITS_SIX_OP
            if (_mode.engine == SIX_OP_ENGINE && v.engine) // the velocity to the DX voice, as on the DX7
            {
                static_cast<SixOpGateEngine *>(v.engine)->velocity_ = v.gain;
                v.gain = 1.f;
            }
#endif
            v.idle = false;
            v.quiet = 0;
            trigger_now(v.voice);
        }
        v.trigger = trigger;

        v.note = v.key + midi_offset;

        if (!v.idle)
        {
            render(v, false, out);
            limit = true;
        }
    }

    _last_out = _mode.output == 1 ? bufferOut[0] : out[0]; // aux engines (drums only): Plaits' OUT

    // the Trig/CV voice of the first engines alone stays as it was (the Wavetable drone peaks at 0.94)
    for (int i = 0; limit && i < FRAME_BUFFER_SIZE; i++)
        out[i] = softLimit(out[i]);
}

static void releaseAll()
{
    for (int v = 0; v < MIDI_VOICES; v++)
    {
        _midi_voices[v].pending = false;
        _midi_voices[v].held = false;
        _midi_voices[v].sustained = false;
    }
    _allocator.ClearNotes();
}

// A Note On pings the LPG as a Trig does; it stays open while the key (or the sustain pedal) is held, and the Note Off
// lets it decay by the Decay knob
void engine::onMidiNote(uint8_t key, uint8_t velocity) // NoteOff: velocity == 0
{
    if (velocity > 0)
    {
        uint8_t v = _allocator.NoteOn(key);
        if (v >= MIDI_VOICES)
            return;
        auto &voice = _midi_voices[v];
        voice.key = key;
        voice.velocity = velocity;
        voice.pending = true;
        voice.held = true;
        voice.sustained = false;
        _midi_seen = true;
        ++_midi_noteons;
    }
    else
    {
        uint8_t v = _allocator.NoteOff(key);
        if (v >= MIDI_VOICES)
            return;
        if (_sustain)
            _midi_voices[v].sustained = true;
        else
            _midi_voices[v].held = false;
    }
}

void engine::onMidiPitchbend(int16_t pitch)
{
    _bend = (float)pitch / 8192 * 12;
}

void engine::onMidiCC(uint8_t cc, uint8_t value)
{
    if (cc == 64) // sustain pedal
    {
        _sustain = value >= 64;
        if (!_sustain)
            for (int v = 0; v < MIDI_VOICES; v++)
                if (_midi_voices[v].sustained)
                {
                    _midi_voices[v].sustained = false;
                    _midi_voices[v].held = false;
                }
    }
    else if (cc == 120 || cc == 123) // all sound off, all notes off
        releaseAll();
}

void engine::onMidiSysex(uint8_t byte)
{
}

void engine::onMidiRTM(uint8_t code)
{
    if (code == MIDI_RTM_STOP)
        releaseAll();
}

void engine::draw()
{
    engine::setParamName(&patch.decay, drone() ? "Level" : "Decay");
    if (_mode.engine == CHIPTUNE_ENGINE) // the arpeggio of the Trig/CV voice; without Trig the chord's inversion
        engine::setParamName(&timbre, engine::hasTrig() ? "Arp" : "Inv.");
}

#include "stmlib/utils/random.cc"
#include "lib/stmlib/dsp/units.cc"

//${SCRIPT_PATH}/main.cc $(find lib/plaits/ -type f -name "*.cc")

#include "lib/plaits/resources.cc"
#if PLAITS_SPEECH
#include "lib/plaits/dsp/speech/sam_speech_synth.cc"
#include "lib/plaits/dsp/speech/lpc_speech_synth_controller.cc"
#include "lib/plaits/dsp/speech/lpc_speech_synth_words.cc"
#include "lib/plaits/dsp/speech/lpc_speech_synth_phonemes.cc"
#include "lib/plaits/dsp/speech/naive_speech_synth.cc"
#include "lib/plaits/dsp/speech/lpc_speech_synth.cc"
#include "lib/plaits/dsp/engine/speech_engine.cc"
#endif
#include "lib/plaits/dsp/engine/string_engine.cc"
#include "lib/plaits/dsp/engine/swarm_engine.cc"
#include "lib/plaits/dsp/engine/chord_engine.cc"
#include "lib/plaits/dsp/engine/waveshaping_engine.cc"
#include "lib/plaits/dsp/engine/modal_engine.cc"
#include "lib/plaits/dsp/engine/fm_engine.cc"
#include "lib/plaits/dsp/engine/snare_drum_engine.cc"
#include "lib/plaits/dsp/engine/grain_engine.cc"
#include "lib/plaits/dsp/engine/virtual_analog_engine.cc"
#include "lib/plaits/dsp/engine/wavetable_engine.cc"
#include "lib/plaits/dsp/engine/additive_engine.cc"
#include "lib/plaits/dsp/engine/noise_engine.cc"
#include "lib/plaits/dsp/engine/hi_hat_engine.cc"
#include "lib/plaits/dsp/engine/particle_engine.cc"
#include "lib/plaits/dsp/engine/bass_drum_engine.cc"
#include "lib/plaits/dsp/engine2/chiptune_engine.cc"
#include "lib/plaits/dsp/engine2/wave_terrain_engine.cc"
// its registration table has the name of chord_engine.cc's
#define kRegistrationTableSize kStringMachineRegistrationTableSize
#define registrations string_machine_registrations
#include "lib/plaits/dsp/engine2/string_machine_engine.cc"
#undef kRegistrationTableSize
#undef registrations
#include "lib/plaits/dsp/engine2/phase_distortion_engine.cc"
#include "lib/plaits/dsp/engine2/virtual_analog_vcf_engine.cc"
#include "lib/plaits/dsp/chords/chord_bank.cc"
#include "lib/plaits/dsp/physical_modelling/string_voice.cc"
#include "lib/plaits/dsp/physical_modelling/string.cc"
#include "lib/plaits/dsp/physical_modelling/modal_voice.cc"
#include "lib/plaits/dsp/physical_modelling/resonator.cc"
#include "lib/plaits/dsp/fm/algorithms.cc"
#include "lib/plaits/dsp/fm/dx_units.cc"
#include "lib/plaits/dsp/voice.cc"
#if PLAITS_SIX_OP
// last: its "using namespace fm" makes Voice ambiguous for what follows (voice.cc); its Voice<6> is fm::Voice
#define Voice fm::Voice
#include "lib/plaits/dsp/engine2/six_op_engine.cc"
#undef Voice
#endif
