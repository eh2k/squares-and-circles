#include "stmlib/stmlib.h"
#include "machine.h"
#include "rings/dsp/strummer.h"

using namespace machine;

struct ResonatorEngine : public Engine
{
    uint8_t model;
    rings::Strummer strummer;
    rings::Part part;
    rings::Patch patch;
    rings::PerformanceState performance_state;
    float bufferOut[FRAME_BUFFER_SIZE];
    float bufferAux[FRAME_BUFFER_SIZE];

    float in[FRAME_BUFFER_SIZE];

    ResonatorEngine() : Engine(AUDIO_PROCESSOR)
    {
        model = 0;
        memset(&strummer, 0, sizeof(rings::Strummer));
        memset(&patch, 0, sizeof(rings::Patch));
        patch.structure = 0.5f;
        patch.brightness = 0.5f;
        patch.damping = 0.5f;
        patch.position = 0.5f;
        memset(&performance_state, 0, sizeof(rings::PerformanceState));
        strummer.Init(0.01f, 48000 / FRAME_BUFFER_SIZE);
        part.Init();
        part.set_model((rings::ResonatorModel)model);
        part.set_polyphony(rings::kMaxPolyphony);
        memset(in, 0, sizeof(in));
    }

    void Process(const ControlFrame &frame, float **out, float **aux) override
    {
        performance_state.strum = frame.trigger;
        performance_state.internal_strum = false;
        performance_state.internal_note = true;
        performance_state.internal_exciter = true;
        performance_state.tonic = 0.2f;
        performance_state.chord = 0;
        performance_state.note = frame.midi.key;

        float *input = frame.audio_in[0];

        performance_state.note += frame.cv_voltage * 12;

        strummer.Process(input, FRAME_BUFFER_SIZE, &performance_state);
        part.Process(performance_state, patch, input, bufferOut, bufferAux, FRAME_BUFFER_SIZE);

        *out = bufferOut;
        *aux = bufferAux;
    }

    void SetParams(const uint16_t *params) override
    {
        model = params[0];
        CONSTRAIN(model, rings::ResonatorModel::RESONATOR_MODEL_MODAL, rings::ResonatorModel::RESONATOR_MODEL_SYMPATHETIC_STRING_QUANTIZED);
        part.set_model((rings::ResonatorModel)model);

        patch.structure = (float)params[2] / UINT16_MAX;
        patch.brightness = (float)params[3] / UINT16_MAX;
        patch.damping = (float)params[4] / UINT16_MAX;
        patch.position = (float)params[5] / UINT16_MAX;
    }

    const char **GetParams(uint16_t *values) override
    {
        static const char *names[]{"", "", "Struc.", "Brighn.", "Damping", "Pos", nullptr};

        if (model == rings::ResonatorModel::RESONATOR_MODEL_MODAL)
            names[0] = ">  Modal";
        else if (model == rings::ResonatorModel::RESONATOR_MODEL_SYMPATHETIC_STRING)
            names[0] = ">  Sympathetic";
        else if (model == rings::ResonatorModel::RESONATOR_MODEL_STRING)
            names[0] = ">  String";
        else if (model == rings::ResonatorModel::RESONATOR_MODEL_FM_VOICE)
            names[0] = ">  FM";
        else
            names[0] = ">  String Quant.";

        values[0] = model;
        values[1] = 0;
        values[2] = patch.structure * UINT16_MAX;
        values[3] = patch.brightness * UINT16_MAX;
        values[4] = patch.damping * UINT16_MAX;
        values[5] = patch.position * UINT16_MAX;

        return names;
    }

    void OnEncoder(uint8_t param_index, int16_t inc, bool pressed) override
    {
        if (param_index == 0)
        {
            if (inc > 0)
                model += 1;
            else if (model > 0)
                model -= 1;

            SetParam(param_index, model);
        }
        else
        {
            Engine::OnEncoder(param_index, inc, pressed);
        }
    }
};

void init_rings()
{
    machine::add<ResonatorEngine>(M_OSC, "Resonator");
}

MACHINE_INIT(init_rings);