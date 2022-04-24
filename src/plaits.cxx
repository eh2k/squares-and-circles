#include "stmlib/stmlib.h"
#include "machine.h"
#include "plaits/dsp/voice.h"

using namespace machine;

template <int engine, int output = 0> // output=0 -> out, output=1 -> aux, output=3 -> stereo
struct PlaitsEngine : public Engine
{
    plaits::Modulations modulations;
    uint8_t buffer[16384];
    stmlib::BufferAllocator allocator;
    plaits::Voice voice;
    plaits::Patch patch;

    float bufferOut[machine::FRAME_BUFFER_SIZE];
    float bufferAux[machine::FRAME_BUFFER_SIZE];
    float out_aux_mix = 0;
    float _pitch;

    PlaitsEngine(float pitch_, float harmonics, float timbre, float morph,
                 const char *param1,
                 const char *param2,
                 const char *param3,
                 const char *param4) : Engine()
    {
        memset(buffer, 0, sizeof(buffer));
        allocator.Init(buffer, 16384);
        voice.Init(&allocator);

        _pitch = pitch_;
        memset(&modulations, 0, sizeof(patch));
        patch.engine = engine;
        patch.note = 60.f + _pitch * 12.f;

        patch.harmonics = harmonics;
        patch.timbre = timbre;
        patch.morph = morph;

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
        param[1].init(param2, &patch.harmonics, harmonics);
        param[2].init(param3, &patch.timbre, timbre);
        param[3].init(param4, &patch.morph, morph);

        if (engine < 13)
        {
            param[4].init("Decay", &patch.decay);
            param[4].value_changed = [&]()
            {
                modulations.trigger_patched = patch.decay < 1.f;
            };

            if (engine >= 8 && output == 2)
                param[5].init("AuxMix", &out_aux_mix);
            else
                param[5].init("Color", &patch.lpg_colour);
        }
        else
        {
            param[0].setStepValue(4.f / 24);
            param[0].flags &= ~Parameter::IS_V_OCT;
        }
    }

    void Process(const machine::ControlFrame &frame, float **out, float **aux) override
    {
        plaits::Frame f;
        f.out = bufferOut;
        f.aux = bufferAux;

        patch.note = (float)frame.midi.key + _pitch * 12.f + (frame.midi.pitch / 128);

        f.size = FRAME_BUFFER_SIZE;

        float last_decay = patch.decay;
        if (frame.midi.on > 0)
        {
            if (engine >= 11)
                patch.morph = 1;
            else
                patch.decay = 1;

            modulations.level = (float)frame.midi.velocity / 128;
            modulations.level_patched = true;
        }
        else
            modulations.level_patched = false;

        modulations.engine = 0;
        modulations.trigger = frame.trigger ? 1 : 0;

        modulations.note = frame.cv_voltage * 12;
        voice.Render(patch, modulations, f);

        patch.decay = last_decay;

        switch (output)
        {
        case 0:
            *out = bufferOut;
            break;
        case 1:
            *out = bufferAux;
            break;
        default:
            for (int i = 0; i < machine::FRAME_BUFFER_SIZE; i++)
                bufferOut[i] = (1 - out_aux_mix) * bufferOut[i] + out_aux_mix * bufferAux[i];
            *out = bufferOut;
            break;
        }
    }
};

void init_plaits()
{
    machine::add<PlaitsEngine<0>>(machine::M_OSC, "Virt.Analog", 0.f, 1.0f, 0.0f, 0.5f, "Freq", "Harm", "Timbre", "Morph");
    machine::add<PlaitsEngine<1>>(machine::M_OSC, "Waveshaping", 0.f, 0.8f, 0.8f, 0.75f, "Freq", "Harm", "Timbre", "Morph");
    machine::add<PlaitsEngine<2>>(machine::M_OSC, "FM", 0.f, 0.8f, 0.8f, 0.75f, "Freq", "Ratio", "Mod.", "Feedb.");
    machine::add<PlaitsEngine<3>>(machine::M_OSC, "Grain", 0.f, 0.8f, 0.8f, 0.75f, "Freq", "Harm", "Timbre", "Morph");
    machine::add<PlaitsEngine<4>>(machine::M_OSC, "Additive", 0.f, 0.8f, 0.8f, 0.75f, "Freq", "Harm", "Timbre", "Morph");
    machine::add<PlaitsEngine<5>>(machine::M_OSC, "Wavetable", 0.f, 0.8f, 0.8f, 0.75f, "Freq", "Harm", "Timbre", "Morph");
    machine::add<PlaitsEngine<6>>(machine::M_OSC, "Chord", 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Morph");
    //machine::add<PlaitsEngine<7>>(machine::M_OSC, "VowelAndSpeech", 0.f, 0.95f, 0.5f, 0.25f, "Freq", "Harm", "Timbre", "Morph");

    // machine::add<PlaitsEngine<8, 2>>(machine::M_OSC, "Swarm", 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Morph");
    // machine::add<PlaitsEngine<9, 2>>(machine::M_OSC, "Noise", 4.f, 0.0f, 1.0f, 1.0f, "Cutoff", "LP/HP", "Clock", "Q");
    // machine::add<PlaitsEngine<10, 2>>(machine::M_OSC, "Particle", 4.f, 0.8f, 0.9f, 1.0f, "Freq", "Harm", "Timbre", "Morph");
    // machine::add<PlaitsEngine<11>>(machine::M_OSC, "String", 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Decay");
    // machine::add<PlaitsEngine<12>>(machine::M_OSC, "Modal", 0.f, 0.5f, 0.5f, 0.5f, "Freq", "Harm", "Timbre", "Decay");

    machine::add<PlaitsEngine<13, 0>>(machine::DRUM, "Analog BD", -1.f, 0.8f, 0.5f, 0.5f, "Pitch", "Drive", "Tone", "Decay");
    machine::add<PlaitsEngine<14, 0>>(machine::DRUM, "Analog SD", 1.f, 0.5f, 0.5f, 0.5f, "Pitch", "Snappy", "Tone", "Decay");
    machine::add<PlaitsEngine<15, 0>>(machine::DRUM, "Analog HH", 0.5f, 0.5f, 0.9f, 0.6f, "Pitch", "Noise", "Tone", "Decay");
    machine::add<PlaitsEngine<15, 1>>(machine::DRUM, "Analog HH2", 0.5f, 0.5f, 0.9f, 0.6f, "Pitch", "Noise", "Tone", "Decay");
    machine::add<PlaitsEngine<13, 1>>(machine::DRUM, "909ish-BD", -1.f, 0.8f, 0.8f, 0.75f, "Pitch", "Punch", "Tone", "Decay");
    machine::add<PlaitsEngine<14, 1>>(machine::DRUM, "909ish-SD", 1.f, 0.5f, 0.5f, 0.5f, "Pitch", "Snappy", "Tone", "Decay");
}

MACHINE_INIT(init_plaits);