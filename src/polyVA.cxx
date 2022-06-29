#include "machine.h"
#include "plaits/dsp/engine/virtual_analog_engine.h"
#include "plaits/dsp/envelope.h"
#include "stmlib/algorithms/voice_allocator.h"
#include "stmlib/utils/random.h"

using namespace machine;

struct PolyVAEngine : public machine::MidiEngine
{
    plaits::VirtualAnalogEngine voice[6];

    uint8_t buffer[LEN_OF(voice) * plaits::kBlockSize * sizeof(float) * 2];
    stmlib::BufferAllocator buffAllocator;

    plaits::EngineParameters parameters[LEN_OF(voice)];
    plaits::LPGEnvelope lpg[LEN_OF(voice)];
    bool enveloped[LEN_OF(voice)] = {};

    stmlib::VoiceAllocator<LEN_OF(voice)> allocator;

    float timbre;
    float morph;
    float harmonics;
    float pitch = 0;

    float decay = 0.5f;
    float hf = 1.f;
    float pan[LEN_OF(voice)];
    float stereo;

    float polyBuffL[machine::FRAME_BUFFER_SIZE];
    float polyBuffR[machine::FRAME_BUFFER_SIZE];
    float voiceBuff[machine::FRAME_BUFFER_SIZE];
    float dummy[machine::FRAME_BUFFER_SIZE];

    PolyVAEngine()
    {
        allocator.Init();
        allocator.set_size(LEN_OF(voice));
        param[0].init_v_oct("Pitch", &pitch);
        param[1].init("Harmo", &harmonics);
        param[2].init("Timbre", &timbre);
        param[3].init("Morph", &morph);
        param[4].init("Decay", &decay, decay);
        param[5].init("Stereo", &stereo, 0.5f);

        memset(buffer, 0, sizeof(buffer));
        buffAllocator.Init(buffer, 16384);

        for (size_t i = 0; i < LEN_OF(voice); i++)
        {
            lpg[i].Init();
            voice[i].Init(&buffAllocator);
        }
    }

    void process(const machine::ControlFrame &frame, OutputFrame &of) override
    {
        std::fill_n(polyBuffL, FRAME_BUFFER_SIZE, 0);
        std::fill_n(polyBuffR, FRAME_BUFFER_SIZE, 0);

        const float short_decay = (200.0f * FRAME_BUFFER_SIZE) / SAMPLE_RATE *
                                  stmlib::SemitonesToRatio(-96.0f * decay);

        const float decay_tail = (20.0f * FRAME_BUFFER_SIZE) / SAMPLE_RATE *
                                     stmlib::SemitonesToRatio(-72.0f * decay + 12.0f * hf) -
                                 short_decay;

        for (size_t i = 0; i < LEN_OF(voice); i++)
        {
            auto p = parameters[i];
            p.note += pitch * 12.f;
            p.timbre = timbre;
            p.morph = morph;
            p.harmonics = harmonics;

            voice[i].Render(p, voiceBuff, dummy, FRAME_BUFFER_SIZE, &enveloped[i]);

            lpg[i].ProcessPing(0.5f, short_decay, decay_tail, hf);

            float l = cosf(pan[i] * M_PI / 2);
            float r = sinf(pan[i] * M_PI / 2);

            for (int s = 0; s < FRAME_BUFFER_SIZE; s++)
            {
                polyBuffL[s] += (voiceBuff[s] * lpg[i].gain()) * l;
                polyBuffR[s] += (voiceBuff[s] * lpg[i].gain()) * r;
            }

            parameters[i].trigger = plaits::TriggerState::TRIGGER_LOW;
        }

        of.push(polyBuffL, FRAME_BUFFER_SIZE);
        of.push(polyBuffR, FRAME_BUFFER_SIZE);
    }

    void onMidiNote(uint8_t key, uint8_t velocity) override // NoteOff: velocity == 0
    {
        if (velocity > 0)
        {
            auto ni = allocator.NoteOn(key);
            parameters[ni].trigger = plaits::TriggerState::TRIGGER_RISING_EDGE;
            parameters[ni].note = key;
            parameters[ni].accent = velocity > 100;

            pan[ni] = 0.5f + stereo * (stmlib::Random::GetFloat() - 0.5f);

            lpg[ni].Trigger();
        }
        else
        {
            allocator.NoteOff(key);
        }
    }

    void onMidiPitchbend(int16_t pitch) override
    {
    }

    void onMidiCC(uint8_t ccc, uint8_t value) override
    {
    }
};

void init_midi_polyVA()
{
    machine::add<PolyVAEngine>("MIDI", "VAx6");
}