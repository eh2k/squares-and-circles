> Feedback welcome: Feel free to leave a message or any hints in the ["General Feedback"](https://github.com/eh2k/squares-and-circles/issues/1) issue.

# □︎●︎ [![WebFlasher](https://img.shields.io/badge/Web-Flasher-blue)](https://eh2k.github.io/□●/flash/?firmware) [![MODULAGrid](https://img.shields.io/badge/MODULAR-Grid-white)](https://www.modulargrid.net/e/modules/browser?SearchName=squares+and+circles&SearchShowothers=1)

<!-- ⧉⦾ ⧇ ⟥⧂ -->
**squares-and-circles** is an alternate firmware for the Eurorack module Ornament & Crime (O_C), specifically targeting the Teensy 4.0.

>  <a href="CHANGELOG.txt">CHANGELOG.txt</a> · New here? Start with the <a href="quick-start.md">Quick start</a>

## ■ Demos

| [![](https://img.youtube.com/vi/WQj3YqGpxRU/0.jpg)](https://www.youtube.com/watch?v=WQj3YqGpxRU) | [![](https://img.youtube.com/vi/QdlwETEaE3A/0.jpg)](https://youtu.be/QdlwETEaE3A) | [![](https://img.youtube.com/vi/lb-pbm1ddRw/0.jpg)](https://youtu.be/lb-pbm1ddRw) | [![](https://img.youtube.com/vi/J1vmVwwdVU4/0.jpg)](https://youtu.be/J1vmVwwdVU4)
|--|--|--|--|

## Acknowledgments & Credits

Thanks to the creators of ornament & crime (o_C) and PJRC for the Teensy, and all those who have worked on the [code and algorithms](https://github.com/eh2k/squares-and-circles/wiki/credits) that are reused here, especially Mutable Instruments.
They served a great source of learning and provided a huge playground and the foundation for this project.

## Motivation / Challenge

<img align="right" src="doc/u_oc.png" width=160px />


Given are the following ingredients: Two buttons, two encoders and a 128x64 display. Sixteen I/O ports (4x trigs, 4x cv and 4x dac) and a Cortex-M7.

Damn many possibilities to complicate it. Mono, stereo and then again CV. A tricky task to design a simple UI logic, not get lost in menu diving, and to get as much out of the hardware as possible (work in progress).

## Concept

Similar to the Monomachine and Machinedrum, this device features four fully configurable engine instances. Each instance can be assigned a synthesizer, effects, or sequencer engine, which can be controlled via trigger inputs, CV inputs, or clock signals.

The available engine types include drums, synthesizers, oscillators, sequencers, and audio processors. The hardware ports and the generated output signals can be internally routed to any engine. For example, you can route the mono audio signal from an oscillator engine to a neighboring effects engine with stereo outputs, enabling seamless signal chaining.

* [Short Press [LEFT]/[RIGHT]] scrolls through the 4 machine-tracks; an engine with a second page of parameters (SYNTH/VA-303) comes with its pages on the way, then the pages of the track's insert effects (`+1 FX/...`, see [Insert FX](#insert-fx)) (track 1 page 1, track 1 page 2, track 1 insert, track 2, ...; the channel bar shows the page in the track's segment). A drum kit (DRUMKITS/*) has its mix page after its parameters, while two instruments or more play (their own TRIGs, MIDI): LEVEL and PAN of each instrument.
* [Long press [LEFT]] enters the machine-selection-page.
* [Long press [RIGHT]] enters the [I/O-Configuration page](#io-configuration).
* [Long press [LEFT] + [RIGHT]] opens the menu: turn an encoder to choose machines/engines, silent mode, CLK & MIDI settings or patch storage, press it to open (on any other page: back to the engine page).
* Silent mode (menu tile with the crossed-out speaker; O_C T4.1/DSM0: long press [MID] too): all outputs at 0 V, USB audio silent, while the engines keep running; any button or encoder press leaves it. While the computer plays USB audio (the tile shows a speaker instead), that playback stays on the outputs and only the tracks are muted. The display shows a `[SILENT]` screen with VU bars of that playback, drifting like a screensaver. The I/O page stays a long press on [RIGHT] away.
* [Long press left or right [ENCODER]] shows the modulation popup
* [Long press [L-ENCODER] + [R-ENCODER]] enters the [patch storage page](#patch-storage) - the patch saved or loaded last is restored at startup
  - DEBUG: skip restore - press [RIGHT] button while startup ).

<br/>
<div style="page-break-after: always;"></div>

# Machines <sup>[wiki](https://github.com/eh2k/squares-and-circles/wiki/%E2%96%A1%EF%B8%8E%E2%97%8F%EF%B8%8E-Machines-&-Engines)</sup>

<img align="right" src="doc/menu_engines.png" width=196px />

* [Long press [LEFT]] enters the machine-selection-page.<br/>
* [Turn [L-ENCODER]] selects the machine (category), [Turn [R-ENCODER]] the engine - the engine of the track is highlighted.<br/>
* [Short press [R-ENCODER]] loads the selected engine.<br/>
* [Long press [R-ENCODER]] loads the selected engine - extended mode: turn [R-ENCODER] to choose, press it to confirm.<br clear="right"/>
   * Load & reset I/O settings <img align="right" src="doc/engine_load_resetio.png" width=196px />
   * Load & keep IO settings<br clear="right"/>
   * Copy #1-4 (parameter values + modulations + I/O settings of track 1-4) <img align="right" src="doc/engine_copy_from.png" width=196px />

<br clear="right"/>

---
###### Screenshots
<img src="doc/engines.png" />

---

## Engines

| Group | Engines |
|---|---|
| **GND** | `---`, [Scope](app/GND/Scope/README.md), [FFT](app/GND/FFT/README.md), [Tuner](app/GND/Tuner/README.md), [Midi-Monitor](app/GND/Midi-Monitor/README.md) |
| **CV** | [V_OCT](app/CV/V_OCT/README.md), [EnvGen_AD](app/CV/EnvGen/README.md), [EnvGen_ADSR](app/CV/EnvGen/README.md), [LFO](app/CV/LFO/README.md), [EnvFollower](app/CV/EnvFollower/README.md), [Vactrol](app/CV/EnvFollower/README.md) |
| **Drums** | [Analog-BD](app/SYNTH/plaits/README.md), [Analog-SD](app/SYNTH/plaits/README.md), [Analog-HH](app/SYNTH/plaits/README.md), [Analog-HH2](app/SYNTH/plaits/README.md), [909ish-BD](app/SYNTH/plaits/README.md), [909ish-SD](app/SYNTH/plaits/README.md), [VA909-BD](app/DRUMS/VA-909/README.md), [VA909-SD](app/DRUMS/VA-909/README.md), [VA909-RS](app/DRUMS/VA-909/README.md), [VA909-CP](app/DRUMS/VA-909/README.md), [VA909-Toms](app/DRUMS/VA-909/README.md), [TR909-HiHat](app/DRUMS/TR909-HiHat/README.md), [808ish-BD](app/DRUMS/808ish-BD/README.md), [808ish-SD](app/DRUMS/808ish-SD/README.md), [808ish-HiHat](app/DRUMS/808ish-HiHat/README.md), [FM-Drum](app/DRUMS/FM-Drum/README.md), [Djembe](app/DRUMS/Djembe/README.md), [Claps](app/DRUMS/Claps/README.md) |
| **DRUMKITS** | [TR-707](app/DRUMKITS/TR-707/README.md), [RZ-1](app/DRUMKITS/RZ-1/README.md), [DMX](app/DRUMKITS/DMX/README.md), [Linn](app/DRUMKITS/Linn/README.md), [DrumSynth](app/DRUMKITS/DrumSynth/README.md): [CR-78](app/DRUMKITS/CR-78/README.md), [CR-8000](app/DRUMKITS/CR-8000/README.md), [DR-110](app/DRUMKITS/DR-110/README.md), [KR-55](app/DRUMKITS/KR-55/README.md), [MFB-512](app/DRUMKITS/MFB-512/README.md), [TR-606](app/DRUMKITS/TR-606/README.md), [TR-808](app/DRUMKITS/TR-808/README.md), [TR-909](app/DRUMKITS/TR-909/README.md) |
| **SYNTH** | [Braids](app/SYNTH/braids/README.md), [Plaits](app/SYNTH/plaits/README.md): Plaits' 16 models (synth and speech), [SAM](app/SYNTH/SAM/README.md), [Rings](app/SYNTH/rings/README.md), [DxFM](app/SYNTH/DxFM/README.md), [DxFM_BNK1-3](app/SYNTH/DxFM/README.md), [VA-303](app/SYNTH/VA-303/README.md) |
| **FX** | [Reverb](app/FX/Reverb/README.md), [ReverbSC](app/FX/ReverbSC/README.md), [Rev-Dattorro](app/FX/Rev-Dattorro/README.md), [Delay](app/FX/Delay/README.md), [Gated-Reverb](app/FX/Gated-Reverb/README.md), [Reverb-HP-LP](app/FX/Reverb-HP-LP/README.md), [JU60_chorus](app/FX/JU60_chorus/README.md), [Drive](app/FX/Drive/README.md), [Ducker](app/FX/Ducker/README.md), [Compressor](app/FX/Compressor/README.md), [FV1emu](app/FX/FV1emu/README.md): FV-1 programs from a flash blob |
| **NOISE** | [NoiseGen](app/NOISE/NoiseGen/README.md), [NES_noise](app/NOISE/NES/README.md), [808_squares](app/NOISE/808_squares/README.md) |
| **FILTER** | [SVF](app/FILTER/SVF/README.md), [MoogLadder](app/FILTER/MoogLadder/README.md) |
| **SEQ** | [EuclidRythm](app/SEQ/EuclidRythm/README.md), [EuclidArp](app/SEQ/EuclidArp/README.md), [TuringMachine](app/SEQ/TuringMachine/README.md), [303-Patterns](app/SEQ/303-Patterns/README.md), [Clock](app/SEQ/Clock/README.md) |

## Machine/Engine

Machines/Engines are controlled by individual parameters.

>[Short press left/right [ENCODER]] changes parameter selection<br/>
>[Rotate left/right [ENCODER]] changes parameter value

An engine that does not fit stays on its track and says so on its page: `OUT OF MEMORY!` when its app got no RAM, `OUT OF CPU!` when the engines of all tracks together need more than the 500 µs of an audio frame. In that case the firmware stops, after 150 ms of overload, the engine or insert FX loaded last (within 3 s), otherwise the most expensive one: its track plays silence (a stopped insert is bypassed), its parameters and the patch keep it, and a message names the track. Loading any engine or insert FX lets all of them run again.

### Modulations

>[Long press left or right [ENCODER]] shows/hides the modulation popup*


For each parameter a modulation can be assigned:
  * **CV**: <img align="right" src="doc/mod_cv.png" width=196px />
    * SRC:
      * `C1`, `C2`, `C3`, `C4` - Analog inputs, `$1A`..`$4H` - virtual signals (outputs of the engines, see [I/O-Configuration](#io-configuration))
        * OP: `THRU`, `SLEW`, `S&H`, `T&H`
          * THRU - Thru Input
          * SLEW - Slew Limiter
            - Rising
            - Falling
          * S&H - Sample and Hold 
            - TRIG: Trig Source
          * T&H - Track and Hold 
            - GATE: Gate Source
      * `+` - Constant internal voltage
        * -9.99V..+9.99V in 10mV steps, set with the knob - nice for fine adjustments (detuning etc)
    * The attenuverter of CV goes from -2 to +2; the bottom line shows the input voltage and the factor (e.g. `+2.5V X 1.0`)
    * Hints:
      * Parameter 0 (top-left) is primarily used for V/OCT control. Therefore, a single V/OCT signal or CV input can be shared by applying modulation to parameter 0 with an attenuverter set to +1, within the -3V to +6V range. It is also possible to select the V/OCT input in the io-configuration page.
      * All other parameters can be modulated via CV-input with a assumed voltage-range of -4V..4V at 2kHz sample rate.
      * Be aware the CV-range is probably limited by hardware to: -3.5v..6.5V<br clear="right"/>
  * **RND**: Trigger generates a random voltage <img align="right" src="doc/mod_rnd.png" width=196px />
    * TRIG: `!`, `T1`, `T2`, `T3`, `T4`, `C1`, `C2`, `C3`, `C4`, `$1A`..`$4H`<br clear="right"/>
  * **ENV**: Triggered Envelope (Attack, Decay) <img align="right" src="doc/mod_env.png" width=196px />

    * TRIG: `!`, `T1`, `T2`, `T3`, `T4`, `C1`, `C2`, `C3`, `C4`, `$1A`..`$4H`
    * ATT.: Attack
    * DEC.: Decay<br clear="right"/>
 * **LFO**: Free/Triggered Sine-LFO <img align="right" src="doc/mod_lfo.png" width=196px />
    * TRIG: `-` (free running), `!`, `T1`, `T2`, `T3`, `T4`, `C1`, `C2`, `C3`, `C4`, `$1A`..`$4H`
    * SHAPE: `SIN`, `TRI`, `SQR`, `STEPS`, `NOISE`
    * FREQ.: `0-127`
    * WAVEFRM: `0-127`<br clear="right"/>
  * **EF**: Envelope Follower <img align="right" src="doc/mod_ef.png" width=196px />
    * SRC: `C1`, `C2`, `C3`, `C4`, `$1A`..`$4H`
    * ATT: Attack
    * REL: Release<br clear="right"/>
  * **TM**: Turing Machine  <img align="right" src="doc/mod_tm.png" width=196px />
    * TRIG: `!`, `CLK`, `T1`, `T2`, `T3`, `T4`, `C1`, `C2`, `C3`, `C4`, `$1A`..`$4H`
    * RESET: `-`, `T1`, `T2`, `T3`, `T4`, `C1`, `C2`, `C3`, `C4`, `$1A`..`$4H`
    * PROB: `0-9` (0-100%)
    * STEPS: `1-16`<br clear="right"/>
  * **SEQ**: 4-Step Sequencer <img align="right" src="doc/mod_seq.png" width=196px />
    * TRIG: `!`, `T1`, `T2`, `T3`, `T4`, `C1`, `C2`, `C3`, `C4`, `$1A`..`$4H`, `CLK/1`, `CLK/4`, `CLK/16`, `CLK/32`, `CLK/96`
    * RESET: `-`, `T1`, `T2`, `T3`, `T4`, `C1`, `C2`, `C3`, `C4`, `$1A`..`$4H`
    * 4-STEPS: `-64..64` (shown as sliders; integer parameters: at most half their range)<br clear="right"/>
 >`!` = current engine trigger<br/>
 >`CLK` = internal clock<br/>
 All modulations have an attenuverter parameter (the knob at the top right of the popup, -/+).
  * The modulation-voltage is attenuverted/multiplied by -1..+1 (CV: -2..+2);
 >Press the encoder of the popup's side to step through its rows (the modulation type first), turn it to change the value.

<div style="page-break-after: always;"></div>

## I/O-Configuration

>[Long press [RIGHT]] enters the I/O-Configuration page.

The I/O-Configuration page lets you virtually patch the engine with the hardware ports and internal gate/cv signals ($1A-$4H). The page title is the engine of the track; the page is a list in three sections, `CTRL/INPUTS`, `INSERT-FX` and `MIX/OUTPUTS` - which rows appear depends on the engine interface. In addition to the trigger, which is set with a rising edge, a gate state is also provided, that can be processed by the engine. Engines like Closed/Open-HiHats have an additional accent input - this works technically like a second trigger. The V/OCT input can optionally be quantized and transposed. In addition to the Tx inputs, the Cx inputs can also be used as a source for triggers and accents. The output can be configured as mono or stereo. Several engines can share the same output - the signal is mixed.

The small symbol in front of an input's value shows what arrives there: a dot that lights up on each trigger (trigger/accent inputs), a small box with the voltage (V/OCT input).

*Virtual signals* `$1A`..`$4H`: the outputs of the engines can be used as trigger or CV source of another track - `$<track><output>`, e.g. `$3A` is the first output of track 3. While turning to a virtual signal, a box shows the track's engine and the output behind it; turning skips the places without an output (shown as `-`, e.g. `-2C`). The trigger, V/OCT and CV lists offer only the CV outputs; the audio outputs are offered only as source of the envelope follower (modulation EF).

### Ctrl / Inputs

<img align="right" src="doc/inputs.png" width=196px />

*In case the Engine supports Triggers/Gates - Trigger Input is configurable:*

 * **TRIG-INPUT**:
   * `-`
   * `T1`, `T2`, `T3`, `T4`, `C1`, `C2`, `C3`, `C4`
   * `$1A`..`$4H`
   * `$1MIDI`..`$4MIDI`: the MIDI out of that track (e.g. [MIDI/GM-Drums](app/MIDI/GM-Drums/README.md), whatever
     its MIDI-OUT and channel), in addition to the MIDI channel of the settings page
     - the notes trigger the engine like a trigger input: trigger, gate, accent (velocity > 100; not for a sequencer
       engine, whose accent input is the reset), and a modulation with TRIG `!` fires on each note
     - engines with V/OCT take the pitch from the notes as well: V/OCT-INPUT shows the same `$nMIDI` and the CV
       input is left out (it comes back when TRIG-INPUT is turned to an input again); drum kits keep their V/OCT
       input, their notes pick the instruments
     - several tracks with the same source share its notes as voices, as the tracks of one MIDI channel do
     - MIDI engines get every message as on their channel; drum kits play their instruments by the keys of the
       notes (GM keys), with their velocity, and keep the trigger inputs of their instruments as with `-`
     - turning passes over the own track and tracks without MIDI output. Changing it, or the source track's engine,
       ends the notes held

*While another track's engine sends MIDI - for MIDI engines without TRIG-INPUT and V/OCT-INPUT (e.g.
[GND/Midi-Monitor](app/GND/Midi-Monitor/README.md)):*

 * **MIDI-INPUT**:
   * `-`
   * `$1MIDI`..`$4MIDI`: as TRIG-INPUT `$nMIDI` above


 * **CLOCK-SOURCE**:
   * `CLK` - global clock -> [CLOCK+TIMING+MIDI-Settings](#clocktimingmidi-settings).
   * `T1`, `T2`, `T3`, `T4`, `C1`, `C2`, `C3`, `C4`
   * `$1A`..`$4H`

*In case the Engine supports Accents (Closed/Open HiHat) - Accent Input is configurable (sequencers and engines without trigger input: **RESET-TRIG**):*

 * **ACCENT-INPUT**:
   * `-`
   * `T1`, `T2`, `T3`, `T4`, `C1`, `C2`, `C3`, `C4`
   * `$1A`..`$4H`

 *In case the Engine supports V/OCT - V/OCT Input is configurable:*

 * **V/OCT-INPUT**: `-`, `C1`, `C2`, `C3`, `C4`, `$1A`..`$4H`
   * V/OCT: -3V..6V for frequency-control (default)
   * `$1MIDI`..`$4MIDI` for engines without TRIG-INPUT (e.g. [CV/V_OCT](app/CV/V_OCT/README.md), the SEQ engines):
     the pitch from the notes of that track, as TRIG-INPUT `$nMIDI` above (the CV input is left out, MIDI note 60 =
     0 V): a sequencer on [MIDI/Chords](app/MIDI/Chords/README.md) is transposed by the chord's root (Chords sends
     it last, a single receiver keeps it). A sequencer takes no accent from the notes (no reset by an accented
     chord), RESET-TRIG restarts it. With TRIG-INPUT `$nMIDI` the row shows the same and does not turn
 * **QUANTIZER**: [OFF, Semitones, Ionian, Dorian, ...](lib/braids/quantizer_scales.h)
   * **ROOT**: `C`, `C#`, `D`, `D#`, `E`, `F`, `F#`, `G`, `G#`, `A`, `A#`, `B`
   * a sequencer (SEQ/303-Patterns, SEQ/303-SeqGen) quantizes its note plus the V/OCT input (the CV or the notes of
     `$nMIDI`) together, so the transposed line stays in the scale; a note midway between two tones of the scale
     takes the upper one, whatever played before
 * **TRANSPOSE**: semitone per step
   * **FINE**: semitone/128

*Engines with additional CV inputs (e.g. CV/EnvFollower) show a `<name>-INPUT` row for each; trigger and CV inputs of the parameter modulations follow in a `MODULATIONS` section.*

<img align="right" src="doc/inputs2.png" width=196px />

*In case the Engine is an AUDIO_PROCESSOR - Input signal mix is configurable:*
 * **AUX-INPUT**: `-`, `C1`, `C2`, `C3`, `C4`, `USB`
   * AUX-IN: -3V..3V for additional audio source for effects (prefer CV4).
   * `USB`: what the computer plays on the module's USB sound card (see [USB-Audio](#--usb-audio))
   * `HOST` (O_C T4.1): the microphone or line input of a USB sound card or headset in the USB host port
   * **AUX-LEVEL**: Feed-in level of the aux-input; only with an AUX-INPUT
 * **#&lt;track&gt; &lt;engine&gt;**: Feed-in level (0-100) of the signal of another track, named by its number and engine (e.g. `#3 Claps`); only tracks with an audio engine are listed

*A section without rows is left out; an engine without any inputs or outputs (e.g. GND/---) shows `NO INPUTS/OUTPUTS`.*

<br clear="right"/>

### Insert FX

<img align="right" src="doc/quick-start/io_insert.png" width=196px />

*The section `INSERT-FX`, between the inputs and the outputs, as the audio runs: engine, insert FX, PAN and LEVEL; only with an engine that plays audio (or an insert set).*

 * **INSERT-FX**, **INSERT-FX 2**: `-` or an effect in series behind the engine, on the track itself: its audio runs
   through the effect before PAN and LEVEL, and what the track plays (also its sends to other tracks and its virtual
   signals) is the effect's output. The second row appears once the first is set: a chain of two (Delay, then Reverb).
   The row offers the effects made for it: the FX first (Delay, Reverb, Compressor, Drive, Ducker, the FV-1
   programs, ...), then the FILTERs (their envelope from the track's trigger, their key tracking from its V/OCT), then
   the GND tools (Scope, FFT, Tuner on the track's signal); not a synth with an external input (Rings, the 303s' Wave
   `AUX`): those stay apps of a track with an AUX-INPUT.
   While the row is turned, a box shows the group and the engine. A mono engine with a stereo effect plays in stereo. The effects' parameters come as pages
   after the engine's own (the title shows the slot: `+1 FX/Delay`, `+2 FX/Reverb`; in the channel bar a line along
   its lower edge instead of the dotted cells of the engine's pages), stepped through with
   [LEFT]/[RIGHT], modulations and all as on a track of its own; an insert has no AUX-INPUT or sends of its own, and
   it gets the MIDI of the track's channel as the engine does
   * Under the row of an insert that uses inputs, indented, its own **TRIG-INPUT**, **ACCENT-INPUT**, **V/OCT-INPUT**,
     **CLOCK-SOURCE** and named CV inputs (as far as it has them): `!` (the default, as the `!` of a modulation's TRIG)
     takes what the track's engine gets (its trigger, V/OCT and clock, as far as the engine has those inputs; the
     quantizer and transpose are always the track's), `-` (`CLK`) none (the module's clock), else an input or a virtual
     signal as on the engine's rows. So a Ducker on a pad's track ducks it on the kick's trigger (`T1`, or `$2A` of a
     sequencer on track 2) and not on the pad's own notes, a FILTER follows a V/OCT of its own. A patch with such an
     input set is not read by a firmware before 2026-11-21 (without, it is as before)
   * The inserts belong to the track: loading another engine keeps them, `Load & reset I/O` (and a MIDI program
     change) drops them, `Copy #n` copies them along with their inputs, a patch saves them (a firmware before
     2026-11-11 does not read a patch with an insert). An insert is loaded 300 ms after the last turn of its row, as
     a change of engine; without RAM for it (a Teensy 4.0 holds two Delays) the slot stays empty, the row stays on
     the effect as `!Delay`, and the box shows `FX: OUT OF RAM`; turning on (or back to it) tries again, a patch
     saved then has none in the slot

<br clear="right"/>

### Mix / Outputs

<img align="right" src="doc/outputs2.png" width=196px />

 * **AUDIO-OUT**
   * `-`, `A`, `B`, `C`, `D`, `A+B`, `C+D`
     - The option `-` (no output) is useful if the signal is to be routed into an AUDIO_PROCESSOR engine
     - `A+B` / `C+D`: stereo engines play in stereo, mono engines on both outputs
 * **PAN**: Stereo panning
   - on stereo engines with mono output, panning is also used for mixing
 * **LEVEL**: Output volume level
   - can result in distortion
 * CV-Engines have a `<name>-OUT` row for each CV output instead (`-`, `A`..`D`)
 * Engines that send MIDI (e.g. [MIDI/GM-Drums](app/MIDI/GM-Drums/README.md)) have a **MIDI-OUT** row, like a CV output:
   port and channel in one value
   * `-` (off), `USB-CH01` .. `USB-CH16` (the USB MIDI to the computer, the default port); on the O_C T4.1 also
     `MIDI-CH01` .. (the MIDI jack) and `HOST-CH01` .. (the USB host port)
   * `USB` only while a computer is connected (it has set up the module's USB and is not asleep), `HOST` only while a
     MIDI device is in the USB host port, as the page opens; without any, no row. A port set before that is not
     there shows `-` and comes back by itself when it is there again
   * the O_C T4.1's MIDI out jack sends only what the engines send there, it is no MIDI thru: what comes in at the
     MIDI input or from a USB host device is not passed on (a synth on MIDI in and out, e.g. a 303 clone played by
     `SEQ/303-Patterns`, would get its own messages back, doubled or in a loop)
   * the channel all the engine sends goes in: by default the track's MIDI channel of the
     [MIDI settings](#clocktimingmidi-settings) (the track's number while it has none), and it follows that until
     another channel is chosen here
   * `USB-ANY` (first on each port) only for an engine that sends on several channels: each message on its own
     channel, its default
   * a change sends All Notes Off on the channels the engine played notes on; so does an engine change
<br clear="right"/>
<div style="page-break-after: always;"></div>

## CLOCK+TIMING+MIDI-Settings

>[Long press [LEFT] + [RIGHT]] opens the menu - turn an encoder to `CLK & MIDI SETTINGS` and press it to get to the CLOCK+TIMING+MIDI-Settings page.

<img align="right" src="doc/config_midi1.png" width=196px />

### - **CLOCK-Source**:
   * `-` none
   * `INT` Internal Clock-Source - with an additional row **TEMPO (BPM)**: `30`-`255`
   * `T1` T1 as Clock-Input (4ppqn)
   * `MIDI` Clock received from MIDI
     - the clock pulses move the position (e.g. the steps of `SEQ/...` engines) only between a MIDI Start or Continue and Stop, as in the MIDI spec: without Start they only set the tempo
     - Stop ends the notes of the Midi-Channels (gates off), the pitch stays

> Default Clock-Source for `SEQ/...` engines. <br/>
> Clock-Source is also used by engines like `FX/Delay` (clocked sync) or `SEQ/Clock`


<img align="right" src="doc/config_midi2.png" width=196px />

### - **TRIG-In-Delay**: 
   * `0.0ms`-`49.5ms` (0.5 ms steps): Trigger input processing delay. Usecase: sync trig signals with midi or CV<br clear="right"/>

<img align="right" src="doc/config_midi0.png" width=196px />

### - **MIDI-Control**:
  - `-` disabled
  - In case OC_T40
    - `USB` the USB-MIDI
    - `USB+T1` [T1 input can be used as MIDI-Input](https://github.com/eh2k/squares-and-circles/wiki/Ornament%E2%80%90and%E2%80%90Crime#-midi-expander)
  - else
    - `MIDI/USB` the USB-MIDI and/or hardware Midi-Input (O_C T4.1: not passed on to its MIDI out, no thru)

In case the MIDI-Control is enabled, the row **CHANNELS** sets the Midi-Channels of the engines (switching it on without any channel set gives `1-4`):
  - `1-4` .. `13-16`: one channel per engine, track 1 on the first of the range, track 2 on the next, ...
  - `1 POLY` .. `16 POLY`: all engines on one channel - single mono engines played together polyphonically (shown as voices `1A`, `1B`, ...)
  - `CUSTOM`: a row per engine (**#1 CHANNEL** .. **#4 CHANNEL**: `-`, `1`...`16`)
  - the channels are stored with the patch; with MIDI-Control `-` no engine listens to MIDI notes, CCs and program changes, MIDI engines included
> [Midi-Engines](app/SYNTH/plaits/README.md#midi) consume the MIDI-Stream directly, therefore the MIDI-Messages are not converted as incoming CVs or triggers.

#### Midi-Control
  * Engines can be loaded/selected by midi program change on the engine's channel:
    - the program number is the engine's position on the engine page (`GND/---` = 0), shown at its top (`ENGINE 29`)
    - from 128 on with Bank Select first (CC 0 = 1, program = number - 128); a number past the last engine changes nothing
    - the engine starts with the default I/O (as "Load & reset I/O" on the engine page), the Midi-Channel stays
  * Default Parameter CC-Mappings
    ````
    | HEX | DEC | parameter-index | CH |
    |-----|-----|-----------------|----|
    |  07 |  07 |      LEVEL      |  * |
    |  0A |  10 |       PAN       |  * |
    |  10 |  16 |        0        |  * |
    |  11 |  17 |        1        |  * |
    |  12 |  18 |        2        |  * |
    |  13 |  19 |        3        |  * |
    |  14 |  20 |        4        |  * |
    |  15 |  21 |        5        |  * |
    |  16 |  22 |        6        |  * |
    |  17 |  23 |        7        |  * |
    ````
  > CC 48-55 (HEX 30-37) for LSB (14Bit CC)

* **Midi-Channel** of an engine
  - The Midi messages are translated to analog events
    - Trig: NoteOn
    - Gate: NoteOn until NoteOff
    - CV: 0V = Key 60
  - an engine with a Midi-Channel counts as triggered, as with a TRIG-INPUT: its envelope plays the notes (e.g. `SYNTH/Braids` has `Decay` instead of `Level`), there is no drone without TRIG-INPUT
  - [Multi-Trig-Drum-Engines](https://github.com/eh2k/squares-and-circles/wiki/DrumSynth)
    - In case trigger to `-` - individual drums/instruments are triggered by individual keys:
        - GM Standard Drum Map: https://github.com/eh2k/squares-and-circles/wiki/TR707#midi-mapping
        - keys the drum machine does not map are silent (an app without a key map: keys 60-75 for instrument 1-16)
        
### - **USB-Audio**:
  - `on` (default), `off` - stored in the module (not in the patch)
  - The module is also a USB sound card (48 kHz, 16 bit, stereo; class compliant, no driver needed):
    - Recording: the stereo sum of all engines (with level and pan, -6 dB headroom)
    - Playback: AUX-Input `USB` of AUDIO_PROCESSOR engines; without an engine taking it, the module plays it as a
      sound card: O_C T4.0 and DSM0 on the outputs a track plays audio on (a stereo pair L/R, a single output
      (L + R) / 2), O_C T4.1 on its codec output (an FX with AUX-Input `USB` replaces it: its output has the dry
      signal)
  - Switching it takes effect at once: the module reconnects to the computer, with or without the sound card
  - O_C T4.1: a USB sound card or headset in the USB host port (also behind a USB 2.0 hub) plays the same stereo sum,
    e.g. as headphones. Its microphone or line input is the AUX-Input `HOST` of AUDIO_PROCESSOR engines.
    - It starts when plugged in (popup "audio out", "audio in" or "audio out+in").
    - Supported: class compliant USB Audio 1.0, full speed, 48 kHz/16 bit; the output with an adaptive or synchronous
      endpoint (most headsets and simple sound cards). The input follows the device's own clock.
    - A device without that shows why (e.g. "no 48k/16bit out", "async out").
    - The microphone stays muted on the device's own headphones.

# Patch Storage
<img align="right" src="doc/patch_storage_load.png" width=196px />

> [Long press `L-ENC + R-ENC`] to enter the patch storage page


* Use `L-ENC` for patch slot selection - `*` marks the patch loaded last
* Turn `R-ENC` to choose `[SAVE]` or `[LOAD]`
* Press `R-ENC` - a box asks `SAVE?` / `LOAD?`, press `R-ENC` again to confirm (any other action cancels)
  * Load `(empty)` - restores all engines to `GND/-` <img align="right" src="doc/patch_storage_save.png" width=196px />
  * Save on `(empty)` adds a new patch slot<br clear="right"/>

# Supported Hardware

 * [Ornament-and-Crime T4.0](https://github.com/eh2k/squares-and-circles/wiki/Ornament%E2%80%90and%E2%80%90Crime)
 * [Squares-and-Circles DSM0](https://github.com/eh2k/squares-and-circles/wiki/Squares&Circles-D_SM-Prototype)
 * [O_C T4.1](https://github.com/eh2k/squares-and-circles/wiki/O_C_T41)

## Hardware setup procedure
  > Use [![WebFlasher](https://img.shields.io/badge/Web-Flasher-blue)](https://eh2k.github.io/□●/flash/?firmware)  to enter calibration mode, or execute advanced setup commands </br>
  >Press and hold the [LEFT] button during power-on to enter the setup procedure.

### Display setup

<img align="right" src="doc/display_setup.png" width=196px />

* Press left encoder to flip180.
* Press right encoder for changing display brightness (50%, maximal).<br clear="right"/>

### Encoder setup

<img align="right" src="doc/encoder_setup.png" width=196px />

* Check the encoder rotation direction.
* Press encoder for reversed setup.<br clear="right"/>

### DAC calibration

<img align="right" src="doc/dac_calib.png" width=196px />

To calibrate the DAC, use a multimeter to accurately set the reference voltages at `0V`, `-2V` and `+2V`.
Start with DAC1 (channel A) - connect the multimeter typically using alligator clip on a patch cable inserted in the jack. Use the right encoder to set the voltage as accurately as possible (press the encoder for coarse adjustment). Do it on all outputs - use the left encoder for channel selection. After calibrating `0V` on all outputs, press [right] to step to the `-2V` calibration. Repeat the procedure and press [right] to calibrating `+2V`.


### ADC calibration

<img align="right" src="doc/adc_calib.png" width=196px />

To calibrate the ADC `0V` reference, remove all patch cables from the module. Use the right encoder to adjust the offset (press the encoder for fast adjustment). Do it on all cv-inputs, select the channel with the left encoder. Press [right] to enter the `-2V` calibration. Now you need to connect the DAC outputs to the cv-inputs. The DAC output produces the reference voltage, that is calibrated on the input. Repeat the calibration procedure and step to the `+2V` calibration.

### I/O Test:

<img align="right" src="doc/io_test.png" width=196px />

* Test/Verify your TRIG or CV inputs.
* The output voltage is set by the cv input voltage (DACx = ADCx).

<br clear="right"/>

## ⧉ Conclusions and the future

The project started as a kind of personal research in embedded DSP that I did over half a year. At some point, it turned out by chance that the O_C was very well suited for this by only upgrading the Teensy. Apart from the small optional [mod](https://github.com/eh2k/squares-and-circles/wiki/Ornament%E2%80%90and%E2%80%90Crime#optional-dac-voltage-range-mod--5v5v-range) on the DAC voltage range, the current O_C hardware unfortunately has some unchangeable limitations for audio applications. As you know, the Teensy 4.0 doesn't have "high-end" ADCs and a limited amount of RAM. On the other hand, it is the the combination of limitations that make the project unique.

I have mixed feelings about the user interface and the firmware concept. On the one hand, the two buttons and the two encoders are sufficient and actually practical for a multifunction module, but on the other hand, something is missing. Additional buttons/encoders could definitely simplify the operation/interaction - let's see if this [puzzle](https://github.com/eh2k/squares-and-circles/wiki/Squares&Circles-D_SM-Prototype) can be solved.

## License

The application code respectively the suite of machines/engines and the third-party source files are published under permissive software licenses.
The circuit models - [SYNTH/VA-303](app/SYNTH/VA-303/README.md), [DRUMS/VA-909](app/DRUMS/VA-909/README.md) and their
libraries `lib/va303`, `lib/va909` - are published under the [Mozilla Public License 2.0](https://mozilla.org/MPL/2.0/):
they can be used in other software, also closed one, but whoever distributes changed versions of their files publishes
the source of those files under the MPL as well.
For the exact license and copyright holder - see the header of individual source code files or readme/license file in the sub folder.

The firmware as a whole is for personal use only.
If you are considering commercial distributing hardware with this firmware, please contact me (eh2k◯gmx.de).

## Trademarks

TR-606, TR-707, TR-808, TR-909, CR-78, CR-8000, DR-110, TB-03 and Juno-60 are trademarks of Roland Corporation. Mutable Instruments, Braids, Plaits, Rings, Elements, Beads and Marbles are trademarks of Mutable Instruments. Oberheim, DMX, Linn, KR-55, RZ-1, MFB-512, Moog, NES, FV-1 and all other product names are trademarks of their respective owners. They only name what an engine emulates; squares-and-circles is not affiliated with or endorsed by any of them.

<!--

````
 _______________
|***************|
|*             *|
|*             *|
|***************|
|               |
| [BL]     [BR] |
|               |
| (EL)     (ER) |
|               |
|( ) ( ) ( ) ( )|
|               |
|( ) ( ) ( ) ( )|
|               |
|( ) ( ) ( ) ( )|
|_______________|
````
-->
