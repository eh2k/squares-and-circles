#include "../../squares-and-circles-api.h"
#include <stdio.h>

ENGINE_ALIAS("DRUMS/RZ-1"); // the patches saved before the app moved to DRUMKITS

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

// the order of the samples in setup(), which is the order of the TRIGS selector
enum : uint8_t { BD_, SD_, CP_, RM_, CB_, CLK_, TM1_, TM2_, TM3_, CH_, OH_, RD_, CR_ };

static std::pair<uint8_t, uint8_t> midi_key_map[] = {
    {48, TM1_}, // TOM1 (high, ~150 Hz)
    {50, TM1_},
    {45, TM2_}, // TOM2 (mid, ~125 Hz)
    {47, TM2_},
    {41, TM3_}, // TOM3 (low, ~95 Hz)
    {43, TM3_},
    {35, BD_},  // BD
    {36, BD_},
    {38, SD_},  // SD
    {40, SD_},
    {37, RM_},  // RIM
    {42, CH_},  // CH - Closed
    {44, CH_},  // CH - Pedal
    {46, OH_},  // OH - Open
    {33, CLK_}, // CLK - Metronome Click
    {39, CP_},  // CP - Clap
    {51, RD_},  // RD - Ride
    {53, RD_},  // RD - Ride Bell
    {59, RD_},  // RD - Ride Edge
    {56, CB_},  // CB - Cowbell
    {49, CR_},  // CR - Crash
    {55, CR_},
    {57, CR_},
    {0xFF, 0},  // END
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
    size_t size_a = 0, size_b = 0;
    auto ROM_A_bin = machine::fs_read("RZ1_ROMA", &size_a);
    auto ROM_B_bin = machine::fs_read("RZ1_ROMB", &size_b);

    if (ROM_A_bin == nullptr || ROM_B_bin == nullptr || size_a < 31615 || size_b < 32768)
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

    SETUP_SAMPLE("BD",  sample_s8(BD,   1627,  20000, 0)); // BD_
    SETUP_SAMPLE("SD",  sample_s8(SD,   3224,  20000, 0)); // SD_
    
    SETUP_SAMPLE("CP",  sample_s8(CP,   2511,  20000, 0)); // CP_
    SETUP_SAMPLE("RM",  sample_s8(RM,   1303,  20000, 0)); // RM_
    
    SETUP_SAMPLE("CB",  sample_s8(CB,   1951,  20000, 0)); // CB_
    SETUP_SAMPLE("CLK", sample_s8(CLK,  507,   20000, 0)); // CLK_

    SETUP_SAMPLE("TM1",  sample_s8(TOM1, 3791,  20000, 0)); // TM1_
    SETUP_SAMPLE("TM2",  sample_s8(TOM2, 3844,  20000, 0)); // TM2_
    SETUP_SAMPLE("TM3",  sample_s8(TOM3, 4087,  20000, 0)); // TM3_

    SETUP_SAMPLE("CH",  sample_s8(CH,   1223,  20000, 0)); // CH_
    SETUP_SAMPLE("OH",  sample_s8(OH,   12009, 20000, 0)); // OH_

    SETUP_SAMPLE("RD",  sample_s8(RD,   13935, 20000, 0)); // RD_
    SETUP_SAMPLE("CR",  sample_s8(CR,   14371, 20000, 0)); // CR_

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
            if (i == CH_)
            {
                if (!(engine::trig() & (1 << (i + 1)))) // OH
                {
                    sample_set_pos(sample_ptr[i], _start, 0.7f * engine::trigLevel(i), 0.2f);
                    sample_set_pos(sample_ptr[i + 1], 2.f, 0, 1.0f); // stopped, parked
                }
            }
            else if (i == OH_)
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
        gfx::drawString(20, 20, "ROMS NOT FOUND\n \n   RZ1_ROMA\n   RZ1_ROMB");
    else
        gfx::drawWaveform(sample_ptr[_select]);
}
