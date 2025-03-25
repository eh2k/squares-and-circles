#include "../squares-and-circles-api.h"

#include "stmlib/stmlib.h"
#include "stmlib/dsp/units.cc"
#include "stmlib/utils/random.cc"

#include "rings/resources.cc"
#include "rings/dsp/string.cc"
#include "rings/dsp/resonator.cc"
#include "rings/dsp/fm_voice.cc"
#include "rings/dsp/part.cc"
#include "rings/dsp/string_synth_part.cc"

#include "rings/dsp/strummer.h"
#include <stdio.h>

static int32_t _model = rings::ResonatorModel::RESONATOR_MODEL_MODAL;
static const char* _models[] = {
    "Modal\tmono",
    "Sympath.\tmono",
    "String\tmono",
    "FM\tmono",
    "StrQuant.\tmono",

    "Modal\tduo",
    "Sympath.\tduo",
    "String\tduo",
    "FM\tduo",
    "StrQuant\tduo",

    "Modal\tquadro",
    "Sympath.\tquadro",
    "String\tquadro",
    "FM\tquadro",
    "StrQuant.\tquadro",
};

static rings::Strummer strummer = {};

static rings::Part part;
static rings::Patch patch = {};
static rings::PerformanceState performance_state = {};

static float _pitch = 0;

void engine::setup()
{
    patch.structure = 0.5f;
    patch.brightness = 0.5f;
    patch.damping = 0.5f;
    patch.position = 0.5f;
    strummer.Init(0.01f, SAMPLE_RATE / FRAME_BUFFER_SIZE);

    part.Init();
    part.set_model(rings::ResonatorModel::RESONATOR_MODEL_MODAL);
    part.set_polyphony(3);

    engine::addParam(V_OCT, &_pitch); // pitch is summed with CV and quantized
    engine::addParam("@Model", &_model, 0, LEN_OF(_models) - 1, (const char**)_models);

    engine::addParam("Struc.", &patch.structure);
    engine::addParam("Brighn.", &patch.brightness);
    engine::addParam("Damping", &patch.damping);
    engine::addParam("Pos", &patch.position);
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto bufferOut = engine::outputBuffer<0>();
    auto bufferAux = engine::outputBuffer<1>();

    part.set_model((rings::ResonatorModel)(_model % 5));

    int32_t polyphony = 1;
    if (_model < 5)
        polyphony = 1;
    else if (_model < 10)
        polyphony = 2;
    else
        polyphony = 4;

    if (polyphony != part.polyphony())
        part.set_polyphony(polyphony);

    performance_state.strum = engine::trig() > 0;
    performance_state.internal_strum = false;
    performance_state.internal_note = true;
    performance_state.internal_exciter = true;
    performance_state.tonic = 0.2f;
    performance_state.chord = 0;
    performance_state.note = DEFAULT_NOTE;

    performance_state.note += engine::cv() * 12;

    strummer.Process(inputL, FRAME_BUFFER_SIZE, &performance_state);
    part.Process(performance_state, patch, inputL, bufferOut, bufferAux, FRAME_BUFFER_SIZE);
}