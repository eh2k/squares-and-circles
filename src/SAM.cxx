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
#include <ctype.h>
#include <stdlib.h>

extern "C"
{
#include "SAM/sam.h"
#include "SAM/reciter.h"
}

using namespace machine;

struct SAM : public SampleEngine
{
    uint8_t _buffer[48000];
    tsample_spec<uint8_t> _sounds[7] = {
        {"electro", _buffer, 0, 22050, 0},
        {"techno", _buffer, 0, 22050, 0},
        {"modular", _buffer, 0, 22050, 0},
        {"synthesizer", _buffer, 0, 22050, 0},
        {"oscillator", _buffer, 0, 22050, 0},
        {"eurorack", _buffer, 0, 22050, 0},
        {"RND_1-9", _buffer, 0, 22050, 0},
    };

    uint8_t _ti = 0;

    void say(const char *text)
    {
        static tsample_spec<uint8_t> *_sample = nullptr;
        _sample = &_sounds[selection];

        SAM_write_buffer = [](int pos, char value)
        {
            if (pos < (int)LEN_OF(_buffer))
            {
                _sample->len = pos + 1;
                ((uint8_t *)_sample->data)[pos] = value;
            }
        };

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
    }

    int _lastSelection = 0;

public:
    SAM() : SampleEngine()
    {
        setup(&_sounds[0], 0, LEN_OF(_sounds));
        say(_sounds[0].name);
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        if (selection == LEN_OF(_sounds) - 1 && frame.trigger)
        {
            char tmp[20];
            sprintf(tmp, "%d", rand() % 10);
            say(tmp);
        }
        else if (_lastSelection != selection)
        {
            say(_sounds[selection].name);
            _lastSelection = selection;
        }

        SampleEngine::process(frame, of);
    }
};

void init_sam()
{
    machine::add<SAM>("SPEECH", "SAM");
}