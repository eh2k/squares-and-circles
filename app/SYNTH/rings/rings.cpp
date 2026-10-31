#include "../../squares-and-circles-api.h"

ENGINE_NAME("SYNTH/Rings");
ENGINE_ALIAS("SYNTH/Resonator"); // the patches of its name before the rename (the release's): restore()

#include "stmlib/dsp/units.cc"
#include "stmlib/utils/random.cc"

#include "rings/resources.cc"
#include "rings/dsp/string.cc"
#include "rings/dsp/resonator.cc"
#include "rings/dsp/fm_voice.cc"
#include "rings/dsp/part.cc"
#include "rings/dsp/string_synth_part.cc"

#include "rings/dsp/strummer.h"

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
    "StrQuant.\tduo",

    "Modal\tquadro",
    "Sympath.\tquadro",
    "String\tquadro",
    "FM\tquadro",
    "StrQuant.\tquadro",
};

static rings::Strummer strummer = {};
static rings::String *_strings = nullptr; // set_strings(): kNumStrings, the heap's

static rings::Part part;
static rings::Patch patch = {};
static rings::PerformanceState performance_state = {};

static float _pitch = 0;
static int32_t _chord = 0;
static int32_t _exciter_hold = 0; // frames left in which the input counts as patched

// The patches of SYNTH/Resonator (the release's name, 2026-04) are of the firmwares that played a V_OCT knob 2 octaves
// lower: the knob 2 octaves lower here, for the same pitch
void engine::restore(uint32_t id, const engine::patch_param *old, int n, engine::patch_param *params)
{
    params[0].value -= 2.f; // V_OCT
}

void engine::setup()
{
    patch.structure = 0.5f;
    patch.brightness = 0.5f;
    patch.damping = 0.5f;
    patch.position = 0.5f;
    strummer.Init(0.01f, SAMPLE_RATE / FRAME_BUFFER_SIZE);

    part.Init();
    part.set_model(rings::ResonatorModel::RESONATOR_MODEL_MODAL);

    // The strings of Sympath., String and StrQuant. (8 x 12 KB) from the heap, reserved here: the model changes in
    // process() (knob, modulation), which must not malloc() (an interrupt on the Teensys). The module's RAM is ~12 KB
    // without them (the DSM: the DTCM pool instead of the SDRAM).
    if (void *mem = malloc(sizeof(rings::String) * rings::kNumStrings))
    {
        _strings = (rings::String *)mem;
        for (int i = 0; i < rings::kNumStrings; i++)
            new (&_strings[i]) rings::String();
    }
    else
        gfx::message("OUT OF MEMORY");
    part.set_strings(_strings);

    engine::addParam(V_OCT, &_pitch); // pitch is summed with CV and quantized
    engine::addParam("@Model", &_model, 0, LEN_OF(_models) - 1, (const char**)_models);

    engine::addParam("Struc.", &patch.structure);
    engine::addParam("Bright", &patch.brightness);
    engine::addParam("Damping", &patch.damping);
    engine::addParam("Pos", &patch.position);
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto bufferOut = engine::outputBuffer<0>();
    auto bufferAux = engine::outputBuffer<1>();

    const auto model = (rings::ResonatorModel)(_model % 5);
    if (_strings == nullptr && model != rings::RESONATOR_MODEL_MODAL && model != rings::RESONATOR_MODEL_FM_VOICE)
    {
        // no memory for the strings (setup())
        memset(bufferOut, 0, sizeof(float) * FRAME_BUFFER_SIZE);
        memset(bufferAux, 0, sizeof(float) * FRAME_BUFFER_SIZE);
        return;
    }
    part.set_model(model);

    int32_t polyphony = 1;
    if (_model < 5)
        polyphony = 1;
    else if (_model < 10)
        polyphony = 2;
    else
        polyphony = 4;

    if (polyphony != part.polyphony())
        part.set_polyphony(polyphony);

    // Rings with IN patched: the input is the exciter (its low-pass absolute instead of relative to the note, no
    // internal pulse or pluck), and without TRIG patched its onsets strike. There are no jacks here: audio above
    // -50 dBFS (RMS of the frame) in the last 100 ms counts as IN patched, TRIG-INPUT as TRIG; a trigger in a
    // silence strikes the internal exciter.
    float energy = 0.0f;
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        energy += inputL[i] * inputL[i];
    if (energy > FRAME_BUFFER_SIZE * 0.00316f * 0.00316f)
        _exciter_hold = SAMPLE_RATE / FRAME_BUFFER_SIZE / 10;
    else if (_exciter_hold > 0)
        _exciter_hold--;
    const bool external_exciter = _exciter_hold > 0;

    performance_state.strum = engine::trig() > 0;
    // a trigger of this frame (TRIG-INPUT, a MIDI note) strikes as it is; the onsets only without one
    performance_state.internal_strum = external_exciter && !engine::hasTrig() && !performance_state.strum;
    performance_state.internal_note = true;
    performance_state.internal_exciter = !external_exciter;
    // As rings/cv_scaler.cc: the tonic is the FREQUENCY knob, for all voices and the string Sympath. tunes to it, the
    // note the V/OCT CV, which each voice keeps while it rings out. Here the V_OCT knob is the tonic (0 = C4, the
    // 0 V note); a modulated one stays at C2, so that a modulation (RND, TM) does not retune the voices ringing out.
    const float pitch = DEFAULT_NOTE + engine::cv() * 12;
    performance_state.tonic = DEFAULT_NOTE - 24;
    if (!engine::isParamModulated(&_pitch))
        performance_state.tonic = DEFAULT_NOTE + _pitch * 12;
    performance_state.note = pitch - performance_state.tonic;

    // StrQuant.: Struc. picks one of the 11 chords, with Rings' hysteresis
    const float chord = patch.structure * (rings::kNumChords - 1);
    const float hysteresis = chord - _chord > 0.0f ? -0.1f : +0.1f;
    _chord = static_cast<int32_t>(chord + hysteresis + 0.5f);
    CONSTRAIN(_chord, 0, rings::kNumChords - 1);
    performance_state.chord = _chord;

    // Rings keeps STRUCTURE below 1: at 1 Interpolate() reads past lut_stiffness and lut_fm_frequency_quantizer
    rings::Patch p = patch;
    CONSTRAIN(p.structure, 0.0f, 0.9995f);

    strummer.Process(inputL, FRAME_BUFFER_SIZE, &performance_state);
    part.Process(performance_state, p, inputL, bufferOut, bufferAux, FRAME_BUFFER_SIZE);
}