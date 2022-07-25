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
    float _pitch = 0.f;
    uint16_t _word = 0;
    float _speed = 0.3f;
    float _formant_shift = 0.5f;
    float _prosody = 0.5f;

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

    SpeechEngine() : Engine(TRIGGER_INPUT | VOCT_INPUT)
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

        param[0].init_v_oct("Pitch", &_pitch);
        param[1].init("WORD", &_word, 0, 0, LEN_OF(_words) - 1);
        param[2].init("Speed", &_speed, _speed);
        param[3].init("Form.Shift", &_formant_shift, _formant_shift);
        param[4].init("Prs.Amnt", &_prosody, _prosody);
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        auto note = (float)machine::DEFAULT_NOTE + _pitch * 12.f;

        note += frame.cv_voltage() * 12;

        const float f0 = NoteToFrequency(note);

        lpc_speech_synth_controller_.Render(false,
                                            frame.trigger,
                                            _words[_word].bank, // Bank
                                            f0,
                                            _prosody,
                                            _speed,             // Speed
                                            _words[_word].addr, // Word
                                            _formant_shift,
                                            1.0f,
                                            _aux,
                                            _out,
                                            machine::FRAME_BUFFER_SIZE);

        of.out = _out;
        of.aux = _aux;
    }

    char tmp[10];
    void onDisplay(uint8_t *buffer) override
    {
        sprintf(tmp, "WORD%0d", _word);
        param[1].name = tmp;

        gfx::drawEngine(buffer, this);
    }
};

void init_speech()
{
    machine::add<SpeechEngine>("SPEECH", "LPC");
}

MACHINE_INIT(init_speech);
