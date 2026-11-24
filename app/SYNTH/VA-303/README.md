# SYNTH/VA-303

A TB-303 bass line voice modelled from the 303's circuit ([lib/va303](../../../lib/va303/README.md), the model in its
[MODEL.md](../../../lib/va303/MODEL.md)): the four-stage diode ladder with its nonlinear pairs, solved without a delay in
the loop, the filter and amp envelopes, the accent sweep circuit and the VCA as the schematic has them, with the parts'
values; what the schematic leaves open (the pot tapers, the levels at the summing node, the diodes' linear ranges) is
fitted to recordings of a real TB-303 (dinsync's "TB-303 reference recordings for x0xb0x builders", a measurement
reference only, not included). No code of the TB-03, of ReBirth or of the ABL3 is in it. The knobs are the 303's pots
as positions 0...1.

Beside SYNTH/XX-303 (Open303 with fitted characters) it is the other way to a 303: the circuit instead of a fitted
control. See [How it differs](#how-it-differs-from-synthxx-303). In the release it replaces SYNTH/XX-303 and loads its
patches ([below](#patches-of-synthxx-303)).

## Parameters

| Knob | Range | Default | |
|---|---|---|---|
| **V_OCT** | -4 ... +4 octaves | -2 (C2) | pitch at 0 V, C2 the 303's C (65.4 Hz) |
| **Acc** | 0 ... 1 | 0.5 | the Accent pot: how much an accented note lifts the cutoff and the level; below about 0.5 only the sweep |
| **Cutoff** | 0 ... 1 | 0.5 | the Cutoff pot (little happens in its lower part, as on the 303) |
| **Reso** | 0 ... 1 | 0.5 | the Resonance pot; self-oscillates at the top at mid and high cutoffs |
| **EnvMod** | 0 ... 1 | 0.5 | the Env Mod pot: the filter envelope's depth; it also lowers the base cutoff |
| **Dec** | 0 ... 1 | 0.5 | the Decay pot: the filter envelope, 200 ms to 2.5 s to a tenth; accented notes always 200 ms |
| **Wave** | SAW, SQR, AUX | SAW | the saw, the squarer's pulse (its width follows the pitch), or AUX: the AUX-INPUT in place of the oscillator |
| **Character** | TB-303d, TB-303e, TB-303h, TB-303i | TB-303h | the constants: fitted to the TB-303 recordings and a x0xb0x, that with the x0xb0x's stronger accent, that with a pattern on the x0xb0x, the accent's long brightness at a low cutoff, the Reso pot's law and the ring at full Reso, or the circuit's ladder; a change takes effect with the next note ([below](#characters)) |
| **Overdrive** | 0 ... 1 | 0 | the Devil Fish's Overdrive: the oscillator's level into the filter, the 303's at 0 (as without it), 66.6 times at 1 (logarithmic); the ladder saturates, its resonance quenched, the output pressed against the 303's limit: up to 16 dB louder at the same peak (at 1 the engine is -5 LUFS; over full scale the firmware limits it in the mix, after the part of the track's LEVEL below the middle) |
| **CutRange** | 303, DF | 303 | the Devil Fish's Cutoff range: DF makes Cutoff its pot, which goes higher and lower than the 303's: the 303's range between 0.35 and 0.8 of the knob, beyond it an octave higher at 1 (the resonance at full Reso an octave above the character's, TB-303h 3.5 kHz, as the manual's 5 kHz "an octave above the typical maximum for the TB-303"; acidvoice.com's TB-303 at 3.3 kHz is TB-303h's Cutoff 0.96 there) and down to silence towards 0 |
| **Muffler** | OFF, SOFT, HARD | OFF | the Devil Fish's Muffler, a clipper on the VCA's output that leaves the bass: SOFT rounds the loudest peaks (an accent's 1 to 2 dB), HARD clips them by 4 to 5 dB with a buzz above 2.4 kHz; a plain note at the knobs' middle stays below both. The Devil Fish's circuit is not published: this one is modelled after its manual's description, the split from the bass after a recording of its switch ([MODEL.md](../../../lib/va303/MODEL.md)) |
| **BassMod** | OFF, ON | OFF | the x0xb0x's bass mod, C20 and C21 0.1 uF for 0.01: the coupling into the VCA at 7.2 Hz instead of 72, +3 dB at 72 Hz, +7.6 dB at 32 Hz (as the Devil Fish's bass) |
| **FilterFM** | 0 ... 1 | 0 | the Devil Fish's Filter FM: the VCA's output (the Muffler in it) modulates the cutoff, linear in Hz, the depth the knob's square: little in the lower half on a plain note, towards 1 nasal and edgy (a held note at the knobs' middle +9 / +14 dB at 800 / 1600 Hz, the treble lower), much more at a low cutoff with high Reso, on accents and with the Overdrive (+20 ... +33 dB in the mids), towards chaos. The depth is chosen, not fitted: no recording of it has a known setting |

Two pages: page 1 the 303's knobs (left encoder V_OCT, Cutoff, EnvMod; right encoder Acc, Reso, Dec), page 2
([screenshot](VA-303_page2.png)) as a list: Wave, Character, Overdrive, CutRange, Muffler, BassMod and FilterFM (the
left encoder selects, the right one edits; a firmware before 2026-11-09 shows them as knobs). For some hours of
2026-11-19 CutRange's slot held ResoTop, a knob for how far Reso lowers the cutoff.

<a name="characters"></a>**Characters**: the same model with another set of constants (`va303::Params`):

- **TB-303d**: fitted to dinsync's recordings and to a x0xb0x (the 303's circuit, built by the user) together, the
  x0xb0x weighed as much: its Cutoff pot at five positions with the resonance off and full, and accents in a row at
  full resonance, which the recordings do not have. The resonance at a low cutoff is weaker, Reso lowers the cutoff
  only where it is high, the accents at full Reso are bright from the first, the Cutoff pot's lower part steeper.
  1.09 dB on the band measure (TB-303 1.40, the earlier TB-303b and TB-303c 1.28), the test notes 1.03; the ring and
  the brightness closer as well (MODEL.md: the resonance's ring, the accent's first 40 ms and its course at a low
  cutoff, the parts of the circuit for them);
  [MODEL.md](../../../lib/va303/MODEL.md#tb-303d-va303_tb303dh-fitrunsntxt). 1k instructions a frame more than the former TB-303.
- **TB-303e**: TB-303d with the x0xb0x's Accent pot measured at four positions: its accent is stronger and lasts
  longer than the recorded TB-303's (+25 dB over a plain note at Cutoff 0 and full Accent, +11 dB after 0.1 s; nothing
  in the pot's first third), the accent into the amp through a diode that turns on softly. Closer to the x0xb0x's
  accents, further from the recordings' (1.21 dB on the band measure, test 1.08);
  [MODEL.md](../../../lib/va303/MODEL.md#tb-303e-va303_tb303eh-fitrunsrtxt). As fast as TB-303d.
- **TB-303h** (the default): TB-303e fitted again with a pattern played on the x0xb0x (Accent, Cutoff, Reso and Decay
  full, accents in a row, slides into accents): its accents at an open cutoff are shorter, as the x0xb0x's (TB-303e's
  2 ... 4 dB too long from 40 to 160 ms); the accent sweep lifting the cutoff's floor as the envelope does: at a low
  cutoff an accented note stays brighter for some 300 ms, as the x0xb0x's, the sweep's parts at the circuit's values;
  the Reso pot's law of the two units, an S: little resonance at 9 and 12 o'clock, most of it by 3 o'clock, little
  more to the stop (a straight law rose steeply in the last quarter and ended 4 dB too peaky); and the ring at full
  Reso shorter, as the top node's load in the circuit (100k) has it: dinsync's ring decay at Reso 50 / 75 / 100 %
  within 0.05 octaves (0.57 without it), the x0xb0x's resonance peak over Reso within 1 dB (4 dB at the stop);
  dinsync's notes 1.14 dB (test 1.09), the TB-03's patterns 4.14 dB;
  [MODEL.md](../../../lib/va303/MODEL.md#tb-303h-va303_tb303hh-fitrunszftxt) (the steps there: TB-303f, TB-303g).
  0.5k instructions a frame more than TB-303e (the feedback's second coupling).
- **TB-303i**: the circuit's ladder, where the others bend its capacitors for the band levels: fitted to both units'
  held notes at Reso 0 it wants the circuit's capacitors and stronger couplings (the filter's output into the VCA,
  R121/C21, 72 Hz); the Cutoff pot's last fifth flatter, as on both units; the ring at full Reso from a scan of the
  feedback and the top node's load; the square an ideal pulse with the units' law (0.54 at C2, -0.05 an octave). The
  phases of both units within 10 ... 35 degrees (TB-303h 80 ... 123), the resonance's growth at a low cutoff within
  1.4 dB of the x0xb0x's (3.3), the accents' attack and course closer; dinsync's notes 1.23 dB (1.13; the square
  notes 1.27), its ring 0.07 octaves (0.05);
  [MODEL.md](../../../lib/va303/MODEL.md#tb-303i-va303_tb303ih-fitrunszsitxt). As fast as TB-303h.

The characters TB-303b and TB-303c (2026-11-03 ... 11-05) were left out for the flash (an app of three sectors):
TB-303d and TB-303e are closer on every measure but the accent's course at Cutoff 0 (MODEL.md). A patch of then plays
another one (its stored index: 1 TB-303b, 2 TB-303c, 3 TB-303d; now 1 TB-303d, 2 TB-303e, 3 TB-303h, 4 TB-303i).
TB-303f and TB-303g (2026-11-05 ... 11-19), the steps from TB-303e to TB-303h, were left out as well: on dinsync's
notes TB-303h is within 0.02 dB of them, on the x0xb0x's pattern and acidvoice.com's TB-303 as close or closer, and
their sounds were some 1 dB from TB-303e's or TB-303h's on the band measure (the app 0.6 KB smaller; until then 3 was
TB-303f, 4 TB-303g, 5 TB-303h, 6 TB-303i). The character TB-303 (index 0, fitted to dinsync's recordings alone: the
numbers below) was left out on 2026-11-08: its ladder latches above about 20 times the Overdrive and falls silent
(MODEL.md, "The Overdrive"); a patch with it plays TB-303d.

## Usage

A trigger starts a note, the gate holds it; a trigger while the gate is still high (overlapping MIDI notes or
SEQ/303-Patterns' slides) slides to the new note, as does a CV change while the gate is high: the 303's slide, an RC of
22 ms; in the first 4 ms of a new note a CV change jumps instead (the CV input's average is only partly at the new
note when its trigger comes: an external sequencer's notes started between the old pitch and the new and slid to it
over some 50 ms). The accent input accents the note if high at the trigger (from MIDI: velocity above 100). A note
starts 4 ms after the trigger, as on the 303. A bare trigger (its gate falls within 10 ms, as from a drum sequencer or
a clock) holds the note 62.5 ms from the trigger, a 303's step (half a 16th at 120 BPM); a trigger within that hold is
a new note, not a slide. A longer gate plays as it is.

<a name="patches-of-synthxx-303"></a>**Patches of SYNTH/XX-303**: a patch with SYNTH/XX-303 (the release's 303 from
2026-10-31 to 2026-11-14), or with the engines XX-303 had taken over (SYNTH/Open303, FILTER/TeeBee of 2026-04), loads
VA-303 when the module has no XX-303 (`ENGINE_ALIAS`, `restore()`): V_OCT, Acc, Cutoff, Reso, EnvMod, Dec and Wave by
their places, with their modulations, the Character VA-303's default (XX-303's characters are none of these), the
Overdrive 0: XX-303's Drive and Type have none (its Drive kept the level, the Overdrive makes the voice louder, some
10 dB at 0.4); SYNTH/Open303 with Wave SQR, FILTER/TeeBee with Wave AUX, as XX-303 loaded them. The knobs are the
same pots, the sound is VA-303's. A module with XX-303 on it loads that one (the name before the alias).

## How it works

[lib/va303](../../../lib/va303/README.md) is the voice; [MODEL.md](../../../lib/va303/MODEL.md) there has every block
with its parts and sources. In short:

- **VCO**: a band-limited saw (PolyBLEP); the square as the 303's squarer makes it from the saw: a pulse whose width
  follows the pitch, its top sagging through a coupling capacitor, its rising edge slower than its falling one (the
  even harmonics of the 303's square), as loud as the saw; TB-303i: the ideal pulse both units measure.
- **VCF**: the four-stage diode ladder (C18 0.018 uF at the bottom, three 0.033 uF; Tim Stinchcombe's analysis): the
  input pair's current into the bottom node, the top pair a single diode to the bias, each pair a conductance that
  falls with the voltage across it (`tanh(v)/v`), solved without a delay in the loop (TPT/ZDF) at 2x. The resonance
  feedback into the input pair's other base through a coupling capacitor, so the ladder self-oscillates at mid and
  high cutoffs, not at low ones; a large resonance saturates the pair that carries it and quenches itself. The cutoff
  from the exponential converter's summing node (Cutoff pot, Env Mod pot's share of the envelope, the accent sweep)
  plus a bias apart from it, which is why the lower part of the Cutoff pot does little and the envelope hardly opens
  the filter there, while from the middle up it sweeps octaves.
- **MEG**: a capacitor charged at the gate, discharged through the Decay pot's series resistance (200 ms to 2.5 s to a
  tenth; accents short the pot: 200 ms). The Env Mod pot's share goes into the cutoff; the envelope's resting level
  lies below zero, so Env Mod up also lowers the base cutoff (a held note gets darker and quieter).
- **Accent**: the Accent Sweep Circuit (the envelope through a diode and 47k into the Resonance pot's second section
  with a 1 uF capacitor, the wiper into the cutoff): at Reso 0 a hard snap, at Reso 1 a swell from the capacitor,
  which keeps its charge from one accent to the next, so accents in a row sweep higher ("wow"). The envelope also
  lifts the VCA through a diode: nothing below about half the Accent pot, +7 dB at 75 %, +10 dB at 100 %, not gated.
- **VEG/VCA**: the amp's envelope (1 uF through 1.5 M) falls while the gate is held, a note is silent after about 3 s;
  at the gate's end the amp stays open and then falls (the circuit's 8 ms and 8 ms; fitted: TB-303h 1.1 ms and
  25.5 ms, TB-303d 14.9 and 7.2 ms); a leak while closed (fitted at -120 dB, the bound of the fit: none heard).
- **Pitch**: the CV in octaves through the slide's RC (22 ms) only while a slide is on; a new note jumps. The 303's
  4 ms from the step to the note.

The constants of the first fit (the character TB-303 until 2026-11-08): the parts' values where the schematic gives
them, the rest fitted to the 400 single notes of dinsync's
TB-303 recordings (each knob at 25, 50, 75 and 100 % over the other four at 0, 25, 50, 75 and 100 %, saw and square,
with and without accent; five of the 25 sets never fitted), measured as all the 303 fits in this repository are: the
mean difference of the 1/6-octave band levels in 10 ms frames: **1.40 dB** over the 320 fitted notes and **1.40 dB**
over the 80 never fitted, the level within 1.4 dB (sd); recordings of the same nominal knobs in different sets differ
by 0.4 to 0.5 dB. On the same notes SYNTH/XX-303's characters: ABL3 2.45 dB, TB303 3.17, TB03 3.16, RB303 4.21,
Open303 7.57. The dynamics (slides, accents in a row, other pitches)
come from the circuit and were cross-checked against the TB-03's recordings (4.0 dB over its test patterns; the
character TB03, fitted to the TB-03, 2.0; TB303 2.7), not fitted. The engine's 2x semi-implicit float build against
the model's reference (8x, iterated, double): 0.05 dB.

CPU (2026-11-19): 20.0k instructions a frame in QEMU with TB-303h and TB-303i (the square 21.6k), 19.5k with
TB-303e, 18.2k with TB-303d; the Muffler 0.7k more, the bass mod 0.3k, the Filter FM 0.1k, the Overdrive and the
Cutoff range nothing. Setup 3 kinsn, RAM 1.3 KB, no tables: an engine change costs nothing audible. Code 12.3 KB, 4
flash sectors (11.9 KB and 3 sectors until 2026-11-21, before XX-303's patches; SYNTH/XX-303: 22.7 KB, 100 KB RAM,
1.4 Minsn of setup).
-21.5 ... -22.1 LUFS with the knobs at 0.5 (test_qemu, the four characters), 3 dB under the other synths: the room for
the accents and the resonance. What passes full scale (the accents at full Reso, the Overdrive) the firmware limits in
the mix (`ENGINE_MODE_SOFT_LIMIT`, as SYNTH/Plaits and SYNTH/DxFM: a track turned down below the middle of LEVEL
saturates less; a firmware without 0dbe484e, 2026-11-06, does not know it and clips at the DAC); before 2026-11-08
the app rounded it off itself from 0.8.

## How it differs from SYNTH/XX-303

- XX-303 is Open303's voice with a fitted control: a linear ladder (its nonlinearity is off), a mip-mapped saw table
  built at load, and per character some 90 polynomial constants that bend its knobs, envelopes and levels towards
  recordings of a voice. VA-303 is the circuit: a nonlinear ladder, a tabled nothing, 59 constants a character, each
  a part or a fitted property of a part (some off at 0). Where XX-303's fits had to add terms for what they saw
  ("the level follows the cutoff", "Env Mod lowers the base", "the gate's tail"), here they fall out of the structure.
- The resonance squelches and quenches; the knobs behave like the 303's (the lower half of Cutoff does little, Env
  Mod lowers the base, the accent's level boost has a threshold, the square's width follows the pitch).
- The same knob layout as XX-303, so its patches load here ([above](#patches-of-synthxx-303)); in place of XX-303's
  Drive the Devil Fish's Overdrive, in the circuit (the level into the filter), not after it, its Cutoff pot's wider
  range, its Muffler on the VCA's output and its Filter FM from there into the cutoff; the x0xb0x's bass mod in the
  coupling into the VCA.
- CPU 18.2k to 20.0k against XX-303's 11.7k to 12.4k instructions a frame (the nonlinear ladder at 2x), half the code,
  almost no RAM, no load cost.

## License and credits

- `VA-303.cpp` and [lib/va303](../../../lib/va303): © 2026 E.Heidt, [Mozilla Public License 2.0](../../../lib/va303/LICENSE).
- Reference: dinsync's [TB-303 reference recordings](http://www.dinsync.info/2010/02/tb-303-reference-recordings-for-x0xb0x.html)
  (not in this repository); Roland's service notes (redrawn by Kevin Lightner), Robin Whittle's Devil Fish
  measurements, Tim Stinchcombe's diode ladder analysis.

TB-303 is a trademark of Roland Corporation. Not affiliated with Roland.
