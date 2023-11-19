#include "stmlib/stmlib.h"
#include "machine.h"
#define private public // ;-)
#include "plaits/dsp/voice.h"
#include "stmlib/dsp/dsp.h"

using namespace machine;

struct InitArgs
{
    int engine;
    int output; // output=0 -> out, output=1 -> aux, output=3 -> stereo
};

struct PlaitsEngine : public Engine
{
    plaits::Modulations modulations;
    plaits::Voice voice;
    plaits::Patch patch;

    float bufferOut[machine::FRAME_BUFFER_SIZE];
    float bufferAux[machine::FRAME_BUFFER_SIZE];
    float out_aux_mix = 0.5f;
    float _pitch = 0;
    float _base_pitch = machine::DEFAULT_NOTE;

    const uint8_t engine = 0;
    const uint8_t output = 0;

    uint8_t *_buffer = nullptr;
    plaits::Engine *_plaitsEngine = nullptr;

    ~PlaitsEngine() override
    {
        machine::mfree(_plaitsEngine);
        machine::mfree(_buffer);
    }

    template <class T>
    void alloc_engine(size_t mem = 48)
    {
        _plaitsEngine = new (machine::malloc(sizeof(T))) T();
        _buffer = (uint8_t *)machine::malloc(mem * sizeof(float));

        stmlib::BufferAllocator allocator;
        allocator.Init(_buffer, mem * sizeof(float));
        _plaitsEngine->Init(&allocator);
    }

    float harmonics, timbre, morph;

    void init_params(const char *h, float hh, const char *t, float tt, const char *m, float mm, const plaits::PostProcessingSettings &settings)
    {
        param[1].init(h, &harmonics, hh);
        param[2].init(t, &timbre, tt);
        param[3].init(m, &morph, mm);
        patch.harmonics = harmonics;
        patch.timbre = timbre;
        patch.morph = morph;
        if (_plaitsEngine)
            _plaitsEngine->post_processing_settings = settings;
    }

    PlaitsEngine(const InitArgs &args) : Engine(TRIGGER_INPUT | VOCT_INPUT), engine(args.engine), output(args.output)
    {
        voice.Init();
        patch.engine = 0;
        memset(&patch, 0, sizeof(patch));
        patch.note = machine::DEFAULT_NOTE + _pitch * 12.f;

        modulations.timbre_patched = true;
        patch.timbre_modulation_amount = 0;

        modulations.frequency_patched = true;
        patch.frequency_modulation_amount = 0;

        modulations.morph_patched = true;
        patch.morph_modulation_amount = 0;

        memset(&modulations, 0, sizeof(modulations));

        patch.lpg_colour = 0.5;
        patch.decay = 0.5;
        modulations.trigger_patched = true; // trigger;
                                            // modulations.level_patched = true; //trigger;
                                            // modulations.level = 1;

        param[0].init_v_oct("Pitch", &_pitch);

        switch (engine)
        {
        case 0:
            alloc_engine<plaits::VirtualAnalogEngine>();
            init_params("Detune", 0.5f, "Square", 0.5f, "CSAW", 0.5f, {0.8f, 0.8f, false});
            break;
        case 1:
            alloc_engine<plaits::WaveshapingEngine>();
            init_params("Waveform", 0.8f, "Fold", 0.8f, "Asym.", 0.75f, {0.7f, 0.6f, false});
            break;
        case 2:
            alloc_engine<plaits::FMEngine>();
            init_params("Ratio", 0.8f, "Mod", 0.8f, "Feedb.", 0.75f, {0.6f, 0.6f, false});
            break;
        case 3:
            alloc_engine<plaits::GrainEngine>();
            init_params("Ratio", 0.8f, "Frm/Fq.", 0.8f, "Width", 0.75f, {0.7f, 0.6f, false});
            param[5].init("PD-Mix", &out_aux_mix, out_aux_mix);
            break;
        case 4:
            alloc_engine<plaits::AdditiveEngine>();
            init_params("Bump", 0.8f, "Peak", 0.8f, "Shape", 0.75f, {0.8f, 0.8f, false});
            break;
        case 5:
            alloc_engine<plaits::WavetableEngine>(64 * sizeof(const int16_t *));
            init_params("Bank", 0.f, "Row", 0.8f, "Column", 0.75f, {0.6f, 0.6f, false});
            param[1].init(param[1].name, param[1].value.fp, 0.0f, 0, 0.5f);
            _plaitsEngine->LoadUserData(nullptr);
            break;
        case 6:
        {
            alloc_engine<plaits::ChordEngine>(plaits::kChordNumChords * plaits::kChordNumNotes + plaits::kChordNumChords + plaits::kChordNumNotes);
            init_params("Chord", 0.5f, "Inv.", 0.5f, "Shape", 0.5f, {0.8f, 0.8f, false});

            auto &chord = static_cast<plaits::ChordEngine *>(_plaitsEngine)->chords_.chord_index_quantizer_.quantized_value_;
            param[1].init_presets("Chord", (uint8_t *)&chord, 8, 0,
                          static_cast<plaits::ChordEngine *>(_plaitsEngine)->chords_.chord_index_quantizer_.num_steps() - 1);
            param[1].print_value = [&](char *tmp)
            {
                const int chord_index = static_cast<plaits::ChordEngine *>(_plaitsEngine)->chords_.chord_index();
                sprintf(tmp, "%s", plaits::chord_names[chord_index]);
            };
        }
        break;
        // case 15: // speech_engine_
        //     alloc_engine<plaits::SpeechEngine>(false, 0.8f, 0.8f);
        //     break;
        // case 8: // swarm_engine_
        //     alloc_engine<plaits::SwarmEngine>(false, -3.0f, 1.0f);
        //     break;
        // case 9: // noise_engine_
        //     alloc_engine<plaits::NoiseEngine>(false, -1.0f, -1.0f);
        //     break;
        // case 10: // particle_engine_
        //     alloc_engine<plaits::ParticleEngine>(false, -2.0f, 1.0f);
        //     break;
        // case 11: // string_engine_
        //     alloc_engine<plaits::StringEngine>(true, -1.0f, 0.8f);
        //     break;
        // case 12: // modal_engine_
        //     alloc_engine<plaits::ModalEngine>(true, -0.5f, 0.8f);
        //     break;
        case 13:
            _base_pitch += -24.f;
            alloc_engine<plaits::BassDrumEngine>();
            init_params(output == 0 ? "Drive" : "Punch", 0.8f, "Tone", 0.5f, "Decay", 0.5f, {0.8f, 0.8f, true});
            break;
        case 14:
            alloc_engine<plaits::SnareDrumEngine>();
            init_params("Snappy", 0.5f, "Tone", 0.5f, "Decay", 0.5f, {0.8f, 0.8f, true});
            break;
        case 15:
            alloc_engine<plaits::HiHatEngine>();
            init_params("Noise", 0.5f, "Tone", 0.9f, "Decay", 0.6f, {0.8f, 0.8f, true});
            break;
        // engines 2
        case 16:
            alloc_engine<plaits::VirtualAnalogVCFEngine>();
            init_params("Harsh", 0.5f, "Cutoff", 0.5f, "Morph", 0.5f, {1.f, 1.f, false});
            out_aux_mix = 0;
            modulations.timbre_patched = false;
            std::swap(param[1], param[3]);
            param[4].init("EnvMod", &patch.timbre_modulation_amount, 0, -1.f, 1.f);
            param[5].init("Decay", &patch.decay, 0.5f, 0, 0.99f);
            break;
#if 0
        case 17:
            alloc_engine<plaits::PhaseDistortionEngine>(plaits::kMaxBlockSize * 4);
            _plaitsEngine->post_processing_settings = {0.7f, 0.7f, false};
            break;
        case 18:
        case 19:
        case 20:
            alloc_engine<plaits::SixOpEngine>(
                plaits::kMaxBlockSize * 4 + plaits::kMaxBlockSize * plaits::kNumSixOpVoices + sizeof(plaits::fm::Patch) * 96 / sizeof(float));
            init_params("", 0.0f, "Mod", 0.5f, "Env", 0.5f, {1.f, 1.f, false});
            param[1].init_presets("Preset", &static_cast<plaits::SixOpEngine *>(_plaitsEngine)->patch_index, 0, 0, 95);
            param[1].print_value = [&](char *name)
            {
                auto i = static_cast<plaits::SixOpEngine *>(_plaitsEngine)->patch_index;
                sprintf(name, ">%.10s", static_cast<plaits::SixOpEngine *>(_plaitsEngine)->patches_[i].name);
            };
            _plaitsEngine->LoadUserData(plaits::fm_patches_table[0]);
            modulations.morph_patched = false;
            param[5].init("EnvMod", &patch.morph_modulation_amount, 0.0f, -1.f, 1.f);

            modulations.timbre_patched = false;
            param[4].init("ModMod", &patch.timbre_modulation_amount, 0.0f, -1.f, 1.f);
            break;
            case 21:
                alloc_engine<plaits::WaveTerrainEngine>(plaits::kMaxBlockSize * 4);
                _plaitsEngine->post_processing_settings = {0.7f, 0.7f, false};
                patch.engine = (engine - 16);
                break;
            case 22:
                alloc_engine<plaits::StringEngine>(16 + 3 * (1024 + 265));
                _plaitsEngine->post_processing_settings = {0.8f, 0.8f, false};
                patch.engine = (engine - 16);
                break;
            case 23:
                alloc_engine<plaits::ChiptuneEngine>(plaits::kChordNumChords * plaits::kChordNumNotes + plaits::kChordNumChords + plaits::kChordNumNotes);
                _plaitsEngine->post_processing_settings = {0.5f, 0.5f, false};
                patch.engine = (engine - 16);
                break;
#endif
        }

        if (engine >= 16)
            patch.engine = (engine - 16);

        _plaitsEngine->Reset();

        if (is_drum())
        {
            param[0].init("Pitch", &_pitch, 0, -1.f, 1.f);
            // modulations.frequency_patched = false;
            // param[4].init("Bend", &patch.frequency_modulation_amount, 0.0f, -1.f, 1.f);
        }
        else
        {
            if (param[4].name == nullptr)
                param[4].init("Decay", &patch.decay);

            if (param[5].name == nullptr)
            {
                if (output == 2)
                    param[5].init("AuxMix", &out_aux_mix, out_aux_mix);
                else
                    param[5].init("Color", &patch.lpg_colour);
            }
        }
    }

    bool is_drum()
    {
        return engine == 13 || engine == 14 || engine == 15;
    }

    void process(const machine::ControlFrame &frame, OutputFrame &of) override
    {
        float a = bufferOut[0] / 256.f;
        ONE_POLE(patch.harmonics, harmonics + a, 0.1f);
        ONE_POLE(patch.timbre, timbre + a, 0.1f);
        ONE_POLE(patch.morph, morph + a, 0.1f);

        modulations.level_patched = false;
        modulations.level = 1.f;

        patch.note = _base_pitch + frame.qz_voltage(this->io, _pitch) * 12;

        float last_decay = patch.decay;
        float last_morph = patch.morph;
        // if (!frame.trigger && frame.gate)
        // {
        //     if (is_drum())
        //         patch.morph = 1;
        //     else
        //         patch.decay = 1;

        //     modulations.level = 1.f;
        //     // modulations.level_patched = true;
        // }
        modulations.engine = patch.engine;
        modulations.trigger = frame.trigger ? 1 : 0;

        if (!is_drum())
            modulations.trigger_patched = patch.decay < 1.f;

        if (!this->io->tr)
        {
            modulations.trigger_patched = false;
            modulations.level_patched = true;
            modulations.level = patch.decay;
            patch.decay = 0.001f;
        }
        else
        {
            modulations.trigger_patched = true;
        }

        modulations.note = 0;
        voice.Render(_plaitsEngine, patch, modulations, bufferOut, bufferAux, machine::FRAME_BUFFER_SIZE);

        patch.decay = last_decay;
        patch.morph = last_morph;

        switch (output)
        {
        case 0:
            of.out = bufferOut;
            break;
        case 1:
            of.out = bufferAux;
            break;
        case 2:
            for (int i = 0; i < machine::FRAME_BUFFER_SIZE; i++)
                bufferOut[i] = stmlib::Crossfade(bufferOut[i], bufferAux[i], out_aux_mix);
            of.out = bufferOut;
            break;
        case 3:
            of.out = bufferOut;
            of.aux = bufferAux;
            break;
        }
    }

    void display() override
    {
        if (param[4].value.fp == &patch.decay)
        {
            if (!this->io->tr)
                param[4].name = "Level";
            else
                param[4].name = "Decay";
        }

        gfx::drawEngine(this);
    }
};

void init_plaits()
{

    // engine1
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "Virt.Analog", {0, 0});
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "Waveshaping", {1, 0});
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "2-OP-FM", {2, 0});
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "Formant/PD", {3, 2});
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "Harmonic", {4, 2});
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "Wavetable", {5, 2});
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "Chord", {6, 0});
    // machine::add<PlaitsEngine<7>>(machine::M_OSC, "VowelAndSpeech", 0.f, 0.95f, 0.5f, 0.25f, "Freq", "Harm", "Timbre", "Morph");

    // machine::add<PlaitsEngine<8, 2>>(machine::M_OSC, "Swarm", 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Morph");
    // machine::add<PlaitsEngine<9, 2>>(machine::M_OSC, "Noise", 4.f, 0.0f, 1.0f, 1.0f, "Cutoff", "LP/HP", "Clock", "Q");
    // machine::add<PlaitsEngine<10, 2>>(machine::M_OSC, "Particle", 4.f, 0.8f, 0.9f, 1.0f, "Freq", "Harm", "Timbre", "Morph");
    // machine::add<PlaitsEngine<11>>(machine::M_OSC, "String", 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Decay");
    // machine::add<PlaitsEngine<12>>(machine::M_OSC, "Modal", 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Decay");

    machine::add<PlaitsEngine, InitArgs>(machine::DRUM, "Analog BD", {13, 0});
    machine::add<PlaitsEngine, InitArgs>(machine::DRUM, "Analog SD", {14, 0});

    machine::add<PlaitsEngine, InitArgs>(machine::DRUM, "Analog HH2", {15, 1});
    machine::add<PlaitsEngine, InitArgs>(machine::DRUM, "Analog HH", {15, 0});
    machine::add<PlaitsEngine, InitArgs>(machine::DRUM, "909ish-BD", {13, 1});
    machine::add<PlaitsEngine, InitArgs>(machine::DRUM, "909ish-SD", {14, 1});
}

void init_plaits2()
{
    // engine2 https://pichenettes.github.io/mutable-instruments-documentation/modules/plaits/firmware/
    machine::add<PlaitsEngine, InitArgs>(machine::SYNTH, "ClassicVAVCF", {16, 2});
    // machine::add<PlaitsEngine, InitArgs>(machine::SYNTH, "SixOpFM", {18, 0});
    // machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "PhaseDistort", {17, 0, 0.f, 0.8f, 0.8f, 0.75f, "Freq", "Harm", "Timbre", "Morph"});
    // machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "WaveTerrain", {21, 0, 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Morph"});
    // machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "StringEngine", {22, 0, 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Morph"});
    // machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "ChipTune", {23, 1, 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Morph"});
}