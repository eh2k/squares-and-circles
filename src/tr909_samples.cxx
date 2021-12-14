
#include <inttypes.h>
#ifndef PROGMEM
#include "pgmspace.h"
#endif

#include "tr909/hihats.h"
#include "tr909/ride.h"
#include "ch_oh.hxx"
#include "sample.hxx"

// TODO: Filter LP-HP

struct TR909_OH : public SampleEngine<uint8_t>
{
    const sample_spec<uint8_t> _sound = {"", HiHats_bin, 24576, 32000, 0};

    TR909_OH() : SampleEngine<uint8_t>(&_sound, 0, 1)
    {
    }
};

constexpr static int ch_start = 24576;
struct TR909_CH : public SampleEngine<uint8_t>
{
    const sample_spec<uint8_t> _sound = {"", &HiHats_bin[ch_start], HiHats_bin_len - ch_start, 32000, 0};

    TR909_CH() : SampleEngine<uint8_t>(&_sound, 0, 1)
    {
    }
};

struct TR909_Ride : public SampleEngine<uint8_t>
{
    const sample_spec<uint8_t> _sound = {"", Ride_bin, 32768, 32000, 0};

    TR909_Ride() : SampleEngine<uint8_t>(&_sound, 0, 1)
    {
    }
};

struct TR909_CH_OH : public CHOH
{
    TR909_OH oh;
    TR909_CH ch;

    TR909_CH_OH()
    {
        _oh = &oh;
        _ch = &ch;
    }
};

void init_samples_tr909()
{
    machine::add<TR909_CH_OH>(machine::DRUM, "TR909-CH-OH");
    machine::add<TR909_OH>(machine::DRUM, "TR909-OH");
    machine::add<TR909_Ride>(machine::DRUM, "TR909-Ride");
}

MACHINE_INIT(init_samples_tr909);