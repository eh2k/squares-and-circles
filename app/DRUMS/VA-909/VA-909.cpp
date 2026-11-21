// Copyright (C)2026 - Eduard Heidt
//
// Author: Eduard Heidt (eh2k@gmx.de)
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// DRUMS/VA909-...: the TR-909 with its voices modelled from the circuit (lib/va909, MODEL.md there), the panel's knobs
// and the accent; one binary, five engines (engine::name() picks the instruments in setup()):
// - DRUMS/VA909-BD, -SD, -RS, -CP, -Toms: one instrument, the track's TRIG-INPUT and ACCENT, mono. The toms: the Tom
//   knob picks LT, MT or HT for the next hit, one Tune and Decay turn the three alike (each in its own range, as the
//   909's knobs at the same place); three voices and the tom noise they share, so a tom rings on while another is hit
//   (Tom modulated: a fill from one track).
// Off for now (VA909_ALL 1 brings them back; DRUMS/TR909-HiHat, after the toms on the engine page, plays the hats and
// the ride):
// - DRUMS/VA909-Hats (CH OH: one voice, as on the 909), DRUMS/VA909-Cymbals (CR RD): the instruments of a part of the
//   circuit as a small kit (setupMultiTrigs(), as DRUMS/TR909-HiHat: each its own TRIG on the I/O page and its General
//   MIDI keys; stereo).
// - DRUMS/VA909-Kit (DRUMS/VA-909 before): all eleven, BD SD LT MT HT RS CP CH OH CR RD (the order of DRUMKITS/TR-909),
//   a kit without an instrument selector. The knobs on three pages as on the panel: BD | SD (a column each), the toms
//   (a row each, TUNE | DECAY), the hats' decays, the cymbals' tunes and Accent / Level; no mods.
// The 909's knobs and mods (knobs too): BD, the hats and the cymbals the mods on page 2; SD and the toms on one page,
// their mods in the row between the 909's knobs (SD's Tune and the toms' Decay go on past the 909's range by a part);
// RS and CP (no knobs on the 909 but LEVEL) their mods and Level on one page. A mod is a part of the circuit changed as
// a pot in its place would (the fitted part's value times a factor; 0.5 the 909's, the Cymbals' decays 1, BD's Drive
// 0), none fitted to a modded 909. The instruments' levels and pans (the 909's LEVEL knobs) are on the firmware's mix
// page of the kits. Accent (the 909's TOTAL ACCENT) while the ACCENT input is patched, else Level.
// The accent is the voices' ACCENT line: a trigger input plays a normal hit, a MIDI note in a kit a softer one by its
// velocity; the ACCENT input high at a hit, or a note over velocity 100, accents the instruments hit with it (as the
// 909's accent track its step). The output is the 909's mix out, a channel each: the mix bus into C154 (a high pass)
// and the inverting master amp. The hi-hat and the cymbals play the 909's ROMs (the blobs 909_HIGH, 909_CRASH,
// 909_RIDE) as its circuit does; CC 4 turns OH's decay down (127: to 0). Each engine allocates only its own voices.


#include "../../squares-and-circles-api.h"
#include <new>
#include "va909/bd.h"
#include "va909/va909_bd_fit.h"
#include "va909/sd.h"
#include "va909/va909_sd_fit.h"
#include "va909/cp.h"
#include "va909/va909_cp_fit.h"
#include "va909/hh.h"
#include "va909/va909_hh_fit.h"
#include "va909/va909_cr_fit.h"
#include "va909/va909_rd_fit.h"
#include "va909/tt.h"
#include "va909/va909_tt_fit.h"
#include "va909/rs.h"
#include "va909/va909_rs_fit.h"

// the engine page lists them in this order (the firmware's engines_order: after DRUMS/Claps, DRUMS/TR909-HiHat after
// the toms)
ENGINE_NAME("DRUMS/VA909-BD");
ENGINE_NAME("DRUMS/VA909-SD");
ENGINE_NAME("DRUMS/VA909-RS");
ENGINE_NAME("DRUMS/VA909-CP");
ENGINE_NAME("DRUMS/VA909-Toms");
#ifndef VA909_ALL
#define VA909_ALL 0 // 1: also the hats, the cymbals (the ROM voices) and the kit of all eleven
#endif
#if VA909_ALL
ENGINE_NAME("DRUMS/VA909-Hats");
ENGINE_NAME("DRUMS/VA909-Cymbals");
ENGINE_NAME("DRUMS/VA909-Kit");
ENGINE_ALIAS("DRUMS/VA-909"); // the same parameters
#endif

// Without VA909_ALL the ROM voices (the hats and the cymbals) and the engines of several TRIGs (setupMultiTrigs(), the
// mix page, stereo) are not compiled: their branches below are constant false. The app is the smaller for it (it fits
// the DSM's SRAM next to SYNTH/Plaits: 2.3 to 2.8 times fewer cycles than run in place from the QSPI flash)
static constexpr bool ROM_VOICES = VA909_ALL != 0, MULTI = VA909_ALL != 0;

#ifndef VA909_VOICES
#define VA909_VOICES 255 // the voices rendered (bits: 1 BD, 2 SD, 4 CP, 8 HH, 16 CR, 32 RD, 64 toms, 128 RS): profiling
#endif

enum
{
    BD,
    SD,
    LT,
    MT,
    HT,
    RS,
    CP,
    CH,
    OH,
    CR,
    RD,
    NUM_INST
};
static const char *const _instNames[NUM_INST] = {"BD", "SD", "LT", "MT", "HT", "RS", "CP", "CH", "OH", "CR", "RD"};

// the engines: their name after "DRUMS/" and their instruments, in the order of their TRIGs
struct EngineDef
{
    const char *name;
    uint8_t count;
    uint8_t inst[NUM_INST];
};
enum
{
    E_KIT,
    E_BD,
    E_SD,
    E_TOMS,
    E_RS,
    E_CP,
    E_HATS,
    E_CYM,
};
static const EngineDef _engines[] = {
    {"VA909-Kit", NUM_INST, {BD, SD, LT, MT, HT, RS, CP, CH, OH, CR, RD}},
    {"VA909-BD", 1, {BD}},
    {"VA909-SD", 1, {SD}},
    {"VA909-Toms", 3, {LT, MT, HT}},
    {"VA909-RS", 1, {RS}},
    {"VA909-CP", 1, {CP}},
    {"VA909-Hats", 2, {CH, OH}},
    {"VA909-Cymbals", 2, {CR, RD}},
};

// General MIDI keys as DRUMKITS/DrumSynth maps them (35/36 BD, 38/40 SD, 41/43 LT, 45/47 MT, 48/50 HT, 42/44 CH,
// crash 49/55/57, ride 51/53/59): {key, instrument}; a kit gets those of its instruments, by their TRIG's index
static const std::pair<uint8_t, uint8_t> _gmKeys[] = {
    {35, BD}, {36, BD}, {37, RS}, {38, SD}, {40, SD}, {39, CP}, {41, LT}, {43, LT}, {45, MT}, {47, MT}, {48, HT},
    {50, HT}, {42, CH}, {44, CH}, {46, OH}, {49, CR}, {55, CR}, {57, CR}, {51, RD}, {53, RD}, {59, RD},
};
static std::pair<uint8_t, uint8_t> _keys[LEN_OF(_gmKeys) + 1];
static const char *_names[NUM_INST];

static int _engine = E_KIT;
static int _count = NUM_INST;        // the engine's instruments
static int8_t _local[NUM_INST];      // an instrument's TRIG index in the engine, -1: not in it
static bool _single = false;         // one TRIG: the track's TRIG-INPUT, mono (the toms: the instrument Tom picks)
static bool _ok = false;             // the voices made (else silence: the message says so)

// the panel's knobs (0..1, the middle by default). BD Tune is the pitch envelope's decay, as on the 909
static float _bdTune = 0.5f, _bdAttack = 0.5f, _bdDecay = 0.5f;
static float _sdTune = 0.5f, _sdTone = 0.5f, _sdSnappy = 0.5f;
static float _ltTune = 0.5f, _ltDecay = 0.5f, _mtTune = 0.5f, _mtDecay = 0.5f, _htTune = 0.5f, _htDecay = 0.5f;
static float _chDecay = 0.5f, _ohDecay = 0.5f, _crTune = 0.5f, _rdTune = 0.5f;
static float _accLevel = 0.8f; // ACCENT patched: the TOTAL ACCENT; else the level (LEVEL_UNITY: as the recordings)

// the mods (0..1; 0.5 the 909, a factor of 1 on the fitted part): the parts named, a factor mod2(k, n) of 2^-n ... 2^n
static float _bdPitch = 0.5f, _bdSweep = 0.5f, _bdHold = 0.5f, _bdDecMax = 0.5f;
static float _bdDrive = 0.f; // 0: the 909's (no drive), as the 909's Drive mods with their pot down
// D10/D11, by their threshold: germanium, the 909's 1N4148, two 1N4148 in series each way, red LEDs (the LEDs another
// of the 909's bass drum mods, the others diodes swapped there as in a distortion pedal's clipper). The body as loud
// as with the 1N4148 at every Drive (BassDrum::setLevelRef(): the diodes change the wave, not the level)
static int32_t _bdDiodes = 1;
static const char *_diodeNames[] = {"Ge", "1N4148", "2x1N4148", "LED"};
// Is, N of the diode law, typical, not measured: germanium 0.25 V at 100 uA, 0.34 V at 1 mA, a soft knee; a red LED
// 1.5 V at 10 uA, 1.9 V at 20 mA. Two 1N4148 in series: the fitted Is, twice the fitted N
static constexpr float GE_IS = 1.6e-7f, GE_N = 1.5f;
static constexpr float LED_IS = 4e-18f, LED_N = 2.f;
// VA909-SD: Decay the tones' decay; Color the snappy's noise band (its low pass and high pass together); Tune (_sdTune)
// the 909's TUNE in its middle half, below and above it R287 as the Pitch mod had it (sdTune909(), sdPitch())
static float _sdTDec = 0.5f, _sdColor = 0.5f;
static inline float sdTune909(float k) { return k <= 0.25f ? 0.f : k >= 0.75f ? 1.f : (k - 0.25f) * 2.f; }
static inline float sdPitch(float k) { return k < 0.25f ? 2.f * k : k > 0.75f ? 2.f * k - 1.f : 0.5f; }
static float _ttBend = 0.5f, _ttNoise = 0.5f;
// VA909-Toms: the instrument of the next hit; TUNE and DECAY of the three toms (Decay: DECAY's whole range in the lower
// half, its top in the middle, above it C21 larger, applyMods())
static int32_t _ttSel = 0;
static const char *_tomNames[] = {"LT", "MT", "HT"};
static float _ttTune = 0.5f, _ttDecay = 0.5f;
static float _rsTune = 0.5f, _rsDec = 0.5f, _rsRing = 0.5f;
static float _cpLen = 0.5f, _cpRate = 0.5f, _cpTail = 0.5f, _cpColor = 0.5f;
static float _hhTune = 0.5f;
static float _crDec = 1.f, _rdDec = 1.f; // 1: the 909's (the ROM's length), less: shorter
static constexpr float ROM_ACCENT = 0.3f;
static float _romAccent = ROM_ACCENT; // Acc-Amt of the hats and the cymbals (below)

static constexpr float ACC_NORMAL = VA909_ACC_NORMAL; // V on the ACCENT line of a hit without accent
static constexpr float ACC_MAX = 5.f;                  // V, the DAC's top
static constexpr float LEVEL_UNITY = 0.8f;             // the Level knob's place of the recordings' level (and the
                                                       // TOTAL ACCENT of a MIDI accent while ACCENT is not patched)
static constexpr float OUT_GAIN = 0.22f;               // V of the voices to the output's full scale: a bass drum at
                                                       // noon peaks at 0.5 (the click) in each channel at LEVEL 128,
                                                       // PAN in the middle (mixLevelL/R() 0.7)

// the panel's LEVEL knobs: the toms at 7, the rim shot at 8, the clap at 3 of 10, the others at their top, as the
// recordings were made. The toms the same at every TUNE, an accent +4 dB; the rim shot's model as SPICE has it (4.4 dB
// quieter with its fitted constants); the clap with the noise bus at the circuit's level (5.1 dB louder at the normal
// accent, which its recordings were not made at). So the peaks before the output's limit (QEMU, a hit every 250 ms,
// Accent 0.8): 0.3 ... 0.7 without accent, 0.43 ... 1.25 with it (the bass drum 1.05, the cymbals 1.2 as they pile
// up); the clap at 5.5 (the kit's) peaked at 1.27 and 2.17
static constexpr float TOM_LEVEL = 0.7f, RS_LEVEL = 0.8f, CP_LEVEL = 0.3f;
// VA909-BD alone: +6 dB over the kit's channel (the kit's 909 balance is its own; LUFS -15.0 with a hit every 250 ms,
// as loud as DRUMS/909ish-BD -14.1, MD/TRX-BD -15.2, CY/KICK -15.7; the click 0.97 after the soft limit, accented it
// clips)
static constexpr float BD_GAIN = 2.f;
// VA909-SD alone: +9 dB (LUFS -15.7, as DRUMS/909ish-SD -15.2; MD/TRX-SD -17.1, DRUMS/808ish-SD -18.0; its peak 0.91
// after the soft limit, accented it clips)
static constexpr float SD_GAIN = 2.818f;
// VA909-RS alone: +3 dB over the kit's channel (the 909's LEVEL at 8, RS_LEVEL; LUFS -19.3, between MD/EFM-RS -21.9
// and MD/P-I-RS -16.6, MD/TRX-RS -27.9; its peak 0.78, accented 0.99)
static constexpr float RS_GAIN = 1.413f;
// VA909-CP alone: +4.5 dB (the 909's LEVEL at 3, CP_LEVEL; LUFS -17.8, between MD/TRX-CP -20.0 and MD/EFM-CP -16.1;
// its peak 0.95 after the soft limit, accented or at Level 1 at the limit)
static constexpr float CP_GAIN = 1.679f;
// VA909-Toms alone: +6 dB (the 909's LEVEL at 7, TOM_LEVEL; Decay in the middle, the 909's longest: LUFS -17.5 LT,
// -18.0 MT, -17.2 HT, as MD/EFM-XT -17.9, MD/TRX-XT -13.0; their peaks 0.55 ... 0.58, accented 0.77 ... 0.80)
static constexpr float TT_GAIN = 2.f;

// the parts each voice plays with: the fitted ones (_xxB, with the LEVEL knobs above), the mods on a copy (_xxP). BD,
// SD, the hat and the cymbals start on the fitted constants themselves, as the kit did (the compiler folds what their
// init() computes of them: the same samples), and take the copy at their first mod
static va909::BDParams _bdP = va909::bdFit;
static va909::SDParams _sdP = va909::sdFit;
static va909::TTParams _ltB = va909::ltFit, _mtB = va909::mtFit, _htB = va909::htFit;
static va909::TTParams _ltP, _mtP, _htP;
static va909::RSParams _rsB = va909::rsFit, _rsP;
static va909::CPParams _cpB = va909::cpFit, _cpP;
static va909::HHParams _hhP = va909::hhFit;

// the voices, only the engine's (nullptr: not in it)
static va909::BassDrum *_bd = nullptr;
static va909::SnareDrum *_sd = nullptr;
static va909::HandClap *_cp = nullptr;
static va909::HiHat *_hh = nullptr;
static va909::Cymbal *_cr = nullptr, *_rd = nullptr;
static va909::Tom *_lt = nullptr, *_mt = nullptr, *_ht = nullptr;
static va909::TomTables *_tomTables = nullptr;
static va909::TomNoise *_tomNoise = nullptr;
static va909::RimShot *_rs = nullptr;
static int _hhInst = CH; // the hat's last instrument: its level and pan
static float *_outL = nullptr, *_outR = nullptr;
static float _monoGain = 0.f; // a single instrument's channel: the kit's at the mix page's LEVEL 128, PAN in the middle
static float _hpXL = 0.f, _hpYL = 0.f, _hpXR = 0.f, _hpYR = 0.f, _hpA = 0.f; // the mix outs' high passes

template <class T>
static T *make()
{
    void *m = malloc(sizeof(T));
    return m ? new (m) T() : nullptr;
}

static bool has(int inst) { return _local[inst] >= 0; }

// The layout of the released drums (DRUMS/808ish-BD, DRUMS/909ish-BD, ...: Pitch | Punch, Tone | Decay; a row per
// instrument in DRUMS/TR909-HiHat): the pitch at the top left, the decay at the bottom right, the punch or snappy above
// it (the right encoder turns the character and the decay); Accent / Level at the bottom of the left column
static void addParams()
{
    switch (_engine)
    {
#if VA909_ALL
    case E_KIT:
        // page 1: BD | SD, an instrument a column (the left encoder turns BD's knobs, the right one SD's): their tunes,
        // attack and snappy, their decays (SD's TONE: the snappy's length)
        engine::addParam("BD-Tune", &_bdTune);
        engine::addParam("SD-Tune", &_sdTune);
        engine::addParam("BD-Att", &_bdAttack);
        engine::addParam("SD-Snap", &_sdSnappy);
        engine::addParam("BD-Dec", &_bdDecay);
        engine::addParam("SD-Tone", &_sdTone);
        // page 2: the toms, a row each
        engine::addParam("\fLT-Tune", &_ltTune);
        engine::addParam("LT-Dec", &_ltDecay);
        engine::addParam("MT-Tune", &_mtTune);
        engine::addParam("MT-Dec", &_mtDecay);
        engine::addParam("HT-Tune", &_htTune);
        engine::addParam("HT-Dec", &_htDecay);
        // page 3: the hats, the cymbals, Accent / Level (draw() names it). The mix page follows
        engine::addParam("\fCH-Dec", &_chDecay);
        engine::addParam("OH-Dec", &_ohDecay);
        engine::addParam("CR-Tune", &_crTune);
        engine::addParam("RD-Tune", &_rdTune);
        engine::addParam("Level", &_accLevel);
        break;
#endif
    case E_BD:
        engine::addParam("Tune", &_bdTune);
        engine::addParam("Attack", &_bdAttack);
        engine::addParam("Level", &_accLevel);
        engine::addParam("Decay", &_bdDecay);
        engine::addParam("\fPitch", &_bdPitch);
        engine::addParam("Sweep", &_bdSweep);
        engine::addParam("Hold", &_bdHold);
        engine::addParam("Dec-Max", &_bdDecMax);
        engine::addParam("Drive", &_bdDrive);
        engine::addParam("Diodes\n%s", &_bdDiodes, 0, 3, _diodeNames);
        break;
    case E_SD:
        // one page: the left column the tones (the left encoder), the right one the snappy; TONE the snappy's length
        // at the decay's place, the mods Decay and Color in the row between
        engine::addParam("Tune", &_sdTune);
        engine::addParam("Snappy", &_sdSnappy);
        engine::addParam("Decay", &_sdTDec);
        engine::addParam("Color", &_sdColor);
        engine::addParam("Level", &_accLevel);
        engine::addParam("Tone", &_sdTone);
        break;
    case E_TOMS:
        // one page: the tom at the character's place, its mods (Bend, Noise) between it and Level | Decay
        engine::addParam("Tune", &_ttTune);
        engine::addParam("Tom", &_ttSel, 0, LEN_OF(_tomNames) - 1, _tomNames);
        engine::addParam("Bend", &_ttBend);
        engine::addParam("Noise", &_ttNoise);
        engine::addParam("Level", &_accLevel);
        engine::addParam("Decay", &_ttDecay);
        break;
    // RS, CP: the 909 has no knobs for them but LEVEL: their mods and Accent / Level on one page
    case E_RS:
        engine::addParam("Tune", &_rsTune);
        engine::addParam("Ring", &_rsRing);
        engine::addParam("Level", &_accLevel);
        engine::addParam("Decay", &_rsDec);
        break;
    case E_CP:
        // Color (the noise's band pass) at the pitch's place, as DRUMS/Claps has it; the tail the decay
        engine::addParam("Color", &_cpColor);
        engine::addParam("Rate", &_cpRate);
        engine::addParam("Length", &_cpLen);
        engine::addParam("Tail", &_cpTail);
        engine::addParam("Level", &_accLevel);
        break;
#if VA909_ALL
    case E_HATS:
        engine::addParam("CH-Dec", &_chDecay);
        engine::addParam("OH-Dec", &_ohDecay);
        engine::addParam("Level", &_accLevel);
        engine::addParam("\fTune", &_hhTune);
        engine::addParam("Acc-Amt", &_romAccent, 0.f, 0.6f);
        break;
    case E_CYM:
        engine::addParam("CR-Tune", &_crTune);
        engine::addParam("RD-Tune", &_rdTune);
        engine::addParam("Level", &_accLevel);
        engine::addParam("\fCR-Dec", &_crDec);
        engine::addParam("RD-Dec", &_rdDec);
        engine::addParam("Acc-Amt", &_romAccent, 0.f, 0.6f);
        break;
#endif
    }
}

void engine::setup()
{
    const char *slash = strchr(engine::name(), '/');
    for (int e = 0; e < (int)LEN_OF(_engines); e++)
        if (slash && !strcmp(slash + 1, _engines[e].name))
            _engine = e;
    const EngineDef &def = _engines[_engine];
    _count = def.count;
    _single = !MULTI || _count == 1 || _engine == E_TOMS;
    for (int i = 0; i < NUM_INST; i++)
        _local[i] = -1;
    for (int i = 0; i < _count; i++)
    {
        _local[def.inst[i]] = (int8_t)i;
        _names[i] = _instNames[def.inst[i]];
    }
    addParams();

    _outL = engine::outputBuffer_f32(OUT_AUDIO_L);
    if (MULTI && !_single)
    {
        _outR = engine::outputBuffer_f32(OUT_AUDIO_R);
        int n = 0;
        for (const auto &k : _gmKeys)
            if (has(k.second))
                _keys[n++] = {k.first, (uint8_t)_local[k.second]};
        _keys[n] = {0xFF, 0};
        engine::setupMultiTrigs(_count, _names, _keys);
    }
    // the kit's channel at the mix page's LEVEL 128, PAN in the middle (mixLevelL() of its defaults)
    _monoGain = cosf((float)128 / 255.f * (float)M_PI_2) * ((4.f / UINT16_MAX) * 128.f * 128.f);
    if (_engine == E_BD)
        _monoGain *= BD_GAIN;
    if (_engine == E_SD)
        _monoGain *= SD_GAIN;
    if (_engine == E_RS)
        _monoGain *= RS_GAIN;
    if (_engine == E_CP)
        _monoGain *= CP_GAIN;
    if (_engine == E_TOMS)
        _monoGain *= TT_GAIN;

    bool oom = false;
    if (has(BD))
    {
        if ((_bd = make<va909::BassDrum>()))
        {
            _bd->init(SAMPLE_RATE, &va909::bdFit);
            if (_engine == E_BD) // Diodes: the body at the 1N4148's level
                _bd->setLevelRef(va909::bdFit.dIs, va909::bdFit.dN);
        }
        oom |= !_bd;
    }
    if (has(SD))
    {
        if ((_sd = make<va909::SnareDrum>()))
            _sd->init(SAMPLE_RATE, &va909::sdFit);
        oom |= !_sd;
    }
    if (has(CP))
    {
        _cpB.vr20 *= CP_LEVEL;
        _cpP = _cpB;
        if ((_cp = make<va909::HandClap>()))
            _cp->init(SAMPLE_RATE, &_cpP);
        oom |= !_cp;
    }
    size_t nHigh = 0, nCrash = 0, nRide = 0;
    bool rom = true;
    if (ROM_VOICES && has(CH))
    {
        const uint8_t *high = machine::fs_read("909_HIGH", &nHigh);
        if ((_hh = make<va909::HiHat>()))
            _hh->init(SAMPLE_RATE, &va909::hhFit, high, (int)nHigh, ACC_NORMAL);
        oom |= !_hh;
        rom &= !_hh || _hh->hasRom();
    }
    if (ROM_VOICES && has(CR))
    {
        const uint8_t *crash = machine::fs_read("909_CRASH", &nCrash);
        if ((_cr = make<va909::Cymbal>()))
            _cr->init(SAMPLE_RATE, &va909::crFit, crash, (int)nCrash, ACC_NORMAL);
        oom |= !_cr;
        rom &= !_cr || _cr->hasRom();
    }
    if (ROM_VOICES && has(RD))
    {
        const uint8_t *ride = machine::fs_read("909_RIDE", &nRide);
        if ((_rd = make<va909::Cymbal>()))
            _rd->init(SAMPLE_RATE, &va909::rdFit, ride, (int)nRide, ACC_NORMAL);
        oom |= !_rd;
        rom &= !_rd || _rd->hasRom();
    }
    if (has(LT))
    {
        _ltB.vr12 *= TOM_LEVEL, _mtB.vr12 *= TOM_LEVEL, _htB.vr12 *= TOM_LEVEL;
        _ltP = _ltB, _mtP = _mtB, _htP = _htB;
        _tomTables = make<va909::TomTables>();
        _tomNoise = make<va909::TomNoise>();
        _lt = make<va909::Tom>(), _mt = make<va909::Tom>(), _ht = make<va909::Tom>();
        if (_tomTables && _tomNoise && _lt && _mt && _ht)
        {
            _tomTables->fill(_ltP);
            _lt->init(SAMPLE_RATE, &_ltP, _tomTables);
            _mt->init(SAMPLE_RATE, &_mtP, _tomTables);
            _ht->init(SAMPLE_RATE, &_htP, _tomTables);
            _tomNoise->init(SAMPLE_RATE, &va909::tnFit);
        }
        else
        {
            oom = true;
            _lt = _mt = _ht = nullptr;
        }
    }
    if (has(RS))
    {
        _rsB.vr19 *= RS_LEVEL;
        _rsP = _rsB;
        if ((_rs = make<va909::RimShot>()))
            _rs->init(SAMPLE_RATE, &_rsP);
        oom |= !_rs;
    }
    _ok = !oom;
    if (oom)
        gfx::message("OUT OF MEMORY");
    else if (!rom)
        gfx::message(_engine == E_KIT    ? "ROMS NOT FOUND:\n909_HIGH 909_CRASH\n909_RIDE"
                     : _engine == E_HATS ? "ROM NOT FOUND:\n909_HIGH"
                                         : "ROMS NOT FOUND:\n909_CRASH\n909_RIDE");
    _hpA = expf(-2.f * 3.14159265f * VA909_MIX_HP / SAMPLE_RATE);
}

// a mod's factor: 2^(-n) at 0, 1 at 0.5 (exactly: the part as fitted), 2^n at 1
static inline float mod2(float k, float n)
{
    return k == 0.5f ? 1.f : exp2f((k - 0.5f) * 2.f * n);
}

// the mods onto the parts' copies when one changed (also as modulated), the voices' scalar coefficients again (none of
// these parts is in a voice's tables: setParams(p, false), some exp() each)
static void applyMods()
{
    static float bd[4] = {0.5f, 0.5f, 0.5f, 0.5f};
    static int32_t bdDiodes = 1;
    if (_bd && (bd[0] != _bdPitch || bd[1] != _bdSweep || bd[2] != _bdHold || bd[3] != _bdDecMax ||
                bdDiodes != _bdDiodes))
    {
        bd[0] = _bdPitch, bd[1] = _bdSweep, bd[2] = _bdHold, bd[3] = _bdDecMax;
        _bdP = va909::bdFit;
        if (_bdDiodes == 0) // germanium for D10/D11: the triangle rounded from some 0.25 V (the 1N4148 at 0.5 V)
            _bdP.dIs = GE_IS, _bdP.dN = GE_N;
        else if (_bdDiodes == 2) // two 1N4148 each way: from some 0.9 V
            _bdP.dN *= 2.f;
        else if (_bdDiodes == 3) // red LEDs: the triangle passes up to some 1.3 V
            _bdP.dIs = LED_IS, _bdP.dN = LED_N;
        _bdP.r27 /= mod2(_bdPitch, 1.f);   // R27 (+15 V into IC12a): the pitch at rest, +-1 octave
        _bdP.r26 /= mod2(_bdSweep, 1.f);   // R26 (ENV3 into IC12a): the sweep's depth x0.5 ... x2, ENV3 faster with it
        _bdP.c7 *= mod2(_bdHold, 1.5f);    // C7: the hold before the decay (some 70 ms) x0.35 ... x2.8
        _bdP.vr5 *= mod2(_bdDecMax, 2.f);  // VR5 DECAY: its range x0.25 ... x4
        _bd->setParams(&_bdP, false);
        if (bdDiodes != _bdDiodes) // the shaper's table again (driveStep() below, a point a frame)
            _bd->shaperChanged();
        bdDiodes = _bdDiodes;
    }
    // Drive: the triangle into D10/D11 x1 ... x16 (a gain before R52), the shaper's table filled again towards it one
    // of its 64 pairs of points a frame (a shaper a pair, some 6k instructions: a change takes 64 frames, 32 ms; with
    // other Diodes at a new drive 32 frames before it for the 1N4148's level there). 4 a frame until 2026-11 (8 ms):
    // turning Drive at 81 % load took a frame up to 127 % of its time, at 87 % the DSM's audio ran dry with its ring
    // of 6 blocks (test_usb/underrun_test.py drive)
    if (_bd)
    {
        _bd->setDrive(_bdDrive > 0.f ? exp2f(_bdDrive * 4.f) : 1.f);
        _bd->driveStep(1);
    }
    // SD: the parts of Tune's outer quarters, Decay, Color
    const float sdP = _sd ? sdPitch(_sdTune) : 0.5f;
    static float sd[3] = {0.5f, 0.5f, 0.5f};
    if (_sd && (sd[0] != sdP || sd[1] != _sdTDec || sd[2] != _sdColor))
    {
        sd[0] = sdP, sd[1] = _sdTDec, sd[2] = _sdColor;
        _sdP = va909::sdFit;
        _sdP.r287 /= mod2(sdP, 1.f);       // R287 (+15 V to TUNE's top) x2 ... x0.5: TUNE's range down or up
        const float t = mod2(_sdTDec, 1.5f); // C83 / C82 (ENV3, ENV2): the tones' decay x0.35 ... x2.8
        _sdP.c83 *= t, _sdP.c82 *= t;
        // Color, the noise's band: C81 / C84, IC40a's low pass on the snappy's noise (15.2 kHz fitted, its Q kept),
        // down to x0.25, up to 20 kHz (the bilinear filter's top); C80, the high pass after it into Q48 (1.5 kHz),
        // x2.8 ... x0.35 with it (540 Hz, more body, ... 4.3 kHz)
        const va909::SDParams &f = va909::sdFit;
        const float lpMax = 20000.f * 6.2831853f * f.r301 * sqrtf(f.c81 * f.c84);
        const float c = _sdColor <= 0.5f ? mod2(_sdColor, 2.f) : 1.f + (_sdColor - 0.5f) * 2.f * (lpMax - 1.f);
        _sdP.c81 /= c, _sdP.c84 /= c;
        _sdP.c80 *= mod2(1.f - _sdColor, 1.5f);
        _sd->setParams(&_sdP, false);
    }
    // Decay above the middle (DECAY at its top there, process()): C21 (ENV2's capacitor) x1 ... x4, the tom to -40 dB
    // up to some 2.6 times the 909's longest (LT 0.6 -> 1.55 s, HT 0.4 -> 1.05 s). VR11 longer would hardly do: at
    // DECAY's top C21 empties mostly through R100 into C23 (x4: 1.3 times); C23 too stretches ENV2's rise as well
    const float dec = _ttDecay > 0.5f ? mod2(_ttDecay, 2.f) : 1.f;
    static float tt[3] = {0.5f, 0.5f, 1.f};
    if (_lt && (tt[0] != _ttBend || tt[1] != _ttNoise || tt[2] != dec))
    {
        tt[0] = _ttBend, tt[1] = _ttNoise, tt[2] = dec;
        const float bend = mod2(_ttBend, 2.f); // C24 (Q18's base): how long the bend at the hit lasts x0.25 ... x4
        const float n2 = _ttNoise * 2.f;       // R114: the tom noise into Q21's collector, 0 ... 4 times (+12 dB)
        const float noise = n2 * n2 > 1e-6f ? n2 * n2 : 1e-6f;
        auto tom = [&](va909::TTParams &p, const va909::TTParams &b, va909::Tom *t) {
            p = b;
            p.c24 *= bend;
            p.r114 /= noise;
            p.c21 *= dec;
            t->setParams(&p);
        };
        tom(_ltP, _ltB, _lt);
        tom(_mtP, _mtB, _mt);
        tom(_htP, _htB, _ht);
    }
    static float rs[3] = {0.5f, 0.5f, 0.5f};
    if (_rs && (rs[0] != _rsTune || rs[1] != _rsDec || rs[2] != _rsRing))
    {
        rs[0] = _rsTune, rs[1] = _rsDec, rs[2] = _rsRing;
        _rsP = _rsB;
        const float f = mod2(_rsTune, 1.f); // C113 ... C118: the three bridged Ts' frequencies together, +-1 octave
        _rsP.c1 /= f, _rsP.c2 /= f, _rsP.c3 /= f;
        _rsP.c119 *= mod2(_rsDec, 1.5f);    // C119 (ENV, Q65's base): the decay x0.35 ... x2.8
        const float q = mod2(_rsRing, 1.5f); // R407 / R414 / R416 up, R394 / R411 / R404 down: the Ts' Q x0.35 ... x2.8
        _rsP.rb1 *= q, _rsP.rb2 *= q, _rsP.rb3 *= q;
        _rsP.rs1 /= q, _rsP.rs2 /= q, _rsP.rs3 /= q;
        _rs->setParams(&_rsP, false);
    }
    static float cp[4] = {0.5f, 0.5f, 0.5f, 0.5f};
    if (_cp && (cp[0] != _cpLen || cp[1] != _cpRate || cp[2] != _cpTail || cp[3] != _cpColor))
    {
        cp[0] = _cpLen, cp[1] = _cpRate, cp[2] = _cpTail, cp[3] = _cpColor;
        _cpP = _cpB;
        _cpP.c62 *= mod2(_cpLen, 1.5f);    // C62: how long the bursts go on x0.35 ... x2.8
        _cpP.c65 /= mod2(_cpRate, 1.f);    // C65: the bursts' sawtooth, their rate x0.5 ... x2
        const float t = mod2(_cpTail, 1.5f); // C59 / C61 (ENV2, Q37's base): the tail x0.35 ... x2.8
        _cpP.c59 *= t, _cpP.c61 *= t;
        _cpP.c42 /= mod2(_cpColor, 1.f);   // C42 / C43: IC26b's band pass on the noise, +-1 octave
        _cp->setParams(&_cpP, false);
    }
    // the hat's ROM clock (IC72c/d's R493, R492, C160: fixed on the 909) as a TUNE as the cymbals have, +-1/2 octave;
    // the voice reads it at each block (from the copy once it moved)
    static bool hhCopy = false;
    if (ROM_VOICES && _hh && (hhCopy || _hhTune != 0.5f))
    {
        _hhP.fclk = va909::hhFit.fclk * mod2(_hhTune, 0.5f);
        if (!hhCopy)
            _hh->setParams(&_hhP);
        hhCopy = true;
    }
    // the cymbals' envelopes over the ROM's address: at 1 as fitted, lower ends them sooner (at 0.2 of it at 0)
    if (ROM_VOICES && _cr)
        _cr->setDecay(0.2f + 0.8f * _crDec);
    if (ROM_VOICES && _rd)
        _rd->setDecay(0.2f + 0.8f * _rdDec);
}

// the output's soft limit: x - 4/27 x^3, 1 at 1.5 and held beyond
static inline float saturate(float x)
{
    if (x > 1.5f)
        return 1.f;
    if (x < -1.5f)
        return -1.f;
    return x - 0.148148148f * x * x * x;
}

// the ACCENT line's voltage for a hit of the TRIG i. Accented (engine::accent(): the ACCENT input high at the hit, or
// a MIDI note over velocity 100 in the frame, the firmware's rule for every engine): raised towards the DAC's top by
// the TOTAL ACCENT (the Accent knob while ACCENT is patched, else LEVEL_UNITY). Else trigLevel() sets it in a kit: 1
// for a trigger input (DEFAULT_TRIG_LEVEL), a normal hit; (velocity / 128)^2 for a MIDI note (the firmware averages it
// with the instrument's last one), a softer one. A single instrument's hit is a normal one
static float accent(int i, float total)
{
    if (engine::accent())
        return ACC_NORMAL + (ACC_MAX - ACC_NORMAL) * total;
    if (!MULTI || _single)
        return ACC_NORMAL;
    const float v = sqrtf(engine::trigLevel(i));
    return ACC_NORMAL * (v > 1.f ? 1.f : v);
}

// the ACCENT line as the hat and the cymbals get it: 0.3 of its excursion from the normal one (Acc-Amt). What their
// exponential converters (Q84, Q70, Q68) see of the line is not known (no recording with a known accent); the drums'
// 2.45 V of it made them +16 ... 22 dB louder and drove their op-amps to the swing (the crash a square for half a
// second at Total Accent 0.8). So: the hat +5 dB, the cymbals +8 dB at Total Accent 0.8 (+6 / +10 dB at 1, the op-amps
// short of their swing), a MIDI velocity of 64 3.4 / 5.4 dB softer (without it 12 / 19 dB)
static float accentRom(int i, float total)
{
    return ACC_NORMAL + (accent(i, total) - ACC_NORMAL) * _romAccent;
}

static void hit(int inst, float total)
{
    const int i = _local[inst];
    switch (inst)
    {
    case BD:
        _bd->trigger(accent(i, total));
        break;
    case SD:
        _sd->trigger(accent(i, total));
        break;
    case CP:
        _cp->trigger(accent(i, total));
        break;
    // one hat: CLOSED/OPEN switches its decay and ROM half; a closed hat cuts the open one (both: the open one)
    case CH:
        if (ROM_VOICES)
            _hhInst = CH, _hh->trigger(true, accentRom(i, total));
        break;
    case OH:
        if (ROM_VOICES)
            _hhInst = OH, _hh->trigger(false, accentRom(i, total));
        break;
    case LT:
        _lt->trigger(accent(i, total)), _tomNoise->trigger();
        break;
    case MT:
        _mt->trigger(accent(i, total)), _tomNoise->trigger();
        break;
    case HT:
        _ht->trigger(accent(i, total)), _tomNoise->trigger();
        break;
    case RS:
        _rs->trigger(accent(i, total));
        break;
    case CR:
        if (ROM_VOICES)
            _cr->trigger(accentRom(i, total));
        break;
    case RD:
        if (ROM_VOICES)
            _rd->trigger(accentRom(i, total));
        break;
    }
}

static bool asleep()
{
    return (!_bd || _bd->idle()) && (!_sd || _sd->idle()) && (!_cp || _cp->idle()) &&
           (!ROM_VOICES || ((!_hh || _hh->idle()) && (!_cr || _cr->idle()) && (!_rd || _rd->idle()))) &&
           (!_lt || (_lt->idle() && _mt->idle() && _ht->idle())) && (!_rs || _rs->idle());
}

void engine::process()
{
    if (!_ok)
    {
        memset(_outL, 0, sizeof(float) * FRAME_BUFFER_SIZE);
        if (MULTI && _outR)
            memset(_outR, 0, sizeof(float) * FRAME_BUFFER_SIZE);
        return;
    }

    if (_engine != E_KIT) // the kit has no mods
        applyMods();
    const EngineDef &def = _engines[_engine];
    const bool patched = engine::hasAccent();
    const float total = patched ? _accLevel : LEVEL_UNITY;
    const uint32_t trig = engine::trig();
    if (_single)
    {
        int sel = _engine == E_TOMS ? _ttSel : 0; // Tom as modulated
        CONSTRAIN(sel, 0, _count - 1);
        if (trig)
            hit(def.inst[sel], total);
    }
    else if (MULTI && trig)
    {
        // in the order of the 909's voices (the kit's of before: the same accents of the same frame)
        static const uint8_t order[] = {BD, SD, CP, CH, OH, LT, MT, HT, RS, CR, RD};
        for (uint8_t inst : order)
            if (has(inst) && (trig & (1 << _local[inst])))
                hit(inst, total);
    }
    if (_bd)
        _bd->setKnobs(_bdTune, _bdAttack, _bdDecay);
    if (_sd)
        _sd->setKnobs(_engine == E_SD ? sdTune909(_sdTune) : _sdTune, _sdTone, _sdSnappy);
    if (ROM_VOICES && _hh)
        _hh->setKnobs(_chDecay, _ohDecay * (1.f - (float)engine::getMidiCC(4) * (1.f / 127.f)));
    if (ROM_VOICES && _cr)
        _cr->setTune(_crTune);
    if (ROM_VOICES && _rd)
        _rd->setTune(_rdTune);
    if (_lt && _engine == E_TOMS)
    {
        const float d = _ttDecay < 0.5f ? _ttDecay * 2.f : 1.f; // DECAY's range in Decay's lower half
        _lt->setKnobs(_ttTune, d);
        _mt->setKnobs(_ttTune, d);
        _ht->setKnobs(_ttTune, d);
    }
    else if (_lt)
    {
        _lt->setKnobs(_ltTune, _ltDecay);
        _mt->setKnobs(_mtTune, _mtDecay);
        _ht->setKnobs(_htTune, _htDecay);
    }

    // all quiet (the voices asleep, the mix outs' high passes settled): silence without a pass over them
    if (asleep() && fabsf(_hpYL) < 1e-6f && fabsf(_hpYR) < 1e-6f)
    {
        _hpXL = _hpYL = _hpXR = _hpYR = 0.f;
        memset(_outL, 0, sizeof(float) * FRAME_BUFFER_SIZE);
        if (MULTI && _outR)
            memset(_outR, 0, sizeof(float) * FRAME_BUFFER_SIZE);
        return;
    }

    // Level (without the ACCENT input): its square, 1 at LEVEL_UNITY (+3.9 dB at the top)
    const float level = patched ? 1.f : (_accLevel * _accLevel) * (1.f / (LEVEL_UNITY * LEVEL_UNITY));
    const float g = OUT_GAIN * level;
    float mixL[FRAME_BUFFER_SIZE] = {}, mixR[FRAME_BUFFER_SIZE] = {};
    float v[FRAME_BUFFER_SIZE];
    // a voice in volts at its individual out, panned into the mix bus by the mix page (one channel: as the kit's
    // at LEVEL 128, PAN in the middle)
    auto pan = [&](int inst) {
        if (!MULTI || _single)
        {
            const float l = _monoGain * g;
            for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
                mixL[i] += v[i] * l;
            return;
        }
        const int c = _local[inst];
        const float l = engine::mixLevelL(c) * g, r = engine::mixLevelR(c) * g;
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            mixL[i] += v[i] * l;
            mixR[i] += v[i] * r;
        }
    };
    auto add = [&](int inst, auto *voice) {
        if (!voice || voice->idle())
            return;
        voice->process(v, FRAME_BUFFER_SIZE);
        pan(inst);
    };
    if (VA909_VOICES & 1)
        add(BD, _bd);
    if (VA909_VOICES & 2)
        add(SD, _sd);
    if (VA909_VOICES & 4)
        add(CP, _cp);
    if (ROM_VOICES && (VA909_VOICES & 8))
        add(_hhInst, _hh);
    if (ROM_VOICES && (VA909_VOICES & 16))
        add(CR, _cr);
    if (ROM_VOICES && (VA909_VOICES & 32))
        add(RD, _rd);
    if ((VA909_VOICES & 64) && _lt && !(_lt->idle() && _mt->idle() && _ht->idle()))
    {
        // the tom noise once for the three toms (into each one's Q21), only while one of them reads it (VCO-3's VCA
        // on: some 0.12 s of a hit)
        float tn[FRAME_BUFFER_SIZE];
        const bool noise = _lt->wantsNoise() || _mt->wantsNoise() || _ht->wantsNoise();
        if (noise)
            _tomNoise->process(tn, FRAME_BUFFER_SIZE);
        auto tom = [&](int inst, va909::Tom *t) {
            if (t->idle())
                return;
            t->process(v, FRAME_BUFFER_SIZE, noise ? tn : nullptr);
            pan(inst);
        };
        tom(LT, _lt);
        tom(MT, _mt);
        tom(HT, _ht);
    }
    if (VA909_VOICES & 128)
        add(RS, _rs);

    // the mix outs: C154 / C153's high passes, IC66a/b invert
    if (MULTI && _outR)
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            const float yl = _hpA * (_hpYL + mixL[i] - _hpXL);
            _hpXL = mixL[i];
            _hpYL = yl;
            const float yr = _hpA * (_hpYR + mixR[i] - _hpXR);
            _hpXR = mixR[i];
            _hpYR = yr;
            _outL[i] = saturate(-yl);
            _outR[i] = saturate(-yr);
        }
    else
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        {
            const float yl = _hpA * (_hpYL + mixL[i] - _hpXL);
            _hpXL = mixL[i];
            _hpYL = yl;
            _outL[i] = saturate(-yl);
        }
}

void engine::draw()
{
    // the TOTAL ACCENT while the ACCENT input is patched, else the level
    engine::setParamName(&_accLevel, engine::hasAccent() ? "Accent" : "Level");
}
