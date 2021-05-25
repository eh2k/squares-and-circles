
#ifndef TEST
#include "pgmspace.h"
#else
#define FLASHMEM
#endif

#include "tr909/hihats.h"
#include "tr909/ride.h"
#include "ch_oh.hxx"
#include "sample.hxx"

//TODO: Filter LP-HP

struct TR909_OH : public Sample<uint8_t, 24576, 32000>
{
    TR909_OH() : Sample<uint8_t, 24576, 32000>(HiHats_bin)
    {
    }
};

constexpr static int ch_start = 24576;
struct TR909_CH : public Sample<uint8_t, HiHats_bin_len - ch_start, 32000>
{
    TR909_CH() : Sample<uint8_t, HiHats_bin_len - ch_start, 32000>(&HiHats_bin[ch_start])
    {
    }
};

struct TR909_Ride : public Sample<uint8_t, 32768, 32000>
{
    TR909_Ride() : Sample<uint8_t, 32768, 32000>(Ride_bin)
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

void init_samples()
{
    machine::add<TR909_CH_OH>(machine::DRUM, "909-CH-OH");
    machine::add<TR909_OH>(machine::DRUM, "909-OH");
    machine::add<TR909_Ride>(machine::DRUM, "909-Ride");
}

MACHINE_INIT(init_samples);