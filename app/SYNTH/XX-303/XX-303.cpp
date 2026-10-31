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

#undef SAMPLE_RATE
#define protected public

// SYNTH/XX-303 (until 2026-10 SYNTH/Open303) on lib/open303: Open303 with characters, chosen by the parameter
// Character: Open303's own voicing (the default), the TB-03's or ReBirth's (knob curves, envelopes, accent and pulse,
// fitted to recordings of the TB-03 and of ReBirth RB-338's 303: characters/), the analog TB-303's (from its
// circuit) or AudioRealism ABL3's (fitted to recordings of it), see README.md. The 303's knobs on the first page,
// Wave, Character, Drive and its Type on the second. Wave AUX puts the AUX-INPUT in place of the oscillator: the 303
// as a filter and VCA for another signal, played by the notes. With Character Open303 it is the Open303 of before
// (one engine per wave, SYNTH/Open303-SAW/-SQR: the original Open303, which sounds the same with the saw and 0.44 dB
// apart with the square).

#include "open303/src/rosic_DecayEnvelope.cpp"
#include "open303/src/rosic_LeakyIntegrator.cpp"
#include "open303/src/wavetable_gen/rosic_MipMappedWaveTable.cpp"
#include "open303/src/wavetable_gen/rosic_FourierTransformerRadix2.cpp"
#include "open303/src/wavetable_gen/rosic_Complex.cpp"
#include "open303/src/rosic_RealFunctions.cpp"
#include "open303/src/rosic_Open303.cpp"
#include "open303/src/rosic_FunctionTemplates.cpp"
#include "open303/src/GlobalFunctions.cpp"
#include "open303/src/rosic_BlendOscillator.cpp"
#include "open303/src/rosic_BiquadFilter.cpp"
#include "open303/src/rosic_NumberManipulations.cpp"
#include "open303/src/rosic_OnePoleFilter.cpp"
#include "open303/src/rosic_MidiNoteEvent.cpp"
#include "open303/src/rosic_TeeBeeFilter.cpp"

#include <new>
#include "stmlib/stmlib.h"
#include "open303/src/rosic_Open303.h"
#include "open303/src/wavetable_gen/rosic_MipMappedWaveTable.h"
#include "characters/Characters.h" // characters[], characterNames[]
#include "drive/Drive.h"           // the drive: five curves, antialiased (aanl.lib)

ENGINE_NAME("SYNTH/XX-303");
// the release's (2026-04) engines it took over, with their parameters: restore()
ENGINE_ALIAS("SYNTH/Open303", "ffffff"); // V_OCT, Acc, Cutoff, Reso, EnvMod, Dec
ENGINE_ALIAS("FILTER/TeeBee", "fffff");  // Cutoff, Res, EnvMode, Decay, Accent

// one mip-mapped table, the saw: the pulse is made from it
static rosic::WaveTable _wave;
static rosic::Open303 open303(&_wave, &_wave);

static float _pitch = -2.f; // V_OCT at C2: 0V -> MIDI 36, the 303's C, as in RB303 and TB-03
static float _acc = 0.5f;
static float _cutoff = 0.25f;
static float _res = 0.33f;
static float _env = 0.25f;
static float _dec = 0.5f;
static int32_t _waveform = 1; // 0 saw, 1 pulse (the default, as the former single engine SYNTH/Open303 had), 2 AUX
static const char *_waveforms[] = {"SAW", "SQR", "AUX"};
static int32_t _char = 0; // the character: characters[] (0 Open303, 1 TB03, 2 RB303, 3 TB303, 4 ABL3, ...)
static float _drive = 0.f; // the diode drive, 0 off
static int32_t _driveType = Drive::DIODE; // the drive's curve: DIODE (the default, the drive before) ... FOLD
static const char *_driveTypes[] = {"DIODE", "TANH", "HARD", "ASINH", "FOLD"};
static Drive _driveFx;
static float _note;
static bool _gate;

// The patches of the release (github.com/eh2k/squares-and-circles, 2026-04). A patch of SYNTH/Open303 with more
// parameters (in between; OLD nullptr) loads as it is. SYNTH/XX-303: 0 V_OCT, 1 Acc, 2 Cutoff, 3 Reso, 4 EnvMod, 5 Dec,
// 6 Wave, 7 Character, 8 Drive, 9 Type
void engine::restore(uint32_t id, const engine::patch_param *old, int n, engine::patch_param *params)
{
    if (old == nullptr)
        return;
    if (id == ENGINE_ID("SYNTH/Open303"))
    {
        // 0 V with V_OCT at C4 was MIDI 60 there as here (DEFAULT_NOTE + 24 against the firmware's 2 octaves down). Its
        // accent came with every note while Acc was above 0, now from the Accent input
        for (int i = 0; i < 6; i++)
            params[i] = old[i];
    }
    else if (id == ENGINE_ID("FILTER/TeeBee"))
    {
        // the 303 filter and its envelope for another signal (the ranges of Open303's): Wave AUX, now with the VCA
        params[2] = old[0]; // Cutoff
        params[3] = old[1]; // Res
        params[4] = old[2]; // EnvMode
        params[5] = old[3]; // Decay
        params[1] = old[4]; // Accent
        params[6].value = 2; // _waveforms: AUX
    }
}

void engine::setup()
{
    engine::addParam(V_OCT, &_pitch); // pitch is summed with CV and quantized
    engine::addParam("Acc", &_acc);
    engine::addParam("Cutoff", &_cutoff);
    engine::addParam("Reso", &_res);
    engine::addParam("EnvMod", &_env);
    engine::addParam("Dec", &_dec);
    // the second page (L/R): the wave, the character, the drive and its type
    engine::addParam("\fWave\n%s", &_waveform, 0, LEN_OF(_waveforms) - 1, _waveforms);
    engine::addParam("Character\n%s", &_char, 0, numCharacters - 1, characterNames);
    engine::addParam("Drive", &_drive);
    engine::addParam("Type\n%s", &_driveType, 0, LEN_OF(_driveTypes) - 1, _driveTypes);

    // the saw's mip map (the pulse is made from the saw). Its prototype and FFT (12.5 KB) and the FFT's spectrum
    // (8 KB) on the heap for the while: on the stack they took 21 KB of the DSM's 22.5 at the load
    bool generated = false;
    if (void *mem = malloc(sizeof(rosic::MipMappedWaveTable)))
    {
        auto tmp = new (mem) rosic::MipMappedWaveTable(); // its FFT's buffers are on the heap anyway
        tmp->fillWithSaw303();
        generated = tmp->generateMipMap(_wave);
        tmp->~MipMappedWaveTable();
        free(mem);
    }
    if (!generated)
        gfx::message("OUT OF MEMORY"); // the oscillator stays silent

    open303.setSampleRate(SAMPLE_RATE, 1);
    open303.setTuning(open303.tuning);
    open303.setAccentAttack(open303.accentAttack);
    open303.setPitchBend(0);
    open303.setSlideTime(open303.slideTime);
    open303.setVolume(-12);
    open303.filter.setMode(rosic::TeeBeeFilter::TB_303);
}

void engine::process()
{
    CONSTRAIN(_char, 0, numCharacters - 1); // a patch from a version with more characters
    open303.setCharacter(characters[_char]); // a change takes effect with the next note
    const bool aux = _waveform == 2;
    if (!aux)
        open303.setWaveform(_waveform); // the saw or the pulse, at once
    open303.setAccent(1.f + _acc * 99.f);
    open303.setCutoff(rosic::linToExp(_cutoff, 0.0, 1.0, 314.0, 2394.0));
    open303.setResonance(1.f + _res * 99.f);
    open303.setEnvMod(1.f + _env * 99.f);
    open303.setDecay(rosic::linToExp(_dec, 0.0, 1.0, 200.0, 2000.0)); // accented notes: the character's

    _note = (float)DEFAULT_NOTE + engine::cv() * 12;
    CONSTRAIN(_note, 0, 128);

    _gate |= engine::gate();

    if (engine::trig() > 0)
        open303.triggerNote(_note, engine::accent() != 0); // the accent input; Acc is how much an accent adds;
                                                          // the exact pitch, slides while the gate is held
    else if (!engine::gate())
    {
        open303.releaseNote(_note);
        _gate = false;
    }
    else
        open303.oscFreq = rosic::pitchToFreq(_note, open303.tuning);

    auto buffer = engine::outputBuffer<0>();
    if (aux) // the AUX-INPUT, (L + R) / 2, into the voice in place of the oscillator, sample by sample in place
    {
        auto inL = engine::inputBuffer<0>();
        auto inR = engine::inputBuffer<1>();
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
            buffer[i] = (inL[i] + inR[i]) * 0.5f;
    }
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        buffer[i] = open303.getSample(aux ? &buffer[i] : nullptr);
    _driveFx.set(_drive, _driveType);
    _driveFx.process(buffer, FRAME_BUFFER_SIZE);
}

void engine::screensaver()
{
    static uint32_t _r = 0;
    static const uint8_t _screenSaver[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x3f, 0x00, 0x00, //>
        0x00, 0x80, 0x0f, 0xe0, 0x01, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x07, 0x00,
        0x00, 0x30, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x30, 0x00,
        0x00, 0x06, 0x00, 0x00, 0x60, 0x00, 0x00, 0x03, 0x00, 0x00, 0x80, 0x00,
        0x80, 0x01, 0x06, 0xc0, 0x00, 0x01, 0xc0, 0x00, 0x0f, 0xe0, 0x01, 0x03,
        0x60, 0x00, 0x0f, 0xe0, 0x01, 0x06, 0x30, 0x80, 0x1f, 0xf0, 0x03, 0x0c,
        0x10, 0x80, 0x1f, 0xf0, 0x03, 0x08, 0x18, 0x80, 0x1f, 0xf0, 0x03, 0x18,
        0x08, 0x80, 0x1f, 0xf0, 0x03, 0x10, 0x08, 0x80, 0x1f, 0xf0, 0x03, 0x30,
        0x04, 0x80, 0x1f, 0xf0, 0x03, 0x20, 0x04, 0x80, 0x1f, 0xf0, 0x03, 0x20,
        0x04, 0x80, 0x1f, 0xf0, 0x03, 0x60, 0x06, 0x00, 0x0f, 0xe0, 0x01, 0x40,
        0x02, 0x00, 0x0f, 0xe0, 0x01, 0x40, 0x02, 0x00, 0x06, 0xc0, 0x00, 0x40,
        0x02, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x40,
        0xe2, 0x07, 0x00, 0x00, 0xe0, 0x47, 0xc2, 0x03, 0x00, 0x00, 0xc0, 0x43,
        0x82, 0x01, 0x00, 0x00, 0x80, 0x41, 0x82, 0x01, 0x00, 0x00, 0x80, 0x41,
        0x86, 0x01, 0x00, 0x00, 0xc0, 0x60, 0x04, 0x03, 0x00, 0x00, 0xc0, 0x60,
        0x04, 0x07, 0x00, 0x00, 0xc0, 0x20, 0x0c, 0x06, 0x00, 0x00, 0x60, 0x20,
        0x08, 0x0e, 0x00, 0x00, 0x70, 0x30, 0x08, 0x1c, 0x00, 0x00, 0x30, 0x10,
        0x18, 0x38, 0x00, 0x00, 0x1c, 0x18, 0x10, 0x70, 0x00, 0x00, 0x0e, 0x08,
        0x20, 0xe0, 0x00, 0x00, 0x0f, 0x0c, 0x60, 0xc0, 0x03, 0xc0, 0x03, 0x06,
        0xc0, 0x00, 0x0f, 0xf0, 0x00, 0x02, 0x80, 0x01, 0xfc, 0x3f, 0x00, 0x01,
        0x00, 0x03, 0xe0, 0x07, 0xc0, 0x00, 0x00, 0x06, 0x00, 0x00, 0x60, 0x00,
        0x00, 0x0c, 0x00, 0x00, 0x30, 0x00, 0x00, 0x30, 0x00, 0x00, 0x1c, 0x00,
        0x00, 0xe0, 0x00, 0x00, 0x07, 0x00, 0x00, 0x80, 0x0f, 0xf0, 0x01, 0x00,
        0x00, 0x00, 0xf8, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xf8, 0x3f, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0x01, 0x00, //>
        0x00, 0xe0, 0xff, 0xff, 0x07, 0x00, 0x00, 0xf0, 0x0f, 0xe0, 0x1f, 0x00,
        0x00, 0xfc, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x7c, 0x00,
        0x00, 0x0f, 0x00, 0x00, 0xf0, 0x00, 0x80, 0x07, 0x06, 0xc0, 0xe0, 0x01,
        0xc0, 0x03, 0x0f, 0xe0, 0x81, 0x03, 0xe0, 0x81, 0x1f, 0xf0, 0x83, 0x07,
        0xf0, 0x80, 0x1f, 0xf0, 0x03, 0x0f, 0x78, 0xc0, 0x3f, 0xf8, 0x07, 0x1e,
        0x38, 0xc0, 0x3f, 0xf8, 0x07, 0x1c, 0x3c, 0xc0, 0x3f, 0xf8, 0x07, 0x3c,
        0x1c, 0xc0, 0x3f, 0xf8, 0x07, 0x38, 0x1c, 0xc0, 0x3f, 0xf8, 0x07, 0x78,
        0x0e, 0xc0, 0x3f, 0xf8, 0x07, 0x70, 0x0e, 0xc0, 0x3f, 0xf8, 0x07, 0x70,
        0x0e, 0xc0, 0x1f, 0xf0, 0x07, 0xf0, 0x0f, 0x80, 0x1f, 0xf0, 0x03, 0xe0,
        0x07, 0x80, 0x0f, 0xe0, 0x03, 0xe0, 0x07, 0x00, 0x0f, 0xe0, 0x01, 0xe0,
        0x07, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe7, 0x07, 0x00, 0x00, 0xe0, 0xe7,
        0xf7, 0x07, 0x00, 0x00, 0xe0, 0xef, 0xe7, 0x07, 0x00, 0x00, 0xe0, 0xe7,
        0xc7, 0x03, 0x00, 0x00, 0xc0, 0xe3, 0xc7, 0x03, 0x00, 0x00, 0xc0, 0xe3,
        0xcf, 0x03, 0x00, 0x00, 0xe0, 0xf1, 0x8e, 0x07, 0x00, 0x00, 0xe0, 0xf1,
        0x8e, 0x0f, 0x00, 0x00, 0xe0, 0x71, 0x1e, 0x0f, 0x00, 0x00, 0xf0, 0x70,
        0x1c, 0x1f, 0x00, 0x00, 0xf8, 0x78, 0x1c, 0x3e, 0x00, 0x00, 0x7c, 0x38,
        0x3c, 0x7c, 0x00, 0x00, 0x3e, 0x3c, 0x38, 0xf8, 0x00, 0x00, 0x1f, 0x1c,
        0x70, 0xf0, 0x03, 0xc0, 0x1f, 0x1e, 0xf0, 0xe0, 0x0f, 0xf0, 0x0f, 0x0f,
        0xe0, 0xc1, 0xff, 0xff, 0x03, 0x07, 0xc0, 0x03, 0xff, 0xff, 0xc0, 0x03,
        0x80, 0x07, 0xfc, 0x3f, 0xe0, 0x01, 0x00, 0x0f, 0xe0, 0x07, 0xf0, 0x00,
        0x00, 0x3e, 0x00, 0x00, 0x7c, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x3f, 0x00,
        0x00, 0xf0, 0x0f, 0xf0, 0x1f, 0x00, 0x00, 0xe0, 0xff, 0xff, 0x07, 0x00,
        0x00, 0x80, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0xf8, 0x3f, 0x00, 0x00};

    gfx::clearRect(0, 0, 128, 64);

    for (int i = 23; i > 0; i--)
    {
        if ((_r & (1 << i)))
            gfx::drawCircle(64, 32, 26 + powf(i, 1.3f));
    }

    if (_gate)
    {
        _r |= 1;
        gfx::drawXbm(40, 8, 48, 48, &_screenSaver[sizeof(_screenSaver) / 2]);
    }
    else
        gfx::drawXbm(40, 8, 48, 48, &_screenSaver[0]);

    _r <<= 1;
}