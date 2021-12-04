
#ifndef TEST
#include "pgmspace.h"
#else
#define FLASHMEM
#endif

#include "tr707/IC34_TR707_SNDROM.h"
#include "tr707/IC35_TR707_SNDROM.h"
#include "ch_oh.hxx"
#include "sample.hxx"

struct TR707 : public SampleEngine<uint8_t>
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

    const sample_spec<uint8_t> _sounds[12] = {
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

    TR707(int sample_num = 0) : SampleEngine<uint8_t>(_sounds, sample_num, sizeof(_sounds)/sizeof(_sounds[0]))
    {
    }
};

struct TR707_CH_OH : public CHOH
{
    TR707 oh;
    TR707 ch;

    TR707_CH_OH() : oh(11), ch(11)
    {
        _oh = &oh;
        _ch = &ch;
    }
};

void init_samples_tr707()
{
    machine::add<TR707>(machine::DRUM, "TR707");
    machine::add<TR707_CH_OH>(machine::DRUM, "TR707-CH-OH");
}

MACHINE_INIT(init_samples_tr707);