#include "stmlib/stmlib.h"
#include "machine.h"
#include "rings/dsp/strummer.h"

using namespace machine;

struct ResonatorEngine : public Engine
{
    uint8_t _model;
    rings::Strummer strummer;
    rings::Part *part;
    rings::Patch patch;
    rings::PerformanceState performance_state;
    float bufferOut[FRAME_BUFFER_SIZE];
    float bufferAux[FRAME_BUFFER_SIZE];

    float in[FRAME_BUFFER_SIZE];

    float _pitch;

    ResonatorEngine() : Engine(TRIGGER_INPUT | VOCT_INPUT | AUDIO_PROCESSOR)
    {
        memset(&strummer, 0, sizeof(rings::Strummer));
        memset(&patch, 0, sizeof(rings::Patch));
        patch.structure = 0.5f;
        patch.brightness = 0.5f;
        patch.damping = 0.5f;
        patch.position = 0.5f;
        memset(&performance_state, 0, sizeof(rings::PerformanceState));
        strummer.Init(0.01f, SAMPLE_RATE / FRAME_BUFFER_SIZE);
        if (void *mem = machine::malloc(sizeof(rings::Part)))
        {
            part = new (mem) rings::Part();
            part->Init();
            part->set_model(rings::ResonatorModel::RESONATOR_MODEL_MODAL);
            part->set_polyphony(rings::kMaxPolyphony);
        }
        memset(in, 0, sizeof(in));

        param[0].init_v_oct("Freq", &_pitch);
        param[1].init_presets("Model", &_model, rings::ResonatorModel::RESONATOR_MODEL_MODAL,
                      rings::ResonatorModel::RESONATOR_MODEL_MODAL,
                      rings::ResonatorModel::RESONATOR_MODEL_SYMPATHETIC_STRING_QUANTIZED);
        param[2].init("Struc.", &patch.structure);
        param[3].init("Brighn.", &patch.brightness);
        param[4].init("Damping", &patch.damping);
        param[5].init("Pos", &patch.position);
    }

    ~ResonatorEngine() override
    {
        machine::mfree(part);
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        if (part == nullptr)
            return;

        part->set_model((rings::ResonatorModel)_model);

        performance_state.strum = frame.trigger;
        performance_state.internal_strum = false;
        performance_state.internal_note = true;
        performance_state.internal_exciter = true;
        performance_state.tonic = 0.2f;
        performance_state.chord = 0;
        performance_state.note = machine::DEFAULT_NOTE;

        float *input = machine::get_aux(AUX_L);

        performance_state.note += frame.qz_voltage(this->io, _pitch) * 12;

        strummer.Process(input, FRAME_BUFFER_SIZE, &performance_state);
        part->Process(performance_state, patch, input, bufferOut, bufferAux, FRAME_BUFFER_SIZE);

        of.out = bufferOut;
        of.aux = bufferAux;
    }

    void display() override
    {
        if (_model == rings::ResonatorModel::RESONATOR_MODEL_MODAL)
            param[1].name = "@Modal";
        else if (_model == rings::ResonatorModel::RESONATOR_MODEL_SYMPATHETIC_STRING)
            param[1].name = "@Sympath.";
        else if (_model == rings::ResonatorModel::RESONATOR_MODEL_STRING)
            param[1].name = "@String";
        else if (_model == rings::ResonatorModel::RESONATOR_MODEL_FM_VOICE)
            param[1].name = "@FM";
        else
            param[1].name = "@StrQuant.";

        gfx::drawEngine(this, part ? nullptr : machine::OUT_OF_MEMORY);
    }
};

void init_rings()
{
    machine::add<ResonatorEngine>(M_OSC, "Resonator");
}

MACHINE_INIT(init_rings);