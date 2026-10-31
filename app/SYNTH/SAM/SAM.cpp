// Copyright (C)2022 - Eduard Heidt
//
// Author: Eduard Heidt (eh2k@gmx.de)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//

#include "../../squares-and-circles-api.h"

#include <stdlib.h>

ENGINE_ALIAS("SPEECH/SAM"); // the patches saved before the app moved to SYNTH/SAM (the group SPEECH is gone)

extern "C"
{
#include "sam_voice/sam.c"
}

static const char *_words[] = {
    "ELECTRO",
    "TECHNO",
    "MODULAR",
    "SYNTHESIZER",
    "OSCILLATOR",
    "EURORACK",
    "RND_0-9",
};

constexpr int RND = LEN_OF(_words) - 1;

// the phonemes of the words and of the digits 0-9 (RND_0-9): sam_voice_text_to_phonemes() of them
// (lib/sam_voice/sam_reciter.c, 460k instructions in setup())
static const char *const _phonemes[RND + 10] = {
    " EHLEHKTROW\x9B", " TEHKNOW\x9B", " MAAJAXLAA5R\x9B", " SIHNTHEHZAYZER\x9B", " AASIHLAETER\x9B", " YUW5RAORAEK\x9B",
    "  ZIY4ROW\x9B", "  WAH4N\x9B", "  TUW4\x9B", "  THRIY4\x9B", "  FOH4R\x9B",
    "  FAY4V\x9B", "  SIH4KS\x9B", "  SEH4VUN\x9B", "  EY4T\x9B", "  NAY4N\x9B",
};
static_assert(LEN_OF(_words) == 7, "_phonemes are of the words");

static int32_t _word = 0;
static float _speed = 0.75f;
static float _mouth = 0.5f;
static float _pitch = 0.5f;
static float _throat = 0.5f;

// sam_voice renders the word while it plays: a few samples per frame instead of the whole word at the trigger
static sam_voice_t _sam;
static uint8_t _in[16];
static int _in_n = 0, _in_i = 0;

// SAM renders at 22050 Hz, interpolated linearly between the samples _a and _b
constexpr uint32_t SAM_STEP = (22050u << 16) / SAMPLE_RATE;
static uint32_t _frac = 0; // 0.16, from _a to _b
static uint8_t _a, _b;
static bool _playing = false;
static bool _start = false; // parsed: the first frames in the next process()

static bool next_sample(uint8_t *v)
{
  if (_in_i == _in_n)
  {
    _in_n = sam_voice_render(&_sam, _in, sizeof(_in));
    _in_i = 0;
    if (_in_n == 0)
      return false;
  }
  *v = _in[_in_i++];
  return true;
}

static void say()
{
  const int i = _word == RND ? RND + rand() % 10 : _word;

  const int speed = (1.f - _speed) * 255; // 0 would be the slowest (256)
  _sam.speed = speed < 1 ? 1 : speed;
  _sam.mouth = _mouth * 255;
  _sam.pitch = _pitch * 255;
  _sam.throat = _throat * 255;

  _playing = false;
  _start = sam_voice_parse(&_sam, _phonemes[i]);
}

void engine::setup()
{
  // param_init("PTICH", &_pitch);
  engine::addParam("Speed", &_speed);
  engine::addParam("@Words", &_word, 0, LEN_OF(_words) - 1, _words);
  engine::addParam("Mouth", &_mouth);
  engine::addParam("Throat", &_throat);

  sam_voice_init(&_sam);
}

void engine::process()
{
  auto outputL = engine::outputBuffer<0>();

  if (_start) // the trigger of the frame before
  {
    _start = false;
    sam_voice_start(&_sam);
    _in_n = _in_i = 0;
    _frac = 0;
    _playing = next_sample(&_a) && next_sample(&_b);
  }

  for (int j = 0; j < FRAME_BUFFER_SIZE; j++)
  {
    if (_playing)
    {
      const float a = ((float)_a - SAM_VOICE_SILENCE) / 128;
      const float b = ((float)_b - SAM_VOICE_SILENCE) / 128;
      outputL[j] = a + (b - a) * ((float)_frac / 65536.f);
      _frac += SAM_STEP;
      if (_frac >> 16)
      {
        _frac &= 0xFFFF;
        _a = _b;
        _playing = next_sample(&_b);
      }
    }
    else
      outputL[j] = 0;
  }

  // a trigger: the phonemes now, after this frame of the word before, the first frames in the next process() (the
  // work of sam_voice_say() in two frames, the word starts 0.5 ms later)
  if (engine::trig())
    say();
}
