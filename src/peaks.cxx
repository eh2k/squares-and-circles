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
#include "base/HiHatsEngine.hxx"

using namespace machine;

template <class T, uint32_t engine_props, int P1 = 0, int P2 = 1, int P3 = 2, int P4 = 3>
struct PeaksEngine : public Engine
{
    T _processor;

    uint16_t params_[4];

    peaks::GateFlags flags[FRAME_BUFFER_SIZE];
    int16_t buffer[FRAME_BUFFER_SIZE];

    PeaksEngine(uint16_t p1 = UINT16_MAX / 2,
                uint16_t p2 = UINT16_MAX / 2,
                uint16_t p3 = UINT16_MAX / 2,
                uint16_t p4 = UINT16_MAX / 2,
                const char *param1 = nullptr,
                const char *param2 = nullptr,
                const char *param3 = nullptr,
                const char *param4 = nullptr) : Engine(engine_props)
    {
        memset(&_processor, 0, sizeof(T));
        _processor.Init();
        std::fill(&flags[0], &flags[FRAME_BUFFER_SIZE], peaks::GATE_FLAG_LOW);

        param[0].init(param1, &params_[P1], p1);
        param[1].init(param2, &params_[P2], p2);

        if (std::is_same<T, peaks::Lfo>::value)
        {
            param[1].step.i = UINT16_MAX / (peaks::LFO_SHAPE_LAST - 1);
            param[1].step2 = param[1].step;
        }

        param[2].init(param3, &params_[P3], p3);
        param[3].init(param4, &params_[P4], p4);
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        _processor.Configure(params_, peaks::CONTROL_MODE_FULL);

        if (frame.trigger)
        {
            flags[0] = peaks::GATE_FLAG_RISING;
            std::fill(&flags[1], &flags[FRAME_BUFFER_SIZE], peaks::GATE_FLAG_HIGH);
        }
        else if (frame.gate)
        {
            std::fill(&flags[0], &flags[FRAME_BUFFER_SIZE], peaks::GATE_FLAG_HIGH);
        }
        else
        {
            flags[0] = flags[0] == peaks::GATE_FLAG_HIGH ? peaks::GATE_FLAG_FALLING : peaks::GATE_FLAG_LOW;
            std::fill(&flags[1], &flags[FRAME_BUFFER_SIZE], peaks::GATE_FLAG_LOW);
        }

        _processor.Process(flags, buffer, FRAME_BUFFER_SIZE);

        of.push(buffer, LEN_OF(buffer));
    }

    void onDisplay(uint8_t *buffer) override
    {
        if (std::is_same<T, peaks::Lfo>::value)
        {
            auto shape = static_cast<peaks::LfoShape>(params_[P2] * peaks::LFO_SHAPE_LAST >> 16);
            switch (shape)
            {
            case peaks::LFO_SHAPE_SINE:
                param[1].name = "Sine";
                break;
            case peaks::LFO_SHAPE_TRIANGLE:
                param[1].name = "Triangle";
                break;
            case peaks::LFO_SHAPE_SQUARE:
                param[1].name = "Square";
                break;
            case peaks::LFO_SHAPE_STEPS:
                param[1].name = "Steps";
                break;
            case peaks::LFO_SHAPE_NOISE:
                param[1].name = "Noise";
                break;
            default:
                param[1].name = "Shape";
                break;
            }
        }

        gfx::drawEngine(buffer, this);
    }
};

class Hihat808 : public HiHatsEngine
{
    PeaksEngine<peaks::HighHat, TRIGGER_INPUT> oh;
    PeaksEngine<peaks::HighHat, TRIGGER_INPUT> ch;

public:
    Hihat808() : oh(UINT16_MAX), ch(UINT16_MAX)
    {
        _ch = &ch;
        _oh = &oh;
    }
};

void init_peaks()
{
    add<PeaksEngine<peaks::FmDrum, TRIGGER_INPUT, 0, 3, 1, 2>>(DRUM, "FM-Drum", INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, "Freq.", "Noise", "FM", "Decay");

    add<PeaksEngine<peaks::BassDrum, TRIGGER_INPUT>>(DRUM, "808ish-BD", INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, "Pitch", "Punch", "Tone", "Decay");
    add<PeaksEngine<peaks::SnareDrum, TRIGGER_INPUT, 0, 2, 1, 3>>(DRUM, "808ish-SD", INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, "Pitch", "Snappy", "Tone", "Decay");

    add<Hihat808>(DRUM, "808ish-HiHat");

    // add<PeaksEngine<peaks::HighHat, TRIGGER_INPUT>>(DRUM, "808ish-HiHat", INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, "Decay");
    add<PeaksEngine<peaks::MultistageEnvelope, TRIGGER_INPUT | OUT_EQ_VOLT_INT16>>(CV, "Envelope", 0, INT16_MAX, INT16_MAX, INT16_MAX, "Attack", "Decay", "Sustain", "Release");
    add<PeaksEngine<peaks::Lfo, TRIGGER_INPUT | OUT_EQ_VOLT_INT16>>(CV, "LFO", 0, 0, INT16_MAX, 0, "Freq.", "Shape", "Param", "Phase");
}

MACHINE_INIT(init_peaks);