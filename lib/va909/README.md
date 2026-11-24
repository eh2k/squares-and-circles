# va909

The voices of a Roland TR-909 modelled from its circuit, for the app DRUMS/VA-909
([app/DRUMS/VA-909](../../app/DRUMS/VA-909/README.md): the engines DRUMS/VA909-BD, -SD, -RS, -CP and -Toms, a part each
with mods; -Hats, -Cymbals and -Kit, all eleven, off for now): every
state a capacitor's voltage, every path the schematic's, the knobs the 909's pots, the accent the voltage the 909's
CPU puts on the voices' ACCENT line. [MODEL.md](MODEL.md) says what each block is and where each constant
comes from; `fit/README.md` (on the branch `va909` only) how the constants were fitted to recordings of a TR-909 and
checked against SPICE. Mozilla Public License 2.0 ([LICENSE](LICENSE)), (c) 2026 E. Heidt.

All eleven: the bass drum (`bd.h`), the snare drum (`sd.h`), the hand clap (`cp.h`), the hi-hat, crash and ride
(`hh.h`), the toms (`tt.h`), the rim shot (`rs.h`).

## Files

- `va909.h`: shared pieces: fast exp2/log2, the 909's saturated-transistor VCA (Ebers-Moll, its base current, its
  collector node: `SatVca`, `BaseCurrent`, `VcaTable`, `vcaCurrent()`), the pots' tapers, tables, the noise.
- `bd.h`: the bass drum: `va909::BDParams` (the constants, the list `VA909_BD_PARAMS` with a comment each;
  `bdCircuit` the parts' values), `va909::BassDrum`.
- `va909_bd_fit.h`: `va909::bdFit`, the constants fitted to the TR-909 (written by `fit/gen_params.py`), and the
  recordings' ACCENT level and mix out high pass (`VA909_ACC_NORMAL`, `VA909_MIX_HP`).
- `sd.h`, `va909_sd_fit.h`: the snare drum (`va909::SnareDrum`, `SDParams`, `sdCircuit`, `sdFit`): its knobs TUNE,
  TONE, SNAPPY.
- `cp.h`, `va909_cp_fit.h`: the hand clap (`va909::HandClap`, `CPParams`, `cpCircuit`, `cpFit`): no knobs.
- `hh.h`, `va909_hh_fit.h`, `va909_cr_fit.h`, `va909_rd_fit.h`, `va909_blep.h`: the ROM voices: `va909::HiHat`
  (`HHParams`, `hhCircuit`, `hhFit`: CH DECAY, OH DECAY, one voice for both) and `va909::Cymbal` (`CYParams`,
  `cyCrashCircuit`, `crFit`, `rdFit`: TUNE), the ROMs' staircase (`RomStair`, band limited by the step of
  `va909_blep.h`) and VCA (`RomVca`). They play the ROM dumps `909_HIGH`, `909_CRASH`, `909_RIDE` (32 KB each; the
  app reads them with `machine::fs_read()`): without them they stay silent.
- `tt.h`, `va909_tt_fit.h`: the toms: `va909::Tom` (`TTParams`, `ltCircuit` / `ttCircuit(tom)`, `ltFit`, `mtFit`,
  `htFit`: TUNE, DECAY; one circuit, each tom its capacitors), `TomTables` (the shaper and VCA tables the three
  share), `TomNoise` (`TNParams`, `tnFit`: the tom noise, one for the three, into each tom's `process()`).
- `rs.h`, `va909_rs_fit.h`: the rim shot (`va909::RimShot`, `RSParams`, `rsCircuit`, `rsFit`): no knobs.
- `build_app.sh`: builds one app of this tree as `app/build.sh` does (for a worktree).

## Using the bass drum

```cpp
#include "va909/bd.h"
#include "va909/va909_bd_fit.h"
static va909::BassDrum bd;
bd.init(SAMPLE_RATE, &va909::bdFit);
bd.setKnobs(tune, attack, decay);   // the panel, 0..1 (coefficients only when they change)
bd.trigger(accentVolts);            // TRIG with the ACCENT line's volts (0..5; VA909_ACC_NORMAL a normal hit)
bd.process(out, FRAME_BUFFER_SIZE); // written, in volts at IC11a (LEVEL at its top)
```

- The output is the voice's individual out (DC coupled); the 909's mix out is a high pass (`VA909_MIX_HP`) and an
  inversion, which the app adds.
- A trigger starts nothing anew but the VCO's phase: the capacitors charge from where they are (rolls, flams).
- The voice sleeps once Q12's base current is under -80 dB of a hit's and its click is over (0.28 s at noon).
- In an app: no `double`, only whitelisted calls (`expf`, `exp2f`, `logf`, `tanf`, `sqrtf`, `memset`). The app
  with only these three voices in QEMU (2026-11-05): RAM 9.9 KB (states, the VCA tables), setup 1.5 M instructions
  (the shapers' tables by Newton, the click network's matrix exponential); in the frame of a hit 9.0k (BD), 11.9k
  (SD), 7.0k (CP).
- A part changed while the voice runs (a mod: the app's copy of `bdFit` with R27 changed, ...): `bd.setParams(&p,
  false)` computes the scalar coefficients again, not the tables (the shaper's and the VCAs', the click's network: the
  parts each `setParams()` names), some exp() each; `setParams(&p)` (`true`) all of it, as `init()`. The snare, the
  clap and the rim shot have the same; the toms have no tables of their own (`TomTables` is the app's). The cymbals'
  `setDecay(d)` reads their fitted envelope over the address at a / d (a decay that is not the 909's), the hat's clock
  is read from its `HHParams` at each block (`fclk`). The bass drum's `setDrive(g)` (a gain before R52, the triangle
  into D10/D11 times g) changes its shaper's table: `driveStep(points)` fills it again towards g, `points` pairs of
  points a call (v and -v: the shaper is odd; some 6k instructions a pair in QEMU, 64 pairs, from the ends in), outside
  `process()`; `shaperChanged()` after `setParams(&p, false)` with other D10/D11 (`dIs`, `dN`: the app's Diodes switch)
  has it filled again too. `setLevelRef(is, n)` keeps the body (Q12's current) as loud as with those diodes: the AC
  part of Q12's gain over the triangle's range, theirs measured first at a new drive (half the points), then the
  table's in every call while it fills, `level()` the ratio, `process()` follows it within 1 ms (1 exactly while every
  point is of those diodes: the same samples).
- The snare and the clap are used the same way (`init`, `setKnobs` for the snare, `trigger`, `process`); each sleeps
  once its VCAs' base currents are gone (BD 0.28 s, SD 0.41 s, CP 0.92 s after a hit at noon).
