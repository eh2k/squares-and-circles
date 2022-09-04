#include "base/SampleEngine.hxx"
#include <inttypes.h>
#ifndef PROGMEM
#include "pgmspace.h"
#endif

#include "eproms/dmx/KICK64.BIN.h"
#include "eproms/dmx/REGSNAR.BIN.h"
#include "eproms/dmx/FATSNARE.BIN.h"
#include "eproms/dmx/DXHATS.BIN.h"
#include "eproms/dmx/DXSHAKE.BIN.h"
#include "eproms/dmx/TAMBRIM.BIN.h"
#include "eproms/dmx/TIMBALE.BIN.h"
#include "eproms/dmx/DXTOM.BIN.h"

#include "eproms/dmx/COWBELL.BIN.h"
#include "eproms/dmx/CONGA.BIN.h"
#include "eproms/dmx/RIDE2A.BIN.h"
#include "eproms/dmx/CRASH.BIN.h"

#include "eproms/lm2/SNARE9.BIN.h"
#include "eproms/lm2/CRASH1.BIN.h"
#include "eproms/lm2/RIDE1.BIN.h"
#include "eproms/lm2/CONGA1.BIN.h"
#include "eproms/lm2/COWBELL1.BIN.h"
#include "eproms/lm2/HAT1.BIN.h"
#include "eproms/lm2/SHAKE.BIN.h"
#include "eproms/lm2/SIDE.BIN.h"
#include "eproms/lm2/STICK.BIN.h"
#include "eproms/lm2/CLAP.BIN.h"
#include "eproms/lm2/TAMB1.BIN.h"
#include "eproms/lm2/TOM6.BIN.h"

#include "eproms/linn9k/bass01.bin.h"
#include "eproms/linn9k/snar01.bin.h"
#include "eproms/linn9k/clap0125.bin.h"
//#include "eproms/linn9k/hat0124.bin.h"

#include "base/HiHatsEngine.hxx"

struct Am6070Engine : public SampleEngine
{
    static int constexpr SAMPLE_RATE = machine::SAMPLE_RATE / 24 * 14;

    const Am6070sample _sounds[27] = {
        {"> L9K-BD9K", bass01_bin, LEN_OF(bass01_bin), SAMPLE_RATE, 4},
        {"> L9K-SD9K", snar01_bin, LEN_OF(snar01_bin), SAMPLE_RATE, 4},
        {"> DMX-KICK", KICK64_BIN, LEN_OF(KICK64_BIN), SAMPLE_RATE, 4},
        {"> LM2-SNARE", SNARE9_BIN, LEN_OF(SNARE9_BIN), SAMPLE_RATE, 4},
        {"> DMX-SNARE", REGSNAR_BIN, LEN_OF(REGSNAR_BIN), SAMPLE_RATE, 6},
        {"> DMX-FATSD", FATSNARE_BIN, LEN_OF(FATSNARE_BIN), SAMPLE_RATE, 4},
        {"> L9K-CP9K", clap0125_bin, LEN_OF(clap0125_bin), SAMPLE_RATE, 3},
        {"> LM2-CLAP", CLAP_BIN, LEN_OF(CLAP_BIN), SAMPLE_RATE / 5 * 3, 2},
        {"> DMX-CLAP", DXSHAKE_BIN, LEN_OF(DXSHAKE_BIN) / 2, SAMPLE_RATE / 3 * 2, 3},
        {"> DMX-RIMSHOT", TAMBRIM_BIN, LEN_OF(TAMBRIM_BIN) / 2, SAMPLE_RATE / 3 * 2, 4},
        {"> LM2-SIDE", SIDE_BIN, LEN_OF(SIDE_BIN), SAMPLE_RATE, 4},
        {"> LM2-STICK", STICK_BIN, LEN_OF(STICK_BIN), SAMPLE_RATE, 4},
        {"> LM2-SHAKE", SHAKE_BIN, LEN_OF(SHAKE_BIN), SAMPLE_RATE, 4},
        {"> DMX-SHAKE", &DXSHAKE_BIN[LEN_OF(DXSHAKE_BIN) / 2], LEN_OF(DXSHAKE_BIN) / 2, SAMPLE_RATE, 4},
        {"> DMX-TIMB", TIMBALE_BIN, LEN_OF(TIMBALE_BIN), SAMPLE_RATE, 4},
        {"> LM2-TAMB", TAMB1_BIN, LEN_OF(TAMB1_BIN), SAMPLE_RATE, 4},
        {"> DMX-TAMB", &TAMBRIM_BIN[LEN_OF(DXSHAKE_BIN) / 2], LEN_OF(TAMBRIM_BIN) / 2, SAMPLE_RATE, 4},
        {"> LM2-CBELL", COWBELL1_BIN, LEN_OF(COWBELL1_BIN), SAMPLE_RATE, 4},
        {"> DMX-CBELL", COWBELL_BIN, LEN_OF(COWBELL_BIN), SAMPLE_RATE, 4},
        {"> LM2-CONGA", CONGA1_BIN, LEN_OF(CONGA1_BIN), SAMPLE_RATE, 4},
        {"> DMX-CONGA", CONGA_BIN, LEN_OF(CONGA_BIN), SAMPLE_RATE, 4},
        {"> LM2-TOM", TOM6_BIN, LEN_OF(TOM6_BIN), SAMPLE_RATE, 4},
        {"> DMX-TOM", DXTOM_BIN, LEN_OF(DXTOM_BIN), SAMPLE_RATE, 4},
        {"> LM2-HIHAT", HAT1_BIN, LEN_OF(HAT1_BIN), SAMPLE_RATE, 2},
        {"> LM2-RIDE", RIDE1_BIN, LEN_OF(RIDE1_BIN), SAMPLE_RATE / 4 * 3, 2},
        {"> DMX-HATS", DXHATS_BIN, LEN_OF(DXHATS_BIN), SAMPLE_RATE, 3},
        {"> DMX-RIDE", RIDE2A_BIN, LEN_OF(RIDE2A_BIN), SAMPLE_RATE / 3 * 2, 3},
        //{"> CRASH", CRASH1_BIN, LEN_OF(CRASH1_BIN), SAMPLE_RATE, 0},
        //{"> CRASH", CRASH_BIN, LEN_OF(CRASH_BIN), SAMPLE_RATE, 0},
    };

    Am6070Engine(int sample_num = 0) : SampleEngine(_sounds, sample_num, LEN_OF(_sounds))
    {
    }
};

struct HIHATS : public SampleEngine
{
    static int constexpr SAMPLE_RATE = machine::SAMPLE_RATE / 3 * 2;

    const Am6070sample _sounds[8] = {
        {"> DMX-HH", DXHATS_BIN, LEN_OF(DXHATS_BIN), SAMPLE_RATE, 3},
        {"> DMX-RD", RIDE2A_BIN, LEN_OF(RIDE2A_BIN), SAMPLE_RATE / 3 * 2, 3},
        {"> DMX-SH", &DXSHAKE_BIN[LEN_OF(DXSHAKE_BIN) / 2], LEN_OF(DXSHAKE_BIN) / 2, SAMPLE_RATE, 4},
        {"> DMX-TMB", &TAMBRIM_BIN[LEN_OF(DXSHAKE_BIN) / 2], LEN_OF(TAMBRIM_BIN) / 2, SAMPLE_RATE, 4},
        {"> LM-HH", HAT1_BIN, LEN_OF(HAT1_BIN), SAMPLE_RATE, 2},
        {"> LM-RD", RIDE1_BIN, LEN_OF(RIDE1_BIN), SAMPLE_RATE / 4 * 3, 2},
        {"> LM-SH", SHAKE_BIN, LEN_OF(SHAKE_BIN), SAMPLE_RATE, 4},
        {"> LM-TAMB", TAMB1_BIN, LEN_OF(TAMB1_BIN), SAMPLE_RATE, 4},
    };

    HIHATS(int sample_num = 0) : SampleEngine(_sounds, sample_num, LEN_OF(_sounds))
    {
    }
};

struct CH_OH : public HiHatsEngine
{
    HIHATS oh;
    HIHATS ch;

    CH_OH() : oh(0), ch(0)
    {
        _oh = &oh;
        _ch = &ch;

        param[0].init(">", &oh.selection, oh.selection, oh.param[1].min.i, oh.param[1].max.i);
        param[0].print_value = [&](char *tmp)
        {
            sprintf(tmp, oh._sounds[oh.selection].name);
        };
        param[0].value_changed = [&]()
        {
            ch.selection = oh.selection;
        };
    }
};

#include "eproms/tr707/IC34_TR707_SNDROM.h"
#include "eproms/tr707/IC35_TR707_SNDROM.h"

struct TR707 : public SampleEngine
{
    static constexpr const uint8_t *BD0 = &IC34_TR707_SNDROM_bin[0x0000];
    static constexpr const uint8_t *BD1 = &IC34_TR707_SNDROM_bin[0x0001];
    static constexpr const uint8_t *SD0 = &IC34_TR707_SNDROM_bin[0x2000];
    static constexpr const uint8_t *SD1 = &IC34_TR707_SNDROM_bin[0x2001];
    static constexpr const uint8_t *RM = &IC35_TR707_SNDROM_bin[0x4000];
    static constexpr const uint8_t *CB = &IC35_TR707_SNDROM_bin[0x4001];
    static constexpr const uint8_t *CP = &IC35_TR707_SNDROM_bin[0x6000];
    static constexpr const uint8_t *TMB = &IC35_TR707_SNDROM_bin[0x6001];

    static constexpr const uint8_t *LT = &IC34_TR707_SNDROM_bin[0x4000];
    static constexpr const uint8_t *MT = &IC34_TR707_SNDROM_bin[0x6000];
    static constexpr const uint8_t *HT = &IC35_TR707_SNDROM_bin[0x0000];

    static constexpr const uint8_t *HH = &IC35_TR707_SNDROM_bin[0x2000];

    const tsample_spec<uint8_t> _sounds[12] = {
        //{"> BD", BD0, 0x2000, 50000, 0},
        {"> BD0", BD0, 0x1000, 25000, 1},
        {"> BD1", BD1, 0x1000, 25000, 1},
        //{"> SD", SD0, 0x2000, 50000, 0},
        {"> SD0", SD0, 0x1000, 25000, 1},
        {"> SD1", SD1, 0x1000, 25000, 1},
        {"> CP", CP, 0x1000, 25000, 1},
        {"> TMB", TMB, 0x1000, 25000, 1},
        {"> RM", RM, 0x1000, 25000, 1},
        {"> CB", CB, 0x1000, 25000, 1},
        {"> LT", LT, 0x2000, 12500, 0},
        {"> MT", MT, 0x2000, 12500, 0},
        {"> HT", HT, 0x2000, 12500, 0},
        {"> HH", HH, 0x2000, 25000, 0},
    };

    TR707(int sample_num = 0) : SampleEngine(_sounds, sample_num, LEN_OF(_sounds))
    {
    }
};

struct TR707_CH_OH : public HiHatsEngine
{
    TR707 oh;
    TR707 ch;

    TR707_CH_OH() : oh(11), ch(11)
    {
        _oh = &oh;
        _ch = &ch;
    }
};

#include "eproms/tr909/hihats.h"
#include "eproms/tr909/ride.h"

// TODO: Filter LP-HP

struct TR909_OH : public SampleEngine
{
    const tsample_spec<uint8_t> _sound = {"", HiHats_bin, 24576, 32000, 0};

    TR909_OH() : SampleEngine(&_sound, 0, 1)
    {
    }
};

constexpr static int ch_start = 24576;
struct TR909_CH : public SampleEngine
{
    const tsample_spec<uint8_t> _sound = {"", &HiHats_bin[ch_start], HiHats_bin_len - ch_start, 32000, 0};

    TR909_CH() : SampleEngine(&_sound, 0, 1)
    {
    }
};

struct TR909_Ride : public SampleEngine
{
    const tsample_spec<uint8_t> _sound = {"", Ride_bin, 32768, 32000, 0};

    TR909_Ride() : SampleEngine(&_sound, 0, 1)
    {
    }
};

struct TR909_CH_OH : public HiHatsEngine
{
    TR909_OH oh;
    TR909_CH ch;

    TR909_CH_OH()
    {
        _oh = &oh;
        _ch = &ch;
    }
};

struct TR909_CR_OR : public HiHatsEngine
{
    TR909_Ride oh;
    TR909_Ride ch;

    TR909_CR_OR()
    {
        _oh = &oh;
        _ch = &ch;
    }
};

void init_sample_roms()
{
    machine::add<TR909_CH_OH>(machine::DRUM, "TR909-HiHat");
    machine::add<TR909_CR_OR>(machine::DRUM, "TR909-Ride");

    machine::add<TR707>(machine::DRUM, "TR707");
    machine::add<TR707_CH_OH>(machine::DRUM, "TR707-HiHat");

#ifndef PRIVATE
    machine::add<Am6070Engine>(machine::DRUM, "Vint.EPROMs");
    machine::add<CH_OH>(machine::DRUM, "Vint.HiHats");
#endif
}