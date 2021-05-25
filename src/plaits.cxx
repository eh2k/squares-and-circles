#include "stmlib/stmlib.h"
#include "machine.h"
#include "plaits/dsp/voice.h"

using namespace machine;

template <int engine, int output = 0> // output=0 -> out, output=1 -> aux, output=3 -> stereo
struct PlaitsEngine : public Engine
{
    const char *param_names[7];
    plaits::Modulations modulations;
    uint8_t buffer[16384];
    stmlib::BufferAllocator allocator;
    plaits::Voice voice;
    plaits::Patch patch;

    float bufferOut[machine::FRAME_BUFFER_SIZE];
    float bufferAux[machine::FRAME_BUFFER_SIZE];
    float out_aux_mix = 0;

    float _pitch;
    float _harmonics = 0.5f;
    float _timbre = 0;
    float _morph = 0;

    PlaitsEngine(float pitch_, float harmonics, float timbre, float morph,
                 const char *param1,
                 const char *param2,
                 const char *param3,
                 const char *param4) : param_names{param1, param2, param3, param4}
    {
        memset(buffer, 0, sizeof(buffer));
        allocator.Init(buffer, 16384);
        voice.Init(&allocator);

        _pitch = pitch_;
        memset(&modulations, 0, sizeof(patch));
        patch.engine = engine;
        patch.note = 60.f + _pitch * 12.f;

        _harmonics = harmonics;
        _timbre = timbre;
        _morph = morph;

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
    }

    void Process(const machine::ControlFrame &frame, float **out, float **aux) override
    {
        plaits::Frame f;
        f.out = bufferOut;
        f.aux = bufferAux;

        patch.note = (float)frame.midi.key + _pitch * 12.f + (frame.midi.pitch / 128);

        f.size = FRAME_BUFFER_SIZE;

        ONE_POLE(patch.harmonics, _harmonics, 0.005f);
        ONE_POLE(patch.timbre, _timbre, 0.005f);
        ONE_POLE(patch.morph, _morph, 0.005f);

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

    void OnEncoder(uint8_t param_index, int16_t inc, bool pressed) override
    {
        if (param_index == 0 && engine < 7)
        {
            if (inc > 0)
                _pitch += !pressed ? 1 : 1.f / 12;
            else
                _pitch -= !pressed ? 1 : 1.f / 12;

            CONSTRAIN(_pitch, -4, 4);
        }
        else
        {
            machine::Engine::OnEncoder(param_index, inc, pressed);
        }
    }

    void SetParams(const uint16_t *params) override
    {
        _pitch = float_range_from_uint16_t(params[0]);
        _harmonics = ((float)params[1]) / UINT16_MAX;
        _timbre = ((float)params[2]) / UINT16_MAX;
        _morph = ((float)params[3]) / UINT16_MAX;

        if (engine < 13)
        {
            patch.decay = ((float)params[4]) / UINT16_MAX;

            if (engine >= 8 && output == 2)
                out_aux_mix = ((float)params[5]) / UINT16_MAX;
            else
                patch.lpg_colour = ((float)params[5]) / UINT16_MAX;

            modulations.trigger_patched = patch.decay < 1;
        }
    }

    const char **GetParams(uint16_t *values) override
    {
        values[0] = float_range_to_uint16_t(_pitch);
        values[1] = _harmonics * UINT16_MAX;
        values[2] = _timbre * UINT16_MAX;
        values[3] = _morph * UINT16_MAX;

        if (engine >= 11)
        {
            param_names[4] = nullptr;
            param_names[5] = nullptr;
        }
        else if (engine >= 8 && output == 2)
        {
            param_names[4] = "Decay";
            values[4] = patch.decay * UINT16_MAX;
            param_names[5] = "AuxMix";
            values[5] = out_aux_mix * UINT16_MAX;
        }
        else if (modulations.trigger_patched)
        {
            param_names[4] = "DecLPG";
            values[4] = patch.decay * UINT16_MAX;
            param_names[5] = "Color";
            values[5] = patch.lpg_colour * UINT16_MAX;
            param_names[6] = nullptr;
        }
        else
        {
            param_names[4] = "LPG-off";
            values[4] = patch.decay * UINT16_MAX;
            param_names[5] = nullptr;
        }

        return param_names;
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

    machine::add<PlaitsEngine<13, 0>>(machine::DRUM, "Analog BD", -0.5f, 0.8f, 0.5f, 0.5f, "Pitch", "Drive", "Tone", "Decay");
    machine::add<PlaitsEngine<14, 0>>(machine::DRUM, "Analog SD", 1.f, 0.5f, 0.5f, 0.5f, "Pitch", "Snappy", "Tone", "Decay");
    machine::add<PlaitsEngine<15, 0>>(machine::DRUM, "Analog HH", 1.f, 0.5f, 0.9f, 0.6f, "Pitch", "Noise", "Tone", "Decay");
    machine::add<PlaitsEngine<15, 1>>(machine::DRUM, "Analog HH2", 1.f, 0.5f, 0.9f, 0.6f, "Pitch", "Noise", "Tone", "Decay");
    machine::add<PlaitsEngine<13, 1>>(machine::DRUM, "909ish-BD", -0.5f, 0.8f, 0.8f, 0.75f, "Pitch", "Punch", "Tone", "Decay");
    machine::add<PlaitsEngine<14, 1>>(machine::DRUM, "909ish-SD", 1.f, 0.5f, 0.5f, 0.5f, "Pitch", "Snappy", "Tone", "Decay");
}

MACHINE_INIT(init_plaits);