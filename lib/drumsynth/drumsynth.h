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

#pragma once

#include <inttypes.h>
#include <stddef.h>
#include <array>

// a point of an envelope (drum_synth_envelope_point): t in samples at 48 kHz
struct EnvXY
{
    uint32_t t;
    float v;
};

// the points of an envelope in the drumkit buffer, packed (varint t, half float v; see README.md)
struct EnvArgs
{
    int32_t n;
    const uint8_t *points;
};

// the points of a waveshaper in the drumkit buffer: int16 x, y on the pixel grid of the RC-808 waveshaper dialog
struct WSArgs
{
    uint32_t n;
    const uint8_t *points;
};

enum BiquadMode : uint32_t
{
    BIQUAD_THRU = 0,
    BIQUAD_LP,
    BIQUAD_HP,
    BIQUAD_BP,
    BIQUAD_NOTCH,
    BIQUAD_PKG,
    BIQUAD_LSV,
    BIQUAD_HSV,
    BIQUAD_LP1, // first order (one pole) low pass at f, output gain g; q is not used
    BIQUAD_HP1, // first order high pass at f, output gain g; q is not used
};

struct BiquadArgs
{
    BiquadMode mode;
    float f;
    float q;
    float g;
};

enum OscType : uint32_t
{
    OSC_NONE = 0,
    OSC_SINE = 1,
    OSC_SAW,
    OSC_TRI,
    OSC_SQUARE,
    OSC_METALLIC,
    OSC_NOISE1,
    OSC_NOISE2,
    OSC_NOISE3,
    OSC_NOISE_NES,
    OSC_NOISE_SID,
    OSC_SAMPLE, // plays entry n of the kit's sample table: fa = its sample rate (Hz), fb = start (samples, fractional, < 0: a delay)
};

// the sample table entry's format
enum SampleFormat : uint32_t
{
    SAMPLE_U8 = 0,    // unsigned bytes, 128 = 0 (as the TR-909's cymbal ROMs)
    SAMPLE_S8 = 1,    // signed bytes
    SAMPLE_S16LE = 2, // signed 16 bit, little endian
};

struct OscArgs
{
    OscType type;
    float fa;
    float fb;
    float duty;
    uint32_t n;
};

struct AmpMod
{
    uint32_t dest;
    float offset;
};

struct VCFArgs
{
    float cutoff;
    float res;
    float envDepth;
    float velDepth;
};

enum PartFlags : uint32_t
{
    BIQUAD_SERIAL = 1 << 1,
    BIQUAD_PARALLEL = 1 << 2,
    VCF = 1 << 3,
    SAMPLE_FALLBACK = 1 << 4, // plays only if the instrument has no OSC_SAMPLE part whose sample was found
};

struct PartArgs
{
    PartFlags flags;
    OscArgs osc;
    EnvArgs osc_pitch;
    EnvArgs osc_amp;
    EnvArgs vca;
    BiquadArgs bq1;
    BiquadArgs bq2;
    WSArgs ws;
    const VCFArgs *vcf;
    EnvArgs vcf_env;
    AmpMod amp_mod;
    float level;
    // set by the loader (OSC_SAMPLE): the sample's data, its length in samples and format; muted: the part
    // does not play (a sample that was not found, or a SAMPLE_FALLBACK part of an instrument with its sample)
    const uint8_t *sample;
    uint32_t sample_len;
    SampleFormat sample_fmt;
    bool muted;
};

struct DrumModel
{
    const char *name;
    size_t n;
    const PartArgs *part;
    uint8_t midi_note; // MIDI note number of the instrument (last byte of the 12 byte name field), 0 if not set
};

struct DrumParams
{
    uint32_t t;
    float attack;
    float decay;
    float stereo;
    float levelL;
    float levelR;
    float velocity; // 0..1 (MIDI velocity / 127), used by VCF velocity depth
};

struct DrumKit
{
    int n;
    const DrumModel *inst;
};

typedef uint32_t *DrumSynth;

// the samples a drumkit refers to by name (its SMPL table): returns the data and sets *len (bytes), nullptr if the
// sample is not there; the data must outlive the models (it is referenced, not copied)
typedef const uint8_t *(*DrumSynthSampleResolver)(const char *name, size_t *len);

#define DRUM_SYNTH_MAX_MODELS 16
#define DRUM_SYNTH_MAX_OSC 32 // oscillators per part (metallic), more is rejected by the loader

extern "C"
{
    // one malloc per instrument, nullptr if out of memory
    DrumSynth drum_synth_init(const DrumModel *inst, void *(*malloc)(size_t size));
    void drum_synth_deinit(DrumSynth inst, void (*free)(void *ptr));
    void drum_synth_process_frame(DrumSynth inst, int part, float freq, const DrumParams *params, float *outL, float *outR, size_t size);
    void drum_synth_reset(DrumSynth inst);
    // waveshaper transfer function of a part (x = -1..1), identity if the part has no waveshaper
    float drum_synth_waveshaper(DrumSynth inst, int part, float x);
    // drumkit buffer (!RC8, see README.md; 4 byte aligned) must outlive the models (envelope data is referenced, not
    // copied); one malloc per instrument
    int drum_synth_load_models(const uint8_t *drumkit, size_t len, DrumModel _instModel[DRUM_SYNTH_MAX_MODELS], void *(*malloc)(size_t size));
    // the same with the samples of the kit's SMPL table (after the instruments: "SMPL", uint32 count, per entry a
    // 16 byte name, uint32 offset, length (samples) and SampleFormat); without a resolver no sample is found
    int drum_synth_load_models_ex(const uint8_t *drumkit, size_t len, DrumModel _instModel[DRUM_SYNTH_MAX_MODELS], void *(*malloc)(size_t size),
                                  DrumSynthSampleResolver resolve);
    void drum_synth_free_models(DrumModel *_instModel, int count, void (*free)(void *ptr));
    // point i of an envelope, false if there is no point i
    bool drum_synth_envelope_point(const EnvArgs *env, int32_t i, EnvXY *xy);
}