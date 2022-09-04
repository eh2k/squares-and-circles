// Copyright (C)2021 - Eduard Heidt
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

#include "machine.h"
#include "stmlib/dsp/dsp.h"
#include "base/SampleEngine.hxx"

namespace sam
{
#include "SAM/sam.c"
#include "SAM/render.c"
#include "SAM/reciter.c"
#include "SAM/debug.c"
}

using namespace sam;
using namespace machine;

static uint8_t *s_buffer;
static int s_maxlen;
static int s_len;

struct SAM : public SampleEngine
{
    uint8_t _buffer[48000];
    tsample_spec<uint8_t> _sounds[6] = {
        {">electro", _buffer, 0, 22050, 0},
        {">techno", _buffer, 0, 22050, 0},
        {">modular", _buffer, 0, 22050, 0},
        {">synthesizer", _buffer, 0, 22050, 0},
        {">oscillator", _buffer, 0, 22050, 0},
        {">eurorack", _buffer, 0, 22050, 0},
    };

    int say(const char *text)
    {
        SAM_write_buffer = [](int pos, char value)
        {
            if (pos < s_maxlen)
            {
                s_len = pos + 1;
                s_buffer[pos] = value;
            }
        };

        s_len = 0;
        s_maxlen = sizeof(_buffer);
        s_buffer = _buffer;

        SetSpeed(64);
        SetMouth(128);
        SetPitch(64 * 2);
        SetThroat(128);

        char input[256] = {};
        sprintf(input, "%s ", text);
        for (int i = 0; input[i] != 0; i++)
            input[i] = toupper((int)input[i]);

        strncat(input, "[", 255);
        TextToPhonemes((unsigned char *)input);
        SetInput(input);
        SAMMain();
        return s_len;
    }

    int _lastSelection = 0;

public:
    SAM() : SampleEngine(&_sounds[0], 0, LEN_OF(_sounds))
    {
        _sounds[0].len = say(&_sounds[0].name[1]);
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        if (_lastSelection != selection)
        {
            _sounds[selection].len = say(&_sounds[selection].name[1]);
            _lastSelection = selection;
        }

        SampleEngine::process(frame, of);
    }
};

void init_sam()
{
    machine::add<SAM>("SPEECH", "SAM");
}

MACHINE_INIT(init_sam);