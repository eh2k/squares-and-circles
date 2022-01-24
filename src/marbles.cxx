#include "machine.h"
#include "stmlib/utils/random.h"
#include "marbles/random/random_sequence.h"
#include "marbles/random/output_channel.h"
#include "marbles/scale_recorder.h"

template <int channel>
struct MX : machine::ModulationSource
{
    marbles::RandomGenerator rg;
    marbles::RandomStream rs;
    marbles::RandomSequence seq;
    marbles::OutputChannel oc;
    marbles::ScaleRecorder sr;

    float value = 0;
    uint32_t last_trig = 0;

    MX()
    {
        rg.Init(0);
        rs.Init(&rg);
        seq.Init(&rs);
        oc.Init();
        sr.Init();
    }

    float process() override
    {
        if (machine::get_trigger(channel) != last_trig)
        {
            last_trig = machine::get_trigger(channel);

            sr.NewNote(machine::get_cv(channel));
            sr.AcceptNote();

            marbles::Scale s;
            sr.ExtractScale(&s);
            oc.LoadScale(0, s);
            float phase = 0;
            //oc.set_spread((attenuverter + 1) / 2);
            oc.Process(&seq, &phase, &value, 1, 0);
        }

        return value;
    }
};

void init_marbles()
{
    // static MX<0> ma1;
    // static MX<1> ma2;
    // static MX<2> ma3;
    // static MX<3> ma4;

    // machine::add_modulation_source("MX1", &ma1);
    // machine::add_modulation_source("MX2", &ma2);
    // machine::add_modulation_source("MX3", &ma3);
    // machine::add_modulation_source("MX4", &ma4);
}