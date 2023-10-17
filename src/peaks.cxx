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

template <class T, uint32_t engine_props>
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

        int P1 = 0;
        int P2 = 1;
        int P3 = 2;
        int P4 = 3;

        if (std::is_same<T, peaks::FmDrum>::value)
        {
            P2 = 3;
            P3 = 1;
            P4 = 2;
        }
        else if (std::is_same<T, peaks::SnareDrum>::value)
        {
            P2 = 2;
            P3 = 1;
            P4 = 3;
        }

        param[0].init(param1, &params_[P1], p1);
        param[1].init(param2, &params_[P2], p2);

        if (std::is_same<T, peaks::Lfo>::value)
        {
            param[1].init_presets(param2, (uint8_t *)&params_[P2], p2, 0, (peaks::LFO_SHAPE_LAST - 1));
            param[1].value_changed = [&]()
            {
                const char *shapes[] = {"Sine", "Triangle", "Square", "Steps", "Noise"};
                param[1].name = shapes[*param[1].value.u8p];
            };
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
};

template <>
void PeaksEngine<peaks::FmDrum, TRIGGER_INPUT | VOCT_INPUT>::process(const ControlFrame &frame, OutputFrame &of)
{
    auto bak = params_[0];
    int val = params_[0];
    val += (frame.qz_voltage(this->io, 0.f) * INT16_MAX / 3); // CV or Midi Pitch ?!
    val += (-2 * INT16_MAX / 3);
    CONSTRAIN(val, 0, UINT16_MAX);
    params_[0] = val;
    _processor.Configure(params_, peaks::CONTROL_MODE_FULL);
    params_[0] = bak;

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

template <>
void PeaksEngine<peaks::Lfo, TRIGGER_INPUT | OUT_EQ_VOLT_INT16>::process(const ControlFrame &frame, OutputFrame &of)
{
    _processor.Configure(params_, peaks::CONTROL_MODE_FULL);
    _processor.set_shape((peaks::LfoShape)*param[1].value.u8p);

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
    add<PeaksEngine<peaks::MultistageEnvelope, TRIGGER_INPUT | OUT_EQ_VOLT_INT16>>(CV, "Envelope", 0, INT16_MAX, INT16_MAX, INT16_MAX, "Attack", "Decay", "Sustain", "Release");
    add<PeaksEngine<peaks::Lfo, TRIGGER_INPUT | OUT_EQ_VOLT_INT16>>(CV, "LFO", 0, 0, INT16_MAX, 0, "Freq.", "Shape", "Param", "Phase");
}

void init_peaks_drums()
{
    add<PeaksEngine<peaks::FmDrum, TRIGGER_INPUT | VOCT_INPUT>>(DRUM, "FM-Drum", INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, "Freq.", "Noise", "FM", "Decay");
    add<PeaksEngine<peaks::BassDrum, TRIGGER_INPUT>>(DRUM, "808ish-BD", INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, "Pitch", "Punch", "Tone", "Decay");
    add<PeaksEngine<peaks::SnareDrum, TRIGGER_INPUT>>(DRUM, "808ish-SD", INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, "Pitch", "Snappy", "Tone", "Decay");
    // add<PeaksEngine<peaks::HighHat, TRIGGER_INPUT>>(DRUM, "808ish-HiHat", INT16_MAX, INT16_MAX, INT16_MAX, INT16_MAX, "Decay");
    add<Hihat808>(DRUM, "808ish-HiHat");
}