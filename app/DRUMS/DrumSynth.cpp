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

// General MIDI Percussion Key Map:
// MIDI Key   Drum Sound             MIDI Key    Drum Sound
// --------   ----------            ----------   ----------
//    35     Acoustic Bass Drum        59      Ride Cymbal 2
//    36     Bass Drum 1               60      Hi Bongo
//    37     Side Stick                61      Low Bongo
//    38     Acoustic Snare            62      Mute Hi Conga
//    39     Hand Clap                 63      Open Hi Conga
//    40     Electric Snare            64      Low Conga
//    41     Low Floor Tom             65      High Timbale
//    42     Closed Hi-Hat             66      Low Timbale
//    43     High Floor Tom            67      High Agogo
//    44     Pedal Hi-Hat              68      Low Agogo
//    45     Low Tom                   69      Cabasa
//    46     Open Hi-Hat               70      Maracas
//    47     Low-Mid Tom               71      Short Whistle
//    48     Hi-Mid Tom                72      Long Whistle
//    49     Crash Cymbal 1            73      Short Guiro
//    50     High Tom                  74      Long Guiro
//    51     Ride Cymbal 1             75      Claves
//    52     Chinese Cymbal            76      Hi Wood Block
//    53     Ride Bell                 77      Low Wood Block
//    54     Tambourine                78      Mute Cuica
//    55     Splash Cymbal             79      Open Cuica
//    56     Cowbell                   80      Mute Triangle
//    57     Crash Cymbal 2            81      Open Triangle
//    58     Vibraslap

static std::pair<uint8_t, uint8_t> midi_key_map[] = {
    {35, -1}, // BD0
    {36, -1}, // BD1
    {37, -1}, // RM
    {38, -1}, // SD0
    {39, -1}, // CP
    {40, -1}, // SD1
    {41, -1}, // LT
    {42, -1}, // HH - Closed
    {43, -1}, // LT2
    {44, -1}, // HH - Pedal
    {45, -1}, // MT
    {46, -1}, // HH - Open
    {47, -1}, // MT2
    {48, -1}, // HT
    {49, -1}, // CR - Crash
    {50, -1}, // HT2
    {51, -1}, // RD - Ride
    {52, -1}, // CY - Chinese
    {53, -1}, // RD2 - Ride Bell
    {54, -1}, // TMB
    {55, -1}, // CR2 - Crash Edge
    {56, -1}, // CB
    {57, -1}, // CR3 - Crash Bow
    {58, -1}, // VS - Vibraslap
    {59, -1},  // RD3 - Ride Edge
    {0, -1},   // FREE
    {0, -1},   // FREE
    {0, -1},   // FREE
    {0, -1},   // FREE
    {0, -1},   // FREE
    {0, -1},   // FREE
    {0xFF, 0}, // END
};

uint8_t& midi_key_entry(uint8_t midi_note)
{
    if(midi_note >= 35 && midi_note <= 59)
    {
        return midi_key_map[midi_note - 35].second;
    }
    for (size_t i = (60-35); i < LEN_OF(midi_key_map); i++)
    {
        if (midi_key_map[i].first == 0)
        {
            midi_key_map[i].first = midi_note;
            return midi_key_map[i].second;
        }
    }
    return midi_key_map[LEN_OF(midi_key_map) - 2].second; // invalid
}

void map_as_multiple(uint8_t a, uint8_t b)
{
    uint8_t* aa = &midi_key_entry(a);
    uint8_t* bb = &midi_key_entry(b);

    if (*bb == 0xFF)
        *bb = *aa;
    if (*aa == 0xFF)
        *aa = *bb;
}

bool is_maped_as_multiple(uint8_t a, uint8_t b)
{
    uint8_t aa = midi_key_entry(a);
    uint8_t bb = midi_key_entry(b);

    return aa == bb;
}

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
int32_t _open_hihat = -1;

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
            midi_key_entry(_instModel[i].midi_note) = i;

            _t[i] = MAX_T;
            _inst[i] = drum_synth_init(&_instModel[i], ::malloc);
        }

        map_as_multiple(35, 36); // BD
        map_as_multiple(38, 40); // SD
        map_as_multiple(42, 44); // Closed HH
        map_as_multiple(41, 43); // LT
        map_as_multiple(45, 47); // MT
        map_as_multiple(48, 50); // HT
        map_as_multiple(51, 59); // RD
        map_as_multiple(49, 55); // CR

        map_as_multiple(49, 51); // CR/RD

        _open_hihat = midi_key_entry(46);

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
            if(_open_hihat != -1 && _instModel[i].midi_note == 42 || _instModel[i].midi_note == 44) // closed hihat
            {
                _t[_open_hihat] = MAX_T; // mute open hihat
            }
        }

        if (_t[i] < MAX_T)
        {
            DrumParams params = {_t[i], 0, stretch, stereo, 0, 0};
            
            if(i == _open_hihat) 
            {
                params.decay = 0.1f + stretch + (-stretch * ((float)engine::getMidiCC(4) / 127.f));
            }

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
