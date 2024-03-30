#include "../squares-and-circles-api.h"

// #include "eproms/tr707/IC35_TR707_SNDROM.h"
const uint8_t *IC35_TR707_SNDROM_bin = machine::fs_read("707_IC35");

constexpr static int ch_start = 24576;

static float _ch_vol = 1.f;
static float _ch_dec = 0.2f;
static float _oh_dec = 0.5f;

int32_t _select = 0;
void *_ch;
void *_oh;

void engine::setup()
{
    const uint8_t *HH = &IC35_TR707_SNDROM_bin[0x2000];
    _ch = dsp_sample_u8(HH, 0x2000, 25000, 0);
    _oh = dsp_sample_u8(HH, 0x2000, 25000, 0);

    engine::addParam("", &_select, 0, 0); // . = hidden
    engine::addParam("CH-Lev", &_ch_vol);
    engine::addParam("CH-Dec", &_ch_dec);
    engine::addParam("OH-Dec", &_oh_dec);
}

void engine::process()
{
    auto outputL = engine::outputBuffer<0>();
    memset(outputL, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    dsp_process_hihats(_ch, _oh, _ch_vol, _ch_dec, _oh_dec, outputL);
}