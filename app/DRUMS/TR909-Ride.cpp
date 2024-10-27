#include "../squares-and-circles-api.h"

// #include "eproms/tr909/ride.h"
static const uint8_t *Ride_bin = machine::fs_read("909_RIDE");

constexpr static int ch_start = 24576;

static float _ch_vol = 1.f;
static float _ch_dec = 0.2f;
static float _oh_dec = 0.5f;

int32_t _select = 0;
void *_ch;
void *_oh;

void engine::setup()
{
    _ch = dsp_sample_u8(Ride_bin, 32768, 32000, 0);
    _oh = dsp_sample_u8(Ride_bin, 32768, 32000, 0);

    engine::addParam("", &_select, 0, 0); // . = hidden
    engine::addParam("CH-Lev", &_ch_vol);
    engine::addParam("CH-Dec", &_ch_dec);
    engine::addParam("OH-Dec", &_oh_dec);
}

void engine::process()
{
    auto outputL = engine::outputBuffer<0>();
    memset(outputL, 0, sizeof(float) * FRAME_BUFFER_SIZE);

    if (engine::accent()) //OH
    {
        dsp_set_sample_pos(_ch, 0, 0, _oh_dec);
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