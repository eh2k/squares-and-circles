#include "../squares-and-circles-api.h"

static float _ch_vol = 1.f;
static float _ch_dec = 0.2f;
static float _oh_vol = 1.f;
static float _oh_dec = 0.5f;
static float _rd_vol = 1.f;
static float _rd_dec = 0.5f;

int32_t _select;
const char *sample_names[] = {"CH", "OH", "RD"};
// void *_samples[3] = {};
void *_ch = nullptr;
void *_oh = nullptr;
void *_rd = nullptr;

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
    const uint8_t *HiHats_bin = machine::fs_read("909_HIGH");
    const uint8_t *Ride_bin = machine::fs_read("909_RIDE");

    if (HiHats_bin == nullptr || Ride_bin == nullptr)
        return;

    const int ch_start = 24576;
    _ch = dsp_sample_u8(HiHats_bin + ch_start, 32768 - ch_start, 32000, 0);
    _oh = dsp_sample_u8(HiHats_bin, 24576, 32000, 0);
    _rd = dsp_sample_u8(Ride_bin, 32768, 32000, 0);

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
        dsp_set_sample_pos(_oh, 0, _oh_vol, _oh_dec);
    }
    else if (engine::trig() & (1 << 0)) // CH
    {
        dsp_set_sample_pos(_ch, 0, _ch_vol, _ch_dec);
        dsp_set_sample_pos(_oh, 0, 0, _oh_dec);
    }

    if (engine::trig() & (1 << 2)) // RD
    {
        dsp_set_sample_pos(_rd, 0, _rd_vol, _rd_dec);
    }

    memset(_outputL, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    memset(_outputR, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    void *_samples[] = {_ch, _oh, _rd};
    for (size_t i = 0; i < LEN_OF(_samples) && _samples[i] != nullptr; i++)
    {
        float tmp[FRAME_BUFFER_SIZE] = {};
        dsp_process_sample(_samples[i], 0, 1, engine::cv(), tmp);
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
        gfx::drawString(20, 20, "ROMS NOT FOUND\n \n   909_HIGH");
}
