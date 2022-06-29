#include "machine.h"
#include "stmlib/algorithms/voice_allocator.h"
#include <map>

using namespace machine;

struct MidiMonitor : public machine::MidiEngine
{
    uint8_t voice[4];
    stmlib::VoiceAllocator<LEN_OF(voice)> allocator;
    int16_t pitch = 0;
    std::map<uint8_t, uint8_t> cc;

    MidiMonitor()
    {
        allocator.Init();
        allocator.set_size(LEN_OF(voice));
    }

    void process(const machine::ControlFrame &frame, OutputFrame &of) override
    {
    }

    void onDisplay(uint8_t *buffer) override
    {
        gfx::drawEngine(buffer, this);

        gfx::drawString(buffer, 2, 12, "VOICE KEY", 0);
        gfx::drawString(buffer, 66, 12, "CONTROL", 0);

        for (int x = 0; x < 128; x += 3)
            gfx::drawPixel(buffer, x, 18);

        char tmp[30];
        for (size_t i = 0; i < LEN_OF(voice); i++)
        {
            sprintf(tmp, "#%d   %4d", i, voice[i]);
            gfx::drawString(buffer, 2, 20 + i * 6, tmp, 0);
        }

        for (int y = 12; y < 60; y += 3)
            gfx::drawPixel(buffer, 64, y);

        sprintf(tmp, "clock: %4.1f", machine::get_bpm());
        gfx::drawString(buffer, 66, 20, tmp, 0);

        sprintf(tmp, "pitch: %4d", pitch);
        gfx::drawString(buffer, 66, 26, tmp, 0);

        int i = 32;
        for(auto c : cc)
        {
            sprintf(tmp, "cc-%d: %4d", c.first, c.second);
            gfx::drawString(buffer, 66, i, tmp, 0);
            i += 6;
        }
    }

    void onMidiNote(uint8_t key, uint8_t velocity) override // NoteOff: velocity == 0
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

    void onMidiPitchbend(int16_t pitch) override
    {
        this->pitch = pitch;
    }

    void onMidiCC(uint8_t ccc, uint8_t value) override
    {
        cc[ccc] = value;
    }
};

void init_midi_monitor()
{
    machine::add<MidiMonitor>("MIDI", "Monitor");
}