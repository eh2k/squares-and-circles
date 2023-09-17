
#include "machine.h"
#include "base/SampleEngine.hxx"
#include "base/HiHatsEngine.hxx"
#include <inttypes.h>

// #include "eproms/tr707/IC34_TR707_SNDROM.h"
// #include "eproms/tr707/IC35_TR707_SNDROM.h"

struct TR707 : public SampleEngine
{
    const uint8_t *IC34_TR707_SNDROM_bin = machine::flash_read("707_IC34");
    const uint8_t *IC35_TR707_SNDROM_bin = machine::flash_read("707_IC35");

    const uint8_t *BD0 = &IC34_TR707_SNDROM_bin[0x0000];
    const uint8_t *BD1 = &IC34_TR707_SNDROM_bin[0x0001];
    const uint8_t *SD0 = &IC34_TR707_SNDROM_bin[0x2000];
    const uint8_t *SD1 = &IC34_TR707_SNDROM_bin[0x2001];
    const uint8_t *RM = &IC35_TR707_SNDROM_bin[0x4000];
    const uint8_t *CB = &IC35_TR707_SNDROM_bin[0x4001];
    const uint8_t *CP = &IC35_TR707_SNDROM_bin[0x6000];
    const uint8_t *TMB = &IC35_TR707_SNDROM_bin[0x6001];

    const uint8_t *LT = &IC34_TR707_SNDROM_bin[0x4000];
    const uint8_t *MT = &IC34_TR707_SNDROM_bin[0x6000];
    const uint8_t *HT = &IC35_TR707_SNDROM_bin[0x0000];

    const uint8_t *HH = &IC35_TR707_SNDROM_bin[0x2000];

    const tsample_spec<uint8_t> _sounds[12] = {
        //{"> BD", BD0, 0x2000, 50000, 0},
        {"BD0", BD0, 0x1000, 25000, 1},
        {"BD1", BD1, 0x1000, 25000, 1},
        //{"> SD", SD0, 0x2000, 50000, 0},
        {"SD0", SD0, 0x1000, 25000, 1},
        {"SD1", SD1, 0x1000, 25000, 1},
        {"CP", CP, 0x1000, 25000, 1},
        {"TMB", TMB, 0x1000, 25000, 1},
        {"RM", RM, 0x1000, 25000, 1},
        {"CB", CB, 0x1000, 25000, 1},
        {"LT", LT, 0x2000, 12500, 0},
        {"MT", MT, 0x2000, 12500, 0},
        {"HT", HT, 0x2000, 12500, 0},
        {"HH", HH, 0x2000, 25000, 0},
    };

    TR707(int sample_num = 0) : SampleEngine(_sounds, sample_num, LEN_OF(_sounds))
    {
    }

    bool init() override
    {
        return IC34_TR707_SNDROM_bin && IC35_TR707_SNDROM_bin;
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

void init_tr707()
{
    machine::add<TR707>(machine::DRUM, "TR707");
    machine::add<TR707_CH_OH>(machine::DRUM, "TR707-HiHat");
}