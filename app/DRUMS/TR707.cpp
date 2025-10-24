#include "../squares-and-circles-api.h"
#include <stdio.h>

// #include "eproms/tr707/IC34_TR707_SNDROM.h"
// #include "eproms/tr707/IC35_TR707_SNDROM.h"

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

static float _pitch = 0.5f;
static float _start = 0.f;
static float _end = 1.f;
static float _amp = 1.f;

int32_t _select = 0;
const char *sample_names[13];
void *sample_ptr[13];
#define SETUP_SAMPLE(name, ptr)                                                                                        \
    sample_names[_select] = name;                                                                                      \
    sample_ptr[_select] = ptr;                                                                                         \
    ++_select

void engine::setup()
{
    auto IC34_TR707_SNDROM_bin = machine::fs_read("707_IC34");
    auto IC35_TR707_SNDROM_bin = machine::fs_read("707_IC35");

    if (IC34_TR707_SNDROM_bin == nullptr || IC35_TR707_SNDROM_bin == nullptr)
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

    SETUP_SAMPLE("BD0", dsp_sample_u8(BD0, 0x1000, 25000, 1)); // 0
    SETUP_SAMPLE("BD1", dsp_sample_u8(BD1, 0x1000, 25000, 1)); // 1
    SETUP_SAMPLE("SD0", dsp_sample_u8(SD0, 0x1000, 25000, 1)); // 2
    SETUP_SAMPLE("SD1", dsp_sample_u8(SD1, 0x1000, 25000, 1)); // 3

    SETUP_SAMPLE("CP", dsp_sample_u8(CP, 0x1000, 25000, 1));   // 4
    SETUP_SAMPLE("TMB", dsp_sample_u8(TMB, 0x1000, 25000, 1)); // 5
    SETUP_SAMPLE("RM", dsp_sample_u8(RM, 0x1000, 25000, 1));   // 6
    SETUP_SAMPLE("CB", dsp_sample_u8(CB, 0x1000, 25000, 1));   // 7

    SETUP_SAMPLE("LT", dsp_sample_u8(LT, 0x1000, 25000, 0)); // 8
    SETUP_SAMPLE("MT", dsp_sample_u8(MT, 0x1000, 25000, 0)); // 9
    SETUP_SAMPLE("HT", dsp_sample_u8(HT, 0x1000, 25000, 0)); // 10
    SETUP_SAMPLE("CH", dsp_sample_u8(HH, 0x1000, 25000, 0)); // 11
    SETUP_SAMPLE("OH", dsp_sample_u8(HH, 0x1000, 25000, 0)); // 12

    engine::addParam("Pitch", &_pitch);
    engine::addParam(MULTI_TRIGS, &_select, 0, LEN_OF(sample_names) - 1, sample_names); // . = hidden
    _select = 0;

    engine::addParam("Start", &_start);
    engine::addParam("End", &_end);
    
    engine::setMultiTrigMidiKeyMap(midi_key_map);
}

void engine::process()
{
    if (sample_ptr[0] == nullptr)
        return;

    auto outputL = engine::outputBuffer<0>();
    memset(outputL, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    auto outputR = engine::outputBuffer<1>();
    memset(outputR, 0, sizeof(float) * FRAME_BUFFER_SIZE);

    for (uint32_t i = 0; i < LEN_OF(sample_ptr); i++)
    {
        if (engine::trig() & (1 << i))
        {
            if (i == 11) // CH
            {
                if (!(engine::trig() & (1 << (i + 1)))) // OH
                {
                    dsp_set_sample_pos(sample_ptr[i], _start, 0.7f * engine::trigLevel(i), 0.2f);
                    dsp_set_sample_pos(sample_ptr[i + 1], _start, 0, 1.0f);
                }
            }
            else
            {
                dsp_set_sample_pos(sample_ptr[i], _start, engine::trigLevel(i), 1.f);
            }
        }
        float tmp[FRAME_BUFFER_SIZE] = {};

        float f = (-2.f + _pitch * 4) * powf(2.f, engine::cv());

        dsp_process_sample(sample_ptr[i], _start, _end, f, tmp);
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
        gfx::drawSample(sample_ptr[_select]);
}
