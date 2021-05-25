#include "stmlib/stmlib.h"
#include "machine.h"
#include "peaks/gate_processor.h"
#include "peaks/drums/bass_drum.h"
#include "peaks/drums/fm_drum.h"
#include "peaks/drums/snare_drum.h"
#include "peaks/drums/high_hat.h"
#include "peaks/modulations/bouncing_ball.h"
#include "peaks/modulations/lfo.h"
#include "peaks/modulations/mini_sequencer.h"
#include "peaks/modulations/multistage_envelope.h"
#include "peaks/number_station/number_station.h"
#include "peaks/pulse_processor/pulse_shaper.h"
#include "peaks/pulse_processor/pulse_randomizer.h"
#include "peaks/gate_processor.h"
#include "ch_oh.hxx"

template <class T, bool cv = false, int P1 = 0, int P2 = 1, int P3 = 2, int P4 = 3>
struct PeaksEngine : public machine::Engine
{
    T _processor;
    const char *param_names[5];
    uint16_t params_[4];

    peaks::GateFlags flags[machine::FRAME_BUFFER_SIZE];
    float buffer[machine::FRAME_BUFFER_SIZE];
    int16_t tmp[machine::FRAME_BUFFER_SIZE];

    PeaksEngine(uint16_t p1 = UINT16_MAX / 2,
                uint16_t p2 = UINT16_MAX / 2,
                uint16_t p3 = UINT16_MAX / 2,
                uint16_t p4 = UINT16_MAX / 2,
                const char *param1 = nullptr,
                const char *param2 = nullptr,
                const char *param3 = nullptr,
                const char *param4 = nullptr) : param_names{param1, param2, param3, param4, nullptr},
                                                params_{p1, p2, p3, p4}
    {
        memset(&_processor, 0, sizeof(T));
        _processor.Init();
        std::fill(&flags[0], &flags[machine::FRAME_BUFFER_SIZE], peaks::GATE_FLAG_LOW);
        SetParams(params_);
    }

    void Process(const machine::ControlFrame &frame, float **out, float **aux) override
    {
        if (frame.trigger)
        {
            flags[0] = peaks::GATE_FLAG_RISING;
            flags[1] = peaks::GATE_FLAG_FALLING;
        }
        else
        {
            flags[0] = peaks::GATE_FLAG_LOW;
            flags[1] = peaks::GATE_FLAG_LOW;
        }

        _processor.Process(flags, tmp, machine::FRAME_BUFFER_SIZE);

        for (int i = 0; i < machine::FRAME_BUFFER_SIZE; i++)
            buffer[i] = ((float)tmp[i]) / INT16_MAX;

        *out = buffer;
    }

    void SetParams(const uint16_t *params) override
    {
        params_[P1] = params[0];
        params_[P2] = params[1];
        params_[P3] = params[2];
        params_[P4] = params[3];
        _processor.Configure(params_, peaks::CONTROL_MODE_FULL);
    }

    const char **GetParams(uint16_t *values) override
    {
        values[0] = params_[P1];
        values[1] = params_[P2];
        if (std::is_same<T, peaks::Lfo>::value)
        {
            auto shape = static_cast<peaks::LfoShape>(values[1] * peaks::LFO_SHAPE_LAST >> 16);
            switch (shape)
            {
            case peaks::LFO_SHAPE_SINE:
                param_names[1] = "Sine";
                break;
            case peaks::LFO_SHAPE_TRIANGLE:
                param_names[1] = "Triangle";
                break;
            case peaks::LFO_SHAPE_SQUARE:
                param_names[1] = "Square";
                break;
            case peaks::LFO_SHAPE_STEPS:
                param_names[1] = "Steps";
                break;
            case peaks::LFO_SHAPE_NOISE:
                param_names[1] = "Noise";
                break;
            default:
                param_names[1] = "Shape";
                break;
            }
        }

        values[2] = params_[P3];
        values[3] = params_[P4];

        return param_names;
    }
};

class Hihat808 : public CHOH
{
    PeaksEngine<peaks::HighHat> oh;
    PeaksEngine<peaks::HighHat> ch;

public:
    Hihat808() : oh(UINT16_MAX), ch(UINT16_MAX)
    {
        _ch = &ch;
        _oh = &oh;
    }
};

void init_peaks()
{
    machine::add<PeaksEngine<peaks::BassDrum>>(machine::DRUM, "808ish-BD", INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, "Pitch", "Punch", "Tone", "Decay");
    machine::add<PeaksEngine<peaks::SnareDrum, false, 0, 2, 1, 3>>(machine::DRUM, "808ish-SD", INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, "Pitch", "Snappy", "Tone", "Decay");

    machine::add<Hihat808>(machine::DRUM, "808ish-CH-OH");

    machine::add<PeaksEngine<peaks::HighHat, false>>(machine::DRUM, "808ish-HiHat", INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, "Decay");
    machine::add<PeaksEngine<peaks::FmDrum, false, 0, 3, 1, 2>>(machine::DRUM, "FM-Drum", INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, "Freq.", "Noise", "FM", "Decay");
    machine::add<PeaksEngine<peaks::MultistageEnvelope, true>>(machine::CV, "Envelope", 0, INT16_MAX, 0, 0, "Attack", "Decay", "Sustain", "Release");
    machine::add<PeaksEngine<peaks::Lfo, true>>(machine::CV, "LFO", 0, INT16_MAX, 0, 0, "Freq.", "Shape", "Param", "Phase");
}

MACHINE_INIT(init_peaks);