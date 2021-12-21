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
#include "stmlib/dsp/units.h"
#include "machine.h"
#include <Arduino.h>

using namespace machine;

struct MidiHandlerImpl : MidiHandler
{
    struct MidiState
    {
        static const int nMax = 4;
        MidiNote note[nMax] = {};
    };

    MidiState midi[16] = {}; // midi-state for all channels
    uint32_t midi_clock = 0; // millis()
    float midi_bpm = 0;
    bool playing = false;

    static MidiHandlerImpl *instance()
    {
        static MidiHandlerImpl _instance;
        return &_instance;
    }

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
        for (int i = 0; i < 4; i++)
            if (machine::get_engine(i)->midi_ch == channel)
                return true;

        return false;
    }

    static inline void NoteOn(uint8_t channel, uint8_t key, uint8_t velocity)
    {
        for (int i = 0; i < MidiState::nMax; i++)
        {
            auto &note = instance()->midi[channel].note[i];
            if (note.on == 0)
            {
                note.on = millis();
                note.key = key;
                note.velocity = velocity;
                break;
            }
        }
    }

    static inline void NoteOff(uint8_t channel, uint8_t key, uint8_t velocity)
    {
        for (int i = 0; i < MidiState::nMax; i++)
        {
            auto &note = instance()->midi[channel].note[i];
            if (note.on && note.key == key)
            {
                note.on = 0;
                break;
            }
        }
    }

    static inline void PitchBend(uint8_t channel, uint16_t pitch_bend)
    {
        for (int i = 0; i < MidiState::nMax; i++)
            instance()->midi[channel].note[i].pitch = (int16_t)pitch_bend - 8192;
    }

    static inline void ControlChange(uint8_t channel, uint8_t controller, uint8_t value)
    {
        for (int i = 0; i < 4; i++)
        {
            auto engine = machine::get_engine(i);
            if (engine->midi_ch == channel)
            {
                if (controller >= 32 && controller < 40)
                    engine->SetParam(controller - 32, (uint16_t)value << 9); //MSB
            }
        }
    }

    static void ProgramChange(uint8_t channel, uint8_t program)
    {
        for (int i = 0; i < 4; i++)
            if (machine::get_engine(i)->midi_ch == channel)
                machine::load_engine(i, program);
    }

    static void Clock()
    {
        static uint32_t last_clock = 0;
        static uint8_t i = 0;

        if (++i > 24)
        {
            i = 0;
            uint32_t ticks = micros() - last_clock;
            auto bpm = (60 * 1000.0f * 1000.0f) / ticks * (25.f / 24);
            
            if (abs(bpm - instance()->midi_bpm) > 1)
                instance()->midi_bpm = bpm;

            ONE_POLE(instance()->midi_bpm, bpm, 0.1f);

            last_clock = micros();
        }

        instance()->midi_clock = millis();
    }

    static void Start()
    {
        instance()->playing = true;
    }

    static void Continue()
    {
        instance()->playing = !instance()->playing;
    }

    static void Stop()
    {
        Reset();
    }

    static void ActiveSensing()
    {
    }

    static void RawMidiData(uint8_t status, uint8_t *data, uint8_t data_size, uint8_t accepted_channel)
    {
    }

    static void Reset()
    {
        instance()->midiReset();
    }

    FASTRUN void midiReceive(uint8_t midiByte) override
    {
        static stmlib_midi::MidiStreamParser<MidiHandlerImpl> midi_stream;
        midi_stream.PushByte(midiByte);
    }

    void midiReset() override
    {
        playing = false;
        memset(midi, 0, sizeof(midi));

        for (int channel = 0; channel < 4; channel++)
        {
            for (int i = 0; i < MidiState::nMax; i++)
            {
                midi[channel].note[i].reset();
            }
        }
    }

    bool getMidiNote(int midi_channel, int midi_voice, MidiNote *note) override
    {
        if (midi_voice < 4)
        {
            memcpy(note, &midi[midi_channel].note[midi_voice], sizeof(MidiNote));
            return note->on;
        }
        else
            return false;
    }

    bool getPlaybackInfo(float *bpm) override
    {
        if (millis() - midi_clock > 100)
            midi_bpm = 0;

        *bpm = midi_bpm;
        return playing;
    }
};

namespace machine
{
    MidiHandler *midi_handler = MidiHandlerImpl::instance();
}