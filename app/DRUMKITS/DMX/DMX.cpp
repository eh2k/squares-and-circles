#include "../../squares-and-circles-api.h"

// Oberheim DMX, played from the factory EPROMs of its 8 voice cards (extern/eproms/dmx/info.txt), one flash blob per
// card. The rates, decays and the split of the ROMs follow the MAME driver (src/mame/oberheim/dmx.cpp, the component
// values of the voice cards, trimmer T1 in the middle). The engine is eprom_kit.h, the same as Linn.cpp's.

constexpr size_t KIT_ROMS = 8;
constexpr size_t KIT_INSTRUMENTS = 16;
constexpr size_t OPEN_HAT = 7; // OH

#include "../eprom_kit.h"

enum : uint8_t
{
    BASS,
    SNARE,
    HIHAT,
    TOM1,  // small toms
    TOM2,  // large toms
    PERC1, // rimshot | tambourine
    PERC2, // claps | shaker
    CYMB,  // crash | ride
};

// the blob of each card (the card is the voice: one of its sounds at a time)
const char *const rom_blob[KIT_ROMS] = {"DMXBASS", "DMXSNARE", "DMXHIHAT", "DMXTOM1",
                                        "DMXTOM2", "DMXPERC1", "DMXPERC2", "DMXCYMB"};

// 0.0033 uF on the card: 27179 Hz; the tom cards raise it with the trigger (36847, 31332 Hz); the snare has 0.0022 uF,
// Perc 2 0.0047 uF. The decaying cards hold 64 samples (2.4 ms) before the decay starts: added to the time constant.
// The split cards play the first half on trigger 3, the second on triggers 1 and 2
const instrument instruments[KIT_INSTRUMENTS] = {
    {BASS, WHOLE, 27179, 4, 1.f, 27.4f + 2.4f, BASS},        // BASS (BASS 3)
    {SNARE, WHOLE, 40769, 4, 1.f, 0, SNARE},                 // SNARE (SNARE 3)
    {PERC1, FIRST_HALF, 27179, 4, 1.f, 0, PERC1},            // RIMSHOT
    {PERC2, FIRST_HALF, 19083, 4, 1.f, 0, PERC2},            // CLAPS
    {PERC1, SECOND_HALF, 27179, 4, 1.f, 0, PERC1},           // TAMB
    {PERC2, SECOND_HALF, 19083, 4, 1.f, 0, PERC2},           // SHAKE
    {HIHAT, WHOLE, 27179, 2, 0.826f, 12.7f + 2.4f, HIHAT},   // CH (HI-HAT CLOSED)
    {HIHAT, WHOLE, 27179, 2, 1.f, 0, HIHAT},                 // OH (HI-HAT OPEN), decay from CC 4
    {TOM1, WHOLE, 36847, 4, 1.f, 56.4f + 2.4f, TOM1},        // TOM1
    {TOM1, WHOLE, 31332, 4, 1.f, 56.4f + 2.4f, TOM1},        // TOM2
    {TOM1, WHOLE, 27179, 4, 1.f, 56.4f + 2.4f, TOM1},        // TOM3
    {TOM2, WHOLE, 36847, 4, 1.f, 56.4f + 2.4f, TOM2},        // TOM4
    {TOM2, WHOLE, 31332, 4, 1.f, 56.4f + 2.4f, TOM2},        // TOM5
    {TOM2, WHOLE, 27179, 4, 1.f, 56.4f + 2.4f, TOM2},        // TOM6
    {CYMB, SECOND_HALF, 27179, 2, 1.f, 0, CYMB},             // RIDE (CYMB 1/2)
    {CYMB, FIRST_HALF, 27179, 2, 1.f, 0, CYMB},              // CRASH (CYMB 3)
};

const char *names[KIT_INSTRUMENTS] = {"BASS", "SNARE", "RIMSHOT", "CLAPS", "TAMB", "SHAKE", "CH",   "OH",
                                      "TOM1", "TOM2",  "TOM3",    "TOM4",  "TOM5", "TOM6",  "RIDE", "CRASH"};

std::pair<uint8_t, uint8_t> midi_key_map[] = {
    {35, 0},   // BASS
    {36, 0},   //
    {38, 1},   // SNARE
    {40, 1},   //
    {37, 2},   // RIMSHOT
    {39, 3},   // CLAPS
    {54, 4},   // TAMB
    {69, 5},   // SHAKE - Cabasa
    {70, 5},   // SHAKE - Maracas
    {42, 6},   // CH - Closed
    {44, 6},   // CH - Pedal
    {46, 7},   // OH - Open
    {50, 8},   // TOM1 - High Tom
    {48, 9},   // TOM2 - Hi-Mid Tom
    {47, 10},  // TOM3 - Low-Mid Tom
    {45, 11},  // TOM4 - Low Tom
    {43, 12},  // TOM5 - High Floor Tom
    {41, 13},  // TOM6 - Low Floor Tom
    {51, 14},  // RIDE
    {53, 14},  // RIDE - Bell
    {59, 14},  // RIDE - Edge
    {49, 15},  // CRASH
    {55, 15},  // CRASH - Splash
    {57, 15},  // CRASH 2
    {0xFF, 0}, // END
};
