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

#include "../../../lib/drumsynth/drumsynth.h"
#include "../../squares-and-circles-api.h"

ENGINE_NAME("DRUMKITS/@!RC8");

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
    // the notes outside 35 ... 59: as many as a kit has instruments (16, _instModel), the CR-78 has 7
    {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, // FREE
    {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, // FREE
    {0xFF, 0}, // END
};

uint8_t& midi_key_entry(uint8_t midi_note)
{
    if(midi_note >= 35 && midi_note <= 59)
    {
        return midi_key_map[midi_note - 35].second;
    }
    for (size_t i = (60-35); i < LEN_OF(midi_key_map) - 1; i++)
    {
        if (midi_key_map[i].first == midi_note) // a note of two instruments: the later one, as for 35 ... 59
            return midi_key_map[i].second;
        if (midi_key_map[i].first == 0)
        {
            midi_key_map[i].first = midi_note;
            return midi_key_map[i].second;
        }
    }
    static uint8_t no_entry;
    no_entry = 0xFF; // no free place: not in the map (was the last place, an instrument already on it)
    return no_entry;
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

// the cymbals, crash keys 49 55 57 and ride keys 51 53 59: the empty keys of a group get the group's own cymbal (the
// first of its keys that has one), and only a kit without one the other group's; a kit with one cymbal plays it on
// all six (CR-8000, KR-55, TR-606: their cymbal on 49). Pairs in a row gave a ride on 51 to the crash keys 49/57
// before a crash on 55 was looked at
static const uint8_t crash_keys[] = {49, 57, 55}; // the first with an instrument is the group's cymbal
static const uint8_t ride_keys[] = {51, 59, 53};

// keys of 35 ... 59: their places in the map directly (midi_key_entry() inlined with its search made the app 2-4 KB
// larger)
static uint8_t first_instrument(const uint8_t *keys)
{
    for (size_t i = 0; i < 3; i++)
        if (midi_key_map[keys[i] - 35].second != 0xFF)
            return midi_key_map[keys[i] - 35].second;
    return 0xFF;
}

static void fill_empty_keys(const uint8_t *keys, uint8_t inst)
{
    for (size_t i = 0; i < 3; i++)
        if (midi_key_map[keys[i] - 35].second == 0xFF)
            midi_key_map[keys[i] - 35].second = inst;
}

static void map_cymbals()
{
    const uint8_t crash = first_instrument(crash_keys);
    const uint8_t ride = first_instrument(ride_keys);
    fill_empty_keys(crash_keys, crash != 0xFF ? crash : ride);
    fill_empty_keys(ride_keys, ride != 0xFF ? ride : crash);
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

// the API has no blob size: an upper bound for the bounds-checked loader (the demo kits have 3-10 KB)
constexpr size_t MAX_DRUMKIT_SIZE = 64 * 1024;

// the samples a drumkit refers to by name (OSC_SAMPLE, e.g. the TR-909 hats ROM 909_HIGH): files of that name; a kit
// whose sample is not there plays its SAMPLE_FALLBACK parts instead
static const uint8_t *resolve_sample(const char *name, size_t *len)
{
    return machine::fs_read(name, len);
}

void engine::setup()
{
    const uint8_t *drumkit = __data;
    // unpack
    inst_count = drum_synth_load_models_ex(drumkit, MAX_DRUMKIT_SIZE, _instModel, ::malloc, resolve_sample);
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
        map_cymbals(); // CR 49 55 57, RD 51 53 59

        const uint8_t oh = midi_key_entry(46); // 0xFF: no instrument on key 46
        _open_hihat = oh < inst_count ? oh : -1;

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
        drum_synth_deinit(_inst[i], ::free);
    }
    drum_synth_free_models(_instModel, inst_count, ::free);
}

void engine::process()
{
    auto buffer = engine::outputBuffer<0>();
    auto bufferAux = engine::outputBuffer<1>();
    memset(buffer, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    memset(bufferAux, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    // the same for every instrument. A change recomputes the filter coefficients of all sounding parts and puts
    // square/saw/triangle on their sample-by-sample path (DSM: ~1.5k cycles a frame): the firmware holds an open CV
    // input still (CV_HYSTERESIS, machine.cpp)
    const float f = pitch * powf(2.f, engine::cv());

    for (int i = 0; i < inst_count; i++)
    {
        if (engine::trig() & (1 << i))
        {
            _t[i] = 0;
            drum_synth_reset(_inst[i]);
            if (_open_hihat >= 0 && i != _open_hihat &&
                (_instModel[i].midi_note == 42 || _instModel[i].midi_note == 44)) // closed hihat
            {
                _t[_open_hihat] = MAX_T; // mute open hihat
            }
        }

        if (_t[i] < MAX_T)
        {
            DrumParams params = {_t[i], 0, stretch, stereo, 0, 0};
            params.velocity = std::min(1.f, sqrtf(engine::trigLevel(i))); // trig level / 128: MIDI velocity / 128, 1 without MIDI
            
            if(i == _open_hihat) 
            {
                params.decay = 0.1f + stretch + (-stretch * ((float)engine::getMidiCC(4) / 127.f));
            }

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

#include "../../../lib/drumsynth/drumsynth.cpp"
#include "../../../lib/misc/Biquad.cpp"
#include "../../../lib/plaits/resources.cc"
