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

struct EnvXY
{
    uint32_t t;
    float v;
};

struct EnvArgs
{
    int32_t n;
    const EnvXY *xy; //[16];
};

struct WS_XY
{
    float x;
    float y;
};

struct WSArgs
{
    uint32_t n;
    const WS_XY *xy; //[8];
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
};

struct DrumModel
{
    const char *name;
    uint8_t midi_note;
    size_t n;
    const PartArgs *part;
};

struct DrumParams
{
    uint32_t t;
    float attack;
    float decay;
    float stereo;
    float levelL;
    float levelR;
};

struct DrumKit
{
    int n;
    const DrumModel *inst;
};

typedef uint32_t *DrumSynth;

extern "C"
{
    DrumSynth drum_synth_init(const DrumModel *inst, void *(*malloc)(size_t size));
    void drum_synth_deinit(DrumSynth inst, void (*free)(void* ptr));
    void drum_synth_process_frame(DrumSynth inst, int part, float freq, const DrumParams *params, float *outL, float *outR, size_t size);
    void drum_synth_reset(DrumSynth inst);
    int drum_synth_load_models(const uint8_t *drumkit, DrumModel _instModel[16], void *(*malloc)(size_t size));
}