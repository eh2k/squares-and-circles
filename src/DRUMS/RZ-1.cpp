#include "../squares-and-circles-api.h"
#include <stdio.h>

// Casio RZ-1 sample ROMs
// HN613256P CM5/CM6 = 256kbit (32KB each) Mask ROMs
// Format: signed 8-bit, mono, 20,000 Hz
//
// ROM A (CM5): Toms 1~3, Kick, Snare, Rimshot, Closed Hi-Hat, Open Hi-Hat, Metronome Click
// ROM B (CM6): Clap, Ride, Cowbell, Crash
//
// Sample offsets (from R-Massive's RZ-1 ROM Hacking article):
// ROM A:
//   Tom 1   :     0 - 3791  (3791)
//   Tom 2   :  3791 - 7635  (3844)
//   Tom 3   :  7635 - 11722 (4087)
//   Kick    : 11722 - 13349 (1627)
//   Snare   : 13349 - 16573 (3224)
//   Rimshot : 16573 - 17876 (1303)
//   CH      : 17876 - 19099 (1223)
//   OH      : 19099 - 31108 (12009)
//   Click   : 31108 - 31615 ( 507)
// ROM B:
//   Clap    :     0 - 2511  (2511)
//   Ride    :  2511 - 16446 (13935)
//   Cowbell : 16446 - 18397 (1951)
//   Crash   : 18397 - 32768 (14371)

static std::pair<uint8_t, uint8_t> midi_key_map[] = {
    {41, 0},   // TOM1 (low)
    {43, 0},
    {45, 1},   // TOM2 (mid)
    {47, 1},
    {48, 2},   // TOM3 (high)
    {50, 2},
    {35, 3},   // BD
    {36, 3},
    {38, 4},   // SD
    {40, 4},
    {37, 5},   // RIM
    {42, 6},   // CH - Closed
    {44, 6},   // CH - Pedal
    {46, 7},   // OH - Open
    {33, 8},   // CLK - Metronome Click
    {39, 9},   // CP - Clap
    {51, 10},  // RD - Ride
    {53, 10},  // RD - Ride Bell
    {59, 10},  // RD - Ride Edge
    {56, 11},  // CB - Cowbell
    {49, 12},  // CR - Crash
    {55, 12},
    {57, 12},
    {0xFF, 0}, // END
};

static float _pitch = 0.5f;
static float _start = 0.f;
static float _end = 1.f;
static float _amp = 1.f;

int32_t _select = 0;
const char *sample_names[13] = {};
sample *sample_ptr[LEN_OF(sample_names)] = {};
#define SETUP_SAMPLE(name, ptr)                                                                                        \
    sample_names[_select] = name;                                                                                      \
    sample_ptr[_select] = ptr;                                                                                         \
    ++_select

void engine::setup()
{
    auto ROM_A_bin = machine::fs_read("RZ1_ROMA");
    auto ROM_B_bin = machine::fs_read("RZ1_ROMB");

    if (ROM_A_bin == nullptr || ROM_B_bin == nullptr)
        return;

    auto TOM1 = &ROM_A_bin[0];
    auto TOM2 = &ROM_A_bin[3791];
    auto TOM3 = &ROM_A_bin[7635];
    auto BD   = &ROM_A_bin[11722];
    auto SD   = &ROM_A_bin[13349];
    auto RM   = &ROM_A_bin[16573];
    auto CH   = &ROM_A_bin[17876];
    auto OH   = &ROM_A_bin[19099];
    auto CLK  = &ROM_A_bin[31108];

    auto CP   = &ROM_B_bin[0];
    auto RD   = &ROM_B_bin[2511];
    auto CB   = &ROM_B_bin[16446];
    auto CR   = &ROM_B_bin[18397];

    SETUP_SAMPLE("BD",  sample_s8(BD,   1627,  20000, 0)); // 3
    SETUP_SAMPLE("SD",  sample_s8(SD,   3224,  20000, 0)); // 4
    
    SETUP_SAMPLE("CP",  sample_s8(CP,   2511,  20000, 0)); // 9
    SETUP_SAMPLE("RM",  sample_s8(RM,   1303,  20000, 0)); // 5
    
    SETUP_SAMPLE("CB",  sample_s8(CB,   1951,  20000, 0)); // 11
    SETUP_SAMPLE("CLK", sample_s8(CLK,  507,   20000, 0)); // 8

    SETUP_SAMPLE("TM1",  sample_s8(TOM1, 3791,  20000, 0)); // 0
    SETUP_SAMPLE("TM2",  sample_s8(TOM2, 3844,  20000, 0)); // 1
    SETUP_SAMPLE("TM3",  sample_s8(TOM3, 4087,  20000, 0)); // 2

    SETUP_SAMPLE("CH",  sample_s8(CH,   1223,  20000, 0)); // 6
    SETUP_SAMPLE("OH",  sample_s8(OH,   12009, 20000, 0)); // 7

    SETUP_SAMPLE("RD",  sample_s8(RD,   13935, 20000, 0)); // 10
    SETUP_SAMPLE("CR",  sample_s8(CR,   14371, 20000, 0)); // 12

    engine::addParam("Pitch", &_pitch);
    engine::addParam(MULTI_TRIGS, &_select, 0, _select - 1, sample_names); // . = hidden
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

    for (uint32_t i = 0; i < LEN_OF(sample_ptr) && sample_ptr[i] != nullptr; i++)
    {
        if (engine::trig() & (1 << i))
        {
            if (i == 6) // CH
            {
                if (!(engine::trig() & (1 << (i + 1)))) // OH
                {
                    sample_set_pos(sample_ptr[i], _start, 0.7f * engine::trigLevel(i), 0.2f);
                    sample_set_pos(sample_ptr[i + 1], _start, 0, 1.0f);
                }
            }
            else if (i == 7) // OH
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
        gfx::drawString(20, 20, "ROMS NOT FOUND\n \n   RZ1_ROMA\n   RZ1_ROMB");
    else
        gfx::drawWaveform(sample_ptr[_select]);
}
