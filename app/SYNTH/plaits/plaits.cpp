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
// SYNTH/Plaits' model Speech, Plaits' speech engine (formant, SAM and LPC vowels, LPC phonemes and words; it took over
// SPEECH/LPC): 25 KB more code
#ifndef PLAITS_SPEECH
#define PLAITS_SPEECH 1
#endif
// SYNTH/6-OP-FM, Plaits' DX7 engine with its 96 patches (SYNTH/DxFM_BNK1-3 plays them with msfa): 30 KB more code
#ifndef PLAITS_SIX_OP
#define PLAITS_SIX_OP 0
#endif
// SYNTH/Plaits' models Noise (filtered noise) and Particle (particle noise): 5 KB more code, and Particle's diffuser,
// 16 KB a voice, makes the memory every SYNTH/Plaits keeps 91 KB instead of 46 KB
#ifndef PLAITS_NOISE
#define PLAITS_NOISE 0
#endif

ENGINE_NAME("SYNTH/Plaits");
ENGINE_ALIAS("M-OSC/Plaits"); // its name for a day (2026-10-26): the patches saved with it
// the engines of the release (2026-04) with their parameters, which restore() takes over
ENGINE_ALIAS("M-OSC/Virt.Analog", "ffffff");  // V_OCT, Detune, Square, CSAW, Decay, Color
ENGINE_ALIAS("M-OSC/Waveshaping", "ffffff");  // V_OCT, Waveform, Fold, Asym, Decay, Color
ENGINE_ALIAS("M-OSC/2-OP-FM", "ffffff");      // V_OCT, Ratio, Mod, Feedb., Decay, Color
ENGINE_ALIAS("M-OSC/Grain", "ffffff");        // V_OCT, Ratio, Frm/Fq., Width, Decay, PD-Mix
ENGINE_ALIAS("M-OSC/Harmonic", "ffffff");     // V_OCT, Bump, Peak, Shape, Decay, AuxMix
ENGINE_ALIAS("M-OSC/Wavetable", "ffffff");    // V_OCT, Bank (0 ... 0.5), Row, Column, Decay, AuxMix
ENGINE_ALIAS("M-OSC/Chord", "fbffff");        // V_OCT, Chord (int 0 ... 16), Inv., Shape, Decay, Color
ENGINE_ALIAS("SYNTH/ClassicVAVCF", "ffffff"); // V_OCT, Morph, Cutoff, Harsh, EnvMod (-1 ... 1), Decay (0 ... 0.99)
ENGINE_ALIAS("MIDI/VAx6", "ffffff");          // V_OCT, Harmo, Timbre, Morph, Decay, Stereo
#if PLAITS_SPEECH
ENGINE_ALIAS("SPEECH/LPC", "fbffff"); // V_OCT, Word (int 0 ... 91), Speed, Form.Shift, Prosody, AuxMix
// the speech engine of its own (2026-10-28): V_OCT, Model, Species, Phon., Decay, AuxMix, Prosody, Speed
ENGINE_ALIAS("SYNTH/Speech", "ffffffff");
#endif
ENGINE_NAME("DRUMS/Analog BD");
ENGINE_NAME("DRUMS/Analog SD");
ENGINE_NAME("DRUMS/Analog HH2");
ENGINE_NAME("DRUMS/Analog HH");
ENGINE_NAME("DRUMS/909ish-BD");
ENGINE_NAME("DRUMS/909ish-SD");
#if PLAITS_SIX_OP
ENGINE_NAME("SYNTH/6-OP-FM");
#endif
// SYNTH/Plaits: Plaits' synth models (oscillators, noises, physical models) in one engine, the model picked by the
// Model knob as on Plaits, also by modulation. Polyphonic over MIDI: the Trig/CV voice and four MIDI voices, each a
// plaits::Voice (LPG, decay envelope) with an engine of its own. Every voice keeps the memory for the largest model
// from setup() on: a change of model makes the new engine in it (placement new, Init(), Reset()) in process(), which
// runs in an interrupt on the Teensys and must not malloc(). The drums and SYNTH/6-OP-FM are an engine each, as
// before; the drums are the Trig/CV voice alone and no MIDI engine.

#define private public // ;-)
#include "plaits/dsp/voice.h"
#include "stmlib/dsp/dsp.h"

using namespace machine;

struct poly_voice
{
    plaits::Voice voice;
    plaits::Engine *engine = nullptr;
    int id = 0;                 // the engine's, as _mode.engine
    int8_t model = -1;          // SYNTH/Plaits: the model the engine is of
    void *engine_mem = nullptr; // SYNTH/Plaits: the memory of the engine and of its Init() (setup_plaits())
    float *small_mem = nullptr;
    float *large_mem = nullptr;
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
int32_t _chord = 8;            // of the engines that take a chord from HARMONICS: from the knob
static float _decay = 0.5f;    // patch.decay; the knob of SYNTH/Plaits
static float _colour = 0.5f;   // patch.lpg_colour; the knob of SYNTH/Plaits
// Plaits' attenuverters of TIMBRE, MORPH and FM with nothing at their CV inputs: how much the decay envelope of a Trig
// or note moves them (Voice::Render(), ApplyModulations()); without Trig the FM one is a fine tune (+-1 semitone)
static float _timbre_env = 0.f; // patch.timbre_modulation_amount; the knobs of SYNTH/Plaits
static float _morph_env = 0.f;  // patch.morph_modulation_amount; the Speed of the model Speech
static float _fm_env = 0.f;     // patch.frequency_modulation_amount; the Prosody of the model Speech
static int32_t _model = 0;     // SYNTH/Plaits: the Model knob
static float _stereo = 0.f;    // SYNTH/Plaits: the Stereo knob
static float *_out_r = nullptr; // SYNTH/Plaits: the right channel (the drums, speech and 6-OP stay mono)
static int _active = 0;        // SYNTH/Plaits: the model of the Model knob with its modulation, in the last process()
static bool _plaits = false;   // SYNTH/Plaits
static bool _smoothed = false; // patch.harmonics, timbre, morph start at the knobs

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

#if PLAITS_SPEECH
#include "plaits/dsp/speech/lpc_speech_synth_words.h"

// The words of the LPC word banks (lpc_speech_synth_words.cc), numbered as SPEECH/LPC's Word 0 ... 91
static const char *const _speech_words[] = {
    "red", "orange", "yellow", "green", "blue", "indigo", "violet",                        // bank 0
    "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", // bank 1
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w",
    "x", "y", "z", // bank 2
    "alpha", "bravo", "charlie", "delta", "echo", "foxtrot", "golf", "hotel", "india", "juliet", "kilo", "lima",
    "mike", "november", "oscar", "papa", "quebec", "romeo", "sierra", "tango", "uniform", "victor", "whiskey", "xray",
    "yankee", "zulu", // bank 3
    "analog", "circuit", "clock", "control", "digital", "electronic", "filter", "frequency", "generator",
    "instrument", "knob", "machine", "modular", "modulator", "operator", "oscillator", "patch", "sequencer",
    "synthesizer", "vca", "voltage", "waveform", // bank 4
};

// The words of plaits::word_banks_ (from LoadNextWord() on the host): data offset and first frame of each word, the
// last entry of a bank its size and frame count
static constexpr struct
{
    uint16_t offset;
    uint16_t frame;
} _bank_words[] = {
    {0, 0}, {123, 26}, {284, 57}, {481, 92}, {647, 123}, {812, 153}, {1034, 192}, {1233, 229}, // bank 0
    {0, 0}, {77, 24}, {144, 44}, {199, 61}, {274, 84}, {348, 104}, {430, 132}, {513, 155}, {606, 174}, {672, 189},
    {817, 213}, {900, 229}, // bank 1
    {0, 0}, {46, 19}, {95, 37}, {161, 62}, {211, 72}, {264, 93}, {321, 112}, {383, 134}, {442, 160}, {508, 183},
    {574, 205}, {638, 227}, {700, 244}, {762, 268}, {825, 289}, {870, 304}, {924, 322}, {975, 340}, {1019, 354},
    {1067, 373}, {1124, 392}, {1185, 408}, {1268, 433}, {1369, 457}, {1426, 479}, {1492, 502}, {1552, 524}, // bank 2
    {0, 0}, {100, 20}, {188, 43}, {286, 61}, {369, 79}, {435, 100}, {583, 136}, {638, 157}, {755, 184}, {851, 204},
    {952, 234}, {1028, 256}, {1095, 276}, {1161, 296}, {1316, 322}, {1409, 348}, {1474, 365}, {1545, 388}, {1640, 411},
    {1790, 439}, {1909, 460}, {2035, 493}, {2149, 517}, {2238, 542}, {2354, 565}, {2452, 585}, {2524, 604}, // bank 3
    {0, 0}, {198, 36}, {361, 72}, {516, 108}, {744, 151}, {922, 186}, {1202, 241}, {1378, 277}, {1642, 329},
    {1894, 375}, {2102, 416}, {2259, 445}, {2453, 483}, {2687, 524}, {2972, 574}, {3203, 617}, {3432, 661},
    {3591, 692}, {3832, 739}, {4119, 797}, {4353, 840}, {4544, 880}, {4802, 926}, // bank 4
};
// a bank's first entry in _bank_words; minus the bank, its first word in _speech_words
static constexpr uint8_t _bank_first[LPC_SPEECH_SYNTH_NUM_WORD_BANKS + 1] = {0, 8, 20, 47, 74, 97};
static_assert(sizeof(plaits::bank_0) == 1233 && sizeof(plaits::bank_1) == 900 && sizeof(plaits::bank_2) == 1552 &&
                  sizeof(plaits::bank_3) == 2524 && sizeof(plaits::bank_4) == 4802,
              "_bank_words is of other word banks");
static_assert(LEN_OF(_speech_words) == _bank_first[LPC_SPEECH_SYNTH_NUM_WORD_BANKS] - LPC_SPEECH_SYNTH_NUM_WORD_BANKS,
              "a name for each word");

// the most frames a word has (58, "synthesizer")
constexpr int max_word_frames(size_t i = 1)
{
    return i == LEN_OF(_bank_words)
               ? 0
               : std::max(_bank_words[i].frame - _bank_words[i - 1].frame, max_word_frames(i + 1));
}
constexpr int kWordFrames = max_word_frames();

// SYNTH/Plaits' model Speech: Plaits' speech engine without the frames of a word bank in memory. Load() decodes all
// words of a bank (14 KB a voice, and the synth words 223k instructions in the frame that changes to them, 74 % in
// QEMU). Here a bank takes its word boundaries from _bank_words, the frames of the word played are decoded as far as
// they are played and kept (word_frames_), and before Render() the frames the controller plays in the block (one, two
// in scan mode) are copied to window_: the same frames, the same sound, in 1 KB a voice
struct SpeechWordEngine : public plaits::SpeechEngine
{
    plaits::LPCSpeechSynth::Frame window_[2];    // the controller's frames k and k + 1 (frames_ = window_ - k)
    plaits::LPCSpeechSynth::Frame *word_frames_; // the frames of word_ decoded so far, decoded_ of them
    plaits::BitStream bits_;                     // on to the next frame of word_
    int word_ = -1;                              // in the loaded bank, -1: none
    int decoded_ = 0;

    // SpeechEngine::Init() without the frames of a bank
    void Init(stmlib::BufferAllocator *allocator) override
    {
        sam_speech_synth_.Init();
        naive_speech_synth_.Init();
        auto &wb = lpc_speech_synth_word_bank_;
        wb.word_banks_ = plaits::word_banks_;
        wb.num_banks_ = LPC_SPEECH_SYNTH_NUM_WORD_BANKS;
        wb.frames_ = window_;
        wb.word_boundaries_ = allocator->Allocate<int>(plaits::kLPCSpeechSynthMaxWords);
        wb.Reset();
        lpc_speech_synth_controller_.Init(&wb);
        word_bank_quantizer_.Init(LPC_SPEECH_SYNTH_NUM_WORD_BANKS + 1, 0.1f, false);
        temp_buffer_[0] = allocator->Allocate<float>(plaits::kMaxBlockSize);
        temp_buffer_[1] = allocator->Allocate<float>(plaits::kMaxBlockSize);
        word_frames_ = allocator->Allocate<plaits::LPCSpeechSynth::Frame>(kWordFrames);
        prosody_amount_ = 0.0f;
        speed_ = 0.0f;
        word_ = -1;
    }

    void Reset() override
    {
        plaits::SpeechEngine::Reset();
        word_ = -1;
    }

    void Render(const plaits::EngineParameters &parameters, float *out, float *aux, size_t size,
                bool *already_enveloped) override
    {
        const float group = parameters.harmonics * 6.0f; // as SpeechEngine::Render(): above 2 the LPC banks
        if (group > 2.0f)
        {
            const int bank = word_bank((group - 2.0f) * 0.275f);
            if (bank >= 0) // else the phonemes
                prepare_frames(bank, parameters.morph, parameters.trigger & plaits::TRIGGER_RISING_EDGE);
        }
        plaits::SpeechEngine::Render(parameters, out, aux, size, already_enveloped);
    }

    // the bank word_bank_quantizer_ gives Render() next (HysteresisQuantizer2::Process(), its state left as it is)
    int word_bank(float value) const
    {
        const auto &q = word_bank_quantizer_;
        value *= q.scale_;
        value += q.offset_;
        const float hysteresis_sign = value > static_cast<float>(q.quantized_value_) ? -1.0f : +1.0f;
        int bank = static_cast<int>(value + hysteresis_sign * q.hysteresis_ + 0.5f);
        CONSTRAIN(bank, 0, q.num_steps_ - 1);
        return bank - 1;
    }

    // Load(bank) without decoding: the words of the bank, and the reset the controller does after a Load()
    void load_bank(int bank)
    {
        auto &wb = lpc_speech_synth_word_bank_;
        const int first = _bank_first[bank];
        wb.num_words_ = _bank_first[bank + 1] - first - 1;
        for (int w = 0; w <= wb.num_words_; w++)
            wb.word_boundaries_[w] = _bank_words[first + w].frame;
        wb.num_frames_ = wb.word_boundaries_[wb.num_words_];
        wb.loaded_bank_ = bank;
        word_ = -1;
        lpc_speech_synth_controller_.playback_frame_ = -1;
        lpc_speech_synth_controller_.last_playback_frame_ = -1;
    }

    // the data of word w of the loaded bank
    const uint8_t *word_data(int w) const
    {
        const auto &wb = lpc_speech_synth_word_bank_;
        return wb.word_banks_[wb.loaded_bank_].data + _bank_words[_bank_first[wb.loaded_bank_] + w].offset;
    }

    static __attribute__((noinline)) uint8_t get_bits(plaits::BitStream &bits, int num_bits)
    {
        return bits.GetBits(num_bits);
    }

    // the next frame of a word into f, which holds the one before (LoadNextWord())
    __attribute__((noinline)) void decode(plaits::BitStream &bits, plaits::LPCSpeechSynth::Frame &f) const
    {
        const auto &wb = lpc_speech_synth_word_bank_;
        int energy = get_bits(bits, 4); // the end code 0xf comes after the last frame
        if (energy == 0)
        {
            f.energy = 0;
        }
        else
        {
            f.energy = wb.energy_lut_[energy];
            bool repeat = get_bits(bits, 1);
            f.period = wb.period_lut_[get_bits(bits, 6)];
            if (!repeat)
            {
                f.k0 = wb.k0_lut_[get_bits(bits, 5)];
                f.k1 = wb.k1_lut_[get_bits(bits, 5)];
                f.k2 = wb.k2_lut_[get_bits(bits, 4)];
                f.k3 = wb.k3_lut_[get_bits(bits, 4)];
                if (f.period)
                {
                    f.k4 = wb.k4_lut_[get_bits(bits, 4)];
                    f.k5 = wb.k5_lut_[get_bits(bits, 4)];
                    f.k6 = wb.k6_lut_[get_bits(bits, 4)];
                    f.k7 = wb.k7_lut_[get_bits(bits, 3)];
                    f.k8 = wb.k8_lut_[get_bits(bits, 3)];
                    f.k9 = wb.k9_lut_[get_bits(bits, 3)];
                }
            }
        }
    }

    // frame k of the loaded bank: from word_frames_, decoded on up to k (from the start of its word for another word)
    const plaits::LPCSpeechSynth::Frame &frame_at(int k)
    {
        const auto &wb = lpc_speech_synth_word_bank_;
        int w = 0;
        while (wb.word_boundaries_[w + 1] <= k)
            w++;
        if (word_ != w)
        {
            word_ = w;
            decoded_ = 0;
            bits_.Init(word_data(w));
        }
        const int start = wb.word_boundaries_[w];
        while (start + decoded_ <= k)
        {
            plaits::LPCSpeechSynth::Frame f = decoded_ ? word_frames_[decoded_ - 1] : plaits::LPCSpeechSynth::Frame{};
            decode(bits_, f);
            word_frames_[decoded_++] = f;
        }
        return word_frames_[k - start];
    }

    // the frames the controller plays in this block into window_ (LPCSpeechSynthController::Render())
    void prepare_frames(int bank, float address, bool trigger)
    {
        auto &wb = lpc_speech_synth_word_bank_;
        auto &c = lpc_speech_synth_controller_;
        if (bank != wb.loaded_bank_)
            load_bank(bank);

        int k = -1;
        bool interpolated = false;
        if (trigger) // the first frame of the word
        {
            int end;
            wb.GetWordBoundaries(address, &k, &end);
        }
        else if (c.remaining_frame_samples_ == 0)
        {
            if (c.playback_frame_ == -1) // scan mode: between two frames of the bank
            {
                k = static_cast<int32_t>(address * (static_cast<float>(wb.num_frames_) - 1.0001f));
                interpolated = true;
            }
            else
            {
                k = c.playback_frame_;
            }
        }
        if (k < 0 || k >= wb.num_frames_)
            return; // none played
        window_[0] = frame_at(k);
        window_[1] = window_[0]; // a frame played without interpolation blends the next one with 0
        if (interpolated && k + 1 == wb.word_boundaries_[word_ + 1]) // the first of the next word, the word kept
        {
            plaits::BitStream bits;
            bits.Init(word_data(word_ + 1));
            window_[1] = {};
            decode(bits, window_[1]);
        }
        else if (interpolated)
        {
            window_[1] = frame_at(k + 1);
        }
        wb.frames_ = window_ - k;
    }
};
#endif

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

// SYNTH/Plaits' Stereo: the ensemble of Plaits' string machine (dsp/fx/ensemble.h) on the mono output, the right
// channel's three taps modulated the other way round. The two channels drift apart in pitch, as SYNTH/Braids' detuned
// second oscillator, without a second voice to compute; their sum stays a chorus. Plaits' ensemble itself gives the
// same on both channels when they get the same signal
struct StereoEnsemble
{
    typedef plaits::FxEngine<1024, plaits::FORMAT_32_BIT> E;
    E engine_;
    uint32_t phase_1_ = 0;
    uint32_t phase_2_ = 0;

    void Init(E::T *buffer) { engine_.Init(buffer); }
    void Reset() { engine_.Clear(); }

    // in may be left. gain: the level after it
    void Process(float amount, float depth, float gain, const float *in, float *left, float *right, size_t size)
    {
        typedef E::Reserve<511> Memory;
        E::DelayLine<Memory, 0> line;
        E::Context c;
        const float dry = 1.f - amount * 0.5f;
        const float a = depth * 160.f; // Plaits' maximum deviation: 176 samples
        const float b = depth * 16.f;
        while (size--)
        {
            engine_.Start(&c);
            const uint32_t one_third = 1417339207UL;
            const uint32_t two_third = 2834678415UL;
            phase_1_ += 67289;  // 0.75 Hz
            phase_2_ += 589980; // 6.57 Hz
            const float mod_1 = plaits::SineRaw(phase_1_) * a + plaits::SineRaw(phase_2_) * b;
            const float mod_2 = plaits::SineRaw(phase_1_ + one_third) * a + plaits::SineRaw(phase_2_ + one_third) * b;
            const float mod_3 = plaits::SineRaw(phase_1_ + two_third) * a + plaits::SineRaw(phase_2_ + two_third) * b;

            const float x = *in++;
            float wet = 0.f;
            c.Read(x, 1.f);
            c.Write(line, 0.f);
            c.Interpolate(line, 192.f + mod_1, 0.33f);
            c.Interpolate(line, 192.f + mod_2, 0.33f);
            c.Interpolate(line, 192.f + mod_3, 0.33f);
            c.Write(wet, 0.f);
            *left++ = (wet * amount + x * dry) * gain;
            c.Interpolate(line, 192.f - mod_1, 0.33f);
            c.Interpolate(line, 192.f - mod_2, 0.33f);
            c.Interpolate(line, 192.f - mod_3, 0.33f);
            c.Write(wet, 0.f);
            *right++ = (wet * amount + x * dry) * gain;
        }
    }
};

static StereoEnsemble _ensemble;
static bool _ensemble_on = false; // processed in the last frame: else its delay line is cleared first

// SYNTH/Plaits: the models, in the order of Plaits' banks (green, red without the drums, then the orange of Plaits 1.2)
struct plaits_model
{
    const char *name; // the Model knob's label: 8 characters fit
    int engine;       // _mode.engine
    const char *harmonics, *timbre, *morph;
    plaits::PostProcessingSettings settings;
    uint16_t mem_cv, mem_midi; // floats for Engine::Init() of the Trig/CV voice and of a MIDI voice
    plaits::Engine *(*make_cv)(void *mem);
    plaits::Engine *(*make_midi)(void *mem);
};

template <class T>
plaits::Engine *make_engine(void *mem)
{
    return new (mem) T();
}

#define MAKE(T) make_engine<T>, make_engine<T>
constexpr uint16_t kStringCvMem = plaits::kMaxBlockSize + 16 + plaits::kNumStrings * (plaits::kDelayLineSize + plaits::kDelayLineSize / 4);
constexpr uint16_t kStringMidiMem = plaits::kMaxBlockSize + plaits::kDelayLineSize + plaits::kDelayLineSize / 4;
constexpr uint16_t kStringMachineMem = kChordBankMem + mem_floats<plaits::Ensemble::E::T>(1024);
constexpr uint16_t kParticleMem = PLAITS_NOISE ? mem_floats<uint16_t>(8192) : 0; // its diffuser
#if PLAITS_SPEECH
constexpr uint16_t kSpeechMem = mem_floats<int>(plaits::kLPCSpeechSynthMaxWords) + 2 * plaits::kMaxBlockSize +
                                mem_floats<plaits::LPCSpeechSynth::Frame>(kWordFrames);
#endif

static const plaits_model _models[] = {
    {"VA", 0, "Detune", "Square", "CSAW", {0.8f, 0.8f, false}, 48, 48, MAKE(plaits::VirtualAnalogEngine)},
    {"Shaping", 1, "Curve", "Fold", "Asym", {0.7f, 0.6f, false}, 48, 48, MAKE(plaits::WaveshapingEngine)},
    {"2-OP-FM", 2, "Ratio", "Mod", "Feedb.", {0.6f, 0.6f, false}, 48, 48, MAKE(plaits::FMEngine)},
    {"Grain", 3, "Ratio", "Frm/Fq.", "Width", {0.7f, 0.6f, false}, 48, 48, MAKE(plaits::GrainEngine)},
    {"Harmonic", 4, "Bump", "Peak", "Shape", {0.8f, 0.8f, false}, 48, 48, MAKE(plaits::AdditiveEngine)},
    {"Wavetbl", WAVETABLE_ENGINE, "Bank", "Row", "Column", {0.6f, 0.6f, false}, 64 * sizeof(const int16_t *), 64 * sizeof(const int16_t *), MAKE(plaits::WavetableEngine)},
    {"Chord", CHORD_ENGINE, "Chord", "Inv.", "Shape", {0.8f, 0.8f, false}, kChordBankMem, kChordBankMem, MAKE(plaits::ChordEngine)},
#if PLAITS_SPEECH
    // HARMONICS the kind of speech (the vowels, the LPC phonemes, the word banks), MORPH the vowel, phoneme or word
    // (draw()); the attenuverters of FM and MORPH with the Trig patched are the intonation and the speed of the words
    {"Speech", SPEECH_ENGINE, "Bank", "Species", "Phon.", {-0.7f, 0.8f, false}, kSpeechMem, kSpeechMem, MAKE(SpeechWordEngine)},
#endif
    {"Swarm", SWARM_ENGINE, "Spread", "Density", "Overlap", {-3.0f, 1.0f, false}, mem_floats<plaits::SwarmVoice>(plaits::kNumSwarmVoices), mem_floats<plaits::SwarmVoice>(plaits::kNumSwarmVoices), MAKE(plaits::SwarmEngine)},
#if PLAITS_NOISE
    // clocked at 5.5 kHz at Clock 0.9
    {"Noise", NOISE_ENGINE, "Filter", "Clock", "Reson.", {-1.0f, -1.0f, false}, plaits::kMaxBlockSize, plaits::kMaxBlockSize, MAKE(plaits::NoiseEngine)},
    // OUT x3 without Plaits' limiter (-2 in Plaits): its peak follower, slow to let go, held the spiky impulses down
    // 10 dB below the oscillators; process()'s softLimit() rounds the peaks
    {"Particle", PARTICLE_ENGINE, "Spread", "Density", "Filter", {3.0f, 1.0f, false}, kParticleMem, kParticleMem, MAKE(plaits::ParticleEngine)},
#endif
    // Plaits' limiter with a gain of 5 (1 in Plaits): the string is 14 dB below the oscillators else. The Trig/CV
    // voice rings three strings in turn (Plaits), a MIDI voice one
    {"String", STRING_ENGINE, "Inharm.", "Bright", "Decay", {-5.0f, 0.8f, true}, kStringCvMem, kStringMidiMem, make_engine<plaits::StringEngine>, make_engine<SingleStringEngine>},
    {"Modal", MODAL_ENGINE, "Struct.", "Bright", "Decay", {-1.0f, 0.8f, true}, plaits::kMaxBlockSize, plaits::kMaxBlockSize, MAKE(plaits::ModalEngine)},
    {"VA-VCF", CLASSIC_VAVCF_ENGINE, "Harsh", "Cutoff", "Morph", {1.f, 1.f, false}, 48, 48, MAKE(plaits::VirtualAnalogVCFEngine)},
    {"PhaseDst", PHASE_DISTORTION_ENGINE, "Ratio", "Amount", "Asym", {0.7f, 0.7f, false}, plaits::kMaxBlockSize * 4, plaits::kMaxBlockSize * 4, MAKE(plaits::PhaseDistortionEngine)},
    {"Terrain", WAVE_TERRAIN_ENGINE, "Terrain", "Radius", "Offset", {0.7f, 0.7f, false}, plaits::kMaxBlockSize * 4, plaits::kMaxBlockSize * 4, MAKE(plaits::WaveTerrainEngine)},
    {"Str.Mach", STRING_MACHINE_ENGINE, "Chord", "Filter", "Regist.", {0.8f, 0.8f, false}, kStringMachineMem, kStringMachineMem, MAKE(plaits::StringMachineEngine)},
    // the Trig/CV voice: Plaits' arpeggiator, clocked by the Trig; the MIDI voices play the chord through the LPG
    {"Chiptune", CHIPTUNE_ENGINE, "Chord", "Arp", "Shape", {0.5f, 0.5f, false}, kChordBankMem, kChordBankMem, make_engine<plaits::ChiptuneEngine>, make_engine<ChiptuneChordEngine>},
};
#undef MAKE

static const char *_model_names[LEN_OF(_models)]; // the Model knob's labels

// a voice's memory, from setup() on: the engine and what its Init() takes, in two parts: the models that take little
// (the wavetable's 64 pointers the most) in the small one, the others (the string machine's ensemble, the strings,
// Particle's diffuser) in the large one. The small ones of all voices first, together: on the DSM they fit into its
// DTCM pool, the large ones (36 KB, 80 KB with Particle) do not and leave it to the other tracks' engines
constexpr size_t kEngineBytes = (std::max({sizeof(plaits::VirtualAnalogEngine), sizeof(plaits::WaveshapingEngine),
                                           sizeof(plaits::FMEngine), sizeof(plaits::GrainEngine),
                                           sizeof(plaits::AdditiveEngine), sizeof(plaits::WavetableEngine),
                                           sizeof(plaits::ChordEngine), sizeof(plaits::SwarmEngine),
#if PLAITS_NOISE
                                           sizeof(plaits::NoiseEngine), sizeof(plaits::ParticleEngine),
#endif
#if PLAITS_SPEECH
                                           sizeof(SpeechWordEngine),
#endif
                                           sizeof(plaits::StringEngine), sizeof(SingleStringEngine),
                                           sizeof(plaits::ModalEngine), sizeof(plaits::VirtualAnalogVCFEngine),
                                           sizeof(plaits::PhaseDistortionEngine), sizeof(plaits::WaveTerrainEngine),
                                           sizeof(plaits::StringMachineEngine), sizeof(plaits::ChiptuneEngine),
                                           sizeof(ChiptuneChordEngine)}) +
                                 7) &
                                ~7;
constexpr size_t kSmallFloats = 64 * sizeof(const int16_t *);
constexpr size_t kLargeCvFloats = std::max({kStringCvMem, kStringMachineMem, kParticleMem});      // the Trig/CV voice
constexpr size_t kLargeMidiFloats = std::max({kStringMidiMem, kStringMachineMem, kParticleMem}); // a MIDI voice

// the voice's engine anew, of the model: in the voice's memory, zeroed as malloc() gave it to the engines before
static void load_model(poly_voice &v, int model)
{
    const plaits_model &m = _models[model];
    const bool cv = &v == &_cv_voice;
    const size_t floats = cv ? m.mem_cv : m.mem_midi;
    float *buffer = floats <= kSmallFloats ? v.small_mem : v.large_mem;

    v.model = model;
    v.id = m.engine;
    v.engine = nullptr;
    if (v.engine_mem == nullptr || buffer == nullptr)
        return; // no memory for it: silent

    memset(v.engine_mem, 0, kEngineBytes);
    memset(buffer, 0, floats * sizeof(float));
    plaits::Engine *e = (cv ? m.make_cv : m.make_midi)(v.engine_mem);
    stmlib::BufferAllocator allocator;
    allocator.Init(buffer, floats * sizeof(float));
    e->Init(&allocator);
    e->post_processing_settings = m.settings;
    if (m.engine == WAVETABLE_ENGINE)
        e->LoadUserData(nullptr);
    e->Reset();
    v.voice.out_post_processor_.Reset(); // the limiter's state, as Plaits on a change of engine
    v.voice.aux_post_processor_.Reset();
    v.engine = e;

    // the wavetables are integrated, the engine differentiates them: from Init() on its first sample is the whole
    // value of the wave, a spike of 4 ... 8 times full scale through an open LPG (a voice that sounds), and the glide
    // from the first wave to the knobs' a bump. At the knobs' wave at once, a block for nothing, then the
    // differentiator without the spike
    if (m.engine == WAVETABLE_ENGINE && !v.idle)
    {
        // 0 ... 1, as Voice::Render() gives them: with a knob at its end the dither (_last_out) takes patch.* past
        // 1, and 7 * 6.9999 reads past the 4 x 64 waves (HARMONICS: bank -1, in front of wave_map_, a NULL wave
        // and a fault on the DSM)
        float hh = patch.harmonics, tt = patch.timbre, mm = patch.morph;
        CONSTRAIN(hh, 0.f, 1.f);
        CONSTRAIN(tt, 0.f, 1.f);
        CONSTRAIN(mm, 0.f, 1.f);
        auto *w = static_cast<plaits::WavetableEngine *>(e);
        w->x_pre_lp_ = w->x_lp_ = w->previous_x_ = tt * 6.9999f;
        w->y_pre_lp_ = w->y_lp_ = w->previous_y_ = mm * 6.9999f;
        w->z_pre_lp_ = w->z_lp_ = w->previous_z_ = hh * 6.9999f;
        plaits::EngineParameters p = {};
        p.trigger = plaits::TRIGGER_LOW;
        p.note = v.note;
        p.harmonics = hh;
        p.timbre = tt;
        p.morph = mm;
        p.accent = 0.8f;
        bool enveloped = false;
        e->Render(p, bufferOut, bufferAux, FRAME_BUFFER_SIZE, &enveloped);
        w->diff_out_.lp_ = 0.f;
    }
}

// what plaits::Voice::Render() takes as Plaits' engine index: speech (15) and chiptune (7) get extra settings there
static int plaits_index(int id)
{
    if (id >= 16)
        return id - 16;
    return id == SPEECH_ENGINE ? 15 : 0;
}

bool is_drum()
{
    return _mode.engine == 13 || _mode.engine == 14 || _mode.engine == 15;
}

// a MIDI engine with MIDI voices; the drums are one voice
bool is_poly()
{
    return !is_drum();
}

// the engines that take the chord from HARMONICS (ChordBank::set_chord(), quantizer centre): the knob is the chord
static bool chord_knob(int id)
{
    return id == CHORD_ENGINE || id == STRING_MACHINE_ENGINE || id == CHIPTUNE_ENGINE;
}

// the chord of the knob (0 ... 1): kChordNumChords equal steps
static int chord_of(float knob)
{
    int chord = (int)(knob * plaits::kChordNumChords);
    CONSTRAIN(chord, 0, plaits::kChordNumChords - 1);
    return chord;
}

// The patches of the release (github.com/eh2k/squares-and-circles, src/SYNTH/plaits.cpp of 2026-04, the ENGINE_ALIAS
// lines with layouts): its engines, each V_OCT, the three knobs, Decay and Color or the AuxMix (PD-Mix; 0.5 ... 1
// there, the same crossfade), and MIDI/VAx6 (six VA voices of Plaits, Stereo the spread) as SYNTH/Plaits with their
// model, sounding as before: the engines played V_OCT 2 octaves lower there (the firmware's, engine::cv()), MIDI/VAx6
// the key plus V_OCT as the MIDI voices here. SYNTH/Plaits: 0 V_OCT, 1 Model, 2 HARMONICS, 3 TIMBRE, 4 MORPH, 5 Decay,
// 6 F-Env, 7 AuxMix, 8 Stereo, 9 T-Env, 10 M-Env, 11 Color. SPEECH/LPC (Plaits' LPC words) as the model Speech with
// the bank and the word of its Word, V_OCT 2 octaves down as well. Patches of the names in between (M-OSC/Plaits; OLD
// nullptr) load as they are, those of SYNTH/Speech (Plaits' speech engine as an engine of its own) with the model
// Speech
void engine::restore(uint32_t id, const engine::patch_param *old, int n, engine::patch_param *params)
{
    if (old == nullptr)
        return;

    static_assert(LEN_OF(_models) == 15 + PLAITS_SPEECH + 2 * PLAITS_NOISE, "the model numbers below");
#if PLAITS_SPEECH
    constexpr int speech = 7; // the model Speech, after Chord
#endif
    static const struct
    {
        uint32_t id;
        int model;
        int last; // where the old Color (11) or AuxMix (7) goes
    } oscillators[] = {
        {ENGINE_ID("M-OSC/Virt.Analog"), 0, 11}, {ENGINE_ID("M-OSC/Waveshaping"), 1, 11},
        {ENGINE_ID("M-OSC/2-OP-FM"), 2, 11},     {ENGINE_ID("M-OSC/Grain"), 3, 7},
        {ENGINE_ID("M-OSC/Harmonic"), 4, 7},     {ENGINE_ID("M-OSC/Wavetable"), 5, 7},
        {ENGINE_ID("M-OSC/Chord"), 6, 11},
    };

    params[0] = old[0]; // V_OCT
    for (const auto &o : oscillators)
        if (o.id == id)
        {
            params[0].value -= 2.f;
            params[1].value = o.model;
            for (int i = 1; i <= 4; i++) // the three knobs, Decay
                params[1 + i] = old[i];
            params[o.last] = old[5];
            if (o.id == ENGINE_ID("M-OSC/Wavetable")) // Bank was 0 ... 0.5, the knob 0 ... 1 now
                params[2].value *= 2.f;
            if (o.id == ENGINE_ID("M-OSC/Chord")) // the int, the same 17 chords: the middle of its knob step
                params[2].value = (old[1].value + 0.5f) / plaits::kChordNumChords;
            return;
        }

    if (id == ENGINE_ID("SYNTH/ClassicVAVCF")) // Morph, Cutoff, Harsh, EnvMod, Decay
    {
        params[0].value -= 2.f;
        params[1].value = 10 + PLAITS_SPEECH + 2 * PLAITS_NOISE; // VA-VCF, after Speech, Noise and Particle
        params[4] = old[1];
        params[3] = old[2];
        params[2] = old[3];
        params[9] = old[4]; // T-Env
        params[5] = old[5];
    }
    else if (id == ENGINE_ID("MIDI/VAx6")) // Harmo, Timbre, Morph, Decay, Stereo
    {
        params[1].value = 0; // VA
        for (int i = 1; i <= 4; i++)
            params[1 + i] = old[i];
        params[8] = old[5];
    }
#if PLAITS_SPEECH
    // Word, Speed, Form.Shift, Prosody, AuxMix: Form.Shift is Species (TIMBRE), Prosody and Speed the attenuverters of
    // FM and MORPH (F-Env, M-Env; the same values, -1 ... 1 now). A modulation of Word has no parameter to go to
    else if (id == ENGINE_ID("SPEECH/LPC"))
    {
        int word = (int)old[1].value;
        CONSTRAIN(word, 0, (int)LEN_OF(_speech_words) - 1);
        int bank = 0;
        while (word >= _bank_first[bank + 1] - (bank + 1))
            bank++;
        const int words = _bank_first[bank + 1] - _bank_first[bank] - 1;
        params[0].value -= 2.f;
        params[1].value = speech;
        // the middle of the bank's step of HARMONICS (SpeechEngine::Render(): word_bank_quantizer_ of 6 steps over
        // (group - 2) * 0.275) and of the word's of MORPH
        params[2].value = (2.f + (bank + 1.5f) / 1.65f) / 6.f;
        params[4].value = (word - (_bank_first[bank] - bank) + 0.5f) / words;
        params[3] = old[3];
        params[7] = old[5];
        params[6] = old[4];
        params[10] = old[2];
    }
    // V_OCT, Model (HARMONICS), Species, Phon., Decay, AuxMix, Prosody, Speed
    else if (id == ENGINE_ID("SYNTH/Speech"))
    {
        params[1].value = speech;
        for (int i = 1; i <= 4; i++) // the three knobs, Decay
            params[1 + i] = old[i];
        params[7] = old[5];
        params[6] = old[6];
        params[10] = old[7];
    }
#endif
}

// String and Modal ring by themselves: no LPG, Decay and Color do nothing (Decay is the Level of the drone)
static bool has_lpg(int id)
{
    return id != STRING_ENGINE && id != MODAL_ENGINE;
}

void init_params(float hh, float tt, float mm, const plaits::PostProcessingSettings &settings)
{
    patch.harmonics = harmonics = hh;
    patch.timbre = timbre = tt;
    patch.morph = morph = mm;
    for (auto &v : _voices)
        if (v.engine)
            v.engine->post_processing_settings = settings;
}

#if PLAITS_SPEECH
static bool on_speech_ui(uint16_t type, uint16_t control, int16_t value, uint16_t mask);
#endif

// SYNTH/Plaits: the memory of all voices, the knobs, the engines of the first model
static void setup_plaits()
{
    _plaits = true;
    _mode = {_models[0].engine, 2};

    const size_t small = kEngineBytes + kSmallFloats * sizeof(float);
    uint8_t *mem = (uint8_t *)malloc(small * LEN_OF(_voices));
    float *large = (float *)malloc((kLargeCvFloats * CV_VOICES + kLargeMidiFloats * MIDI_VOICES) * sizeof(float));
    if (mem == nullptr || large == nullptr)
        gfx::message("OUT OF MEMORY"); // without the large part: the models that take it are silent
    for (size_t i = 0; mem && i < LEN_OF(_voices); i++)
    {
        auto &v = _voices[i];
        v.engine_mem = mem + i * small;
        v.small_mem = (float *)(mem + i * small + kEngineBytes);
        const size_t large_at = i < CV_VOICES ? i * kLargeCvFloats
                                              : CV_VOICES * kLargeCvFloats + (i - CV_VOICES) * kLargeMidiFloats;
        v.large_mem = large ? large + large_at : nullptr;
    }

    for (size_t i = 0; i < LEN_OF(_models); i++)
        _model_names[i] = _models[i].name;

    // the right channel only for this engine of the binary (outputBuffer<1>() would make all of them stereo)
    _out_r = engine::outputBuffer_f32(OUT_AUDIO_R);
    if (auto *mem = (StereoEnsemble::E::T *)malloc(1024 * sizeof(StereoEnsemble::E::T)))
        _ensemble.Init(mem);
    else
        _out_r = nullptr; // mono

    harmonics = timbre = morph = 0.5f;
    out_aux_mix = 0.f; // Plaits' OUT
    engine::addParam(V_OCT, &_v_oct);
    engine::addParam("Model", &_model, 0, LEN_OF(_models) - 1, _model_names);
    engine::addParam(_models[0].harmonics, &harmonics); // named after the model in draw()
    engine::addParam(_models[0].timbre, &timbre);
    engine::addParam(_models[0].morph, &morph);
    engine::addParam("Decay", &_decay);
    // page 2: each knob under the one of page 1 it acts on (F-Env under V_OCT, T-Env under TIMBRE, M-Env under MORPH,
    // Color under Decay: the LPG, AuxMix under Model: which of the model's outputs)
    engine::addParam("\fF-Env", &_fm_env, -1.f, 1.f);
    engine::addParam("AuxMix", &out_aux_mix);
    engine::addParam("Stereo", &_stereo);
    engine::addParam("T-Env", &_timbre_env, -1.f, 1.f);
    engine::addParam("M-Env", &_morph_env, -1.f, 1.f);
    engine::addParam("Color", &_colour);
#if PLAITS_SPEECH
    engine::setUIHandler(on_speech_ui);
#endif

    for (auto &v : _voices)
        load_model(v, 0);
}

void engine::setup()
{
    const char *name = engine::name();

    while (*name++ != '/')
        ;

    if (!strcmp(engine::name(), "SYNTH/Plaits"))
        _mode = {-1, 2}; // setup_plaits()
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
#if PLAITS_SIX_OP
    else if (!strcmp(name, "6-OP-FM"))
        _mode = {SIX_OP_ENGINE, 0};
#endif
    else
        _mode = {-1, 2};

    for (auto &v : _voices)
        v.voice.Init();

    memset(&patch, 0, sizeof(patch));
    patch.note = DEFAULT_NOTE;
    memset(&modulations, 0, sizeof(modulations));

    patch.lpg_colour = 0.5;
    patch.decay = 0.5;

    switch (_mode.engine)
    {
    case -1:
        setup_plaits();
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

    if (!_plaits) // SYNTH/Plaits: load_model()
        for (auto &v : _voices)
        {
            v.id = _mode.engine;
            if (v.engine)
                v.engine->Reset();
        }

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

    // of the voice's engine: in SYNTH/Plaits the voices go over to another model one a frame (process())
    patch.engine = plaits_index(v.id);
    modulations.engine = patch.engine;
    // * 1.02f * kChordNumChords - 0.5f in the quantizer: the chord, no glide through the others
    const float harmonics = patch.harmonics;
    if (chord_knob(v.id))
        patch.harmonics = (_chord + 0.5f) / (1.02f * plaits::kChordNumChords);
    // the chiptune's envelope (Plaits: the TIMBRE attenuverter) on the arpeggiator of the Trig/CV voice, which
    // bypasses the LPG: Decay 1 none (the attenuverter at noon), the lower the shorter, OUT and the bass on AUX
    patch.timbre_modulation_amount = v.id == CHIPTUNE_ENGINE ? 1.f - _decay : _timbre_env;
    patch.morph_modulation_amount = _morph_env;
    patch.frequency_modulation_amount = _fm_env;
    // the chiptune's MIDI voices and drone: the LPG as a VCA, the squares stay square
    patch.lpg_colour = v.id == CHIPTUNE_ENGINE ? 1.f : _colour;

    if (v.id == CHORD_ENGINE)
        static_cast<plaits::ChordEngine *>(v.engine)->chords_.chord_index_quantizer_.quantized_value_ = _chord;
#if PLAITS_SIX_OP
    if (v.id == SIX_OP_ENGINE)
    {
        auto *e = static_cast<SixOpGateEngine *>(v.engine);
        e->patch_index = _patch;
        e->gate_ = v.held;
    }
#endif

    v.voice.Render(v.engine, patch, modulations, bufferOut, bufferAux, FRAME_BUFFER_SIZE);
    patch.harmonics = harmonics;

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

// SYNTH/Plaits: the voices go over to the model of the Model knob, one a frame, the sounding ones first: a change is
// the engine's Init() and Reset(), up to 32 KB written (String, Particle's diffuser), five at once more than a frame
// can take. A voice that is triggered goes over at once (switch_voice()); until then a voice plays its model on
static void switch_voices()
{
    poly_voice *next = nullptr;
    for (auto &v : _voices)
        if (v.model != _active && (next == nullptr || (next->idle && !v.idle)))
            next = &v;
    if (next)
        load_model(*next, _active);
}

static void switch_voice(poly_voice &v)
{
    if (_plaits && v.model != _active)
        load_model(v, _active);
}

void engine::process()
{
    float *out = engine::outputBuffer<0>();
    memset(out, 0, sizeof(float) * FRAME_BUFFER_SIZE);

    if (_plaits)
    {
        _active = _model; // with its modulation
        CONSTRAIN(_active, 0, (int)LEN_OF(_models) - 1);
        _mode.engine = _models[_active].engine;
        switch_voices();
    }

    // the Wavetable's bank: HARMONICS up to 0.5, the four banks crossfaded (above: the same without crossfades)
    const float h = _plaits && _mode.engine == WAVETABLE_ENGINE ? harmonics * 0.5f : harmonics;
    if (!_smoothed) // from the knobs on (the patch's, set after setup())
    {
        patch.harmonics = h, patch.timbre = timbre, patch.morph = morph;
        _smoothed = true;
    }
    float a = _last_out / 256.f;
    ONE_POLE(patch.harmonics, h + a, 0.1f);
    ONE_POLE(patch.timbre, timbre + a, 0.1f);
    ONE_POLE(patch.morph, morph + a, 0.1f);
    _chord = chord_of(harmonics);

    // On its MIDI channel the firmware also makes a Trig of each Note On (midi.cpp process()), in this frame: that
    // one the MIDI voices play already
    const bool midi_trig = _midi_noteons > 0;
    _midi_noteons = 0;

    // the Classic VA-VCF's decay envelope does not hold at 1
    const float decay = _mode.engine == CLASSIC_VAVCF_ENGINE ? std::min(_decay, 0.99f) : _decay;
    patch.decay = decay;
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
        switch_voice(_cv_voice);
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

        // a drum is computed all the time, as when it was the only voice (its noise and random state go on). The model
        // first: an idle voice's LPG is closed, it needs no start without a spike (load_model())
        if (trig)
            switch_voice(_cv_voice);
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
    // chiptune) always: Plaits' limiter of Modal or Swarm lets the attack of a Trig through, up to 1.35. And with the
    // pitch envelope (F-Env): Chord and Wavetbl, 48 semitones down, peak at 1.5
    bool limit = (_mode.engine >= SPEECH_ENGINE && !is_drum() && _mode.engine != CLASSIC_VAVCF_ENGINE) ||
                 (_fm_env != 0.f && !drone());
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
            switch_voice(v); // while idle, as the Trig/CV voice
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

    if (_out_r == nullptr)
        return;
    if (_stereo <= 0.f) // mono: as before, on both channels
    {
        memcpy(_out_r, out, sizeof(float) * FRAME_BUFFER_SIZE);
        _ensemble_on = false;
        return;
    }
    if (!_ensemble_on)
        _ensemble.Reset();
    _ensemble_on = true;
    // amount and depth as the string machine's ensemble at its Filter knob's ends. Dry and wet cancel each other in
    // part (a comb, the taps 4 ms late): each channel 2 dB down at 0.5 and 2.6 dB at 1 (QEMU, six models), made up
    _ensemble.Process(_stereo, 0.35f + 0.65f * _stereo, 1.f + 0.35f * sqrtf(_stereo), out, out, _out_r,
                      FRAME_BUFFER_SIZE);
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++) // the dry and the wet signal add up to more than 1
    {
        out[i] = softLimit(out[i]);
        _out_r[i] = softLimit(_out_r[i]);
    }
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

static char _chord_label[24]; // "Chord\n<name>": the knob's name, the chord as its value

#if PLAITS_SPEECH
static char _speech_bank_label[16]; // "Bank\n<kind or bank>"
static char _speech_word_label[24]; // "Word\n<word>"

// The model Speech's HARMONICS above 2/6 (SpeechEngine::Render()): the 6 steps of word_bank_quantizer_, the LPC
// phonemes and the 5 word banks, each 1/9.9 of the knob
constexpr float kSpeechLPC = 2.f / 6.f;
constexpr float kSpeechBankWidth = 1.f / (6.f * 0.275f * 6.f);

// the step of HARMONICS h above kSpeechLPC (word_bank_quantizer_ without its hysteresis)
static int speech_step(float h)
{
    int step = (int)((h - kSpeechLPC) / kSpeechBankWidth);
    CONSTRAIN(step, 0, LPC_SPEECH_SYNTH_NUM_WORD_BANKS);
    return step;
}

// the word bank of the modulated HARMONICS, -1: none (the vowels or the phonemes)
static int speech_bank()
{
    const float h = engine::getParamValueModulated_f32(&harmonics);
    return h > kSpeechLPC ? speech_step(h) - 1 : -1;
}

// the words of a word bank
static int speech_words(int bank)
{
    return _bank_first[bank + 1] - _bank_first[bank] - 1;
}

// The model Speech: the left encoder steps Bank and Word a bank or word a click. The firmware turns a float knob by
// 1/128 (1/24 when turned fast): a bank is 13 clicks wide, a word 4 ... 16, and a click did not always change them.
// Bank goes to the middle of the next step (below the phonemes: the LPC vowels, 2/6, below that the firmware turns
// it), Word with Trig or MIDI to the middle of the next word; without them MORPH scans the bank's frames and turns on
// as any knob. The value without modulation: the modulation goes on top
static bool on_speech_ui(uint16_t type, uint16_t control, int16_t value, uint16_t mask)
{
    if (type != EVENT_ENCODER || control != ENCODER_L || !_plaits || _models[_active].engine != SPEECH_ENGINE)
        return false;

    if (engine::isParamSelected(&harmonics))
    {
        if (harmonics <= kSpeechLPC) // the vowels: on into the phonemes from the click that passes 2/6
        {
            if (value <= 0 || harmonics + value / 128.f <= kSpeechLPC)
                return false;
            harmonics = kSpeechLPC + 0.5f * kSpeechBankWidth;
            return true;
        }
        const int step = speech_step(harmonics) + value;
        if (step < 0)
            harmonics = kSpeechLPC;
        else
            harmonics = kSpeechLPC + (std::min(step, LPC_SPEECH_SYNTH_NUM_WORD_BANKS) + 0.5f) * kSpeechBankWidth;
        return true;
    }

    const int bank = speech_bank();
    if (engine::isParamSelected(&morph) && bank >= 0 && !drone()) // GetWordBoundaries()
    {
        const int words = speech_words(bank);
        int word = std::min((int)(morph * words), words - 1) + value;
        CONSTRAIN(word, 0, words - 1);
        morph = (word + 0.5f) / words;
        return true;
    }
    return false;
}

// The model Speech: what HARMONICS and MORPH play (SpeechEngine::Render()): HARMONICS the vowels of the formant filter,
// of SAM and of LPC, the LPC phonemes, then the word banks; MORPH in a word bank the word
static void draw_speech()
{
    static const char *const kinds[] = {"Formant", "SAM", "LPC", "Phoneme", "Colours", "Numbers", "Letters", "NATO",
                                        "Synth"};
    const float group = engine::getParamValueModulated_f32(&harmonics) * 6.f;
    const int bank = speech_bank();
    // crossfades: formant 0, SAM 1, LPC 2
    const int kind = group <= 2.f ? (group < 0.5f ? 0 : group < 1.5f ? 1 : 2) : 4 + bank;
    snprintf(_speech_bank_label, sizeof(_speech_bank_label), "Bank\n%s", kinds[kind]);
    engine::setParamName(&harmonics, _speech_bank_label);

    if (bank >= 0) // GetWordBoundaries()
    {
        const int words = speech_words(bank);
        int word = (int)(engine::getParamValueModulated_f32(&morph) * words);
        CONSTRAIN(word, 0, words - 1);
        const char *name = _speech_words[_bank_first[bank] - bank + word];
        // 8 characters fit: "synthes." for "synthesizer"
        snprintf(_speech_word_label, sizeof(_speech_word_label), strlen(name) > 8 ? "Word\n%.7s." : "Word\n%s", name);
        engine::setParamName(&morph, _speech_word_label);
    }
}
#endif

void engine::draw()
{
    if (!_plaits)
    {
        engine::setParamName(&_decay, drone() ? "Level" : "Decay");
        return;
    }

    // the knobs of the model that plays (with the modulation of the Model knob)
    const plaits_model &m = _models[_active];
    if (chord_knob(m.engine))
    {
        snprintf(_chord_label, sizeof(_chord_label), "Chord\n%s",
                 plaits::chord_names[chord_of(engine::getParamValueModulated_f32(&harmonics))]);
        engine::setParamName(&harmonics, _chord_label);
    }
    else
        engine::setParamName(&harmonics, m.harmonics);
    if (m.engine == CHIPTUNE_ENGINE) // the arpeggio of the Trig/CV voice; without Trig the chord's inversion
        engine::setParamName(&timbre, engine::hasTrig() ? "Arp" : "Inv.");
    else
        engine::setParamName(&timbre, m.timbre);
    engine::setParamName(&morph, m.morph);
#if PLAITS_SPEECH
    if (m.engine == SPEECH_ENGINE)
        draw_speech();
#endif

    // what does nothing for the model is not shown (an empty name)
    const bool lpg = has_lpg(m.engine);
    engine::setParamName(&_decay, drone() ? "Level" : lpg ? "Decay" : "");
    engine::setParamName(&_colour, lpg && m.engine != CHIPTUNE_ENGINE ? "Color" : "");
    engine::setParamName(&_timbre_env, m.engine != CHIPTUNE_ENGINE ? "T-Env" : "");
    // page 2 begins with it. Speech: with the Trig patched the attenuverters of FM and MORPH are the words' intonation
    // and speed (Voice::Render()); without Trig the one of FM is a fine tune for every model
    const bool speech = m.engine == SPEECH_ENGINE;
    engine::setParamName(&_fm_env, drone() ? "\fFine" : speech ? "\fProsody" : "\fF-Env");
    engine::setParamName(&_morph_env, speech ? "Speed" : "M-Env");
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
#if PLAITS_NOISE
#include "lib/plaits/dsp/engine/noise_engine.cc"
#endif
#include "lib/plaits/dsp/engine/hi_hat_engine.cc"
#if PLAITS_NOISE
#include "lib/plaits/dsp/engine/particle_engine.cc"
#endif
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
