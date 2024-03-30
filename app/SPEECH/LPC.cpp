
#include "../squares-and-circles-api.h"
#include "stmlib/stmlib.h"
#include "stmlib/dsp/units.h"
#include <stdio.h>

#define private public
#include "plaits/dsp/speech/lpc_speech_synth_controller.h"
#include "plaits/dsp/speech/lpc_speech_synth_words.h"

#define FLASHMEM __attribute__((section(".text")))

#include "plaits/dsp/speech/lpc_speech_synth_controller.cc"
#include "plaits/dsp/speech/lpc_speech_synth_words.cc"
#include "plaits/dsp/speech/lpc_speech_synth_phonemes.cc"
#include "plaits/dsp/speech/lpc_speech_synth.cc"

#include "stmlib/dsp/units.cc"
#include "stmlib/utils/random.cc"

// from #include "plaits/resources.cc"
namespace plaits
{
const int8_t lut_lpc_excitation_pulse[] FLASHMEM = {
       0,      0,      0,      0,
       1,      1,      1,      2,
       3,      3,      4,      5,
       6,      7,      8,      9,
      10,     12,     13,     15,
      17,     19,     21,     23,
      25,     28,     30,     33,
      36,     38,     41,     44,
      48,     51,     54,     57,
      61,     64,     68,     71,
      75,     78,     82,     85,
      89,     92,     95,     99,
     102,    105,    108,    110,
     113,    115,    118,    120,
     121,    123,    124,    125,
     126,    127,    127,    127,
     127,    126,    125,    124,
     122,    121,    118,    116,
     113,    110,    107,    104,
     100,     96,     92,     87,
      83,     78,     73,     68,
      63,     58,     53,     47,
      42,     37,     31,     26,
      21,     16,     11,      6,
       1,     -3,     -8,    -12,
     -16,    -20,    -23,    -26,
     -29,    -32,    -35,    -37,
     -39,    -40,    -42,    -43,
     -43,    -44,    -44,    -44,
     -44,    -44,    -43,    -42,
     -41,    -40,    -39,    -38,
     -36,    -35,    -33,    -32,
     -30,    -29,    -27,    -26,
     -24,    -23,    -22,    -20,
     -19,    -18,    -18,    -17,
     -16,    -16,    -16,    -16,
     -16,    -16,    -17,    -17,
     -18,    -19,    -20,    -21,
     -23,    -24,    -26,    -27,
     -29,    -30,    -32,    -34,
     -35,    -37,    -39,    -40,
     -42,    -43,    -44,    -46,
     -47,    -48,    -48,    -49,
     -49,    -50,    -50,    -50,
     -49,    -49,    -48,    -47,
     -46,    -45,    -44,    -42,
     -41,    -39,    -37,    -35,
     -33,    -31,    -29,    -27,
     -25,    -23,    -20,    -18,
     -16,    -14,    -13,    -11,
      -9,     -8,     -7,     -6,
      -5,     -4,     -3,     -3,
      -3,     -3,     -3,     -4,
      -5,     -6,     -7,     -9,
     -10,    -12,    -14,    -16,
     -18,    -21,    -23,    -26,
     -29,    -32,    -34,    -37,
     -40,    -43,    -46,    -49,
     -51,    -54,    -56,    -59,
     -61,    -63,    -65,    -67,
     -68,    -70,    -71,    -72,
     -72,    -73,    -73,    -73,
     -73,    -72,    -72,    -71,
     -70,    -69,    -67,    -66,
     -64,    -63,    -61,    -59,
     -57,    -54,    -52,    -50,
     -48,    -45,    -43,    -41,
     -39,    -37,    -35,    -33,
     -31,    -29,    -27,    -26,
     -25,    -23,    -22,    -21,
     -20,    -20,    -19,    -19,
     -19,    -18,    -18,    -18,
     -19,    -19,    -19,    -20,
     -20,    -21,    -21,    -22,
     -22,    -23,    -23,    -24,
     -24,    -25,    -25,    -25,
     -26,    -26,    -26,    -25,
     -25,    -25,    -24,    -24,
     -23,    -22,    -21,    -20,
     -19,    -18,    -16,    -15,
     -13,    -12,    -10,     -8,
      -6,     -4,     -3,     -1,
       1,      3,      5,      7,
       8,     10,     12,     13,
      15,     16,     17,     18,
      19,     20,     21,     21,
      22,     22,     22,     22,
      22,     22,     22,     22,
      21,     21,     20,     19,
      19,     18,     17,     16,
      15,     14,     14,     13,
      12,     11,     10,     10,
       9,      9,      8,      8,
       8,      8,      8,      8,
       8,      8,      8,      9,
       9,     10,     11,     12,
      13,     14,     15,     16,
      17,     18,     19,     21,
      22,     23,     24,     26,
      27,     28,     29,     30,
      31,     32,     33,     33,
      34,     35,     35,     35,
      36,     36,     36,     36,
      35,     35,     35,     34,
      34,     33,     33,     32,
      31,     30,     30,     29,
      28,     27,     26,     25,
      25,     24,     23,     22,
      22,     21,     20,     20,
      19,     19,     19,     19,
      19,     19,     19,     19,
      19,     19,     20,     20,
      21,     21,     22,     22,
      23,     23,     24,     25,
      25,     26,     27,     27,
      28,     29,     29,     29,
      30,     30,     30,     31,
      31,     31,     31,     30,
      30,     30,     29,     29,
      28,     28,     27,     26,
      26,     25,     24,     23,
      22,     21,     20,     19,
      18,     17,     16,     15,
      15,     14,     13,     12,
      12,     11,     10,     10,
      10,      9,      9,      9,
       9,      9,      9,      9,
       9,     10,     10,     10,
      11,     11,     12,     12,
      12,     13,     13,     14,
      14,     14,     15,     15,
      15,     15,     15,     15,
      15,     15,     15,     15,
      14,     14,     14,     13,
      12,     12,     11,     10,
       9,      8,      8,      7,
       6,      5,      4,      3,
       2,      1,      0,     -1,
      -2,     -3,     -4,     -4,
      -5,     -6,     -6,     -7,
      -7,     -8,     -8,     -8,
      -9,     -9,     -9,     -9,
      -9,     -9,     -9,     -9,
      -8,     -8,     -8,     -7,
      -7,     -7,     -6,     -6,
      -5,     -5,     -5,     -4,
      -4,     -4,     -3,     -3,
      -3,     -2,     -2,     -2,
      -2,     -1,     -1,     -1,
      -1,     -1,     -1,     -1,
      -1,     -1,     -1,     -1,
      -1,     -1,     -2,     -2,
      -2,     -2,     -2,     -2,
      -2,     -2,     -2,     -2,
      -2,     -2,     -2,     -2,
      -2,     -2,     -2,     -2,
      -2,     -2,     -2,     -2,
      -2,     -1,     -1,     -1,
      -1,     -1,     -1,      0,
       0,      0,      0,      0,
};
}

const float a0 = (440.0f / 8.0f) / 48000.0f;

inline float NoteToFrequency(float midi_note)
{
    midi_note -= 9.0f;
    CONSTRAIN(midi_note, -128.0f, 127.0f);
    return a0 * 0.25f * stmlib::SemitonesToRatio(midi_note);
}

static float _pitch = 0.f;
static int32_t _word = 0;
static float _speed = 0.3f;
static float _formant_shift = 0.5f;
static float _prosody = 0.5f;
static float _out_aux_mix = 0.0f;

struct
{
    uint8_t bank;
    float addr;
} _words[92];

static plaits::LPCSpeechSynthController lpc_speech_synth_controller_;
static plaits::LPCSpeechSynthWordBank lpc_speech_synth_word_bank_;

static uint8_t buffer[16384] = {};
static stmlib::BufferAllocator allocator;

void engine::setup()
{
    allocator.Init(buffer, 16384);

    lpc_speech_synth_word_bank_.Init(plaits::word_banks_,
                                     LPC_SPEECH_SYNTH_NUM_WORD_BANKS,
                                     &allocator);
    int j = 0;
    for (uint8_t i = 0; i < LPC_SPEECH_SYNTH_NUM_WORD_BANKS; i++)
    {
        lpc_speech_synth_word_bank_.Load(i);

        for (int l = 0; l < lpc_speech_synth_word_bank_.num_words_; l++)
            _words[j++] = {i, 1.f / lpc_speech_synth_word_bank_.num_words_ * l};
    }

    lpc_speech_synth_controller_.Init(&lpc_speech_synth_word_bank_);

    engine::addParam(V_OCT, &_pitch);
    engine::addParam("Word", &_word, 0, LEN_OF(_words) - 1);
    engine::addParam("Speed", &_speed);
    engine::addParam("Form.Shift", &_formant_shift);
    engine::addParam("Prosody", &_prosody);
    engine::addParam("AuxMix", &_out_aux_mix);
}

void engine::process()
{
    auto note = (float)DEFAULT_NOTE;

    note += engine::cv() * 12;

    const float f0 = NoteToFrequency(note);

    auto _out = engine::outputBuffer<0>();
    auto _aux = engine::outputBuffer<1>();

    lpc_speech_synth_controller_.Render(false,
                                        engine::trig(),
                                        _words[_word].bank, // Bank
                                        f0,
                                        _prosody,
                                        _speed,             // Speed
                                        _words[_word].addr, // Word
                                        _formant_shift,
                                        1.0f,
                                        _aux,
                                        _out,
                                        FRAME_BUFFER_SIZE);

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        _out[i] = stmlib::Crossfade(_out[i], _aux[i], _out_aux_mix);
}