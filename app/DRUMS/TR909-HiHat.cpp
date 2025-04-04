#include "../squares-and-circles-api.h"

static float _ch_vol = 1.f;
static float _ch_dec = 0.2f;
static float _oh_dec = 0.5f;

int32_t _select = 0;
void *_ch = nullptr;
void *_oh = nullptr;

void engine::setup()
{
    const uint8_t *HiHats_bin = machine::fs_read("909_HIGH");

    if (HiHats_bin == nullptr)
        return;

    _oh = dsp_sample_u8(HiHats_bin, 24576, 32000, 0);
    const int ch_start = 24576;
    _ch = dsp_sample_u8(HiHats_bin + ch_start, 32768 - ch_start, 32000, 0);

    engine::addParam("", &_select, 0, 0); // . = hidden
    engine::addParam("CH-Lev", &_ch_vol);
    engine::addParam("CH-Dec", &_ch_dec);
    engine::addParam("OH-Dec", &_oh_dec);
}

void engine::process()
{
    if (_oh == nullptr)
        return;

    auto outputL = engine::outputBuffer<0>();
    memset(outputL, 0, sizeof(float) * FRAME_BUFFER_SIZE);

    if (engine::accent()) // OH
    {
        dsp_set_sample_pos(_oh, 0, 1.f, _oh_dec);
    }
    else if (engine::trig()) // CH
    {
        dsp_set_sample_pos(_ch, 0, _ch_vol, _ch_dec);
        dsp_set_sample_pos(_oh, 0, 0, _oh_dec);
    }

    dsp_process_sample(_ch, 0, 1, engine::cv(), outputL);
    dsp_process_sample(_oh, 0, 1, engine::cv(), outputL);
}

void engine::draw()
{
    if (_oh == nullptr)
        gfx::drawString(20, 20, "ROMS NOT FOUND\n \n   909_HIGH");
}
