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

#include "../squares-and-circles-api.h"

#include <string.h>
#include <stdio.h>
#include <algorithm>
#include <inttypes.h>

extern "C"
{
#include "SAM/sam.c"
#include "SAM/render.c"
#include "SAM/reciter.c"
}

// static const char *_words[2];
static const char *_words[] = {
    "ELECTRO",
    "TECHNO",
    "MODULAR",
    "SYNTHESIZER",
    "OSCILLATOR",
    "EURORACK",
    "RND_0-9",
};

static char _phenoms[LEN_OF(_words)][256];

static void init_phenoms()
{
  for (size_t i = 0; i < LEN_OF(_words); i++)
  {
    // char input[256] = {};
    // int i = 0;
    // auto text = _words[_word];
    // for (; text[i] != 0; i++)
    //   input[i] = (int)text[i]; // toupper
    // input[i] = '[';
    size_t len = strlen(_words[i]);
    memcpy(_phenoms[i], _words[i], len);
    _phenoms[i][len] = '[';
    _phenoms[i][len + 1] = 0;
    // auto len = snprintf(_phenoms[i], 256, "%s[", _words[i]);
    TextToPhonemes((unsigned char *)_phenoms[i]);
  }
}

static int32_t _word = 0;
static float _speed = 0.75f;
static float _mouth = 0.5f;
static float _pitch = 0.5f;
static float _throat = 0.5f;

static uint8_t _buffer[48000];
static size_t _buffer_len = 0;
static uint32_t _buffer_pos = 0;

void say()
{
  if (_word == LEN_OF(_phenoms) - 1)
  {
    sprintf(_phenoms[_word], "%d[]", rand() % 10);
    TextToPhonemes((unsigned char *)_phenoms[_word]);
  }

  SAM_write_buffer = [](int pos, char value)
  {
    if (pos < (int)LEN_OF(_buffer))
    {
      _buffer_len = pos + 1;
      _buffer[pos] = value;
    }
  };

  SetSpeed((1.f - _speed) * 255);
  SetMouth(_mouth * 255);
  SetPitch(_pitch * 255);
  SetThroat(_throat * 255);

  SetInput(_phenoms[_word]);
  SAMMain();
  _buffer_pos = 0;
}

void engine::setup()
{
  // param_init("PTICH", &_pitch);
  engine::addParam("Speed", &_speed);
  engine::addParam("@Words", &_word, 0, LEN_OF(_words) - 1, _words);
  engine::addParam("Mouth", &_mouth);
  engine::addParam("Throat", &_throat);

  init_phenoms();
}

void engine::process()
{
  auto outputL = engine::outputBuffer<0>();

  if (engine::trig())
  {
    say();
  }

  if (_buffer_pos + 12 < _buffer_len)
  {
    for (int j = 0; j < 24; j += 2)
    {
      float sample = ((float)_buffer[_buffer_pos] - 127) / 128;
      outputL[j] = outputL[j + 1] = sample;
      ++_buffer_pos;
    }
  }
  else
    std::fill_n(outputL, FRAME_BUFFER_SIZE, 0);
}