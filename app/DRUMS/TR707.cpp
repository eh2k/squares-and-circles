#include "../squares-and-circles-api.h"
#include <stdio.h>

// #include "eproms/tr707/IC34_TR707_SNDROM.h"
// #include "eproms/tr707/IC35_TR707_SNDROM.h"
static auto IC34_TR707_SNDROM_bin = machine::fs_read("707_IC34");
static auto IC35_TR707_SNDROM_bin = machine::fs_read("707_IC35");

static float _pitch = 0.5f;
static float _start = 0.f;
static float _end = 1.f;
static float _amp = 1.f;

int32_t _select = 0;
const char *sample_names[12];
void *sample_ptr[12];
#define SETUP_SAMPLE(name, ptr)   \
    sample_names[_select] = name; \
    sample_ptr[_select] = ptr;    \
    ++_select

void engine::setup()
{
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

    SETUP_SAMPLE("BD0", dsp_sample_u8(BD0, 0x1000, 25000, 1));
    SETUP_SAMPLE("BD1", dsp_sample_u8(BD1, 0x1000, 25000, 1));
    SETUP_SAMPLE("SD0", dsp_sample_u8(SD0, 0x1000, 25000, 1));
    SETUP_SAMPLE("SD1", dsp_sample_u8(SD1, 0x1000, 25000, 1));

    SETUP_SAMPLE("CP", dsp_sample_u8(CP, 0x1000, 25000, 1));
    SETUP_SAMPLE("TMB", dsp_sample_u8(TMB, 0x1000, 25000, 1));
    SETUP_SAMPLE("RM", dsp_sample_u8(RM, 0x1000, 25000, 1));
    SETUP_SAMPLE("CB", dsp_sample_u8(CB, 0x1000, 25000, 1));

    SETUP_SAMPLE("LT", dsp_sample_u8(LT, 0x1000, 25000, 0));
    SETUP_SAMPLE("MT", dsp_sample_u8(MT, 0x1000, 25000, 0));
    SETUP_SAMPLE("HT", dsp_sample_u8(HT, 0x1000, 25000, 0));
    SETUP_SAMPLE("HH", dsp_sample_u8(HH, 0x1000, 25000, 0));

    engine::addParam("Pitch", &_pitch);
    engine::addParam(">Sample", &_select, 0, LEN_OF(sample_names) - 1, sample_names); // . = hidden
    _select = 0;

    engine::addParam("Start", &_start);
    engine::addParam("End", &_end);
}

void engine::process()
{
    auto outputL = engine::outputBuffer<0>();
    memset(outputL, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    dsp_process_sample(sample_ptr[_select], _start, _end, _pitch, outputL);
}