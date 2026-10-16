#include "../../squares-and-circles-api.h"

static float _ch_vol = 1.f;
static float _ch_dec = 0.2f;
static float _oh_vol = 1.f;
static float _oh_dec = 0.5f;
static float _rd_vol = 1.f;
static float _rd_dec = 0.5f;

int32_t _select;
const char *sample_names[] = {"CH", "OH", "RD"};
// void *_samples[3] = {};
sample *_ch = nullptr;
sample *_oh = nullptr;
sample *_rd = nullptr;

static std::pair<uint8_t, uint8_t> midi_key_map[] = {
    {42, 0}, // HH - Closed
    {44, 0}, // HH - Pedal
    {46, 1}, // HH - Open
    {51, 2}, // RD - Ride
    {59, 2}, // RD - Ride Edge
    {53, 2}, // RD - Ride Bell
    // {49, 3},  // CR - Crash
    // {55, 3},  // CR - Crash Edge
    // {57, 3},  // CR - Crash Bow
    {0xFF, 0}, // END
};

float *_outputL = nullptr;
float *_outputR = nullptr;

void engine::setup()
{
    size_t size_hh = 0, size_rd = 0;
    const uint8_t *HiHats_bin = machine::fs_read("909_HIGH", &size_hh);
    const uint8_t *Ride_bin = machine::fs_read("909_RIDE", &size_rd);

    if (HiHats_bin == nullptr || Ride_bin == nullptr || size_hh < 32768 || size_rd < 32768)
        return;

    const int ch_start = 24576;
    _ch = sample_u8(HiHats_bin + ch_start, 32768 - ch_start, 32000, 0);
    _oh = sample_u8(HiHats_bin, 24576, 32000, 0);
    _rd = sample_u8(Ride_bin, 32768, 32000, 0);

    engine::addParam("CH-Lev", &_ch_vol);
    engine::addParam("CH-Dec", &_ch_dec);
    engine::addParam("OH-Lev", &_oh_vol);
    engine::addParam("OH-Dec", &_oh_dec);
    engine::addParam("RD-Lev", &_rd_vol);
    engine::addParam("RD-Dec", &_rd_dec);

    _outputL = engine::outputBuffer_f32(OUT_AUDIO_L);
    _outputR = engine::outputBuffer_f32(OUT_AUDIO_R);
    engine::setupMultiTrigs(3, sample_names, midi_key_map);
}

void engine::process()
{
    if (_oh == nullptr)
        return;

    if (engine::trig() & (1 << 1)) // OH
    {
        sample_set_pos(_oh, 0, _oh_vol, _oh_dec);
    }
    else if (engine::trig() & (1 << 0)) // CH
    {
        sample_set_pos(_ch, 0, _ch_vol, _ch_dec);
        sample_set_pos(_oh, 0, 0, _oh_dec);
    }

    if (engine::trig() & (1 << 2)) // RD
    {
        sample_set_pos(_rd, 0, _rd_vol, _rd_dec);
    }

    memset(_outputL, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    memset(_outputR, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    sample *_samples[] = {_ch, _oh, _rd};
    for (size_t i = 0; i < LEN_OF(_samples) && _samples[i] != nullptr; i++)
    {
        float tmp[FRAME_BUFFER_SIZE] = {};
        sample_process(_samples[i], 0, 1, 1.f, engine::cv(), tmp);
        float levelL = engine::mixLevelL(i);
        float levelR = engine::mixLevelR(i);
        for (size_t j = 0; j < FRAME_BUFFER_SIZE; j++)
        {
            _outputL[j] += tmp[j] * levelL;
            _outputR[j] += tmp[j] * levelR;
        }
    }
}

void engine::draw()
{
    if (_oh == nullptr)
        gfx::drawString(20, 20, "ROMS NOT FOUND\n \n   909_HIGH\n   909_RIDE");
}
