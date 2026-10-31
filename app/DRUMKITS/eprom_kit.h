// The engine of the drum machines played from the dumps of their sample EPROMs, one flash blob per ROM (DMX.cpp,
// Linn.cpp): a sample player per instrument for the AM6070 codes, a hit stops the other instruments of its voice (the
// machine plays one sound per voice at a time), V/OCT, LEVEL and PAN per instrument. An instrument whose ROM is
// missing stays silent; the screen lists the missing blobs.
//
// Defined before including it:
//   KIT_ROMS, KIT_INSTRUMENTS   (constexpr) the number of ROM blobs and of instruments
//   OPEN_HAT                    (constexpr) the instrument whose decay follows MIDI CC 4
// and after it, with these names:
//   rom_blob[KIT_ROMS]          the blob names: at most 15 characters, the firmware refuses a longer one
//   instruments[KIT_INSTRUMENTS], names[KIT_INSTRUMENTS], midi_key_map[] ({0xFF, 0} at the end)
//
// app/build.sh rebuilds an app when its .cpp changed: touch DMX.cpp and Linn.cpp after a change here.

#pragma once

#include "../squares-and-circles-api.h"

enum : uint8_t
{
    WHOLE,
    FIRST_HALF,
    SECOND_HALF,
};

struct instrument
{
    uint8_t rom;
    uint8_t part;    // WHOLE, or a half of the ROM
    uint16_t rate;   // Hz
    uint8_t amp_mul; // AM6070 gain, 4 reaches full scale
    float gain;
    float tau_ms; // decay time constant, 0: none
    uint8_t voice;
};

extern const char *const rom_blob[KIT_ROMS];
extern const instrument instruments[KIT_INSTRUMENTS];
extern const char *names[KIT_INSTRUMENTS];
extern std::pair<uint8_t, uint8_t> midi_key_map[];

static float _pitch = 0.5f;
static float _start = 0.f;
static float _end = 1.f;

int32_t _select = 0;
sample *sample_ptr[KIT_INSTRUMENTS] = {};
float _decay[KIT_INSTRUMENTS] = {};
bool rom_missing[KIT_ROMS] = {};

void engine::setup()
{
    const uint8_t *rom[KIT_ROMS] = {};
    size_t rom_size[KIT_ROMS] = {};
    for (size_t r = 0; r < KIT_ROMS; r++)
    {
        rom[r] = machine::fs_read(rom_blob[r], &rom_size[r]);
        rom_missing[r] = rom[r] == nullptr || rom_size[r] < 2;
    }

    for (size_t i = 0; i < KIT_INSTRUMENTS; i++)
    {
        const auto &inst = instruments[i];
        if (rom_missing[inst.rom])
            continue;

        // a slot takes any chip: the whole blob, or its halves
        size_t len = rom_size[inst.rom];
        const uint8_t *data = rom[inst.rom];
        if (inst.part != WHOLE)
        {
            len /= 2;
            if (inst.part == SECOND_HALF)
                data += len;
        }

        sample_ptr[i] = sample_Am6070(data, len, inst.rate, inst.amp_mul);
        // parked past the end: a firmware of before 2026-10-27 runs the samples of a voice in [start, end) even when
        // it is silent (amp 0), all of them at once after loading
        sample_set_pos(sample_ptr[i], 2.f, 0, 1.f);

        // the sample player's decay: a time constant of 12 ms + decay * half the sample's length
        _decay[i] = 1.f;
        if (inst.tau_ms > 0)
        {
            const float half_ms = 500.f * len / inst.rate;
            _decay[i] = std::max(0.f, std::min(0.99f, (inst.tau_ms - 12.f) / half_ms));
        }
    }

    engine::addParam("Pitch", &_pitch);
    engine::addParam(MULTI_TRIGS, &_select, 0, KIT_INSTRUMENTS - 1, names);
    engine::addParam("Start", &_start);
    engine::addParam("End", &_end);

    engine::setMultiTrigMidiKeyMap(midi_key_map);
}

static float decay(size_t i)
{
    if (i == OPEN_HAT)
        return 1.f - (float)engine::getMidiCC(4) / 127.f;
    return _decay[i];
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

    const uint32_t trig = engine::trig();
    for (size_t i = 0; i < KIT_INSTRUMENTS; i++)
    {
        if (!(trig & (1 << i)) || sample_ptr[i] == nullptr)
            continue;

        // a hit stops the others of its voice (of two hits on one voice in the same frame the later one plays: OH
        // over CH), parked as in setup(). Stopped with their own decay, as the player recomputes it only when it changes
        for (size_t j = 0; j < KIT_INSTRUMENTS; j++)
            if (j != i && instruments[j].voice == instruments[i].voice && sample_ptr[j] != nullptr)
                sample_set_pos(sample_ptr[j], 2.f, 0, decay(j));

        sample_set_pos(sample_ptr[i], _start, instruments[i].gain * engine::trigLevel(i), decay(i));
    }

    const float v_oct = (-2.f + _pitch * 4) + engine::cv();

    for (size_t i = 0; i < KIT_INSTRUMENTS; i++)
    {
        if (sample_ptr[i] == nullptr)
            continue;

        float tmp[FRAME_BUFFER_SIZE] = {};
        sample_process(sample_ptr[i], _start, _end, 1.f, v_oct, tmp);
        if (silent(tmp)) // stopped or played out: nothing to mix, no pan (cosf, sinf) to compute
            continue;
        const float levelL = engine::mixLevelL(i);
        const float levelR = engine::mixLevelR(i);
        for (size_t k = 0; k < FRAME_BUFFER_SIZE; k++)
        {
            outputL[k] += tmp[k] * levelL;
            outputR[k] += tmp[k] * levelR;
        }
    }
}

void engine::draw()
{
    if (sample_ptr[_select] != nullptr)
    {
        gfx::drawWaveform(sample_ptr[_select]);
        return;
    }

    // the selected instrument has no ROM: in the waveform's place the missing blobs (room for 3, "..." for more)
    gfx::drawString(2, 47, "ROMS NOT FOUND:", 0);
    size_t missing = 0;
    for (bool m : rom_missing)
        missing += m;
    if (missing == KIT_ROMS)
    {
        gfx::drawStringEx(2, 53, 0, "%s ... %s", rom_blob[0], rom_blob[KIT_ROMS - 1]);
        return;
    }
    int n = 0;
    for (size_t r = 0; r < KIT_ROMS && n < 3; r++)
        if (rom_missing[r])
        {
            gfx::drawString(2 + 42 * n, 53, n == 2 && missing > 3 ? "..." : rom_blob[r], 0);
            ++n;
        }
}
