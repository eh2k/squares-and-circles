#include "stmlib/stmlib.h"
#include "machine.h"
#include "peaks/gate_processor.h"
#include "peaks/modulations/bouncing_ball.h"
#include "peaks/modulations/mini_sequencer.h"
#include "peaks/modulations/multistage_envelope.h"
#include "peaks/number_station/number_station.h"
#include "peaks/pulse_processor/pulse_shaper.h"
#include "peaks/pulse_processor/pulse_randomizer.h"
#include "peaks/gate_processor.h"

namespace gfx
{
    void drawEngineWithScope(machine::Engine *engine, int8_t scope[128], int i, int y);
    void push_scope(int8_t scope[128], uint8_t &i, int8_t x);
}

using namespace machine;

template <class T, uint32_t engine_props>
struct PeaksCVEngine : public Engine
{
    int8_t scope[128] = {};
    float y = 0;
    uint8_t i = 0;

    T _processor;

    uint16_t params_[4];

    peaks::GateFlags flags[FRAME_BUFFER_SIZE];
    int16_t buffer[FRAME_BUFFER_SIZE];

    PeaksCVEngine(uint16_t p1 = UINT16_MAX / 2,
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

        param[0].init(param1, &params_[P1], p1);
        param[1].init(param2, &params_[P2], p2);

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

        if ((frame.t % 50) == 0)
        {
            gfx::push_scope(scope, i, y);
            y = 0;
        }
        else
            y = std::max(y, (float)buffer[0] / INT16_MAX * 16);
    }

    void display() override
    {
        gfx::drawEngineWithScope(this, scope, i, 56);
    }
};

void init_cv_peaks()
{
    add<PeaksCVEngine<peaks::MultistageEnvelope, TRIGGER_INPUT | OUT_EQ_VOLT_INT16>>(CV, "EnvGen_ADSR", 0, INT16_MAX, INT16_MAX, INT16_MAX, "Attack", "Decay", "Sustain", "Release");
}