#include "../../squares-and-circles-api.h"

// LinnDrum (LM-2), played from its sample EPROMs (extern/eproms/lm2/info.txt), one flash blob per ROM. Its voices
// and their rates follow the MAME driver (src/mame/linn/linndrum.cpp). Bass, claps and cabasa of the LinnDrum were
// never dumped: their slots have the Linn 9000's chips. The engine is eprom_kit.h, the same as DMX.cpp's.

constexpr size_t KIT_ROMS = 12;
constexpr size_t KIT_INSTRUMENTS = 16;
constexpr size_t OPEN_HAT = 8; // OH

#include "../eprom_kit.h"

enum : uint8_t
{
    BASS,
    SNARE,
    SIDESTICK,
    CLAPS,
    TAMB,
    CABASA,
    COWBELL,
    HAT,
    TOMS,
    CONGAS,
    RIDE,
    CRASH,
};

const char *const rom_blob[KIT_ROMS] = {"LINNBASS", "LINNSNAR", "LINNSSTK", "LINNCLAP", "LINNTAMB", "LINNCABA",
                                        "LINNCOWB", "LINNHAT",  "LINNTOM",  "LINNCNGA", "LINNRIDE", "LINNCRSH"};

// The voices: 8 "mux drums" on one clock (bass, claps, tamb, cabasa, cowbell, hat, ride, crash: one sound each at a
// time), the snare/sidestick section and the tom/conga section (one sound of each section at a time). The mux clock
// is a factory trimmer: 28 kHz (the published 28-35 kHz; MAME's estimate of the 74LS627 gives 24.8 kHz). Snare,
// toms and congas have tuning knobs; the rates are MAME's at its default knob positions, the snare's 28 kHz like the
// mux drums. The closed hat: the EG holds 5.6 ms, then decays with 56.7 ms (HIHAT DECAY knob in the middle).
// MAME's sidestick chip (sstk1) has the CRC32 of the DMX's STIK, whose second half is the DMX's tambourine: the
// sidestick is its first half
const instrument instruments[KIT_INSTRUMENTS] = {
    {BASS, WHOLE, 28000, 4, 1.f, 0, BASS},                   // BASS
    {SNARE, WHOLE, 28000, 4, 1.f, 0, SNARE},                 // SNARE
    {SIDESTICK, FIRST_HALF, 28000, 4, 1.f, 0, SNARE},        // SIDESTK
    {CLAPS, WHOLE, 28000, 3, 1.f, 0, CLAPS},                 // CLAPS
    {TAMB, WHOLE, 28000, 4, 1.f, 0, TAMB},                   // TAMB
    {CABASA, WHOLE, 28000, 4, 1.f, 0, CABASA},               // CABASA
    {COWBELL, WHOLE, 28000, 4, 1.f, 0, COWBELL},             // COWBELL
    {HAT, WHOLE, 28000, 2, 1.f, 56.7f + 5.6f, HAT},          // CH
    {HAT, WHOLE, 28000, 2, 1.f, 0, HAT},                     // OH, decay from CC 4
    {TOMS, WHOLE, 47392, 4, 1.f, 0, TOMS},                   // HITOM
    {TOMS, WHOLE, 40827, 4, 1.f, 0, TOMS},                   // MIDTOM
    {TOMS, WHOLE, 34261, 4, 1.f, 0, TOMS},                   // LOTOM
    {CONGAS, WHOLE, 42140, 4, 1.f, 0, TOMS},                 // HICONGA
    {CONGAS, WHOLE, 34261, 4, 1.f, 0, TOMS},                 // LOCONGA
    {RIDE, WHOLE, 28000, 2, 1.f, 0, RIDE},                   // RIDE
    {CRASH, WHOLE, 28000, 2, 1.f, 0, CRASH},                 // CRASH
};

const char *names[KIT_INSTRUMENTS] = {"BASS",  "SNARE",  "SIDESTK", "CLAPS",   "TAMB",    "CABASA", "COWBELL", "CH",
                                      "OH",    "HITOM",  "MIDTOM",  "LOTOM",   "HICONGA", "LOCONGA", "RIDE",   "CRASH"};

std::pair<uint8_t, uint8_t> midi_key_map[] = {
    {35, 0},   // BASS
    {36, 0},   //
    {38, 1},   // SNARE
    {40, 1},   //
    {37, 2},   // SIDESTK
    {39, 3},   // CLAPS
    {54, 4},   // TAMB
    {69, 5},   // CABASA
    {56, 6},   // COWBELL
    {42, 7},   // CH - Closed
    {44, 7},   // CH - Pedal
    {46, 8},   // OH - Open
    {50, 9},   // HITOM - High Tom
    {48, 9},   //         Hi-Mid Tom
    {47, 10},  // MIDTOM - Low-Mid Tom
    {45, 10},  //          Low Tom
    {43, 11},  // LOTOM - High Floor Tom
    {41, 11},  //         Low Floor Tom
    {62, 12},  // HICONGA - Mute Hi Conga
    {63, 12},  //           Open Hi Conga
    {64, 13},  // LOCONGA - Low Conga
    {51, 14},  // RIDE
    {53, 14},  // RIDE - Bell
    {59, 14},  // RIDE - Edge
    {49, 15},  // CRASH
    {55, 15},  // CRASH - Splash
    {57, 15},  // CRASH 2
    {0xFF, 0}, // END
};
