#include "stmlib/stmlib.h"
#include "machine.h"
#include "plaits/dsp/voice.h"

using namespace machine;

struct InitArgs
{
    int engine;
    int output; // output=0 -> out, output=1 -> aux, output=3 -> stereo
    float pitch_offset;
    float harmonics;
    float timbre;
    float morph;
    const char *param1;
    const char *param2;
    const char *param3;
    const char *param4;
};

struct PlaitsEngine : public Engine
{
    plaits::Modulations modulations;
    stmlib::BufferAllocator allocator;
    plaits::Voice voice;
    plaits::Patch patch;

    float bufferOut[machine::FRAME_BUFFER_SIZE];
    float bufferAux[machine::FRAME_BUFFER_SIZE];
    float out_aux_mix = 0;
    float _pitch = 0;
    float _base_pitch = machine::DEFAULT_NOTE;

    stmlib::HysteresisQuantizer chord_index_quantizer_;

    ~PlaitsEngine() override
    {
        machine::mfree(_plaitsEngine);
        machine::mfree(_buffer);
    }

    const uint8_t engine = 0;
    const uint8_t output = 0;

    uint8_t *_buffer = nullptr;
    plaits::Engine *_plaitsEngine = nullptr;

    template <class T>
    void init(bool already_enveloped, float out_gain, float aux_gain, size_t mem = 48)
    {
        _plaitsEngine = new (machine::malloc(sizeof(T))) T();
        _buffer = (uint8_t *)machine::malloc(mem * sizeof(float));

        auto *s = &_plaitsEngine->post_processing_settings;
        s->already_enveloped = already_enveloped;
        s->out_gain = out_gain;
        s->aux_gain = aux_gain;

        allocator.Init(_buffer, mem * sizeof(float));
        _plaitsEngine->Init(&allocator);
        _plaitsEngine->Reset();
    }

    PlaitsEngine(const InitArgs &args) : Engine(TRIGGER_INPUT | VOCT_INPUT), engine(args.engine), output(args.output)
    {
        voice.Init();

        switch (engine)
        {
        case 0:
            init<plaits::VirtualAnalogEngine>(false, 0.8f, 0.8f);
            break;
        case 1:
            init<plaits::WaveshapingEngine>(false, 0.7f, 0.6f);
            break;
        case 2:
            init<plaits::FMEngine>(false, 0.6f, 0.6f);
            break;
        case 3:
            init<plaits::GrainEngine>(false, 0.7f, 0.6f);
            break;
        case 4:
            init<plaits::AdditiveEngine>(false, 0.8f, 0.8f);
            break;
        case 5:
            init<plaits::WaveshapingEngine>(false, 0.6f, 0.6f);
            break;
        case 6:
            init<plaits::ChordEngine>(false, 0.8f, 0.8f, plaits::kChordNumChords * plaits::kChordNumNotes);
            break;
        // case 7: // speech_engine_
        //     init<plaits::SpeechEngine>(false, 0.8f, 0.8f);
        //     break;
        // case 8: // swarm_engine_
        //     init<plaits::SwarmEngine>(false, -3.0f, 1.0f);
        //     break;
        // case 9: // noise_engine_
        //     init<plaits::NoiseEngine>(false, -1.0f, -1.0f);
        //     break;
        // case 10: // particle_engine_
        //     init<plaits::ParticleEngine>(false, -2.0f, 1.0f);
        //     break;
        // case 11: // string_engine_
        //     init<plaits::StringEngine>(true, -1.0f, 0.8f);
        //     break;
        // case 12: // modal_engine_
        //     init<plaits::ModalEngine>(true, -0.5f, 0.8f);
        //     break;
        case 13:
            init<plaits::BassDrumEngine>(true, 0.8f, 0.8f);
            break;
        case 14:
            init<plaits::SnareDrumEngine>(true, 0.8f, 0.8f);
            break;
        case 15:
            init<plaits::HiHatEngine>(true, 0.8f, 0.8f);
            break;
        }

        memset(&modulations, 0, sizeof(patch));
        patch.engine = 0;
        patch.note = machine::DEFAULT_NOTE + _pitch * 12.f;

        patch.harmonics = args.harmonics;
        patch.timbre = args.timbre;
        patch.morph = args.morph;

        patch.frequency_modulation_amount = 0;
        patch.morph_modulation_amount = 0;
        patch.timbre_modulation_amount = 0;

        memset(&modulations, 0, sizeof(modulations));

        patch.lpg_colour = 0.5;
        patch.decay = 0.5;
        modulations.trigger_patched = true; // trigger;
                                            // modulations.level_patched = true; //trigger;
                                            // modulations.level = 1;

        param[0].init_v_oct("Freq", &_pitch);
        param[1].init(args.param2, &patch.harmonics, args.harmonics);
        param[2].init(args.param3, &patch.timbre, args.timbre);
        param[3].init(args.param4, &patch.morph, args.morph);

        if (engine == 6) // Chords
        {
            chord_index_quantizer_.Init();

            param[1].step.f = 1.f / (plaits::kChordNumChords - 1);
            param[1].print_value = [&](char *tmp)
            {
                const int chord_index = this->chord_index_quantizer_.Process(
                    this->patch.harmonics, plaits::kChordNumChords, 0);
                sprintf(tmp, "%s", plaits::chord_names[chord_index]);
            };
        }

        _base_pitch += args.pitch_offset;

        if (engine < 13)
        {
            param[4].init("Decay", &patch.decay);

            if (engine >= 8 && output == 2)
                param[5].init("AuxMix", &out_aux_mix);
            else
                param[5].init("Color", &patch.lpg_colour);
        }
        else
        {
            param[0].init("Freq", &_pitch, 0, -1.f, 1.f);
        }
    }

    void process(const machine::ControlFrame &frame, OutputFrame &of) override
    {
        plaits::Frame f = {bufferOut, bufferAux};
        patch.note = _base_pitch;

        f.size = FRAME_BUFFER_SIZE;

        float last_decay = patch.decay;
        float last_morph = patch.morph;
        if (!frame.trigger && frame.gate)
        {
            if (engine >= 11)
                patch.morph = 1;
            else
                patch.decay = 1;

            modulations.level = 1.f;
            modulations.level_patched = true;
        }
        else
            modulations.level_patched = false;

        modulations.engine = 0;
        modulations.trigger = frame.trigger ? 1 : 0;

        if (engine < 13)
            modulations.trigger_patched = patch.decay < 1.f;

        if (!this->io->tr)
        {
            modulations.trigger_patched = false;
            modulations.level_patched = true;
            modulations.level = patch.decay;
            patch.decay = 0.001f;
        }

        modulations.note = frame.qz_voltage(this->io, _pitch) * 12;
        voice.Render(_plaitsEngine, patch, modulations, f);

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
        default:
            for (int i = 0; i < machine::FRAME_BUFFER_SIZE; i++)
                bufferOut[i] = (1 - out_aux_mix) * bufferOut[i] + out_aux_mix * bufferAux[i];
            of.out = bufferOut;
            break;
        }
    }

    void display() override
    {
        if (param[4].name)
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
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "Virt.Analog", {0, 0, 0.f, 1.0f, 0.0f, 0.5f, "Freq", "Harm", "Timbre", "Morph"});
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "Waveshaping", {1, 0, 0.f, 0.8f, 0.8f, 0.75f, "Freq", "Harm", "Timbre", "Morph"});
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "FM", {2, 0, 0.f, 0.8f, 0.8f, 0.75f, "Freq", "Ratio", "Mod.", "Feedb."});
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "Grain", {3, 0, 0.f, 0.8f, 0.8f, 0.75f, "Freq", "Harm", "Timbre", "Morph"});
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "Additive", {4, 0, 0.f, 0.8f, 0.8f, 0.75f, "Freq", "Harm", "Timbre", "Morph"});
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "Wavetable", {5, 0, 0.f, 0.8f, 0.8f, 0.75f, "Freq", "Harm", "Timbre", "Morph"});
    machine::add<PlaitsEngine, InitArgs>(machine::M_OSC, "Chord", {6, 0, 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Morph"});
    // machine::add<PlaitsEngine<7>>(machine::M_OSC, "VowelAndSpeech", 0.f, 0.95f, 0.5f, 0.25f, "Freq", "Harm", "Timbre", "Morph");

    // machine::add<PlaitsEngine<8, 2>>(machine::M_OSC, "Swarm", 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Morph");
    // machine::add<PlaitsEngine<9, 2>>(machine::M_OSC, "Noise", 4.f, 0.0f, 1.0f, 1.0f, "Cutoff", "LP/HP", "Clock", "Q");
    // machine::add<PlaitsEngine<10, 2>>(machine::M_OSC, "Particle", 4.f, 0.8f, 0.9f, 1.0f, "Freq", "Harm", "Timbre", "Morph");
    // machine::add<PlaitsEngine<11>>(machine::M_OSC, "String", 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Decay");
    // machine::add<PlaitsEngine<12>>(machine::M_OSC, "Modal", 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Decay");

    machine::add<PlaitsEngine, InitArgs>(machine::DRUM, "Analog BD", {13, 0, -36.f, 0.8f, 0.5f, 0.5f, "Pitch", "Drive", "Tone", "Decay"});
    machine::add<PlaitsEngine, InitArgs>(machine::DRUM, "Analog SD", {14, 0, 0.f, 0.5f, 0.5f, 0.5f, "Pitch", "Snappy", "Tone", "Decay"});

    machine::add<PlaitsEngine, InitArgs>(machine::DRUM, "Analog HH2", {15, 1, 0.f, 0.5f, 0.9f, 0.6f, "Pitch", "Noise", "Tone", "Decay"});
    machine::add<PlaitsEngine, InitArgs>(machine::DRUM, "Analog HH", {15, 0, 0.f, 0.5f, 0.9f, 0.6f, "Pitch", "Noise", "Tone", "Decay"});
    machine::add<PlaitsEngine, InitArgs>(machine::DRUM, "909ish-BD", {13, 1, -36.f, 0.8f, 0.8f, 0.75f, "Pitch", "Punch", "Tone", "Decay"});
    machine::add<PlaitsEngine, InitArgs>(machine::DRUM, "909ish-SD", {14, 1, -12.f, 0.5f, 0.5f, 0.5f, "Pitch", "Snappy", "Tone", "Decay"});
}

MACHINE_INIT(init_plaits);