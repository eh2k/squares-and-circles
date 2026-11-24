# va303

A TB-303 voice modelled from its circuit, for SYNTH/VA-303 ([app/SYNTH/VA-303](../../app/SYNTH/VA-303/README.md)):
the diode ladder with its nonlinear pairs solved without a delay in the loop, the envelopes, the accent sweep circuit
and the VCA as the schematic has them, the knobs the 303's pots. [MODEL.md](MODEL.md) says what each block is and
where each constant comes from; `fit/README.md` (on the branch `va303` only, see below) how the constants were fitted
to recordings of a TB-303 and checked. Mozilla Public License 2.0 ([LICENSE](LICENSE)), (c) 2026 E. Heidt. No code
of the TB-03, of ReBirth or of the ABL3 is in it.

## Files

- `va303.h`: the whole model, header only: `va303::Params` (the constants, the list `VA303_PARAMS` with a comment
  each; `defaultParams` the circuit's start values, `paramNames`), `va303::Voice`.
- `va303_tb303.h`: `va303::tb303Params`, the first fit, to dinsync's TB-303 alone (written by `fit/gen_params.py`);
  the character TB-303 until 2026-11-08, no longer in the app (its ladder latches above about 20 times the Overdrive,
  MODEL.md, "The Overdrive").
- `va303_tb303d.h`: `va303::tb303dParams`, the character TB-303d: fitted to dinsync's TB-303 and a x0xb0x together,
  Reso's shift of the cutoff growing with the cutoff (`resoCutHz`) (MODEL.md, "TB-303d").
- `va303_tb303e.h`: `va303::tb303eParams`, the character TB-303e: TB-303d with the x0xb0x's Accent pot, D27 a soft
  diode (`accVcaKnee`) (MODEL.md, "TB-303e"). (TB-303b and TB-303c, `va303_tb303b.h` / `va303_tb303c.h`, were removed
  on 2026-11-05; their fits are fit/runs/e.txt and g.txt. TB-303f and TB-303g, `va303_tb303f.h` / `va303_tb303g.h`,
  the steps from TB-303e to TB-303h, on 2026-11-19; their fits are fit/runs/t.txt and x.txt.)
- `va303_tb303h.h`: `va303::tb303hParams`, the character TB-303h: TB-303e with a pattern on the x0xb0x (shorter
  accents at an open cutoff), the accent sweep lifting the cutoff's floor (`accFloor`, the sweep's parts at the
  circuit's values), the Reso pot's S (`resoS`), the top node's load near the circuit's and the feedback's second
  coupling (`fbHp2`): the ring at full Reso (MODEL.md, "TB-303f" ... "TB-303h"); the app's default.
- `va303_tb303i.h`: `va303::tb303iParams`, the character TB-303i: the circuit's ladder (its capacitors, the couplings
  into the filter and into the VCA), the Cutoff pot's flatter top, the ideal pulse of both units (MODEL.md, "TB-303i").
- `fit/`: the fit's tools (native builds of the voice, the measure, the fit, the checks), on the branch `va303` only,
  not on main.
- `build_app.sh`: builds one app of this tree as `app/build.sh` does (for a worktree without the udynlink submodule
  or its own `.build`). It needs what the main checkout of squares-and-circles has: the toolchain in
  `.build/xpack-arm-none-eabi-gcc-15.2.1-1.1/bin` and the venv `.build/.venv` (both from its `machine/toolchain.sh` /
  `app/build.sh`), `mkmodule` from its `machine/lib/udynlink` submodule; the branch `va303` alone has none of them.

## Using the voice

```cpp
#include "va303/va303.h"
#include "va303/va303_tb303h.h"
static va303::Voice voice;
voice.init(SAMPLE_RATE, FRAME_BUFFER_SIZE, &va303::tb303hParams);
voice.setNoteDelay(4);                              // the 303's 4 ms from the step to the note (default 0)
voice.setCvSettle(4);                               // ms after a new note in which the CV jumps, not slides (default 0)
voice.setWave(1);                                   // 0 saw, 1 square, 2 the external input
voice.setKnobs(cutoff, reso, envmod, decay, accent); // the pots, 0...1
voice.setParams(&va303::tb303iParams);               // another set of constants (a character): with the next note
voice.setOverdrive(g);                              // the Devil Fish's Overdrive: the level into the filter, 1 the 303's
voice.setDfCutoff(on);                              // the Devil Fish's Cutoff range: Cutoff as its pot, wider
voice.setMuffler(m);                                // the Devil Fish's Muffler: 0 off, 1 soft, 2 hard
voice.setFilterFm(pot);                             // the Devil Fish's Filter FM: the output into the cutoff
voice.setBassMod(on);                               // the x0xb0x's bass mod: the coupling into the VCA at 7.2 Hz for 72
voice.block(note, trig, gate, accent, out, in);      // a block: the sequencer's signals, the note in MIDI (fractional)
```

- `block()` takes what the firmware gives an app each frame: a trigger starts a note (while the gate is still on it
  slides to it, as the 303 plays overlapping steps), the gate's end releases it, a change of the note while the gate
  is held slides (the 303's slide: an RC switched into the CV, 22 ms), the accent input at the trigger. The 303's
  4 ms from the step to the note (`setNoteDelay()`, not a constant: the CPU's, and the fit renders without it) is a
  FIFO of blocks. `setCvSettle()`: within that time after a new note the note's changes jump instead of sliding, for
  a CV that comes through an input's average and is only partly at the new note when the trigger comes (the app: 4
  ms, the firmware's average 2 ms, the OCT41's 4).
- `in` (wave 2) replaces the oscillator with an external signal: the 303 as a filter and VCA played by the notes.
- `setDfCutoff()`: the Cutoff knob as the Devil Fish's pot, the 303's range between 0.35 and 0.8 of it, an octave
  higher at 1 (Reso's pull held at the 303's stop: its fitted law would eat the octave), towards 0 down to silence.
  `setFilterFm()`: the output, AC coupled, into the cutoff in Hz at the control's rate, 10 kHz per unit at 1 (the
  knob's square), chosen, not fitted. Both off by default: the voice as without them, bit for bit (MODEL.md, "The
  Devil Fish's Cutoff range and Filter FM").
- `kOsc()`: the linear ladder's oscillation threshold the resonance is relative to (17.0 for the 303's capacitors).

## Builds

Compile-time choices (`-D`), the engine's defaults first:

| Macro | Engine | Reference | |
|---|---|---|---|
| `VA303_REAL` | `float` | `double` | the arithmetic |
| `VA303_OS` | 2 | 8 | the oversampling of the core (ladder, oscillator, couplings, VCA); 2x decimates with a 39-tap halfband, else a 255-tap windowed sinc |
| `VA303_ITER` | 0 | 6 | 0: the ladder's conductances from the sample before (semi-implicit); n: iterated n times to convergence |
| `VA303_FAST` | `VA303_ITER == 0` | | the control every 4 core samples with a fast 2^x and a Pade tangent, else every sample with the library's functions |

The engine's build against the reference over the TB-303 recordings' notes: 0.05 dB mean of the 1/6-octave band
measure (fit/cmpbuild.py), 0.2 dB at most.

In an app: no `double`, no non-whitelisted calls (SYNTH/VA-303 links `exp2f`, `expf`, `logf`, `tanf` and `memset`). RAM
1.3 KB (the voice's state and the note FIFO, test_qemu), no tables, setup 3 kinsn: an engine change costs nothing
audible (the first note after a character's change computes its oscillation threshold, 3 kinsn once). SYNTH/VA-303 in
QEMU (2026-11-19): 18.2k instructions a frame with TB-303d, 19.5k with TB-303e, 20.0k with TB-303h and TB-303i (the
square 21.6k), the Muffler 0.7k more, the bass mod 0.3k, the Filter FM 0.1k; 11.9 KB of code with its four characters,
the Devil Fish's Cutoff range and its Filter FM.
