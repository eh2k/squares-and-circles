// Copyright (C)2026 - Eduard Heidt
//
// Author: Eduard Heidt (eh2k@gmx.de)
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// SYNTH/VA-303: a TB-303 voice modelled from its circuit (lib/va303, MODEL.md there): the diode ladder with its
// nonlinear pairs, the envelopes and the accent as the schematic has them, the constants fitted to recordings of a
// TB-303. The knobs are the 303's pots as positions 0...1; the Overdrive, the Cutoff range, the Muffler and the Filter
// FM are the Devil Fish's (the oscillator's level into the filter, its Cutoff pot's wider range, a clipper on the VCA's
// output, the VCA's output into the cutoff), the bass mod the x0xb0x's (C20/C21 0.1 uF).

#include "../../squares-and-circles-api.h"
#include "va303/va303.h"
#include "va303/va303_tb303d.h" // dinsync's TB-303 and the user's x0xb0x together: the resonance, the accents at full Reso
#include "va303/va303_tb303e.h" // TB-303d with the x0xb0x's Accent pot: a stronger, longer accent
#include "va303/va303_tb303h.h" // TB-303e with a pattern on the x0xb0x, long bright accents at a low cutoff, the Reso
                                // pot's S and the ring at full Reso of dinsync's TB-303
#include "va303/va303_tb303i.h" // the circuit's ladder with the couplings and the Cutoff pot both units want

ENGINE_NAME("SYNTH/VA-303");
// the 303 of the release (github.com/eh2k/squares-and-circles) it replaces there, SYNTH/XX-303 (2026-10-31 ...
// 2026-11-14), and the release's (2026-04) engines that one had taken over, with their parameters: restore()
ENGINE_ALIAS("SYNTH/XX-303", "ffffffbbfb"); // V_OCT, Acc, Cutoff, Reso, EnvMod, Dec, Wave, Character, Drive, Type
ENGINE_ALIAS("SYNTH/Open303", "ffffff");    // V_OCT, Acc, Cutoff, Reso, EnvMod, Dec
ENGINE_ALIAS("FILTER/TeeBee", "fffff");     // Cutoff, Res, EnvMode, Decay, Accent

static va303::Voice _voice;

static float _pitch = -2.f; // V_OCT at C2: 0 V -> MIDI 36, the 303's C, as SYNTH/XX-303 and SEQ/303-Patterns
static float _acc = 0.5f;
static float _cutoff = 0.5f;
static float _res = 0.5f;
static float _env = 0.5f;
static float _dec = 0.5f;
static int32_t _waveform = 0; // 0 saw, 1 square, 2 AUX: the input in place of the oscillator
static const char *_waveforms[] = {"SAW", "SQR", "AUX"};
// the characters: TB-303d: fitted to dinsync's TB-303 recordings and a x0xb0x (its resonance at a low cutoff, its
// accents at full Reso), with what the band levels don't see (the resonance's ring, the accent's
// first 40 ms and its course); TB-303e: that with the x0xb0x's Accent pot (a stronger, longer accent); TB-303h (the
// default since 2026-11-06): that with a pattern on the x0xb0x (shorter accents at an open cutoff), the accent sweep
// lifting the cutoff's floor (long bright accents at a low cutoff), the Reso pot's S (most of the resonance at three
// quarters of its turn) and a shorter ring at full Reso; TB-303i: the circuit's ladder (its capacitors, the couplings
// into the filter and into the VCA, the Cutoff pot's flatter top), right in the phases of both units, where the others
// bend the capacitors for the band levels. The index a patch stores: until 2026-11-03 1 was CIRCUIT and 2 TB-303b; until
// 2026-11-05 1 TB-303b, 2 TB-303c, 3 TB-303d (TB-303b and TB-303c left out for the flash, TB-303d and TB-303e better on
// the measures: a patch of then plays another one); until 2026-11-08 0 was TB-303, the recordings alone (left out: its
// ladder latches above about 20 times the Overdrive, MODEL.md), so the knob starts at 1; until 2026-11-19 3 was
// TB-303f and 4 TB-303g, the steps from TB-303e to TB-303h (left out: TB-303h as close or closer on every reference;
// no patch had them, the app not released), 5 TB-303h and 6 TB-303i
static int32_t _char = 3; // TB-303h
static const char *_characterNames[] = {"TB-303d", "TB-303e", "TB-303h", "TB-303i"}; // 1 ... 4
static const va303::Params *character(int32_t i)
{
    return i == 4   ? &va303::tb303iParams
           : i == 3 ? &va303::tb303hParams
           : i == 2 ? &va303::tb303eParams
                    : &va303::tb303dParams;
}
// the Overdrive: the Devil Fish's pot in place of R62 (220k, the oscillator into the filter) from the 303's level at 0
// (the voice as without it) to 66.6 times at 1, logarithmic; the pot's range below the 303's level is left out
static float _overdrive = 0.f;
// the Muffler: the Devil Fish's switch on the VCA's output (its circuit unpublished: a clipper above the bass, MODEL.md)
static int32_t _muffler = 0;
static const char *_mufflers[] = {"OFF", "SOFT", "HARD"};
// the bass mod: the x0xb0x's C20 and C21 0.1 uF for 0.01, the coupling into the VCA at 7.2 Hz for 72
static int32_t _bassMod = 0;
static const char *_offOn[] = {"OFF", "ON"};
// the Cutoff range: 303 the 303's pot, DF the Devil Fish's (the 303's range between 0.35 and 0.8 of the knob, an
// octave above it at 1, the notes silent towards 0). In the slot where ResoTop (how far Reso lowers the cutoff, a
// float) was on 2026-11-19 and an empty one before: a patch of then has 0 there (303), or ResoTop's float (constrained)
static int32_t _cutRange = 0;
static const char *_cutRanges[] = {"303", "DF"};
// the Filter FM: the Devil Fish's pot, the VCA's output into the cutoff (0: none; stronger on accents and with the
// Overdrive, chaotic towards 1)
static float _filterFm = 0.f;
static float _note;

// A bare trigger (a pulse whose gate falls within TRIG_PULSE of it, as from a drum sequencer or a clock) holds the note
// MIN_GATE from the trigger, a 303's step (its gate half a 16th at 120 BPM): it played a blip of a block plus the amp's
// hold and fall before. A longer gate (a sequencer's gate, MIDI) plays as it is; a trigger during such a hold is a new
// note, not a slide (the voice's tie: only a gate held into the trigger slides).
static constexpr int TRIG_PULSE = 20; // frames, 10 ms
static constexpr int MIN_GATE = 125;  // frames, 62.5 ms
static int _sinceTrig = MIN_GATE;     // frames since the trigger, up to MIN_GATE
static bool _pulse;                   // the gate fell within TRIG_PULSE: held to MIN_GATE
static bool _gateIn;                  // the gate input in the frame before

// The level. The voice's gain is the model's: its fit put the recordings' level at 1, where the engine is -29 LUFS
// with the knobs at 0.5 (test_qemu). OUT_GAIN lifts it to -21.5 ... -22.1 LUFS (the four characters): 3 dB under the
// other synths, the room for the accents and the resonance. What passes 1 (the Overdrive: up to 1.5) the firmware
// limits in the mix (ENGINE_MODE_SOFT_LIMIT, after the part of the track's LEVEL below the middle: turned down, it
// saturates less), as SYNTH/Plaits and SYNTH/DxFM.
static constexpr float OUT_GAIN = 2.512f; // +8 dB

// The patches of the release's 303s (the aliases above; a dev module with SYNTH/XX-303 loads that one by its name):
// the knobs by their places, the same layout (V_OCT at C2 as here), the sound VA-303's. SYNTH/XX-303: 0 V_OCT, 1 Acc,
// 2 Cutoff, 3 Reso, 4 EnvMod, 5 Dec, 6 Wave, 7 Character (Open303, TB03, ...: none of them here, the default), 8 Drive
// and 9 Type: none, the Overdrive stays 0 (XX-303's Drive, a curve after the voice, kept the level, -25.5 LUFS at 0.4 in
// test_qemu against -26.3 at 0; the Overdrive drives the ladder louder, -12.4 against -22.1). SYNTH/Open303 and
// FILTER/TeeBee as XX-303 took them over.
void engine::restore(uint32_t id, const engine::patch_param *old, int n, engine::patch_param *params)
{
    if (old == nullptr) // values of another length: loaded as they are
        return;
    if (id == ENGINE_ID("SYNTH/XX-303"))
    {
        for (int i = 0; i < 7; i++) // V_OCT ... Wave
            params[i] = old[i];
    }
    else if (id == ENGINE_ID("SYNTH/Open303"))
    {
        for (int i = 0; i < 6; i++)
            params[i] = old[i];
        params[6].value = 1; // its wave, the pulse: SQR
    }
    else if (id == ENGINE_ID("FILTER/TeeBee"))
    {
        // the 303 filter and its envelope for another signal: Wave AUX, now with the VCA
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
    engine::addParam(V_OCT, &_pitch);
    engine::addParam("Acc", &_acc);
    engine::addParam("Cutoff", &_cutoff);
    engine::addParam("Reso", &_res);
    engine::addParam("EnvMod", &_env);
    engine::addParam("Dec", &_dec);
    // the second page, drawn as the list ("\f\v"; a firmware before 2026-11-09: as knobs): the wave, the character,
    // the Devil Fish's Overdrive and Cutoff range, the Muffler, the bass mod, the Devil Fish's Filter FM (appended: a
    // patch of before loads its values, these at their defaults; the Cutoff range in the slot an empty one held, then
    // ResoTop)
    engine::addParam("\f\vWave\n%s", &_waveform, 0, LEN_OF(_waveforms) - 1, _waveforms); // page 2 as the list
    engine::addParam("Character\n%s", &_char, 1, LEN_OF(_characterNames), _characterNames);
    engine::addParam("Overdrive", &_overdrive);
    engine::addParam("CutRange\n%s", &_cutRange, 0, LEN_OF(_cutRanges) - 1, _cutRanges);
    engine::addParam("Muffler\n%s", &_muffler, 0, LEN_OF(_mufflers) - 1, _mufflers);
    engine::addParam("BassMod\n%s", &_bassMod, 0, LEN_OF(_offOn) - 1, _offOn);
    engine::addParam("FilterFM", &_filterFm);

    _voice.init(SAMPLE_RATE, FRAME_BUFFER_SIZE, character(_char));
    _voice.setNoteDelay(4.f); // the 303's 4 ms from the step to the note
    // the CV input's average (2 ms, the OCT41 4 ms) is only partly at a new note when its trigger comes: within 4 ms
    // of a new note the CV jumps instead of sliding (it slid there 22 ms: a fifth up 253 cents flat after 24 ms)
    _voice.setCvSettle(4.f);
    engine::setMode(ENGINE_MODE_SOFT_LIMIT);
}

void engine::process()
{
    CONSTRAIN(_char, 1, (int32_t)LEN_OF(_characterNames)); // a patch with TB-303 (0), or from a version with more
    _voice.setParams(character(_char));                        // a change takes effect with the next note
    const bool aux = _waveform == 2;
    _voice.setWave(_waveform);
    _voice.setKnobs(_cutoff, _res, _env, _dec, _acc);
    _voice.setOverdrive(_overdrive > 0 ? expf(_overdrive * 4.1987f) : 1.f); // 66.6^x: ln 66.6 = 4.1987
    CONSTRAIN(_cutRange, 0, (int32_t)LEN_OF(_cutRanges) - 1); // a patch of 2026-11-19: ResoTop's float there
    _voice.setDfCutoff(_cutRange != 0);
    _voice.setFilterFm(_filterFm);
    CONSTRAIN(_muffler, 0, (int32_t)LEN_OF(_mufflers) - 1);
    _voice.setMuffler(_muffler);
    _voice.setBassMod(_bassMod != 0);

    _note = (float)DEFAULT_NOTE + engine::cv() * 12;
    CONSTRAIN(_note, 0, 128);
    const bool trig = engine::trig() > 0;
    const bool gateIn = engine::gate() != 0;
    const bool tie = trig && _gateIn; // the gate held into the trigger: a slide
    if (trig)
    {
        _sinceTrig = 0;
        _pulse = false;
    }
    else if (_sinceTrig < MIN_GATE)
        _sinceTrig++;
    if (!gateIn && _sinceTrig < TRIG_PULSE)
        _pulse = true;
    const bool gate = gateIn || (_pulse && _sinceTrig < MIN_GATE);
    _gateIn = gateIn;

    auto buffer = engine::outputBuffer<0>();
    const float *in = nullptr;
    if (aux) // the AUX-INPUT, (L + R) / 2, in place of the oscillator, in place
    {
        auto inL = engine::inputBuffer<0>();
        auto inR = engine::inputBuffer<1>();
        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
            buffer[i] = (inL[i] + inR[i]) * 0.5f;
        in = buffer;
    }
    // a trigger starts a note (a slide while the gate is still on), the gate's end releases it, a CV change while the
    // gate is held slides; the accent input at the trigger
    _voice.block(_note, trig, gate, engine::accent() != 0, buffer, in, tie);
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        buffer[i] *= OUT_GAIN;
}

void engine::draw()
{
}
