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

#include "../../squares-and-circles-api.h"
#include "stmlib/algorithms/voice_allocator.h"
#include "stmlib/dsp/dsp.h"

ENGINE_NAME("SYNTH/DxFM;SYNTH/DxFM_BNK1-3");

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

// msfa renders blocks of N (64) samples, the engine reads 24 a frame: each voice has its block and a read position.
// The positions are staggered (setup()), so the voices render on different frames: 8 voices are 3 blocks in
// every frame, not 8 blocks in one frame and none in the next two.
struct dxfm
{
    int note = 0;     // the note Dx7Note::init() got; the pitch beyond it goes in by masterTune
    int key_down = 0; // 0: released, -1: held (MIDI), n > 0: keyup when it has counted down to 1
    int velo = 127;
    int32_t tune = 0; // Q24 octaves above note
    bool idle = true; // released and silent: not computed
    bool started = false; // a block computed since the note started: before it a keyup leaves the envelope at 0
    bool silent = true;
    uint8_t quiet = 0;
    // MIDI voices: set by the handlers, taken over by process()
    uint8_t key = 0;
    uint8_t velocity = 0;
    uint8_t pending = 0;
    bool sustained = false;
    int pos = N;
    Lfo lfo;
    Dx7Note dx7_note;
    int32_t buf[N];
};

constexpr int CV_VOICES = 2;   // Trig/CV: two voices take turns, so the release of a note rings on under the next
constexpr int MIDI_VOICES = 8; // ENGINE_MODE_MIDI_IN: polyphonic
constexpr uint8_t NOTE_ON = 1;
constexpr uint8_t NOTE_OFF = 2;

static dxfm voices[CV_VOICES + MIDI_VOICES] = {};
static dxfm *const midi_voices = &voices[CV_VOICES];
static stmlib::VoiceAllocator<MIDI_VOICES> _allocator;
static FmCore _fm_core; // only scratch buffers: one for all voices
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

static float _bend = 0;        // semitones, +-12 like the firmware's pitch bend for V/OCT
static bool _sustain = false;  // CC 64
static uint8_t _midi_noteons = 0; // Note Ons since the last process()

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

// the voice name: 10 characters in the patch, not terminated
static void setProgName()
{
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
}

static bool loadDXPatch(uint8_t prog)
{
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
            setProgName();
            return true;
        }

        snprintf(_progName, sizeof(_progName), "INVALID");
    }

    return false;
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
        size_t len = 0;
        const uint8_t *syx = machine::fs_read("DXFMSYX0", &len);
        if (syx && len >= 32 * 128) // 32 packed voices
        {
            fm_patches_table[0] = syx;
            nprogs += 32;
        }
    }

    if (nprogs > 96)
        _progSelect = 96;

    for (size_t prog = 0; prog < nprogs; prog++)
    {
        const uint8_t *sysexData = fm_patches_table[prog / 32];
    }

    engine::addParam(V_OCT, &_pitch);

    if (nprogs == 0)
    {
        nprogs = 1;
    }

    setProgName();
    engine::addParam(_progName, &_progSelect, 0, nprogs - 1);

    _rate = -1.0f;
    engine::addParam("Rate", &_rate, -1.5f, -0.5f);

    _hold = 0;
    engine::addParam("Hold", &_hold, 0, SAMPLE_RATE / FRAME_BUFFER_SIZE);
    engine::addParam("Stereo", &_stereolize);

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

    _allocator.Init();
    _allocator.set_size(MIDI_VOICES);

    loadDXPatch(0); // If patch available, select first

    // voice v starts as if it had played v frames: with 8 voices every frame renders 3 blocks
    for (size_t v = 0; v < LEN_OF(voices); v++)
    {
        voices[v].pos = N - (int)(v * FRAME_BUFFER_SIZE) % N;
        voices[v].lfo.reset(&LFO_RATE());
    }

    engine::setMode(ENGINE_MODE_MIDI_IN);
}

// Velocity of a Trig, and the cap of a MIDI velocity: the left half of Rate lowers it down to 64
static int rateVelocity()
{
    float r = _rate + 1.5f; // 0..1
    return r < 0.5f ? 64 + 64 * (r * 2) : 127;
}

static void noteOn(dxfm &voice, int note, int velo)
{
    voice.note = note;
    voice.velo = velo;
    voice.lfo.keydown();
    voice.dx7_note.keyup();
    voice.dx7_note.init(data, voice.note, velo);

    if (OSC_SYNC())
        voice.dx7_note.oscSync();

    voice.idle = false;
    voice.started = false;
    voice.quiet = 0;
}

static void keyUp(dxfm &voice)
{
    voice.key_down = 0;
    voice.dx7_note.keyup();
}

// Q24 octaves from the voice's note to the note to play
static int32_t tuneTo(const dxfm &voice, float note)
{
    return (note - voice.note) / 12.f * (1 << 24);
}

static void render(dxfm &voice)
{
    int32_t lfovalue = voice.lfo.getsample();
    int32_t lfodelay = voice.lfo.getdelay();

    voice.silent = voice.idle;
    if (voice.idle)
        return;

    memset(voice.buf, 0, sizeof(voice.buf));
    controllers.masterTune = voice.tune;
    voice.dx7_note.compute(voice.buf, &_fm_core, lfovalue, lfodelay, &controllers);
    voice.started = true;

    // released and below -96 dB for 8 blocks (~11 ms): not computed until the next note
    if (voice.key_down == 0)
    {
        int32_t peak = 0;
        for (int i = 0; i < N; i++)
            peak = std::max(peak, std::abs(voice.buf[i]));

        voice.quiet = peak < (1 << 12) ? voice.quiet + 1 : 0;
        voice.idle = voice.quiet >= 8;
    }
}

// Up to KNEE as it is (single notes of the built-in voices peak at 0.84), above it rounded off softly, 1.0 at
// KNEE + 3 * (1 - KNEE) = 1.3: the chords sum their notes (eight notes up to 2.6)
static inline float softLimit(float x)
{
    constexpr float KNEE = 0.85f;
    if (x > KNEE)
        return KNEE + (1.f - KNEE) * stmlib::SoftClip((x - KNEE) / (1.f - KNEE));
    if (x < -KNEE)
        return -KNEE - (1.f - KNEE) * stmlib::SoftClip((-x - KNEE) / (1.f - KNEE));
    return x;
}

static void mix(dxfm &voice, float *out)
{
    if (voice.idle && voice.silent) // only the position moves on, as if it rendered silence
    {
        voice.pos += FRAME_BUFFER_SIZE;
        if (voice.pos > N)
            voice.pos -= N;
        return;
    }

    constexpr float gain = 8.f / INT32_MAX;
    for (int n = 0; n < FRAME_BUFFER_SIZE;)
    {
        if (voice.pos == N)
        {
            render(voice);
            voice.pos = 0;
        }

        const int k = std::min(FRAME_BUFFER_SIZE - n, N - voice.pos);
        if (!voice.silent)
            for (int i = 0; i < k; i++)
                out[n + i] += voice.buf[voice.pos + i] * gain;

        n += k;
        voice.pos += k;
    }
}

void engine::process()
{
    applyRate(_rate); // * ((float)rand() / INT32_MAX * 0.1f));

    if (_progSelect != _prog)
    {
        _prog = _progSelect; // an invalid patch is not tried again in every block

        if (loadDXPatch(_progSelect))
            for (auto &voice : voices)
            {
                voice.dx7_note.keyup();
                voice.lfo.reset(&LFO_RATE());
                voice.dx7_note.update(data, voice.note, voice.velo);
            }
    }

    // On its MIDI channel the firmware also makes a Trig of each Note On (midi.cpp process()), in this frame: that
    // one the MIDI voices play already
    const bool midi_trig = _midi_noteons > 0;
    _midi_noteons = 0;

    // see midinote_to_logfreq
    float note = (2.f + engine::cv()) * 12.f + DEFAULT_NOTE;

    if (engine::trig() && !midi_trig)
    {
        other_voice->key_down = 2 + _hold;
        noteOn(*other_voice, note, rateVelocity());
        std::swap(active_voice, other_voice);
    }
    else if (engine::gate())
    {
        // keep key down
    }
    else
    {
        for (int v = 0; v < CV_VOICES; v++)
            if (voices[v].key_down > 1)
                --voices[v].key_down;
    }

    for (int v = 0; v < CV_VOICES; v++)
    {
        if (voices[v].key_down == 1 && voices[v].started)
            keyUp(voices[v]);

        if (!voices[v].idle)
            voices[v].tune = tuneTo(voices[v], note);
    }

    const float midi_offset = _pitch * 12.f + _bend; // the V_OCT knob, 0 plays the key

    for (int v = 0; v < MIDI_VOICES; v++)
    {
        auto &voice = midi_voices[v];
        if (voice.pending & NOTE_ON)
        {
            noteOn(voice, voice.key + _pitch * 12.f, std::max(1, voice.velocity * rateVelocity() / 127));
            voice.key_down = -1;
        }
        if (voice.pending & NOTE_OFF)
            voice.key_down = 1 + _hold;
        voice.pending = 0;

        if (voice.key_down > 1)
            --voice.key_down;
        else if (voice.key_down == 1 && voice.started)
            keyUp(voice);

        if (!voice.idle)
            voice.tune = tuneTo(voice, voice.key + midi_offset);
    }

    auto bufferL = engine::outputBuffer<0>();
    auto bufferR = engine::outputBuffer<1>();

    // even voices right, odd voices left, by Stereo
    float side[2][FRAME_BUFFER_SIZE] = {};
    for (size_t v = 0; v < LEN_OF(voices); v++)
        mix(voices[v], side[v % 2]);

    float stereo = (1.f - _stereolize);
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        bufferL[i] = softLimit(side[0][i] * stereo + side[1][i]);
        bufferR[i] = softLimit(side[0][i] + side[1][i] * stereo);
    }
}

static void releaseAll()
{
    for (int v = 0; v < MIDI_VOICES; v++)
    {
        midi_voices[v].pending |= NOTE_OFF;
        midi_voices[v].sustained = false;
    }
    _allocator.ClearNotes();
}

void engine::onMidiNote(uint8_t key, uint8_t velocity) // NoteOff: velocity == 0
{
    if (velocity > 0)
    {
        uint8_t v = _allocator.NoteOn(key);
        if (v >= MIDI_VOICES)
            return;
        auto &voice = midi_voices[v];
        voice.key = key;
        voice.velocity = velocity;
        voice.pending = NOTE_ON;
        voice.sustained = false;
        ++_midi_noteons;
    }
    else
    {
        uint8_t v = _allocator.NoteOff(key);
        if (v >= MIDI_VOICES)
            return;
        auto &voice = midi_voices[v];
        if (_sustain)
            voice.sustained = true;
        else
            voice.pending |= NOTE_OFF;
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
                if (midi_voices[v].sustained)
                {
                    midi_voices[v].sustained = false;
                    midi_voices[v].pending |= NOTE_OFF;
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
    if (code == MIDI_RTM_STOP) // like the firmware's MIDI Stop for the other engines: the gates fall
        releaseAll();
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
        return;
    }

    // the MIDI voices while any sounds: filled = key held, open = release
    bool any = false;
    for (int v = 0; v < MIDI_VOICES; v++)
        any |= !midi_voices[v].idle;

    for (int v = 0; any && v < MIDI_VOICES; v++)
    {
        const auto &voice = midi_voices[v];
        const int x = 72 + v * 7;
        if (voice.key_down < 0 || voice.sustained)
            gfx::fillRect(x, 52, 5, 5);
        else if (!voice.idle)
            gfx::drawRect(x, 52, 5, 5);
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