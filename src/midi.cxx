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

#include "stmlib/stmlib.h"
#include "stmlib/midi/midi.h"
#include "machine.h"
#include <Arduino.h>

struct MidiHandler
{
    static void RawByte(uint8_t byte)
    {
    }

    static void BozoByte(uint8_t byte)
    {
    }

    static void Aftertouch(uint8_t byte, uint8_t byte2, uint8_t byte3 = 0)
    {
    }

    static void SysExEnd()
    {
    }

    static void SysExStart()
    {
    }

    static void SysExByte(uint8_t byte)
    {
    }

    static uint8_t CheckChannel(uint8_t channel)
    {
        return channel < 4;
    }

    static inline void NoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
    {
        for (int i = 0; i < machine::MidiState::nMax; i++)
        {
            if (machine::midi[channel].note[i].on == 0)
            {
                machine::midi[channel].note[i].on = millis();
                machine::midi[channel].note[i].key = note;
                machine::midi[channel].note[i].velocity = velocity;
                break;
            }
        }
    }

    static inline void NoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
    {
        for (int i = 0; i < machine::MidiState::nMax; i++)
        {
            if (machine::midi[channel].note[i].on && machine::midi[channel].note[i].key == note)
            {
                machine::midi[channel].note[i].on = 0;
                break;
            }
        }
    }

    static inline void PitchBend(uint8_t channel, uint16_t pitch_bend)
    {
        for (int i = 0; i < machine::MidiState::nMax; i++)
            machine::midi[channel].note[i].pitch = (int16_t)pitch_bend - 8192;
    }

    static inline void ControlChange(uint8_t channel, uint8_t controller, uint8_t value)
    {
        uint16_t params[8];
        machine::get_engine(channel)->GetParams(params);
        switch (controller)
        {
        case 1:
            params[0] = value << 9;
            break;
        case 2:
            params[1] = value << 9;
            break;
        case 7:
            params[2] = value << 9;
            break;
        case 10:
            params[3] = value << 9;
            break;
        }

        machine::get_engine(channel)->SetParams(params);
    }

    static void ProgramChange(uint8_t channel, uint8_t program)
    {
        machine::midi[channel].program = program;
    }

    static void Clock()
    {
        static uint32_t last_clock = 0;
        static uint8_t i = 0;

        if (++i > 24)
        {
            i = 0;
            uint32_t ticks = micros() - last_clock;
            machine::midi_bpm = (60 * 1000.0f * 1000.0f) / ticks * (25.f / 24);
            last_clock = micros();
        }

        machine::midi_clock = millis();
    }

    static void Start()
    {
    }

    static void Continue()
    {
    }

    static void Stop()
    {
        for (int channel = 0; channel < 4; channel++)
        {
            for (int i = 0; i < machine::MidiState::nMax; i++)
            {
                machine::midi[channel].note[i].on = 0;
            }
        }
    }

    static void ActiveSensing()
    {
    }
    static void Reset()
    {
        memset(machine::midi, 0, sizeof(machine::midi));

        for (int channel = 0; channel < 4; channel++)
        {
            for (int i = 0; i < machine::MidiState::nMax; i++)
            {
                machine::midi[channel].note[i].reset();
            }
        }
    }
    static void RawMidiData(uint8_t status, uint8_t *data, uint8_t data_size, uint8_t accepted_channel)
    {
    }
};

static stmlib_midi::MidiStreamParser<MidiHandler> midiInput;

namespace machine
{
    void midiReset()
    {
        MidiHandler::Reset();
    }

    FASTRUN void midiReceive(uint8_t midiByte)
    {
        midiInput.PushByte(midiByte);
    }
}