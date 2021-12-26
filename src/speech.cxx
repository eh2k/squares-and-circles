#include "plaits/dsp/speech/lpc_speech_synth_controller.h"
#include "plaits/dsp/speech/lpc_speech_synth_words.h"

#include "stmlib/stmlib.h"
#include "stmlib/dsp/units.h"
#include "machine.h"

using namespace machine;

const float a0 = (440.0f / 8.0f) / 48000.0f;

inline float NoteToFrequency(float midi_note)
{
    midi_note -= 9.0f;
    CONSTRAIN(midi_note, -128.0f, 127.0f);
    return a0 * 0.25f * stmlib::SemitonesToRatio(midi_note);
}

struct SpeechEngine : public Engine
{
    float _pitch = 0.5f;
    float _speed = 0.3f;
    float _formant_shift = 0.5f;
    float _prosody = 0.5f;
    uint16_t _word = 0;
    struct
    {
        uint8_t bank;
        float addr;
    } _words[92];

    plaits::LPCSpeechSynthController lpc_speech_synth_controller_;
    plaits::LPCSpeechSynthWordBank lpc_speech_synth_word_bank_;

    float _out[machine::FRAME_BUFFER_SIZE];
    float _aux[machine::FRAME_BUFFER_SIZE];
    float _tmp[machine::FRAME_BUFFER_SIZE];

    uint8_t buffer[16384];
    stmlib::BufferAllocator allocator;

    SpeechEngine() : Engine()
    {

        memset(buffer, 0, sizeof(buffer));
        allocator.Init(buffer, 16384);
        lpc_speech_synth_word_bank_.Init(plaits::word_banks_,
                                         LPC_SPEECH_SYNTH_NUM_WORD_BANKS,
                                         &allocator);
        int j = 0;
        for (uint8_t i = 0; i < LPC_SPEECH_SYNTH_NUM_WORD_BANKS; i++)
        {
            lpc_speech_synth_word_bank_.Load(i);

            for (int l = 0; l < lpc_speech_synth_word_bank_.num_words(); l++)
                _words[j++] = {i, 1.f / lpc_speech_synth_word_bank_.num_words() * l};
        }

        lpc_speech_synth_controller_.Init(&lpc_speech_synth_word_bank_);
    }

    void Process(const ControlFrame &frame, float **out, float **aux) override
    {
        auto note = (float)frame.midi.key + _pitch * 12.f + (frame.midi.pitch / 128);

        note += frame.cv_voltage * 12;

        const float f0 = NoteToFrequency(note);

        lpc_speech_synth_controller_.Render(false,
                                            frame.trigger,
                                            _words[_word].bank, //Bank
                                            f0,
                                            _prosody,
                                            _speed,  //Speed
                                            _words[_word].addr, //Word
                                            _formant_shift,
                                            1.0f,
                                            _aux,
                                            _out,
                                            machine::FRAME_BUFFER_SIZE);

        *out = _out;
        *aux = _aux;
    }

    void OnEncoder(uint8_t param_index, int16_t inc, bool pressed) override
    {
        if (param_index == 1)
        {
            auto select = _word;
            if (inc > 0)
                select++;
            else if (inc < 0 && _word > 0)
                select--;

            SetParam(param_index, select);
        }
        else
        {
            Engine::OnEncoder(param_index, inc, pressed);
        }
    }

    void SetParams(const uint16_t *params) override
    {
        _pitch = float_range_from_uint16_t(params[0]);
        _word = params[1];
        CONSTRAIN(_word, 0, sizeof(_words)/sizeof(_words[0])-1);
        _speed = ((float)params[2]) / UINT16_MAX;
        _formant_shift = ((float)params[3]) / UINT16_MAX;
        _prosody = ((float)params[4]) / UINT16_MAX;
    }

    const char **GetParams(uint16_t *values) override
    {
        static const char *names[]{"Pitch", "%d-WORD", "Speed", "Form.Shift", "Prs.Amnt", nullptr};
        values[0] = float_range_to_uint16_t(_pitch);
        values[1] = _word;
        values[2] = _speed * UINT16_MAX;
        values[3] = _formant_shift * UINT16_MAX;
        values[4] = _prosody * UINT16_MAX;
        return names;
    }
};

void init_speech()
{
    machine::add<SpeechEngine>("SPEECH", "LPC");
}

MACHINE_INIT(init_speech);
