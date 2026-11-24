# Writing squares-and-circles apps

`app/` holds the **apps** (engines) of the squares-and-circles firmware: the guide for writing one, for people and
for AI assistants. Each app has its own directory `app/<CATEGORY>/<Name>/` with its source `<Name>.cpp`, its doc
`README.md`, the screenshots the doc shows, and the build output. `<Name>.cpp` is compiled on its own into a
relocatable [udynlink](https://github.com/eh2k/udynlink) module (`<Name>.bin` next to it), which the firmware loads at
runtime into one of its four engine slots (tracks). The module name on the device is `<CATEGORY>/<Name>.bin`.
Target: Cortex-M7 (O_C with Teensy 4.0, DSM0), 48 kHz, 128×64 mono display, 2 buttons, 2 encoders.

- The whole app contract is `app/squares-and-circles-api.h`. It is marked WIP, so breaking changes are possible.
- The apps of this repository are the examples: when a detail below is unclear, the header and an app that does the
  same are the reference.

## Minimal app

```cpp
// <license header (MIT like the other apps) with YOUR user as author, not the name in the other apps' headers>

#include "../../squares-and-circles-api.h"

static float _pitch = 0.f; // V_OCT knob (-4..+4 oct), summed into engine::cv() by the firmware
static float _decay = 0.5f;
static float _phase = 0.f;
static float _env = 0.f;

void engine::setup()
{
    engine::addParam(V_OCT, &_pitch); // param 0 is conventionally the pitch knob
    engine::addParam("Decay", &_decay); // float, default range 0..1
}

void engine::process()
{
    if (engine::trig()) // referencing trig() gives the engine a Trig input
        _env = 1.f;

    const float note = DEFAULT_NOTE + engine::cv() * 12.f; // referencing cv() gives a V/OCT input
    const float inc = 440.f * powf(2.f, (note - 69.f) / 12.f) / SAMPLE_RATE;
    const float k = 0.999f + _decay * 0.00099f;

    auto out = engine::outputBuffer<0>(); // mono audio, +-1.0 = full scale
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        out[i] = (2.f * _phase - 1.f) * _env;
        _phase += inc;
        if (_phase >= 1.f)
            _phase -= 1.f;
        _env *= k;
    }
}

void engine::draw()
{
    gfx::drawWaveform(engine::outputBuffer<0>()); // scope of our output (forces compact layout)
}
```

Save it as `app/SYNTH/MySaw/MySaw.cpp` and build it with `bash app/build.sh`; the web app's MCP connection runs it on
the module without flashing (`load_app`: from the module's RAM, shown as `DEV/MySaw`).

## Lifecycle

| hook | when | notes |
|---|---|---|
| global ctors | at load | API calls work here (`DRUMS/808ish-BD.cpp` allocates its output buffer this way), but prefer `setup()` |
| `engine::setup()` | once after load | register params and outputs, allocate, init DSP |
| `engine::process()` | every frame: `FRAME_BUFFER_SIZE` (24) samples at `SAMPLE_RATE` (48000), i.e. every 0.5 ms | **required**: without it the engine never starts. Real-time: no gfx, no big allocations |
| `engine::draw()` | UI refresh, after the firmware drew the header and knobs | optional |
| `engine::screensaver()` | while the screensaver is shown | optional; you own the full 128×64, so `gfx::clearRect(0, 0, 128, 64)` first |

- Only `setup, process, draw, screensaver, restore, __ui_event_handler, __midi_event_handler` are exported, and the
  constants of the `ENGINE_ALIAS` lines and the `ENGINE_INSERT_FX` marks (see `--public-symbols` in `build.sh`).
  `engine::release()` is declared in the header, but never called. Memory from `malloc`/`new` is tracked and freed
  on unload.
- `engine::t()` counts `process()` calls, one per 0.5 ms (2 kHz). Use it for timers and modulo tricks
  (`engine::t() % 50 == 0`). `millis()` and `micros()` are available too.
- Some library classes misbehave as static objects. Construct those with placement new into a static buffer
  (`FX/Gated-Reverb.cpp`: `fv1 = new (mem) FV1();`).

## The firmware infers your I/O from the symbols you use

There is no manifest. At load time the firmware sets the engine type and the I/O page from the API symbols your
module links against:

| you reference | effect |
|---|---|
| `engine::trig()`, `engine::gate()` | Trig input |
| `engine::accent()` | Accent input |
| `engine::cv()`, `engine::cv_i32()` | V/OCT input with quantizer and transpose |
| `engine::inputBuffer<0/1>()` | audio processor: gets the Aux input plus the AUX sends of other engines (an effect marked `ENGINE_INSERT_FX()` is also offered as **INSERT-FX** of a track, below) |
| `engine_sync::step/stepChanged/stepReset/samples_per_step` | Sequencer engine: clock input, accent input acts as reset |
| `engine_sync::ppn()` | Clock input |
| `engine::outputBuffer<1>()` or `OUT_AUDIO_R` | stereo output |
| `gfx::drawWaveform()`, `gfx::drawScope()` | forces `ENGINE_MODE_COMPACT` |
| `engine::mixLevelL/R()`, `trigLevel()` | per-instrument level and pan (multi-trig): the kit's mix page |
| `engine::midiOut()` (and `midiNoteOn/Off`, `midiCC`, ...) | MIDI-OUT port and channel on the I/O page |

So call only what you need. For example, an effect that calls `engine::trig()` gets a trigger input.
`engine::hasTrig()` does not add an input; it tells whether the user patched a trigger. A common idiom is to turn the
"Decay" knob into a "Level" VCA when nothing is patched (`SYNTH/plaits.cpp`, `SYNTH/braids.cpp`).

An effect marked `ENGINE_INSERT_FX();` (once per engine, after its `ENGINE_NAME` like an `ENGINE_ALIAS`; anywhere in
an app without one) also runs as an **insert FX** of a track: the rows INSERT-FX and INSERT-FX 2 of the I/O page put up
to two more engines after the track's engine, in its audio and in series. Its `inputBuffer<0/1>()` is then the output
of the stage before it (the engine's, before LEVEL, as the AUX sends get it; the first insert's for the second), mono
on both sides, and the last stage's output is what the track plays, through the track's LEVEL and PAN. An insert has
inputs of its own, rows under its INSERT-FX row (TRIG-INPUT, ACCENT-INPUT, V/OCT-INPUT, CLOCK-SOURCE as far as it uses
them, and its named CV inputs): `!` by default, what the track's engine gets; set to an input or a virtual signal, it
is the insert's own. The quantizer, TRANSPOSE, FINE, the MIDI channel and MIDI-OUT are the track's. Its parameters are
pages after the engine's, drawn as on a track of its own (`draw()`, the UI handler, `isParamOnPage()` work there); a
patch stores the inserts with the track. Mark an effect that works on the audio alone, or whose trigger and V/OCT are
optional or can come from inputs of its own (the FILTERs, `FX/Ducker`); not a synth with an external input
(`SYNTH/rings`), nor an engine without an audio output (`CV/EnvFollower`: an insert plays what it outputs). Marked so
far: FX/*, FILTER/*, GND/Scope, FFT and Tuner.
`engine::hasAccent()` tells the same of the ACCENT-INPUT (an input or a virtual signal, not MIDI: a note's velocity
over 100 sets `accent()` without it); an older firmware does not start an app that calls it.

## Inputs and control signals

| API | meaning |
|---|---|
| `engine::trig()` | nonzero in the frame of a trigger. For multi-trig engines it is a bitmask, bit *i* = instrument *i* |
| `engine::gate()` / `engine::accent()` | gate held / accent (the ACCENT-INPUT high at the trigger, or a MIDI note's velocity over 100; a sequencer gets no accent from another track's notes, `$<track>MIDI`: its accent is the reset) |
| `engine::cv()` | pitch in volts (1.0 = 1 octave) relative to `DEFAULT_NOTE` (MIDI 60). It sums the V/OCT input after the quantizer, the MIDI note and pitch bend, the I/O transpose and fine-tune, and the `V_OCT` knob if registered: 0 V with the knob at C4 is 0, C4. Convert with `DEFAULT_NOTE + engine::cv() * 12.f` |
| `engine::cv_i32()` | same, in `PITCH_PER_OCTAVE` (1536) units per volt |
| `engine::inputBuffer<0/1>()` | audio in L/R (float, `FRAME_BUFFER_SIZE`) |
| `engine::getMidiCC(n)` | last CC value on the engine's MIDI channel (no MIDI mode needed). Convention: CC 4 = hi-hat openness |
| `engine::qz_process(pitch)`, `qz_lookup(note)`, `qz_enabled()`, `qz()` | the quantizer scale chosen on the I/O page. `qz_lookup(64 + k)` are its tones from the root up (1/128 semitones), the root is `qz_process(p, &n) - qz_lookup(n)` (`qz_lookup()`: `SEQ/TuringMachine.cpp`) |
| `engine::qz_note(pitch)` | a sequencer's note: pitch plus the V/OCT input (MIDI note, CV) quantized together, then bend, TRANSPOSE, FINE; no hysteresis (the same note, the same tone), midway between two tones the upper; without a scale `pitch + cv_i32()`. In place of `qz_process(pitch) + cv_i32()`, whose sum leaves the scale |
| `engine::name()` | registered engine name; branch on it when one binary serves several engines |
| `machine::raw_adc_voltage(i)`, `raw_dac_buffer(i)`, `raw_digital_inputs()` | raw hardware access |

## Outputs

- **Audio:** `engine::outputBuffer<0>()` / `<1>()` return float buffers registered as `AUDIO_L` / `AUDIO_R`.
  ±1.0 is full scale. Use only `<0>` for a mono engine. `<1>()` is resolved at load, so every engine of the binary
  is stereo; `outputBuffer_f32(OUT_AUDIO_R)` in `setup()` makes only the engine that calls it stereo
  (`SYNTH/plaits.cpp`: SYNTH/Plaits stereo, the drums of the binary mono).
- **Named buffers:** `engine::outputBuffer_f32(name)` / `outputBuffer_i16(name)`.
  - `OUT_AUDIO` (mono), `OUT_AUDIO_L` and `OUT_AUDIO_R` are audio. An i16 audio buffer uses 32768 as full scale.
  - Any other name is a **CV output** (max 8), routable on the I/O page. Examples: `OUT_CV`, `OUT_VOCT`, `OUT_GATE`,
    `OUT_TRIG "1"` (string concatenation gives "TRIG1"), `"SLIDE"`, `"Out1"`.
  - CV scaling: f32 uses 1.0 = 1 V. i16 uses `PITCH_PER_OCTAVE` = 1 V, so a 5 V gate is `5 * PITCH_PER_OCTAVE`.
  - Each call **allocates** a new buffer. Call it once in `setup()` and keep the pointer.
- The host does not clear buffers. Write (or `memset`) every sample of every output in every `process()`.
- Limits per engine: 2 audio outs, 8 CV outs, 4 CV ins, 24 parameters. An older firmware has 4 CV outs: it returns
  nullptr for a fifth (writing it faults), so check the pointer. Each CV output is also a virtual signal of the
  track, `$<track>A` ... `H` in the order the app requests them.

## Parameters

```cpp
engine::addParam("Cutoff", &f);                       // float, 0..1
engine::addParam("Freq", &f, -5.f, 5.f);              // float, custom range
engine::addParam("Decay", &i, 0, UINT16_MAX);         // int32 (peaks-style 16-bit knob)
engine::addParam("Mode", &i, 0, LEN_OF(names) - 1, names); // int32 with value labels
```

- The UI writes directly into your variable. Parameters (and their modulation) are saved with the patch.
- **Modulation:** right before `process()` the firmware writes base+modulation into the variable and restores the base
  value afterwards. So `process()` sees modulated values for free. In `draw()` use
  `engine::getParamValueModulated_f32/_i32(&v)` for the modulated value. Don't write a modulatable param variable
  inside `process()`: the restore overwrites it.
- **Identity is the pointer.** Calling `addParam` again with the same variable replaces that entry in place. Apps use
  this to change the range, labels or name at runtime (`SYNTH/braids.cpp` `draw()`).
- **Names and labels are stored by pointer.** Use literals or static buffers. `FX/Delay.cpp` rewrites its `time_info`
  label buffer in `draw()`, a `>` name (a text item) with the value in it; `SEQ/Clock.cpp` the same for Source and BPM.
- **int ranges:** `max > 255` gives a knob (encoder step = range/24, shown as 0–127). `max ≤ 255` gives a stepped
  value (step 1, shown as number or label).
- **Layout:** even indices (0, 2, 4) form the left column and left encoder; odd indices (1, 3, 5) the right column and
  right encoder. Up to 6 params are shown as knobs. With 7–10 params the right column becomes a small text list, and
  more than 10 become a full list. Param 0 is conventionally `V_OCT`.
- **Pages:** a name starting with `\f` (`"\fChar"`) begins a new page of parameters (up to 4 pages). The `\f` is
  dropped from the name, and it combines with the other prefixes (`"\f>Mode"`). Each page is laid out as if the
  engine had only its own parameters, with the column rule above counted from the page's first parameter.
  `"\f\vName"` begins a page drawn as the list (as one of more than 10 parameters is): a row each with the name, the
  value and a small knob of its position, an arrow at the selected one; the left encoder selects, the right one
  edits; a two-line label `"Wave\n%s"` as `Wave` and its value, no row for an empty slot `""` (`SYNTH/VA-303.cpp`
  page 2). The buttons L/R step through the pages of all tracks: track 1 page 1, track 1 page 2, track 2, ... The
  channel bar shows the pages in the track's segment (the page shown filled, the others dotted). The web app's
  emulator shows the page shown, with a tab per page that switches it. Values, modulations and patches still index
  the one parameter list. An older firmware shows all parameters on one page (and the `\f` as a glyph).
  `isParamOnPage(&v)` tells `draw()` whether the page of `v` is shown, e.g. to draw on one page only.
  `setParamName` with or without `\f` sets or removes the break. `selectParam(&v)` switches to the page of `v`. Pages
  don't go with `ENGINE_MODE_COMPACT`: it pads the whole list to 5 parameters, not each page. A name has at most 7
  characters on a page of knobs (a column). Examples: `SYNTH/VA-303.cpp`, `SYNTH/plaits.cpp`, `DRUMS/VA-909.cpp`.
  A multi-trig kit has one more page, the firmware's mix page (see below).
- Helpers: `setParamName(&v, "Level")`, `selectParam(&v)`, `isParamSelected(&v)`, `isParamModulated(&v)`,
  `isParamOnPage(&v)`, `setParameterSelectionWrap(n)` (limits the left encoder to even params ≤ n, counted on the
  page).

### Special parameter names

| name | effect |
|---|---|
| `V_OCT` | pitch knob with piano display, summed into `engine::cv()` |
| `V_QTZ` (int) | exposes the quantizer scale as a knob |
| `MULTI_TRIGS` | instrument selector of a multi-trig drum engine (see below) |
| `""` | empty slot: skipped by selection, keeps column alignment (and no knob in the web app). An encoder whose column has only empty slots still turns one: ignore its value |

### Name prefixes and formatting

| prefix / content | rendering |
|---|---|
| `>Name` | text item instead of a knob. Value labels starting with `>` also render as text (`">CLASSIC"`) |
| `>>Name` | text item of a stepped int (max ≤ 255) without the number shown while it is turned or modulated: for a name that holds the value itself (`SEQ/Clock.cpp` BPM, `">>120 BPM"`) |
| `@Name` | value shown as a plain integer while turning (instead of 0–127) |
| `$Name` | value shown as ± offset around 128 (`SEQ_SWING`) |
| `#Name` | compact two-digit numeric field (sequencers: `#LEN`, `#HIT`, `#ROT`). A float `#` field shows its value as 000 ... 127 of its range, as a MIDI CC (three digits, its box 6 px wider), and is turned as the other floats: 128 detents end to end slowly, 24 fast; an older firmware shows `00` for it |
| `_Name` | small `name 000` field, only drawn while selected (`GND/Scope.cpp`, `GND/FFT.cpp`) |
| `.name` or `"."` | not drawn (hidden or placeholder, or when the app draws its own UI) |
| `~Name` | big knob |
| `\t` prefix | shifts the knob right |
| `Label\n%fmt` | two-line label. A `%` makes the name a `printf` format for the value: float (`"@Offset\n%+.1fV"`), int `%d` (`"@BPM\n%d"`, `"@Prob\n%d%%"`; result max 15 chars), or int with labels `%s` (`"Range\n%s"`) |
| `"Big\tsmall"` in value labels | two-line label (`SYNTH/rings.cpp`: `"Modal\tmono"`) |

## Engine modes

`engine::setMode(ENGINE_MODE_COMPACT | ENGINE_MODE_MIDI_IN)` (the flags OR together):

- `ENGINE_MODE_COMPACT` lays the knobs out in the tight 5–6 parameter grid even if you have fewer params, which leaves
  screen room for your own graphics. `drawWaveform`/`drawScope` switch it on implicitly.
- `ENGINE_MODE_MIDI_IN`: see MIDI below.
- `ENGINE_MODE_MIDI_OUT_ANY`: the app sends MIDI on several channels, see MIDI out below.
- `ENGINE_MODE_SOFT_LIMIT`: the app's audio may go over ±1 (a polyphonic synth: the plain sum of its voices). The
  firmware limits it in the mix (as it is up to 0.85, 1.0 at 1.3), after the part of the track's LEVEL below the
  middle and before the part above it: turned down, a chord saturates less. The FX sends to other tracks get it raw
  (before LEVEL, unlimited); the virtual signals get it limited without LEVEL. Don't limit the sum in the app then:
  SYNTH/plaits (not its drums) and SYNTH/DxFM use it.

## Display (`gfx::`)

- 128×64, 1 bit. The firmware draws the header (y 0–8: engine name, outputs) and the knobs before your `draw()`.
  Draw into the free area only. Call gfx functions only from `draw()` and `screensaver()`. One exception: the name
  in the header (x -1, y 0, font 1) may be extended, e.g. by the page's section (draw the name again over it, then the
  section; stay left of the outputs from x 100 on).
- Functions: `drawString(x, y, s, font=1)` (font 0 = small; `\n` works), `drawStringEx(x, y, font, fmt, ...)`,
  `drawStringCenter`, `setPixel`, `drawLine`, `draw/fill/clear/invertRect`, `draw/fill/clearCircle`,
  `drawXbm(x, y, w, h, bits)` (XBM, see https://xbm.jazzychad.net/), `drawPiano(x, y, note_in_volts)`,
  `drawMessageBox(title, msg)`, `message(msg)`, `displayBuffer()` (raw).
- `drawWaveform(ptr, x=0, y=-1)`: when `ptr` is one of your output buffers it shows a live scope of it. Any other
  pointer is treated as a `sample*`, and the whole sample is shown.
- `drawScope(y, frame, x_scale, y_scale)`: scope of any float frame (`GND/Scope.cpp`).

## UI events

```cpp
static bool onUI(uint16_t type, uint16_t control, int16_t value, uint16_t mask)
{
    if (type == EVENT_BUTTON_UP && control == ENCODER_R) { toggle(); return true; } // consumed
    return false; // default handling (parameter change, modulation popup, ...)
}
// in setup(): engine::setUIHandler(onUI);   later: engine::setUIHandler(nullptr) to detach
```

`type` is `EVENT_ENCODER` (`value` = increments), `EVENT_BUTTON_UP` or `EVENT_BUTTON_HOLD`. `control` is
`ENCODER_L/R` or `BUTTON_L/R`. Returning `true` on `EVENT_BUTTON_HOLD` suppresses the modulation popup.
Examples: `SEQ/Clock.cpp` changes another parameter on a turn and returns `false`, so the turn still applies (BPM
with SYNC: FREE from the tempo shown); `SYNTH/plaits.cpp`.

## MIDI

```cpp
void engine::setup() { engine::setMode(ENGINE_MODE_MIDI_IN); }
void engine::onMidiNote(uint8_t key, uint8_t velocity) {} // velocity 0 = note off
void engine::onMidiPitchbend(int16_t pitch) {}            // +-8192
void engine::onMidiCC(uint8_t cc, uint8_t value) {}
void engine::onMidiSysex(uint8_t byte) {}
void engine::onMidiRTM(uint8_t code) {}                   // engine::MIDI_RTM_START / STOP / CONTINUE
```

- Define **all five** handlers; empty bodies are fine. They are weak declarations that the dispatcher installed by
  `setMode()` calls unconditionally.
- A MIDI engine gets the messages of its MIDI channel in the handlers. On that channel the firmware still turns the
  notes into trig, gate and V/OCT as for any engine, in the next frame. An engine that also references `trig()` gets
  each Note On twice: skip the Trig of a frame with a Note On in it (`SYNTH/DxFM.cpp`: the handler counts them). From
  the MIDI out of another track (`$<track>MIDI`) the handler also runs before the frame, so the same check holds.
- Keep the handlers short; record state and render in `process()`. Examples: `SYNTH/plaits.cpp` (4-voice poly with
  `stmlib::VoiceAllocator`), `GND/Midi-Monitor.cpp`.

### MIDI out

```cpp
engine::midiNoteOn(10, 36, 100);  // channel 1..16, key, velocity
engine::midiNoteOff(10, 36);
engine::midiCC(1, 74, 64);
engine::midiProgramChange(1, 5);
engine::midiPitchbend(1, -8192);  // +-8192
engine::midiRealtime(0xF8);       // clock; MIDI_RTM_START / STOP / CONTINUE
engine::midiOut(0x99, 36, 100);   // raw: status byte (channel 0..15 in its low nibble), 2 data bytes
```

- Referencing `midiOut` (the helpers are inline wrappers) gives the engine the **MIDI-OUT** row on the I/O page, like
  a CV output: port and channel in one value, `-`, `USB-CH01` .. `USB-CH16`. The channel replaces the one of every
  message. By default it is the track's MIDI channel of the CLK & MIDI settings (the track's number while it has
  none), so the channel an app passes does not matter. An app that sends on several channels calls
  `engine::setMode(ENGINE_MODE_MIDI_OUT_ANY)` in `setup()`: each port then also has `USB-ANY` (first, the default),
  which keeps the channel in each status byte. The row lists only the ports with someone on them.
- At most 3 bytes a message, no SysEx. Callable from `process()`, `draw()` and the handlers: the firmware queues it
  (64 messages) and its main loop sends it, so it costs `process()` nothing; the timing is the frame's (0.5 ms) plus
  the main loop's.
- Send the Note Offs yourself. When the engine goes or its MIDI-OUT or channel changes, the firmware sends All Notes
  Off (CC 123) on the channels the engine played notes on.
- `ENGINE_MODE_MIDI_IN` is independent of it: an engine can send without taking the MIDI input.
- Inside the module, another track takes it with **TRIG-INPUT** `$<track>MIDI` on its I/O page (V/OCT-INPUT for an
  engine without TRIG-INPUT, in place of the CV; MIDI-INPUT for a MIDI engine without either): a MIDI engine gets the
  messages in its handlers, a drum kit the notes by its key map, any other engine trig/gate/V-OCT from the notes, in
  the frame. Several receivers share the notes as voices (the voice used longest ago to a new note): one receiver
  keeps the last Note On, so send the most important note of a chord last. Each Note On is also a trigger in the frame
  of a MIDI engine (`trig()`, the `!` of its modulations), as from a trigger input; `hasTrig()` is true with
  `$<track>MIDI`.
- Examples: `SEQ/303-Patterns.cpp`, `SEQ/EuclidArp.cpp` (MIDI next to the CV outputs).

## Multi-trig drum engines

```cpp
static const char *_names[] = {"BD", "SD", "CH", "OH"};
static int32_t _sel = 0;
static std::pair<uint8_t, uint8_t> _keys[] = {{36, 0}, {38, 1}, {42, 2}, {46, 3}, {0xFF, 0}}; // {midi key, index}, 0xFF ends

// setup():
engine::addParam(MULTI_TRIGS, &_sel, 0, LEN_OF(_names) - 1, _names);
engine::setMultiTrigMidiKeyMap(_keys);

// process():
for (size_t i = 0; i < LEN_OF(_names); i++)
    if (engine::trig() & (1 << i))
        start_voice(i, engine::trigLevel(i));           // trigLevel: velocity-derived level
// mix voice i with engine::mixLevelL(i) / engine::mixLevelR(i)  (per-instrument level/pan from the mix page)
```

- When Trig-Input is patched, the one trigger fires the selected instrument. When it is `-`, every instrument gets its
  own trigger source on the I/O page, plus its MIDI key. With TRIG-INPUT `$<track>MIDI` the notes another track's
  app sends with `engine::midiOut()` play the instruments by the key map, and the instruments keep their own trigger
  sources as with `-`.
- `engine::setupMultiTrigs(count, names, keyMap)` is the variant without a selector param and without TRIG-INPUT
  (`DRUMS/TR909-HiHat.cpp`, `DRUMS/VA-909.cpp`): each instrument always has its own TRIG, another track's MIDI comes
  in by MIDI-INPUT. An app that uses `engine::accent()` has the row ACCENT-INPUT there too; `accent()` is set in a
  frame where an instrument is hit while the input is high, so it accents all instruments hit in that frame (as a
  MIDI note over velocity 100 does).
- Level and pan of the instruments (what `mixLevelL/R()` return, 8..248, 128 by default, saved with the patch) are
  set on the kit's **mix page**: the firmware adds it after the app's own pages, a row with LEVEL and PAN for each
  instrument that plays (with TRIG-INPUT patched the selected one, else those with their own TRIG and those a MIDI
  note played); L-ENC selects the instrument, R-ENC turns LEVEL or PAN, a press on R-ENC switches between them. The
  app's `draw()` and UI handler don't run there, and `isParamOnPage()` is 0 for all its parameters. The page is there
  only while it lists two instruments or more (not with TRIG-INPUT patched), so the page count of a kit changes at run
  time. An app without `mixLevelL/R()`, or with 4 pages of its own, gets none.
- Conventions: closed hi-hat chokes the open one; CC 4 controls the open hi-hat decay.
- Examples: `DRUMKITS/TR-707.cpp`, `DRUMKITS/RZ-1.cpp`, `DRUMKITS/DMX.cpp`, `DRUMKITS/DrumSynth.cpp`.

## Sequencers and clock (`engine_sync::`)

- `stepChanged()` is true in the frame a new 16th step begins; `stepReset()` is true when that step is a reset (restart
  the pattern). `step()` wraps at 128, so for `step() % len` with a `len` that does not divide 128 use `steps()`,
  which counts on from the last reset (call it in every `process()`).
- `samples_per_step()` is the measured step length in samples (default 6000 = 120 BPM; tracked between 60 and
  240 BPM). Use it for gate lengths.
- `ppn()` is the raw clock position (1–96, `engine_sync::CLOCK_RESET` = 97). `clk_bpm()` is BPM × 100, or 0 when no
  clock. The firmware sets it before every `process()`. The module shows no tempo in the header; an app that shows it
  draws it itself (`SEQ/Clock.cpp` in a text item, `GND/Midi-Monitor.cpp`).
- CV outputs play every sample of the buffer, so a pulse can begin inside the frame (`SEQ/Clock.cpp` FREE: on its
  sample). Another track that takes the output as a virtual signal (`$<track>A`) reads sample 0 of each frame only.
- Write gates and V/OCT with `outputBuffer_i16` (`5 * PITCH_PER_OCTAVE` = 5 V). Quantize a note together with its
  transposition, `engine::qz_note(note)` (`SEQ/303-Patterns.cpp`), not the note alone plus `cv_i32()`;
  `engine::qz_process()` (with hysteresis) is for a CV.
- Examples: `SEQ/EuclidRythm.cpp`, `SEQ/303-Patterns.cpp`, `SEQ/TuringMachine.cpp`, `SEQ/EuclidArp.cpp`,
  `SEQ/Clock.cpp`, `FX/Delay.cpp` (tempo-synced).

## Persistence

- Param values, modulations and the multi-trig mapping/level/pan are saved with the patch automatically.
- For extra state, call `engine::setPatchStateEx(&state, sizeof(state))` in `setup()`. The raw bytes of a static object
  are saved and restored with the patch (`SEQ/TuringMachine.cpp` its RNG state).

## Memory, flash data, files, samples

- Non-const globals live in the module's RAM (`RAM_SIZE` in `app/apps.txt`). RAM is shared by 4 engines and the
  firmware, so keep it lean. Make lookup tables `const` so they stay in the flash image (`BIN_SIZE`). `FLASHMEM` is
  empty in app builds and only documents intent.
- `malloc/calloc/realloc/free/new/delete` work. The firmware tracks every live block of an engine (no fixed limit) and
  frees them on unload. Check for `nullptr`.
- Allocate in `setup()`, `draw()` or the UI handler, not in `process()`: it runs in an interrupt, which can come while
  the main loop is inside `malloc()` (loading another track's engine, USB), and the heap has no lock. What `process()`
  may need, `setup()` reserves (`SYNTH/plaits.cpp`: SYNTH/Plaits keeps the memory of its largest model in every voice
  and makes another model's engine in it with placement new).
- `machine::fs_read("NAME", &size)` returns a named flash blob (ROM dumps, patch banks uploaded with the web app), or
  `nullptr` when it is missing. Show a hint in that case (`DRUMKITS/TR-707.cpp`: "ROMS NOT FOUND"). Names without a
  `/` are records of the blob index, at most 15 characters; names with one are in the chain of apps (31 characters).
- **Data blobs of an app:** a file `app/<CAT>/<Name>/<file>.smp` next to the app is the blob `<CAT>/<file>.smp`, which
  the app reads with `fs_read()`. `app/dump.py` lists it in `app/index.json` `"data"`; the web app writes and updates it
  like an app. Large tables belong there rather than in the app: on the DSM0 an app is copied into
  the internal RAM only as a whole, and one too large for it runs from the QSPI flash at 2 to 3 times the cycles.
  `.smp` is the only extension.
- `ENGINE_NAME("CAT/@MAGIC")` registers one engine per flash blob whose content starts with `MAGIC`. The menu name is the
  blob name, and `__data` points to the blob (`FX/FV1emu.cpp` `FX/@!FV1`, `DRUMKITS/DrumSynth.cpp` `DRUMKITS/@!RC8`).
- FatFS subset: `f_open/f_read/f_write/f_close/f_tell/f_size/f_opendir/f_findnext/f_closedir`. Volumes are `SD/`,
  `USB/`, `SYSTEM/` and `FLASH/`, as far as the module has them. Keep I/O per `process()` small (one frame per call).
- Samples: `sample_u8/s8/Am6070(data, len, rate, shift)`, `sample_wav(data, len)`,
  `sample_wav_from_file(path, reuse)`, `sample_set_pos(s, pos, amp, decay)`,
  `sample_process(s, start, end, speed, cv_volts, outL, outR=nullptr)` and `sample_get_info()`.
  `sample_process` **adds** to the output, so clear the buffer first.

## Build

```bash
bash app/build.sh              # build changed apps (Git Bash on Windows)
bash app/build.sh --rebuild    # build all
bash app/build.sh --setup      # only the build tools (.venv, gcc, udynlink in .build/), no app built
MCPU=cortex-m4 bash app/build.sh --rebuild   # -mcpu for mkmodule (default cortex-m7)
```

- Needs bash, python3, curl, unzip, git and jq. The first run creates `.build/.venv`, downloads xpack
  `arm-none-eabi-gcc` 15.2.1 (`GCC_VERSION` in `build.sh`) into `.build/` and clones `eh2k/udynlink`. The gcc: on
  Linux x64 only the parts the build uses, 31 MB from the release 0.0.0 of this repository (checked by its SHA-256,
  needs xz and sha256sum), else the xpack archive (307 MB).
- An app is rebuilt when its `.bin` mtime differs from the `.cpp` mtime. Output goes to
  `app/<CAT>/<Name>/<Name>.{bin,elf,elf.txt,log}`, `app/index.json` and `app/apps.txt` (`app/dump.py`: `BIN_SIZE`,
  `RAM_SIZE` and `CRC32` of every app).
- `-mcpu` comes from `MCPU` (default `cortex-m7`), passed as `--mcpu` to mkmodule. The ISA is the same as `cortex-m4`
  (ARMv7E-M); the difference is the instruction scheduling for the M7's dual-issue pipeline.
- Compile flags: always `-fsingle-precision-constant -DNDEBUG -pedantic -fno-exceptions -I. -I./lib/`, plus by default
  `-mfloat-abi=hard -mfpu=fpv5-d16 -ffast-math`. The working directory is the repo root, so include as
  `"../../squares-and-circles-api.h"`, `"lib/stmlib/..."` or `"stmlib/..."`.
- **The build fails if the log mentions `__aeabi_*`.** Those are ARM runtime helpers the loader cannot provide: 64-bit
  integer division, `double` math, and float math under `-mfloat-abi=soft`. Stick to `float` and 32-bit ints.
- **The menu name** is `CATEGORY/FileName`, or what `ENGINE_NAME` says, after the `#include` of the API, at the
  start of its line (`build.sh` takes the names from the preprocessor output and gives them to mkmodule as the module
  name):
  ```cpp
  ENGINE_NAME("SYNTH/Foo;SYNTH/Foo2");  // (or the macro once per name) -> several menu entries from one binary
  ```
  - Names are limited to 19 characters, category, slash and all. The macro is a `static_assert` on that, so a longer
    name fails the build.
  - A version after the name, `ENGINE_NAME("SEQ/Clock#v2")`, is part of the ID a patch stores, not of the name the
    menu, the web app and the README show. An engine whose patches of before mean something else gets the next
    version, the name before as `ENGINE_ALIAS` and a `restore()` that converts them (below). Example: `SEQ/Clock.cpp`
    (`SEQ/Clock#v2`), a parameter put first: the old ones move one place on (`params[2] = old[1]`).
  - An `ENGINE_NAME` inside an `#if`/`#ifdef` that is off is no engine.
  - A patch stores the engine as the CRC32 of its name. An app that gets another name or moves to another folder keeps
    its patches with `ENGINE_ALIAS("OLD/Name");` in the source: the old name, after the `ENGINE_NAME` it belongs to,
    once per old name. `build.sh` reads these lines like the names (`app/engine_aliases.py`) and compiles them into
    the binary; a patch loads the engine of its name first, then the one with that alias.
  - An engine that took over engines with other parameters gives their layout: `ENGINE_ALIAS("M-OSC/Chord",
    "fbffff")`, a letter per old parameter as the patch stores it (`'f'` float, `'b'`/`'h'`/`'i'` int of 1/2/4 bytes).
    A patch of that name then calls the app's `engine::restore(id, old, n, params)`: `id` is `ENGINE_ID("M-OSC/Chord")`,
    `old` the old values (`float value`, `void *mod`), `params` this engine's with their defaults; the app sets
    `params` (`params[2] = old[1]` moves the modulation along, `params[1].value = 6` a constant), the firmware applies
    them and drops the old modulations no parameter took. Values of another length load as they are, as with an alias
    without a layout; then `restore()` gets `old` nullptr and `params` as they loaded. Older firmwares don't call it.
    Examples: `SYNTH/plaits.cpp`, `SYNTH/braids.cpp` (M-OSC/Waveforms with V_OCT 2 octaves down).
- **Magic comments** (grepped by `build.sh`, anywhere in the file, conventionally below the license):
  ```cpp
  // build_flags: -mfloat-abi=hard -mfpu=fpv5-d16 -ffast-math -funroll-loops  -> REPLACES the default flags
  // src_files: lib/x/a.c lib/x/b.c      (repo-root relative, repeatable) -> extra translation units
  ```
  - The greps are substring matches: `// xbuild_flags:` is **not** disabled, and any line containing `src_files` counts.
  - `-mfloat-abi=soft` apps (`SEQ/EuclidRythm.cpp`, `SEQ/TuringMachine.cpp`) must avoid float
    arithmetic because of the `__aeabi_` rule. They use fixed-point math (`fpsin`).
- `WARNING: Ingoring unknown symbol '.text' in relocation list` (on the console, not in the log) is harmless: it
  comes from mkmodule's first pass, and its second pass relocates those pointers -- the labels of a `valueMap`,
  a table of function pointers -- as data.
- On Windows, if MSYS2's `python3` or `arm-none-eabi-gcc` come first in `PATH`, mkmodule fails with
  `No module named 'elftools'` (the `.venv` has a `python.exe` but no `python3`) or builds with the wrong gcc. Put a
  `python3` that runs `.venv/Scripts/python.exe` and `.build/xpack-arm-none-eabi-gcc-15.2.1-1.1/bin` first.
- Since GCC 12, libstdc++ computes `std::exp`/`std::log`/`std::pow` of `std::complex<float>` with `cexpf`/`clogf`,
  which are not whitelisted (see below). Write them out with `expf`/`logf`/`atan2f`/`std::polar`
  (`lib/bbd/bbd_filter.cc`).
- **Only whitelisted external symbols resolve**: the API, float libm (`sinf cosf tanf tanhf powf expf exp2f logf
  log2f log10f sqrtf fmodf floorf ceilf truncf atanf atan2f sinhf coshf`), `memset memcpy memmove memcmp` (`memcmp`
  only on a current firmware; an older one shows `7 MEMCMP`), `strlen strcpy strncpy strcat strcmp strncmp strchr
  strrchr strstr stristr`, `sprintf/snprintf/vsnprintf`, `malloc/free/new/delete`, `rand/srand`, and a few more. An
  unresolved symbol makes the engine show a debug screen that lists it (`7 MEMCMP`, `ERROR:8`), and the engine doesn't
  start. The build does not catch it: check `arm-none-eabi-nm -u app/<CAT>/<Name>/<Name>.elf` for anything else.

## Porting patterns and conventions

- One app is one file. Use file-scope `static` globals with `_name` style and the MIT license header with author:
  a new app names the person it is written for (ask for the name, or leave the author line out), never the author
  of the existing apps; only a changed copy of an app keeps its notice, with a line for the changes added
  (the circuit models SYNTH/VA-303, DRUMS/VA-909 and `lib/va303`, `lib/va909` are MPL-2.0: the notice at the top of
  each file, `LICENSE` in the lib; a file added there gets it too).
- The app's `README.md` ends with `## License and credits`, kept as short as `app/SYNTH/plaits/README.md`'s: one line
  per part (the app's file, each library, third-party data or reference used), each with © year, author, license
  name and one link, then a trademark line if a name is one.
- Use `FRAME_BUFFER_SIZE`, `SAMPLE_RATE`, `DEFAULT_NOTE`, `PITCH_PER_OCTAVE`, `LEN_OF` and `CONSTRAIN`, not literals.
- Reuse DSP from `lib/` (Mutable Instruments stmlib/plaits/braids/peaks/rings/clouds/streams, msfa, faust,
  soundpipe, ...) by `#include`-ing the `.cc` files, usually at the end of the app.
  - Symbol clashes: `#define kFoo kFoo2` before the include (`SYNTH/braids.cpp`).
  - `#define private public` / `#define protected public` is the accepted way to reach into library internals.
- int16 libraries (peaks): use `outputBuffer_i16(OUT_AUDIO)` and build `GateFlags` from trig/gate
  (`DRUMS/808ish-BD.cpp`).
- Library block size ≠ 24: bridge it with a FIFO (`stmlib::RingBuffer` as in `SYNTH/DxFM.cpp` and `GND/FFT.cpp`). A
  trigger that arrives in a frame with no render has to wait for the next one: keep it until a block is rendered.
- Several models in one engine, chosen like a waveform: an int parameter with labels picks the model at runtime. The
  other knobs keep their values across a switch; don't set them in `process()` to a model's defaults, since the
  firmware restores parameter variables after it. Knobs that mean something else per model get the model's names in
  `draw()` (`setParamName()`; `SYNTH/plaits.cpp` SYNTH/Plaits: Plaits' 16 models, each its own library engine, a
  knob that does nothing in a model named `""`). When each model wants its own defaults, make it an engine of its own
  instead (several `ENGINE_NAME`s, the model from `engine::name()`, below).
- `lib/drumsynth` is a copy of a repository of its own: change it there.
- 64-bit integer multiplies and shifts are inline on the M7; only 64-bit division needs a helper, which the
  `__aeabi_` rule forbids. A quotient with a known range can be found by shift and subtract instead.
- A library that computes at another rate than 48 kHz, where pitch and times matter: resample it (4-point Hermite)
  rather than run it fast; an effect's input the other way, its output a block late, so a block of input is always
  there.
- More than 10 parameters on a page need no UI of their own: the firmware shows them as a list, the left encoder
  scrolls and the right one edits. An instrument's own pages read better as pages (`\f`, above). Report a missing
  blob from `setup()` with `gfx::message()`, which only sets the text.
- 0 V with the `V_OCT` knob at C4 plays C4 (MIDI 60); the knob shows the note of 0 V. An engine for another register
  sets the knob's default (`SYNTH/VA-303.cpp`: C2, the 303's C at 0 V, as `SEQ/303-Patterns` sends it).
- Faust DSP: implement the `faust/ui.hxx` `UIGlue` callbacks as `addParam` (`FX/Rev-Dattorro.cpp`, `DRUMS/Djembe.cpp`).
- One binary, many engines: several `ENGINE_NAME` names, then branch with `strcmp(engine::name(), ...)` in `setup()`
  (`SYNTH/plaits.cpp`, `CV/EnvGen.cpp`). The I/O is inferred per binary, so every engine of it gets each input any
  of them references; engines with other inputs go into apps of their own. Names may span categories, and a
  category the firmware has not seen is a new menu entry of its own, sorted after the known ones. In a category the
  engine page lists an app's engines together, in the order of its `ENGINE_NAME`s. The MIDI program numbers follow
  the page's order.
- When you add an engine, also add it to the engine list in `../README.md`.

## Reference apps

| topic | app |
|---|---|
| smallest audio generator | `NOISE/NoiseGen.cpp` |
| audio effect / processor | `FX/Reverb.cpp`, `FX/Delay.cpp`, `FILTER/SVF.cpp` |
| CV generator | `CV/LFO.cpp`, `CV/V_OCT.cpp`, `CV/EnvGen.cpp` |
| synth voice with V_OCT, trig and accent | `SYNTH/VA-303.cpp` |
| sequencer | `SEQ/EuclidRythm.cpp`, `SEQ/303-Patterns.cpp` |
| multi-trig drum machine | `DRUMKITS/TR-707.cpp`, `DRUMKITS/DrumSynth.cpp` |
| MIDI poly synth | `SYNTH/DxFM.cpp` (MIDI voices next to Trig/CV voices, staggered blocks); `SYNTH/plaits.cpp` (a library voice and engine per MIDI voice, idle voices skipped; the drums of the same binary no MIDI engine) |
| MIDI out | `SEQ/303-Patterns.cpp`, `SEQ/EuclidArp.cpp` |
| custom UI, patch state | `SEQ/Clock.cpp`, `SEQ/TuringMachine.cpp` |
| screensaver | `GND/Scope.cpp`, `SEQ/TuringMachine.cpp`, `CV/EnvGen.cpp` |
| model picked by a parameter | `SYNTH/plaits.cpp` (SYNTH/Plaits: an engine per model, memory reserved in `setup()`, knob names per model, 2 pages) |

## Checklist

- [ ] `process()` exists and writes every sample of every output buffer every call.
- [ ] Named buffers (`outputBuffer_f32/_i16`, `inputBuffer_f32/_i16`) are requested once, in `setup()`.
- [ ] Only the inputs you need are referenced (`trig`, `cv`, `inputBuffer`, `engine_sync`); they define the I/O page.
- [ ] Param names and labels live in static storage; `V_OCT` is param 0 if the engine has pitch.
- [ ] No `double`, no 64-bit division, no non-whitelisted libc calls; the build log has no `__aeabi_`.
- [ ] Missing flash blobs or files are handled and shown (in `draw()`, or with `gfx::message()` from `setup()`).
- [ ] Gfx calls happen only in `draw()` and `screensaver()`; the screensaver clears the screen first.
- [ ] `ENGINE_NAME` is ≤ 19 characters (a version `#v2` counts); the README engine list is updated.
