// Copyright (C)2024 - E.Heidt
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

// xuild_flags: -fno-inline -mfloat-abi=hard -mfpu=fpv5-d16
// ENGINE_NAME: DRUMS/@!RC8

#include "../../lib/drumsynth/drumsynth.h"
#include "../squares-and-circles-api.h"

static std::pair<uint8_t, uint8_t> midi_key_map[] = {
    {35, 0},  // BD0
    {36, 1},  // BD1
    {38, 2},  // SD0
    {40, 3},  // SD1
    {39, 4},  // CP
    {54, 5},  // TMB
    {37, 6},  // RM
    {56, 7},  // CB
    {41, 8},  // LT
    {43, 8},  // LT
    {45, 9},  // MT
    {47, 9},  // MT
    {48, 10}, // HT
    {50, 10}, // HT
    {42, 11}, // HH - Closed
    {44, 11}, // HH - Pedal
    {46, 12}, // HH - Open
    {0xFF, 0}, //END
};

static constexpr size_t n = 8;

float pitch = 1.f;
float stereo = 0.5f;
float stretch = 1.f;

DrumModel _instModel[16] = {};
DrumSynth _inst[16] = {};

size_t inst_count = 0;
int32_t inst_selection = 0;

uint32_t _t[16] = {};

const char *inst_names[16] = {};

char __debug[128];

constexpr int MAX_T = UINT16_MAX * 4;

void engine::setup()
{
    const uint8_t *drumkit = __data;
    // unpack
    inst_count = drum_synth_load_models(drumkit, _instModel, ::malloc);
    if (inst_count > 0)
    {
        for (int i = 0; i < inst_count; i++)
        {
            inst_names[i] = _instModel[i].name;
            _t[i] = MAX_T;
            _inst[i] = drum_synth_init(&_instModel[i], ::malloc);
        }

        engine::addParam("Pitch", &pitch, 0.5f, 1.5f);
        engine::addParam(MULTI_TRIGS, &inst_selection, 0, inst_count - 1, inst_names);
        engine::addParam("Decay", &stretch, 0.1f, 2.0f);
        engine::addParam("Stereo", &stereo);
    }
    
    engine::setMultiTrigMidiKeyMap(midi_key_map);
}

void engine::release()
{
    for (int i = 0; i < inst_count; i++)
    {
        ::free(_inst[i]);
    }
}

void engine::process()
{
    auto buffer = engine::outputBuffer<0>();
    auto bufferAux = engine::outputBuffer<1>();
    memset(buffer, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    memset(bufferAux, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    float tmpL[FRAME_BUFFER_SIZE];
    float tmpR[FRAME_BUFFER_SIZE];

    for (int i = 0; i < inst_count; i++)
    {
        if (engine::trig() & (1 << i))
        {
            _t[i] = 0;
            drum_synth_reset(_inst[i]);
        }

        if (_t[i] < MAX_T)
        {
            DrumParams params = {_t[i], 0, stretch, stereo};

            float f = pitch * powf(2.f, engine::cv());
            float a = stereo;
            float b = 1.f - a;

            params.levelL = engine::mixLevelL(i) * engine::trigLevel(i);
            params.levelR = engine::mixLevelR(i) * engine::trigLevel(i);

            drum_synth_process_frame(_inst[i], -1, f, &params, buffer, bufferAux, FRAME_BUFFER_SIZE);

            _t[i] += FRAME_BUFFER_SIZE;
        }
    }
}

void engine::draw()
{
}

#include "../../lib/drumsynth/drumsynth.cpp"
#include "../../lib/misc/Biquad.cpp"
#include "../../lib/plaits/resources.cc"
