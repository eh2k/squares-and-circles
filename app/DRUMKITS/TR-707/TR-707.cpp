#include "../../squares-and-circles-api.h"
#include <stdio.h>

ENGINE_ALIAS("DRUMS/TR707"); // the patches saved before the app became DRUMKITS/TR-707

// #include "eproms/tr707/IC34_TR707_SNDROM.h"
// #include "eproms/tr707/IC35_TR707_SNDROM.h"

// placeholders for the optional cymbals, replaced in setup()
constexpr uint8_t RD_KEY = 0xF0;
constexpr uint8_t CR_KEY = 0xF1;

static std::pair<uint8_t, uint8_t> midi_key_map[] = {
    {35, 0},   // BD0
    {36, 1},   // BD1
    {38, 2},   // SD0
    {40, 3},   // SD1
    {39, 4},   // CP
    {54, 5},   // TMB
    {37, 6},   // RM
    {56, 7},   // CB
    {41, 8},   // LT
    {43, 8},   // LT
    {45, 9},   // MT
    {47, 9},   // MT
    {48, 10},  // HT
    {50, 10},  // HT
    {42, 11},  // HH - Closed
    {44, 11},  // HH - Pedal
    {46, 12},  // HH - Open
    {51, RD_KEY}, // RD - Ride
    {59, RD_KEY}, // RD - Ride Edge
    {53, RD_KEY}, // RD - Ride Bell
    {49, CR_KEY}, // CR - Crash
    {55, CR_KEY}, // CR - Crash Edge
    {57, CR_KEY}, // CR - Crash Bow
    {0xFF, 0},    // END
};

static float _pitch = 0.5f;
static float _start = 0.f;
static float _end = 1.f;
static float _amp = 1.f;

int32_t _select = 0;
const char *sample_names[15] = {};
sample *sample_ptr[LEN_OF(sample_names)] = {};
#define SETUP_SAMPLE(name, ptr)                                                                                        \
    sample_names[_select] = name;                                                                                      \
    sample_ptr[_select] = ptr;                                                                                         \
    ++_select

void engine::setup()
{
    size_t size34 = 0, size35 = 0, size19 = 0, size22 = 0;
    auto IC34_TR707_SNDROM_bin = machine::fs_read("707_IC34", &size34);
    auto IC35_TR707_SNDROM_bin = machine::fs_read("707_IC35", &size35);
    auto IC19_Crash = machine::fs_read("707_IC19", &size19);
    auto IC22_Ride = machine::fs_read("707_IC22", &size22);

    if (IC34_TR707_SNDROM_bin == nullptr || IC35_TR707_SNDROM_bin == nullptr || size34 < 0x8000 ||
        size35 < 0x8000)
        return;

    auto BD0 = &IC34_TR707_SNDROM_bin[0x0000];
    auto BD1 = &IC34_TR707_SNDROM_bin[0x0001];
    auto SD0 = &IC34_TR707_SNDROM_bin[0x2000];
    auto SD1 = &IC34_TR707_SNDROM_bin[0x2001];
    auto RM = &IC35_TR707_SNDROM_bin[0x4000];
    auto CB = &IC35_TR707_SNDROM_bin[0x4001];
    auto CP = &IC35_TR707_SNDROM_bin[0x6000];
    auto TMB = &IC35_TR707_SNDROM_bin[0x6001];
    auto LT = &IC34_TR707_SNDROM_bin[0x4000];
    auto MT = &IC34_TR707_SNDROM_bin[0x6000];
    auto HT = &IC35_TR707_SNDROM_bin[0x0000];
    auto HH = &IC35_TR707_SNDROM_bin[0x2000];

    SETUP_SAMPLE("BD0", sample_u8(BD0, 0x1000, 25000, 1)); // 0
    SETUP_SAMPLE("BD1", sample_u8(BD1, 0x1000, 25000, 1)); // 1
    SETUP_SAMPLE("SD0", sample_u8(SD0, 0x1000, 25000, 1)); // 2
    SETUP_SAMPLE("SD1", sample_u8(SD1, 0x1000, 25000, 1)); // 3

    SETUP_SAMPLE("CP", sample_u8(CP, 0x1000, 25000, 1));   // 4
    SETUP_SAMPLE("TMB", sample_u8(TMB, 0x1000, 25000, 1)); // 5
    SETUP_SAMPLE("RM", sample_u8(RM, 0x1000, 25000, 1));   // 6
    SETUP_SAMPLE("CB", sample_u8(CB, 0x1000, 25000, 1));   // 7

    SETUP_SAMPLE("LT", sample_u8(LT, 0x1000, 12500, 0)); // 8
    SETUP_SAMPLE("MT", sample_u8(MT, 0x1000, 12500, 0)); // 9
    SETUP_SAMPLE("HT", sample_u8(HT, 0x1000, 12500, 0)); // 10
    SETUP_SAMPLE("CH", sample_u8(HH, 0x1000, 25000, 0)); // 11
    SETUP_SAMPLE("OH", sample_u8(HH, 0x1000, 25000, 0)); // 12

    // the cymbals are optional: their keys go to the index they get, or nowhere
    uint8_t rd = 0xFF, cr = 0xFF;
    if (IC22_Ride != nullptr && size22 >= 0x8000)
    {
        rd = _select;
        SETUP_SAMPLE("RD", sample_u8(IC22_Ride, 0x8000, 25000, 0)); // 13
    }
    if (IC19_Crash != nullptr && size19 >= 0x8000)
    {
        cr = _select;
        SETUP_SAMPLE("CR", sample_u8(IC19_Crash, 0x8000, 25000, 0)); // 14, or 13 without the ride
    }
    for (auto &k : midi_key_map)
    {
        if (k.first == 0xFF)
            break;
        if (k.second == RD_KEY)
            k.second = rd;
        else if (k.second == CR_KEY)
            k.second = cr;
    }

    // parked past the end: a firmware of before 2026-10-27 runs the samples of a voice in [start, end) even when it is
    // silent (amp 0), all of them at once after loading
    for (int i = 0; i < _select; i++)
        sample_set_pos(sample_ptr[i], 2.f, 0, 1.f);

    engine::addParam("Pitch", &_pitch);
    engine::addParam(MULTI_TRIGS, &_select, 0, _select - 1, sample_names); // . = hidden
    _select = 0;

    engine::addParam("Start", &_start);
    engine::addParam("End", &_end);

    engine::setMultiTrigMidiKeyMap(midi_key_map);
}

static bool silent(const float *frame) // all samples +-0
{
    uint32_t bits = 0;
#pragma GCC unroll 4 // not all 24: the app's flash
    for (size_t k = 0; k < FRAME_BUFFER_SIZE; k++)
    {
        uint32_t b;
        memcpy(&b, &frame[k], sizeof(b));
        bits |= b;
    }
    return (bits << 1) == 0; // without the sign
}

void engine::process()
{
    auto outputL = engine::outputBuffer<0>();
    memset(outputL, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    auto outputR = engine::outputBuffer<1>();
    memset(outputR, 0, sizeof(float) * FRAME_BUFFER_SIZE);

    if (sample_ptr[0] == nullptr)
        return;

    for (uint32_t i = 0; i < LEN_OF(sample_ptr) && sample_ptr[i] != nullptr; i++)
    {
        if (engine::trig() & (1 << i))
        {
            if (i == 11) // CH
            {
                if (!(engine::trig() & (1 << (i + 1)))) // OH
                {
                    sample_set_pos(sample_ptr[i], _start, 0.7f * engine::trigLevel(i), 0.2f);
                    sample_set_pos(sample_ptr[i + 1], 2.f, 0, 1.0f); // stopped, parked
                }
            }
            else if (i == 12) // OH
            {
                sample_set_pos(sample_ptr[i], _start, engine::trigLevel(i),
                                    1.f - (float)engine::getMidiCC(4) / 127.f);
            }
            else
            {
                sample_set_pos(sample_ptr[i], _start, engine::trigLevel(i), 1.f);
            }
        }
        float tmp[FRAME_BUFFER_SIZE] = {};

        float v_oct = (-2.f + _pitch * 4) + engine::cv();

        sample_process(sample_ptr[i], _start, _end, 1.f, v_oct, tmp);
        if (silent(tmp)) // stopped or played out: nothing to mix, no pan (cosf, sinf) to compute
            continue;
        float levelL = engine::mixLevelL(i);
        float levelR = engine::mixLevelR(i);
        for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            outputL[i] += tmp[i] * levelL;
            outputR[i] += tmp[i] * levelR;
        }
    }
}

void engine::draw()
{
    if (sample_ptr[0] == nullptr)
        gfx::drawString(20, 20, "ROMS NOT FOUND\n \n   707_IC34\n   707_IC35");
    else
        gfx::drawWaveform(sample_ptr[_select]);
}
