#include "machine.h"
#include "base/SampleEngine.hxx"
#include "base/HiHatsEngine.hxx"

// #include "eproms/tr909/hihats.h"
// #include "eproms/tr909/ride.h"

// TODO: Filter LP-HP

struct TR909_OH : public SampleEngine
{
    const uint8_t *HiHats_bin = machine::flash_read("909_HIGH");
    const tsample_spec<uint8_t> _sound = {"", HiHats_bin, 24576, 32000, 0};

    TR909_OH() : SampleEngine(&_sound, 0, 1)
    {
    }

    bool init() override
    {
        return HiHats_bin;
    }
};

constexpr static int ch_start = 24576;
struct TR909_CH : public SampleEngine
{
    const uint8_t *HiHats_bin = machine::flash_read("909_HIGH");
    const tsample_spec<uint8_t> _sound = {"", HiHats_bin + ch_start, 32768 - ch_start, 32000, 0};

    TR909_CH() : SampleEngine(&_sound, 0, 1)
    {
    }

    bool init() override
    {
        return HiHats_bin;
    }
};

struct TR909_Ride : public SampleEngine
{
    const uint8_t *Ride_bin = machine::flash_read("909_RIDE");
    const tsample_spec<uint8_t> _sound = {"", Ride_bin, 32768, 32000, 0};

    TR909_Ride() : SampleEngine(&_sound, 0, 1)
    {
    }

    bool init() override
    {
        return Ride_bin;
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

void init_tr909()
{
    machine::add<TR909_CH_OH>(machine::DRUM, "TR909-HiHat");
    machine::add<TR909_CR_OR>(machine::DRUM, "TR909-Ride");
}