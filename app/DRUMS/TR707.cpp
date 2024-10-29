#include "../squares-and-circles-api.h"
#include <stdio.h>

// #include "eproms/tr707/IC34_TR707_SNDROM.h"
// #include "eproms/tr707/IC35_TR707_SNDROM.h"

static float _pitch = 0.5f;
static float _start = 0.f;
static float _end = 1.f;
static float _amp = 1.f;

int32_t _midi_trigs = 0;
int32_t _select = 0;
const char *sample_names[13];
void *sample_ptr[13];
#define SETUP_SAMPLE(name, ptr)   \
    sample_names[_select] = name; \
    sample_ptr[_select] = ptr;    \
    ++_select

void engine::setup()
{
    auto IC34_TR707_SNDROM_bin = machine::fs_read("707_IC34");
    auto IC35_TR707_SNDROM_bin = machine::fs_read("707_IC35");

    if(IC34_TR707_SNDROM_bin == nullptr || IC35_TR707_SNDROM_bin == nullptr )
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
    SETUP_SAMPLE("CH", dsp_sample_u8(HH, 0x1000, 25000, 0));
    SETUP_SAMPLE("OH", dsp_sample_u8(HH, 0x1000, 25000, 0));

    engine::addParam("Pitch", &_pitch);
    engine::addParam(MULTI_TRIGS, &_select, 0, LEN_OF(sample_names) - 1, sample_names); // . = hidden
    _select = 0;

    engine::addParam("Start", &_start);
    engine::addParam("End", &_end);

    engine::setMode(ENGINE_MODE_MIDI_IN);
}

void engine::process()
{
    if(sample_ptr[0] == nullptr)
        return;

    auto outputL = engine::outputBuffer<0>();
    memset(outputL, 0, sizeof(float) * FRAME_BUFFER_SIZE);
    auto outputR = engine::outputBuffer<1>();
    memset(outputR, 0, sizeof(float) * FRAME_BUFFER_SIZE);

    for (uint32_t i = 0; i < LEN_OF(sample_ptr); i++)
    {
        if (engine::trig() & (1 << i) || _midi_trigs & (1 << i))
        {
            if (i == 11) // CH
            {
                if (!(engine::trig() & (1 << (i + 1)))) // OH
                {
                    dsp_set_sample_pos(sample_ptr[i], _start, 0.7f, 0.2f);
                    dsp_set_sample_pos(sample_ptr[i + 1], _start, 0, 1.0f);
                }
            }
            else
            {
                dsp_set_sample_pos(sample_ptr[i], _start, 1.f, 1.f);
            }

            _midi_trigs &= ~(1 << i);
        }
        float tmp[FRAME_BUFFER_SIZE] = {};
        dsp_process_sample(sample_ptr[i], _start, _end, -2.f + (_pitch * 4), tmp);
        float levelL = engine::mixLevelL(i);
        float levelR = engine::mixLevelR(i);
        for(size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            outputL[i] += tmp[i] * levelL;
            outputR[i] += tmp[i] * levelR;
        }
    }
}

void engine::draw()
{
    if(sample_ptr[0] == nullptr)
        gfx::drawString(20, 20, "ROMS NOT FOUND\n \n   707_IC34\n   707_IC35");
    else
        gfx::drawSample(sample_ptr[_select]);
}

void engine::onMidiNote(uint8_t key, uint8_t velocity) // NoteOff: velocity == 0
{
    if (velocity > 0)
    {
        switch (key)
        {
        case 35: // BD0
            _midi_trigs |= (1 << 0);
            break;
        case 36: // BD1
            _midi_trigs |= (1 << 1);
            break;
        case 38: // SD0
            _midi_trigs |= (1 << 2);
            break;
        case 40: // SD1
            _midi_trigs |= (1 << 3);
            break;
        case 39: // CP
            _midi_trigs |= (1 << 4);
            break;
        case 54: // TMB
            _midi_trigs |= (1 << 5);
            break;
        case 37: // RM
            _midi_trigs |= (1 << 6);
            break;
        case 56: // CB
            _midi_trigs |= (1 << 7);
            break;
        case 41: // LT
        case 43: // LT
            _midi_trigs |= (1 << 8);
            break;
        case 45: // MT
        case 47: // MT
            _midi_trigs |= (1 << 9);
            break;
        case 48: // HT
        case 50: // HT
            _midi_trigs |= (1 << 10);
            break;
        case 42: // CH
        case 44: // CH
        case 46: // OH
            _midi_trigs |= (1 << 11);
            break;
        }
    }
    else
    {
    }
}

void engine::onMidiPitchbend(int16_t pitch)
{
}

void engine::onMidiCC(uint8_t ccc, uint8_t value)
{
    // nothing implemented..
}

void engine::onMidiSysex(uint8_t byte)
{
}