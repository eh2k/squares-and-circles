#include "../squares-and-circles-api.h"
#include "../lib/stmlib/algorithms/voice_allocator.h"
#include <map>

static uint8_t voice[4] = {};
static stmlib::VoiceAllocator<LEN_OF(voice)> allocator = {};
static int16_t pitch = 0;
static uint8_t cc[128] = {};

void engine::setup()
{
    allocator.Init();
    allocator.set_size(LEN_OF(voice));
    engine::setMode(ENGINE_MODE_MIDI_IN);
    for (auto &c : cc)
        c = 0xFF;
}

void engine::process()
{
}

void engine::draw()
{
    gfx::drawString(2, 12, "VOICE KEY", 0);
    gfx::drawString(66, 12, "CONTROL", 0);

    for (int x = 0; x < 128; x += 3)
        gfx::setPixel(x, 18);

    char tmp[30];
    for (size_t i = 0; i < LEN_OF(voice); i++)
    {
        sprintf(tmp, "#%d   %4d", i, voice[i]);
        gfx::drawString(2, 20 + i * 6, tmp, 0);
    }

    for (int y = 12; y < 60; y += 3)
        gfx::setPixel(64, y);

    int bpm = machine::clk_bpm();
    int bpm2 = ((bpm % 100) / 10);
    sprintf(tmp, "clock: %3d.%d", bpm / 100, bpm2);
    gfx::drawString(66, 20, tmp, 0);

    sprintf(tmp, "pitch: %4d", pitch);
    gfx::drawString(66, 26, tmp, 0);

    int y = 32;
    for (int i = 0; i < 128; i++)
    {
        if (cc[i] < 0xFF)
        {
            sprintf(tmp, "cc-%d: %4d", i, cc[i]);
            gfx::drawString(66, y, tmp, 0);
            y += 6;
        }
    }
}

void engine::onMidiNote(uint8_t key, uint8_t velocity) // NoteOff: velocity == 0
{
    if (velocity > 0)
    {
        auto ni = allocator.NoteOn(key);
        voice[ni] = key;
    }
    else
    {
        auto ni = allocator.NoteOff(key);
        voice[ni] = 0;
    }
}

void engine::onMidiPitchbend(int16_t value)
{
    pitch = value;
}

void engine::onMidiCC(uint8_t ccc, uint8_t value)
{
    cc[ccc % 128] = value;
}

void engine::onMidiSysex(uint8_t byte)
{
}
