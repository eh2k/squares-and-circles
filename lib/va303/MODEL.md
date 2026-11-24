# The model behind lib/va303

A TB-303 voice built from its circuit: every block below is a part of the schematic (Roland's service notes as Kevin
Lightner redrew them; `tb303.gif` in the fit's work directory) with its parts' values, Robin Whittle's measurements
(the Devil Fish manual and "Investigating some unique aspects of the TB-303's sound"), and Tim Stinchcombe's analysis
of the diode ladder (timstinchcombe.co.uk, "diode"). What a part leaves open (pot tapers, the levels at the summing
nodes, the diodes' linear ranges) is fitted to recordings of a TB-303: dinsync's "TB-303 reference recordings for
x0xb0x builders" (dinsync.info, 2010), 400 single notes, see `fit/README.md` (the tools are on the branch `va303`
only). No code of the TB-03, of ReBirth or of the ABL3 is in it; the TB-03 and ABL3 recordings of the other branches
serve as a cross-check of the dynamics only. The character TB-303b (below) adds two measures of the same recordings,
the resonance's ring and the accent's first 40 ms, and the parts they pointed to.

The constants are the fields of `va303::Params` (`va303.h`, the `VA303_PARAMS` list, each with its comment); the
fitted sets are the characters' (`va303_tb303d.h`, `va303_tb303e.h`, `va303_tb303h.h`, `va303_tb303i.h`, the app's
default `va303::tb303hParams`; the first fit `va303::tb303Params` is no longer in the app, TB-303b, TB-303c, TB-303f
and TB-303g are left out with their headers). This file says where each comes from.

## The signal path

```
  pitch CV ──slide RC (22 ms, while sliding)──► VCO: saw / squarer ──C──► ┐
                                                                            ├─► input pair Q12 ──► diode ladder ──► C27 ──► VCA (BA662) ──► C38 ──► out
  Cutoff pot ─┐                                       feedback ◄── Reso pot ◄─┘ (C: highpass)        ▲
  Env Mod pot ◄── MEG ◄── gate, Decay pot, accent ────┼─► summing node (R71 2.2k) ─► expo converter ─┘ (the ladder's current)
  Accent pot ──► accent sweep circuit (D24, R46, VR4b, C13, R72) ─┘                                  VEG (C42, R123) + accent ──► VCA current
```

## The oscillator

- **Saw**: the VCO (IC11a, Q26, Q27/Q24/Q25) ramps and resets; rendered band-limited (PolyBLEP) at the core rate.
  `sawSign` -1: the 303's saw falls (Open303 renders it negated as well); it matters for the asymmetric stages.
- **Square**: the 303 makes it from the saw with a transistor stage behind a coupling capacitor (Q25, Q24, Q27, D25,
  C33, R105, Q28). Its width follows the pitch: `width = sqWidth + sqWidthK (1 - 65.4 Hz / f)`, w the share of the
  period the pulse is high after the saw's reset. The band levels can't tell w from 1 - w (the same magnitudes), the
  phases can: `fit/pulse.py` takes a square note's harmonics over those of the saw note with the same knobs, times an
  ideal falling saw (the filter, the couplings and the output drop out, the polarity stays), and fits an ideal pulse.
  The width falls with the pitch:
  - the dinsync TB-303: 0.530 at C2, alike in all 100 knob settings;
  - a x0xb0x (the 303's analog circuit, 2026-11-03): 0.544 at C1, 0.471 at C2, 0.430 at C3, 0.382 at C4, 0.352 at
    C5, `sqWidthK` -0.10;
  - Mike Janney's 303 (ABL's author, KVR's Open303 thread, 2009-12-04, "Pulsewidth versus frequency"): 0.589 at C1
    to 0.424 at C4, -0.096.

  The TB-03's law (0.41 at C1, 0.51 at C2, 0.56 at C3, measured for
  Character TB03) is its mirror: the defaults keep its +0.1. TB-303 and TB-303b have -0.075: the K that gives the
  x0xb0x's harmonics in their pulse shape (an ideal pulse needs -0.10; TB-303 -0.0725, TB-303b -0.073). The pulse is
  band-limited at both edges (PolyBLEP), its top sags through the next coupling capacitor (`sqSag`, a one-pole
  highpass) and its rising edge is slower than its falling one (`sqRise`, a one-pole on the rise only); `sqGain` its
  level relative to the saw. The fit makes them 47 Hz and 198 Hz: so the harmonics' magnitudes match the
  recordings' to 0.6 dB (H2 ... H12 over H1). The phases say otherwise: over its saw the 303's pulse is an ideal one
  to 1 %, the model's (over its ideal saw) 16 % off. With the shape the phases want (`sqSag` 8.7 Hz, no slow edge) the
  square notes lose 0.14 dB on the band measure, 3 dB of their second harmonic. Where the filter is open (SET-A1 at
  Cutoff 100 %) an ideal pulse of width 0.53 matches the recordings' odd harmonics within 0.3 dB (over H1, `sqGain`
  aside); at Cutoff 50 % it is 2 ... 3.5 dB too bright at H5 ... H8, as much as the saw note of the same knobs is (the
  Cutoff pot's law, "The level at Cutoff 0" below). So the sag and the slow edge carry the cutoff's error for the
  square notes, which the saw notes have no part to take up; the 303's pulse is the ideal one the phases say. With
  TB-303d's law of the Cutoff pot (2026-11-05) the saw at Cutoff 50 % is right within 1.5 dB over H1 ... H8 (TB-303c 4.1),
  and an ideal pulse of width 0.53 ... 0.54 comes within 1 ... 1.5 dB of the fitted shape's magnitudes: the sag and the
  slow edge still match the magnitudes a little better, the ideal pulse only the phases (not to be heard on a 65 Hz
  pulse): kept as fitted.
  With the circuit's ladder (TB-303i, 2026-11-07) the same: an ideal pulse costs dinsync's square notes 0.14 dB (1.27
  against 1.13), where the model is too bright anyway (the C, D, E sets, SET-A1 at a high cutoff), and a fixed lowpass
  after the filter for both waves (tried as `outLp`, removed again) did not take that over: it went to 11 kHz. But
  the shape bends the low notes most: at C1 the shaped pulse is 23 % off an ideal one (an effective width of 0.82,
  both units about 0.55), at C2 9 % (0.67 for 0.53). TB-303i takes the ideal pulse (user, 2026-11-07) with the units'
  law, `sqWidthOct` (new): the width changes by the same an octave, `+ sqWidthOct log2(f / 65.4 Hz)` (from the pitch:
  (note - 36) / 12), -0.05 (the x0xb0x -0.047 from 38 to 579 Hz, its 2026-11-03 takes -0.054, Mike Janney's -0.055),
  where `sqWidthK`'s `1 - 65.4 / f` flattens above 200 Hz.
- **Into the filter** through a coupling capacitor: `hp1` (Open303's 44.5 Hz, measured on a 303 by Robin Schmidt).
- **The Overdrive** (the engine, not a constant): the Devil Fish's (Robin Whittle, 1993) pot in place of R62 220k, the
  oscillator into the input pair's base: none, the 303's level, up to 66.6 times (R62 3.3k), "which causes the filter
  to operate under duress" and quenches the resonance: "the filter hits its limits at the high and low extremes of the
  waveform and cannot resonate at these limits" (Devil Fish manual, pp. 4, 18). `Voice::setOverdrive()` scales the
  oscillator (or the external input) before `hp1`; the input pair, the diode pairs and the output's limit do the rest:
  at 66.6 times the peak stays at the limit (+5 dB over the 303's level), the rms rises by up to 16 dB. The engine's
  knob starts at the 303's level (0: as without it) and leaves the pot's range below it out. The first fit
  (`va303_tb303.h`, the character TB-303 until 2026-11-08) latches above about 20 times: its input pair takes 1 /
  `driveIn` (4.4) at most, its diode pairs 1 / `driveLadder` (1.5), where in the circuit both are the same bias
  current, so the bottom node C18 runs away (-100 ... -200) and the ladder's output stays at a DC that fades through
  `hp2`; the reference build (8x, iterated, double) does the same. The others (`driveIn` 0.65 ... 0.75, their pairs
  near linear or TB-303i's 0.59) rise to 66.6 times. So TB-303 left the app (its constants stay here, written by
  `fit/finalize.sh`, the example in README.md).

## The filter

- **The diode ladder** (Q12 ... Q22): four nodes with the capacitors C18 0.018 uF (bottom), C19, C24 and C26
  0.033 uF, joined by the transistor pairs Q13/Q15, Q14/Q17 and Q16/Q23 wired as diodes; at the top the pair Q22,
  biased at a common point, "effectively a single diode in each arm" (Stinchcombe). The input pair Q12 feeds its
  current into the bottom node; its tail current, from the exponential converter, biases all the pairs and so sets
  the cutoff (Stinchcombe: `w_c = I / (4 C V_T)` for equal capacitors). In the model: `cap1` 0.545 (0.018 / 0.033),
  `topG` the top pair's conductance relative to the others (Stinchcombe's d, 1). With these the linear ladder's
  poles are his: the characteristic polynomial `s^4 + 8 s^3 + 20 s^2 + 16 s + 2` in units of the cutoff, a 4th-order
  lowpass whose poles lie far apart (0.15, 1.2, 2.8, 3.8), hence its gentle knee and the "18 dB" impression.
- **The nonlinearity**: each diode pair conducts `I = I_0 tanh(v / 2 V_T)`, so its conductance falls as the voltage
  across it grows: `tanh(v) / v` of the stage's voltage difference in units of its linear range (`driveLadder`); the
  input pair's current is `tanh` of its differential voltage (`driveIn`), which is the signal less the feedback. A
  large resonance saturates the pair that carries it: the resonance quenches itself, as Whittle describes ("the
  resonance of the filter is quenched when the filter is driven by a heavy input signal"). The conductances are taken
  from the sample before (the engine) or iterated to convergence (the reference, `VA303_ITER`); the four trapezoidal
  integrators and the feedback are then solved exactly, from the top node down (TPT/ZDF, no delay in the loop).
- **The feedback**: from the output buffer through the Resonance pot's first section into the input pair's other
  base, coupled by a capacitor: `fbHp`, a one-pole highpass; `fbHp2` (TB-303h) a second one, C15 and C23 both being in
  the way (0: none). The feedback `k = kOsc resoMax (q + resoCurve q (q - 1))`, with `resoS` (TB-303h) an S on top,
  `+ resoS q (q - 1) (2 q - 1)`, k held at 0 (0: none; below 0 the law is flat at both ends and steep between, as
  both units' resonance grows: little at a quarter and half the pot's turn, most of it by three quarters),
  `kOsc` the linear ladder's oscillation threshold, computed from the poles at init (17.0 for the 303's capacitors):
  with `resoMax` above 1 the filter self-oscillates at Reso 1 where the highpass lets it, i.e. at mid and high
  cutoffs, not at low ones ("The filter will only self oscillate at mid and high frequencies", Whittle). The level:
  a ladder with its feedback at the input loses 1/(1+k) of its passband, the recordings' level does not change with
  Reso (SET-A2, SET-E2: within 1 dB), so the output is scaled by `1 + resoGain k` (`resoGain` 1: the loss made up;
  Open303's TeeBeeFilter does the like with its `(1 + r)`). `resoCut`: octaves the cutoff moves per unit of Reso
  (the recordings' resonance peak at Cutoff 0 lies below the cutoff the harmonics' shape gives at Reso 0; 0: none). `resoCutHz` (TB-303d): the shift grows with the cutoff the knobs set, `fc^2 / (fc^2 + resoCutHz^2)` of it,
  for at a low cutoff Reso hardly moves it (a x0xb0x's full Reso up to some 700 Hz, dinsync's SET-A2) and at a high one
  it does (SET-E*); 0: the same everywhere (TB-303 ... TB-303c). The units differ most there: at full Cutoff and Reso
  the characters resonate at about 2 kHz (fitted to dinsync's TB-303, which has no such note, and the x0xb0x, 0.8 ...
  1.3 kHz), acidvoice.com's TB-303 at 3.3 kHz (per harmonic, 30 ... 100 ms into its plain notes); TB-303h with
  `resoCut` alone at 0.15 ... 0.25 of its own comes from 1.75 to 2.8 ... 3.0 kHz, its plain notes there 6.35 -> 3.2 dB
  and its accents (not fitted) 8.99 -> 7.24 dB; at Cutoff 60 % a little worse (2.98 -> 3.13 dB), at 0 the same (one
  clip, Env Mod taken as 0). On 2026-11-19 the app had that as a knob for some hours (ResoTop, `Voice::setResoTop()`:
  `resoCut` times 1 - t), replaced by the Devil Fish's Cutoff range, which gets there too (below, "The Devil Fish's
  Cutoff range and Filter FM"). A wider converter lowers that resonance (+1 octave: 2.2 -> 1.9 kHz), for the shift
  grows with the cutoff: with TB-303h's `resoCut` -3.99 and `resoCutHz` 6.4 kHz the cutoff after the shift at Reso 1
  rises with the converter's up to some 3.4 kHz of it (TB-303h's top), falls up to 10.6 kHz and rises again. Whether
  the unit, the knob's position or the law at the top: open (the fits hold `resoCut` at its bound, -4).
- **The top node's load**: each arm of the top node goes through C27/C25 0.1 uF into a base of the buffer's pair Q21,
  biased through R113/R114 100k; at the ring's frequencies the capacitors are a short, so the node (C26 0.033 uF across
  the arms) is loaded by a differential 200k: it drains at `1 / (2 R C26)` a second whatever the cutoff, where the
  diode pairs' conductances, which carry the resonance, scale with the cutoff. Hence a ring that decays by the same dB
  a millisecond at every frequency, as the recordings show at full Reso (`fit/ring.py`: about 2.3 dB/ms from 600 Hz to
  5 kHz), where a feedback that is a fixed fraction of the oscillation threshold decays by the same
  dB a period, i.e. faster the higher the cutoff (the fit v5: 2.5 dB/ms at 700 Hz, 16 at 8 kHz). `topLeak`, the
  resistance an arm in kOhm (0: none, the character TB-303 and the defaults); with it `resoMax` can be 1, the ladder never
  oscillating since the load takes more than the feedback gives above it.
- **Out of the filter** through C27 0.1 uF into the buffer Q21/Q20/Q19 and on: `hp2` (Open303's 24 Hz).
- **The cutoff** is the ladder's bias current: the exponential converter's (Q9, Q10, Q11) plus a bias apart from it.
  The converter's part is `2^(cutLow + cutRange c + envDepth taper(m) (meg + envRest) + accDepth sweep)`, in octaves
  because every input is a current into the node R71 2.2k at the converter's base, which is linear in the voltages:
  the Cutoff pot VR3 (50k B, linear) through R73 100k and TM3, the envelope through the Env Mod pot VR5 (50k A) and
  R63 220k, the accent sweep through R72 100k. The recordings' held saw notes (their harmonics against the linear
  ladder's response, `fit/calib.py`) put the cutoff at 185, 305, 906 and 1629 Hz for the Cutoff knob at 25, 50, 75
  and 100 % with Reso and Env Mod at 0, and at the same 185 Hz with the knob at 0: the lower part of the pot does
  nothing, and there the envelope and the accent don't open the filter either (SET-A3, SET-A5: no sweep to see at
  Cutoff 0, where at Cutoff 50 % the same Env Mod sweeps octaves). So the model has a **floor**, `cutFloor` Hz, a bias
  current the converter's part adds to: where the converter's part is small, the sweeps, which multiply it, stay
  under the floor; from the middle of the pot up they are the sweeps of the circuit. The envelope scales the floor
  linearly as well (`envFloor` per unit of the Env Mod pot): with the envelope's resting level `envRest` below zero
  the Env Mod pot lowers the base cutoff, as a held note at Cutoff 0 gets darker and 12 dB quieter from Env Mod 0 to
  100 % in the recordings (and in the TB-03's). `accFloor` (TB-303g): the accent sweep, into the same summing node,
  lifts the floor as well, linearly per unit of the sweep (0: none). `cutTop` a soft top of the converter (0: none). `cutPotTop` (TB-303i): where the Cutoff pot's right stop sits on its line, its last fifth flatter (0: a straight line; both units' cutoff rises less over the pot's last quarter than over the one before). `envDepth`: 9.8 V of
  envelope through 220k into 2.2k at 18 mV an octave = 5.2 octaves as the circuit says; fitted. The Env Mod pot's
  lower end goes to ground through R61 10k, so its wiper keeps `R61 / (R61 + VR5)` = 1/6 of the envelope at Env Mod 0
  (`envR61` 0.2, the share of the pot; 0: none, the character TB-303 and the defaults): the recordings' plain notes at Env
  Mod 0 fade 4 dB in their first 200 ms at Decay 25 %, 1 dB at Decay 100 %, more at a low cutoff than at a high one
  (SET-A1, SET-A4): the envelope still moves the filter.

## The envelopes and the accent

- **MEG** (the filter envelope): C62 1 uF, charged at the gate (`megAttack`), discharged through the Decay pot VR6
  (1M A) in series with R138 68k: `tau = megC (megRs + megRpot taper(d))` ms, with the defaults' 90k 90 ms to
  1.09 s, i.e. 200 ms to 2.5 s from 100 to 10 % as the schematic notes ("DECAY VR MAX T=2.5 sec MIN T=200ms"). An
  accent shorts the pot (IC12 4066). R117 22k is not in series, as this text had it until 2026-11-04: from 5.333 V it
  biases D26, which holds the node the discharge ends at (through D28/D27) at about 0.6 V; a simulation of that part
  gives 73 ... 77 ms with the pot shorted (180 ms to a tenth), near the fits' `megRs` of 56 ... 67k. `decayTaper`: the pot's audio taper, `(10^(k d) - 1) / (10^k - 1)`.
- **The accent sweep circuit** (Whittle's name): the envelope times the Accent pot VR7 (50k B), through D24
  (`accDiode`, its drop relative to the envelope's peak) and R46 47k (`accR1`) into one end of the Resonance pot's
  second section VR4b (50k), C13 1 uF at its other end (`accC` = 50k x 1 uF), the wiper through R72 100k (`accRm`)
  into the cutoff's node (`accDepth`: 9.8 V through 100k into 2.2k = 12 octaves). At Reso 0 the wiper is at the
  diode's end: the sweep is the envelope itself, a hard snap; at Reso 1 it is the capacitor's voltage, which charges
  slowly, swells and keeps its charge from one accent to the next, so accents in a row sweep higher ("wow").
  The diode conducts while the source is above the wiper; the node equations are solved per sample.
- **VEG and VCA**: the amp's envelope C42 1 uF with R123 1.5 M (`vegTau` 1.5 s) falls while the gate is held towards
  a level below zero (`vegTarget`: the VCA's current ends before the capacitor is full, a note is silent after some
  3 s, "~3 to 4 seconds"); at the gate's end the amp stays open `gateHold` 8 ms and then falls linearly in
  `gateFall` 8 ms ("8 ms of normal volume and 8 ms of linear decay"); `vegAttack` 3 ms. The accent adds to the VCA's
  current, not gated, through the diode D27 (R120, C36, R119): `accVca` times what the Accent pot's share of the
  envelope is above the diode's threshold `accVcaThr`. The Accent pot's wiper charges C36 0.033 uF through R119 47k
  (1.55 ms, `accC36`), C36 drains through R120 22k and D27 into the VCA's control while above the threshold ("to soften
  the attack a little", Whittle), scaled so that a slow envelope gives the same share as without it (0: none, the
  character TB-303 and the defaults). `accVcaTau` (TB-303d, TB-303e): the envelope that drives the accent into the amp
  falls with a time of its own instead of the MEG's: it follows the MEG up and falls from its peak with `accVcaTau` ms
  (0: the MEG itself, TB-303). `accVcaKnee` (TB-303e): D27 a diode, not a switch: it turns on over that width (of the
  envelope's peak) around its threshold, the softplus of the voltage over it, its slope blending C36's charge and
  discharge (0: a switch). The recordings show the threshold plainly (SET-A5): the
  accented note is as loud as the plain one with the Accent pot at 25 and 50 %, 7 dB louder at 75 %, 10 dB at 100 %.
  `accHp` a coupling (0 none). `bleed`: what the closed VCA lets through (-60 dB in the recordings).
- **Which notes are accented** (since 2026-11-05): the accent is the step's, taken at each trigger, a slid note's too
  (the 303 latches it per step; until then a slide kept the accent of the note before, and the x0xb0x's +8 dB on an
  accented slid note were missing), and it ends with the gate: its sweep, its share of the amp and the short decay.
  The x0xb0x's MIDI firmware drops it so; before, an accented note sounded on into the rest after it, at full Cutoff
  and Reso 5 dB under the note 40 ... 60 ms after the gate. The 303's own sequencer holds the accent to the step's end
  (Sonic Potions), which a voice without steps does not know; dinsync's notes (gates of 1.3 s) cannot tell, and the
  fit's measures did not change (1.213 / 1.083 dB, the x0xb0x's parts the same), nor did test_qemu's checksums.
- **The output amplifier** clips the loudest notes on one side: every accented saw of the recordings peaks at -0.36
  to -0.39 of full scale on its negative side whatever the knobs (SET-A5, D1, E1, E5), the squares' positive side
  reaches 0.53; Whittle: "With high levels of signal coming out of the VCA and with the Volume pot set at or near
  maximum, some clipping can occur in the output amplifier. This typically only happens on the loudest (for instance
  the accented) notes" (Devil Fish manual, p. 24, which also notes that the main output's polarity is opposite to the
  filter's: the model's output is the recordings' upside down). `vcaSat` the positive side's limit, `satAsym` the
  negative one's relative to it, a soft limit `u tanh(u)/u` with the ladder's Pade (0: none). The recordings' limit is
  likely their chain's, not the 303's: their phases are rotated (below, "The recordings' phases"), and the saws'
  negative and the squares' positive peaks stop at the same 0.37 ... 0.38 whatever their spectra, which a limit before
  the rotation would not give. That ceiling the model does not copy, but its soft limit is in use: up to TB-303f the
  fits kept it above the notes' peaks, TB-303g's and TB-303h's take the loudest notes down (36 of dinsync's 400 by
  more than 0.5 dB, up to 2.7 dB: the accents at Accent 100 % and at a high cutoff, SET-A5-14/16, E2-02/04, B5-14/16,
  D2-02), which then match better (A5-14 1.05 dB on the band measure, without the limit 1.71). The x0xb0x's clean
  takes want it too: without it TB-303h's accent at Cutoff 0 starts 2.7 dB too loud (with it 1.4), at Cutoff 12 h
  with the Accent pot at the right stop 5 dB (1), so it is the 303's own (2026-11-06).
- **The bass mod** (the engine, not a constant): the x0xb0x's mod, C20 and C21 0.1 uF for 0.01 ("absolutely essential if
  you want your x0xb0x to be a bassline machine", the x0xb0x mod guide), the couplings into the VCA: R121 220k with C21
  at 72 Hz moves to 7.2 Hz. Whatever share of the fitted couplings (`hp1`, `hp2`) that is, the change is the ratio of
  the two, a shelf (s + w72) / (s + w7.2) = 1 + 9 w7.2 / (s + w7.2) in front of the amp: +3 dB at 72 Hz, +7.6 dB at 32
  Hz, +1.8 dB at 100 Hz, +20 dB at the lowest (`Voice::setBassMod()`). The Devil Fish's bass is alike: "In the TB-303,
  32 Hz was down by 5 dB. Now it is down by 1 dB" (its manual, p. 6). In front of the amp, the output's limit and the
  Muffler get the bass as well; a plain note at the knobs' middle 2.6 dB louder in rms, 1.6 dB in LUFS (test_qemu).
  Below 20 Hz it lifts what the fitted couplings let through after the ladder (TB-303h's `hp2` 13.7 Hz: the notes'
  thumps, in PAT 2 at the level of its 60 ... 80 Hz band) by 17 dB; where the 303 has its 72 Hz coupling there, the fits
  put most of it in front of the filter (`hp1`), so the model's thumps are likely louder than a 303's with the mod
  (Characters.md: the clips).
- **The Muffler** (the engine): the Devil Fish's three-position switch on the VCA's output, "two types of muted
  clipping", which "softens the loudest extremes of the sound and introduces a moderate level square wave clipping
  buzz, whilst allowing the bass to pass largely unaffected"; it "affects sounds which are louder than usual" (the
  accents, the Overdrive, a loud input) "and has little or no effect when the signal level at the VCA output is low"
  (its manual, p. 6). Its circuit is not published (a mod guide's diodes to ground from C38 are "a suggestion, not an
  authentic devilfish mod"), so the model follows the description: diodes from the output behind a capacitor whose
  voltage follows the bass (a one-pole at 50 Hz); what is above it is linear up to the diodes' knee, then soft (the
  Pade's tanh) towards their limit, at the voice's level: SOFT (two in series) knee 0.2, limit 0.45; HARD (one) 0.15,
  0.22. A plain note at the knobs' middle peaks at 0.15 above the bass and stays clean; an accent at Accent 100 %
  (0.45 ... 0.6) loses 1 ... 2 dB of its peaks with SOFT, 4 ... 5 dB with HARD, which lowers the mids (600 ... 2400 Hz
  -3 dB) and adds the buzz above 2.4 kHz (up to +13 dB at 8 times the Overdrive); below 150 Hz within 1.1 dB.
  `Voice::setMuffler()`, after the output's limit, at the core's rate (2x).
  - **Against the Devil Fish (2026-11-08):** the one recording of it with the Muffler switched, Robin Whittle's rw11
    ("More drastic Filter FM and pulsations", firstpr.com.au/rwi/dfish/sounds/: "Muffler (switched from 0 to 1 at 44
    sec.)", 128 kbps mono, Overdrive, Cut Off, Env Mod, Accent and Filter FM turned as it plays). The switch is at
    45.1 s of the file: from one 0.3 s window to the next the peaks fall from 0.85 to 0.42 of full scale (-6 dB), the
    rms stays (0.107 / 0.098), the crest factor falls from 18 to 12.6 dB, below 150 Hz nothing changes (51.8 / 51.8
    dB), 150 ... 600 Hz -0.9 dB, 600 ... 2400 Hz -3 dB, above 2.4 kHz -2 dB; after it the peaks stay at 0.42 ... 0.49,
    a ceiling. The model's clipper on the take before the switch (1.2 s, its level +2.6 dB for the drift of the body's
    quantiles), fitted to the take after it (`fit/muffler_rw11.py`, the quantiles of |x| from 95 % up): the split at
    40 ... 60 Hz fits best (0.28 ... 0.32 dB), 75 Hz 0.38, 100 Hz 0.48, 300 Hz 0.92; the ceiling at 0.33 ... 0.36 of
    full scale, the knee anywhere from 0.12 to 0.27 (the ratio of limit to knee from 1.3 to 3 within 0.07 dB): the
    shape between knee and limit is not in the take. So the split went from 100 to 50 Hz; the levels stay where the
    description puts them (the recording's level and the Devil Fish's against the 303's are unknown, and the take's
    peaks are Filter FM's spikes, crest 18 dB, which the clipper takes 8 dB off where a 303 note without Filter FM
    has 9 ... 12 dB of crest). What the clipper does not give: 600 ... 2400 Hz lose 3 dB more in the take than the
    fitted clipper takes there, maybe the Filter FM, which the muffled output drives, maybe a muting of the Muffler's
    own; not modelled. Position 2 is in no recording.
  - **rw17 ... rw19** (two Devil Fishes with their Filter FM crossed, "I think I may have altered the Muffler settings",
    no positions): `fit/muffler_scan.py` scans for steps of the peaks within 0.1 s at a steady rms and bass, a ceiling
    on one side (max / 99th percentile of |x| under 1.35) and none on the other (over 1.45) finds rw11's switch and four
    in rw17 (its right channel: on at 138.6 s, off at 142.3, on at 271.1, off at 279.1), none it would trust in rw18 and
    rw19. The clipper fitted to each: the split 30 ... 100 Hz (100, 50, 30, 30), around the 50 Hz of rw11. The two
    muffled stretches of rw17 have ceilings a factor of 2 apart (at their best splits 0.126 and 0.061 of full scale, if
    the recording's gain stayed): maybe the two positions, as SOFT's limit is twice HARD's here. Their spectra lose far
    more than a clipper does (600 ... 2400 Hz -3 ... -13 dB, above 2.4 kHz up to -31 dB), where both Devil Fishes'
    Filter FM is driven by the muffled outputs: less level, less FM, fewer sidebands; so whether the Muffler also mutes
    (a lowpass while the diodes conduct) the recordings cannot tell. The videos were not searched (no downloader here).
- **The Devil Fish's Cutoff range and Filter FM** (the engine, 2026-11-19; the app's CutRange and FilterFM; both off by
  default, the voice then bit for bit as without them: 160 renders of the four characters, both waves, the Overdrive,
  Muffler and bass mod; test_qemu's checksums the same, 0.27k instructions a frame more).
  - **The Cutoff range** (`Voice::setDfCutoff()`): the manual (pp. 6, 8, 24) has the Devil Fish's Cutoff pot go "higher
    and lower", the filter's resonance at its right stop with no Env Mod "approximately 5 kHz. This is an octave above
    the typical maximum for the TB-303", the 303's range "between about 10.30 and 3 o'clock" (0.35 ... 0.8 of the
    turn), and below it the filter can pass "no audio at the frequencies the VCO is producing". The model: the knob is
    that pot, the character's range between 0.35 and 0.8; above, the cutoff (the converter's part and the floor, before
    the converter's top) times 2 to the 5 octaves a turn (an octave over the last 0.2, above the character's own top:
    the units' tops differ, so not 5 kHz absolute); below at the 303's slope (`cutRange` over 0.45 of the turn: some 5
    octaves at the left stop). Reso's pull (`resoCut`) is the one at the 303's stop: computed at the extended position,
    its fitted law would eat the octave (above). The clock positions and the top's figure do not agree at the 303's
    slope (3 o'clock would leave some 3 octaves above it): the top's figure is held. The resonance per harmonic at Reso
    1, Env Mod 0 (a held C2): at 0.8 as the 303 at 1 (TB-303d 1.77 kHz, e and h 1.83, i 1.90), at 0.9 2.5 ... 2.7, at 1
    3.5 ... 3.7 kHz; on acidvoice.com's 303_100 TB-303h at 0.96 resonates at 3.0 kHz (that unit 3.26), its plain notes
    6.35 -> 3.21 dB (`fit/acidvoice.py`, app/SYNTH/VA-303/Acidvoice.md on the branch va303). The left stop: the accent
    test's pattern (A#1, A#2) 39 ... 56 dB under the 303's Cutoff 0 (0.25: -11 dB, 0.15: -25 ... -29).
  - **The Filter FM** (`Voice::setFilterFm()`): "the audio output of the VCA" (the Muffler in it), AC coupled, via
    the Filter FM pot into the filter's frequency: "edginess and complexity dependent on the signal level coming out
    of the VCA - so it is stronger on accented notes. At higher settings, the results approach chaos" (p. 5; pp. 18
    f. with the Overdrive: "spluttering", the self-oscillation jumping an octave from bar to bar). The manual has the
    Filter Tracking pot and the FM jack linear in Hz, the pot's law not; the model takes it linear too: the output
    (the voice's level, a plain note at the knobs' middle peaks at 0.15) times `FM_HZ` 10 kHz at the pot's right stop,
    its square below, into the cutoff before its clamps (not below 1 Hz: the converter's current not below 0, the
    filter shut for those instants), coupled at 10 Hz (the output's limit is asymmetric: DC with the Overdrive). At the
    control's rate (every 4 core samples, 24 kHz, the output's mean over them): at the core's rate the code took 560 B
    more (the core is inlined twice), which the three flash sectors do not have; the octave bands of a held note up
    to 3.2 kHz within 0.5 ... 5 dB of that, above up to 9 dB, the top band (12.8 kHz up) lifted by the folding
    (still over 100 dB under the note's strongest band). The depth is chosen, not fitted: rw11 and rw17 ... rw19
    (above) have Filter FM, but turned as they play, no setting known. What it does (four held A#2, TB-303h, octave
    bands re FM 0): at the knobs' middle 0.5 -5 ... +4 dB (the 800 Hz band up, the treble down: the negative
    half-waves shut the filter), 1: +9 / +14 dB at 800 / 1600 Hz, the top -11; at Cutoff 0.3, Reso 0.9, Env 0: 1
    lifts 800 Hz ... 3.2 kHz by 23 ... 33 dB; with 8 times the Overdrive up to +27 dB, TB-303i up to +47. A fuzz of
    600 runs (the characters, waves, Overdrive to 66.6, Muffler, bass mod, the range, FM 0 ... 1, notes 0 ... 128,
    the AUX input with noise, switched within a run): no NaN, peak 0.66, at rest afterwards at most -75 dB (a fresh
    voice -78).

## The pitch

- The CV in octaves (the 303's DAC is 1 V/octave), through the slide's RC only while a slide is on: the 303 switches R
  (100k) into C35 0.22 uF, `slideTau` 22 ms ("60 ms" to settle); a new note without a slide jumps. The RC runs on after
  the gate (the 303's slide switch holds to the next step; until 2026-11-08 the gate's end set the pitch to the target
  at once), and a character's change waits for the next new note, not a slid one (until then a slide applied it
  mid-note). ("The usual rather fast slide of a TB-303 is caused by the 100K resistance of the DAC feeding a 0.22uF
  capacitor", Devil Fish manual p. 16; its 103.3k are the Devil Fish's: its 3.3k protection
  resistor added.)
- The note delay: the 4 ms the 303's CPU takes from the step to the note (Whittle), `Voice::setNoteDelay()` (the app: 4
  ms), a FIFO of blocks. Until 2026-11-08 a constant (`noteDelay`), which the fit held at 0 (the recordings' notes are
  aligned to their onsets) and which only `va303_tb303.h` had at 4: from TB-303d on the characters' headers took it from
  their runs, 0, so the app played without it from 2026-11-05 to 11-08 (only its TB-303 had the 4 ms). Now the engine's,
  not a character's: the CPU's, not the circuit's, and a character's change no longer moves the FIFO's read index under
  a note.
- The CV's settling (2026-11-19), `Voice::setCvSettle()` (the app: 4 ms; the fit 0): within it after a new note the
  note's changes jump. The firmware's CV input is the average of its ADC ring (2 ms, the OCT41 4 ms), so in the
  trigger's block it is only partly at the new note, and the rest, a change while the gate is held, slid 22 ms: a
  fifth up from an external sequencer started 253 cents flat and was 57 cents flat after 56 ms (host test, the average
  over 2 ms); now 1 cent from 20 ms on. A slide (a tied trigger) and a later CV change glide as before; test_qemu's
  renders unchanged (their CV moves only with a trigger).

## The rates

- The core runs `VA303_OS` times oversampled (2x in the engine: the ladder's nonlinearity and the PolyBLEP are
  band-limited enough there) and is decimated with the 39-tap halfband of SYNTH/XX-303's Drive (flat to 19 kHz,
  -72 dB from 29 kHz); the reference 8x with a 255-tap windowed sinc.
- The engine runs the control (the envelopes, the accent sweep, the amp, the exponential converter and the pitch)
  every 4 core samples, i.e. at 24 kHz, and ramps the filter's coefficient and the oscillator's increment between
  (`CTRL`), with a fast 2^x and a Pade tangent; the reference every core sample with the library's functions. The
  audio (oscillator, ladder, couplings, VCA) runs every core sample. Engine against the same model with the control
  every sample: 0.02 dB; against the reference over the recordings' notes: 0.05 dB mean, 0.2 dB at most
  (fit/cmpbuild.py, with the fitted constants).

## Fitted values (`va303_tb303.h`, fit/runs/v5.txt)

The character TB-303 until 2026-11-08 (left out of the app then: "The Overdrive" above).

Held at the circuit's values: the capacitors (`cap1` 0.545, `topG` 1), the VEG (`vegTau` 1.5 s), the slide (22 ms),
the note delay (4 ms), the pulse width's law with the pitch (the recordings have one pitch: `sqWidthK` -0.075, a
x0xb0x's, see Square above; until 2026-11-03 the TB-03's +0.1). The fit (five stages, fit/README.md) gave the rest;
what it says about the 303:

- **Cutoff**: the converter's part 78 Hz with the pot down (`cutLow` 6.29), 4.45 octaves over the pot, a soft top at
  9.7 kHz; the floor 62 Hz, scaled 2.7 x the Env Mod pot's share of the envelope. The envelope's rest level -0.28 of
  its peak, its depth 5.7 octaves (the circuit says 5.2), the Env Mod pot nearly linear (`envTaper` 0.59). With Reso
  up the cutoff falls 0.62 octaves.
- **Resonance**: the feedback at Reso 1 is 0.86 of the linear ladder's oscillation threshold, with a curve
  (`resoCurve` 1.19: little below half, most in the top quarter, as the 303's "log at both ends" pots), its coupling
  52 Hz, the level made up by 1 + 0.23 k. The input pair stays nearly linear (`driveIn` 0.22: the saw at a fifth of
  the pair's range), the ladder's pairs are driven moderately (`driveLadder` 0.68).
- **MEG**: 67k in series with 855k (the pot's 1M less what the fit took off), so 150 ms to 2.1 s to a tenth and
  150 ms on accents (the schematic: 200 ms to 2.5 s), the pot's taper 0.83 (log-ish), the attack 0.4 ms.
- **Accent**: the diode 0.29 of the envelope's peak, R46 small (0.04 of the pot), R72 1.9 x the pot, the capacitor
  62 ms, the depth 9.1 octaves (the circuit: 12 at the pot's full travel); into the amp 1.7 x the Accent pot's share
  of the envelope above 0.05, through a coupling of 0.5 Hz (a slow fade of the boost).
- **Amp**: the VEG falls with 1.5 s towards -0.07 (a held note is silent after about 4 s), the attack 0.15 ms, the
  gate held 13.9 ms after the gate and falling in 9.5 ms, the leak -62 dB.
- **Couplings**: into the filter 134 Hz (Open303 measured 44.5 Hz on its 303; the recordings want less bass before
  the ladder), out of it 2.4 Hz (none).
- **Square**: width 0.559 at C2, the sag's coupling 48 Hz, the rising edge 235 Hz (0.7 ms), 0.63 of the saw's
  amplitude.

Against the 400 notes (fit/cmp.py, the measure of fit/README.md): **1.40 dB** over the 320 fitted notes, **1.40 dB**
over the 80 never fitted (the level within 1.3 / 1.7 dB sd); SYNTH/XX-303's characters on the same notes: ABL3
2.45, TB303 3.17, TB03 3.16, RB303 4.21, Open303 7.57. With the
capacitors and the VEG left free the fit reaches 1.33 / 1.38 dB (runs/v4.txt: the bottom capacitor 1.6 x the others
and a VEG that never ends), not worth the circuit for this character; TB-303b takes the capacitors free since a x0xb0x
showed what they bring (below).

## TB-303b (fit/runs/e.txt; left out of the app on 2026-11-05)

The character TB-303b and its header `va303_tb303b.h` were removed when TB-303e came (VA-303 in three flash sectors);
TB-303d and TB-303e start from it. What follows is how it came about.

The same model with the four parts above switched on (the top node's load, R61, C36, the output's one-sided clipping)
and fitted again, with two more measures of the recordings beside the band levels: the ring after
each reset of the saw (its decay in dB/ms and its frequency, the medians in six windows of each saw note with Reso at
50 % or more, `fit/ring.py`) and the accented notes' first 40 ms (the peak and the energy of 4 ... 40 ms,
`fit/attack.py`). First (`fit/fit_b.sh`, runs/c.txt, d.txt) from v5 with the circuit's values of the new parts and
the capacitors held; then (`fit/fit_e.sh`, runs/e.txt, 2026-11-04) from d.txt with the capacitors free and the
accent's coupling into the amp off, after an A/B of the engine against a x0xb0x (below). The VEG's time and the
pulse's law with the pitch held as in v5.

What the x0xb0x showed (the engine with d.txt on an OCT41, the same notes on both, the x0xb0x's knobs turned by hand
along the engine's; the recordings and their analysis in the main checkout's `extern/x0x_reference/x0xb0x-2026-11-04`,
not in the repository):

- **The resonance lifts the band above its peak.** At full Reso every harmonic above the peak is 8 ... 10 dB louder
  than at Reso 0, on the x0xb0x and in dinsync's SET-A2 alike, for the saw and the square (so a linear property of the
  filter, not the input pair's clipping: a larger `driveIn` gives an intermodulation pattern, not a level step). d.txt
  had 3 ... 4 dB: its `resoCut` lowers the whole cutoff to put the peak where the 303's is, which costs the treble. At
  the same ring decay its ring was 5 ... 6 dB louder over the fundamental than the x0xb0x's.
- **The treble falls steeper.** With the Cutoff pot at 50 % d.txt was 7 dB too loud at the 10th harmonic and 12 dB at
  the 20th (SET-A1), at 100 % 3 and 6 dB.
- **The accent into the amp is not coupled.** With `accHp` (d.txt 0.5 Hz) the plain notes after accents were 2 ... 8 dB
  quieter than the others (a run of accents, then a plain note: -7.6 dB); the x0xb0x's stay at their level. Without it
  the band levels are the same (1.37 dB).
- Its Cutoff pot spans dinsync's lower three quarters (at its top as dinsync's at 75 %): a unit's spread (TM3). Its
  full Reso at Cutoff 0 is dinsync's SET-A2-13 to 1 dB. Env Mod at Cutoff 0 opens the filter on both alike (+13 and
  +11 dB at the onset, -17 and -16 dB after 300 ms).

What the fits made of the new parts:

- `cap1` 1.69, `topG` 0.35 (the circuit's 0.545 and 1; fit v4, with the VEG free as well, had 1.63 and 0.28): with
  them the resonance lifts the band above its peak (SET-A2 at Reso 100 %: +9 ... +4 dB over the 5th ... 24th harmonic,
  the recording +9 ... +4) and the treble falls as the 303's (SET-A1 at Cutoff 100 %: -28 / -42 dB at the 10th / 20th
  harmonic, the recording -28 / -43; at 50 % still 5 and 9 dB too bright). The linear ladder's poles are then 0.053,
  0.61, 1.91 and 3.37 (the circuit's 0.15, 1.21, 2.71, 3.77), its oscillation threshold `kOsc` 11.3 (17.0) at 0.87
  (1.38), i.e. the poles further apart and the resonance further above the lowest one. The circuit has no such
  capacitors: the values stand in for what the model lacks in the ladder (the pairs' unequal currents, the buffer's
  input, the top node's bias), measured, not explained.
- `topLeak` 49.6k an arm, with `resoMax` 1.001 (R113/R114 alone are 100k: 1.3 dB/ms; the node loses more than the
  buffer's bias, the pairs' base currents, the diodes' own losses; d.txt 28.7k).
- `envR61` 0.10 (the circuit's 0.2).
- `accC36` 29 ms (the circuit's 1.55 ms): the fit makes it a slower rise of the accent into the amp. With 1.55 ms (in
  d.txt) the band levels are the same and the attack's peaks 1.52 dB off instead of 1.12, so it stands in for
  something else in the first periods of an accent (below). `accHp` 0.
- `vcaSat` and `satAsym` as in d.txt (2.08, 5.9): the positive side (the recordings' negative one) limited at 0.67 of
  full scale, above the peaks of most notes; the fit did not take the recordings' ceiling of 0.38.
- The rest near v5: `driveIn` 0.55 (v5 0.22), `driveLadder` 0.43 (0.68), `fbHp` 43 Hz (52), `hp1` 118 Hz (134),
  `hp2` 13 Hz (2.4), the floor 128 Hz (62), `accVcaThr` 0.12 (0.05), `accVca` 3.0 (1.7), `resoCurve` 1.03 (1.19),
  `megRs` 60k (67k).
- `sqWidthK` -0.075 (since runs/d.txt; c.txt had +0.1): the pulse's width falls with the pitch as on the x0xb0x
  (Square, above). -0.075 is the K that gives the x0xb0x's harmonics in TB-303b's pulse shape (`sqSag` 47 Hz,
  `sqRise` 205 Hz; with an ideal pulse it is -0.10). With `sqWidth` 0.561 the second harmonic is weakest around
  80 ... 100 Hz (-21 dB), -16.6 dB at C2 and -10.8 dB at C4 (before: -16.6 and -12.6, flat in between). At C2, where
  all the recordings are, nothing changes. TB-303 the same (-0.075, its shape gives -0.0725).

| | TB-303 (v5) | TB-303b (d.txt, until 2026-11-04) | TB-303b (e.txt) |
|---|---|---|---|
| band levels, the 320 fitted / 80 test notes | 1.40 / 1.40 dB | 1.37 / 1.37 dB | 1.27 / 1.30 dB |
| level difference sd | 1.38 dB | 1.26 dB | 1.13 dB |
| the Cutoff pot's notes (SET-A1) | | 1.9 dB | 1.1 dB |
| ring decay, mean of the cells (Reso x half octave) | 1.58 octaves | 0.55 octaves | 0.56 octaves |
| ring at Reso 100 %, 700 Hz ... 8 kHz (the recordings: 2.4 ... 1.5 dB/ms) | 2.5 ... 15.6 dB/ms | 1.8 ... 1.7 dB/ms | 1.65 ... 1.88 dB/ms |
| ring at Reso 75 %, 700 Hz ... 1.4 kHz (the recordings: 5.0 ... 7.4 dB/ms) | 14 ... 21 dB/ms | 14 ... 21 dB/ms | 11 ... 16 dB/ms |
| accent: 4 ... 40 ms, accented less plain, mean difference to the recordings (peak / energy) | 1.95 / 1.24 dB | 1.13 / 1.05 dB | 0.97 / 0.71 dB |
| SET-A5 at Accent 100 % (the recordings: +15.6 / +16.7 dB) | +29.3 / +20.4 dB | +23.1 / +18.3 dB | +22.7 / +20.9 dB |
| brightness, each period's centroid, mean difference | 0.233 octaves | 0.192 octaves | 0.167 octaves |
| the TB-03's patterns (not fitted), test / test-hiq | 4.00 / 3.84 dB | 3.76 / 3.67 dB | 4.03 / 3.83 dB |
| engine against reference | 0.05 dB | 0.06 dB | 0.06 dB |
| QEMU, instructions a frame (test_qemu, knobs at 0.5) | 17.1k | 18.2k | 18.1k |

Open:

- **The ring at Reso 75 %** still decays twice too fast. The pot's law that gives it (`resoCurve` -0.3: 3.8 ...
  13.6 dB/ms against the recordings' 5.0 ... 15.0, tried with d.txt) costs the band levels: with the slow ring the
  model's resonance is louder and sweeps further down than the 303's (SET-E2-09: 4.6 to 1.6 kHz against 3.3 to
  1.2 kHz). The x0xb0x's ring at full Reso decays at 3.5 dB/ms (dinsync's 2.4, the model's 1.8).
- **The accent at Cutoff 0** fades too soon: accented over plain on the fundamental the x0xb0x has +22.5 / +10.4 /
  +4.8 dB at the onset / 100 / 200 ms, e.txt +19.0 / +3.8 / +1.8 (d.txt +15.4 / +2.2 / +0.1); dinsync's notes there are
  3 to 5 dB louder than the model's in their first 200 ms as well. The MEG's time on accents at the circuit's 90 ms
  (`megRs`) gives +7.8 dB at 100 ms but 1.42 dB on the band levels unfitted, and the x0xb0x's plain notes at Decay 0
  fall faster than the model's, not slower: the accent's way into the amp has a slower time of its own. TB-303c
  (below) takes it up.
- **The accents in a row** at full Reso sweep higher on the x0xb0x (the centroid 673 to 856 Hz over six accented
  16ths) than in the model (564 to 701 Hz).

## TB-303c (fit/runs/g.txt; left out of the app on 2026-11-05)

Removed with TB-303b (its header `va303_tb303c.h`); TB-303d starts from it.

TB-303b fitted again for the accent at a low cutoff (`fit/fit_f.sh`, then `fit/fit_g.sh`, 2026-11-04), the third
character beside it.

- **What the recordings show.** At an open cutoff (SET-C5, D5, E5: the cutoff far above the first harmonics) the first
  harmonic shows the amp alone. There the accented note over the plain one falls with about 106 ms from a threshold at
  0.18 of the envelope's peak, for every position of the Accent pot within 0.3 dB: nothing at 25 %, +4.0 / +6.8 /
  +8.5 dB at 50 / 75 / 100 % at the onset, the end at 150 ms. The filter's sweeps want the MEG's time (some 60 ms);
  with it no threshold, knee or compression gives the amp's course (0.55 dB at best). e.txt stood in for it with a
  slow C36 (29 ms: a slow rise as well, which E5 does not show) and a short sweep at a low cutoff (R46 1.5k, D24
  2.8 V: both far from the schematic).
- **The model:** `accVcaTau` (above), the drive into the amp falling with its own time.
- **The first fit** (`fit_f.sh`, runs/f.txt): from e.txt with `accVcaTau` 106 ms and R46 47k (`accR1` 0.94), D24 0.6 V
  (`accDiode` 0.06), C36 through R119 1.55 ms (`accC36`) at the circuit's values (held), the threshold and gain of the
  amp's accent as measured (0.18, 2.82) and the output's limit at the recordings' 0.40 of full scale on both sides
  (`vcaSat` 2.08, `satAsym` 1) as the start. Measured as well: the level's course after the attack (`fit/attack.py`
  `course()`, the energy of 40 ... 450 ms in five windows, `--cw` 15), which the band levels weigh little. First the
  accent, MEG, the saturation, R61 and the gain over all 320 notes, then everything. It took the break out of SET-A5's
  accent but lifted the note at Accent 25 % by +0.9 dB (the recordings: nothing).
- **D24's threshold.** That lift is the sweep's, not the amp's: f.txt without the accent into the amp keeps it,
  without the sweep it is gone. With D24's drop at 0.06 of the envelope's peak the sweep starts below a quarter of the
  Accent pot; `accDiode` 0.12 / 0.16 / 0.20 / 0.24 give +0.4 / +0.1 / -0.1 / -0.3 dB at 25 % (f.txt otherwise). The
  second fit (`fit_g.sh`, runs/g.txt) holds it at 0.20, the rest as the first from f.txt. More than the diode's 0.6 V:
  the Accent pot's wiper is loaded (R46 into the sweep, R119 into C36), which takes its lower positions down further
  than a linear pot's; the model does not have that load.
- **What it made of them:** `accVcaTau` 135 ms, `accVcaThr` 0.27, `accVca` 2.65, `accDepth` 10.5 (e.txt 8.9), `accRm`
  1.85 (2.05); `vcaSat` 2.38, `satAsym` 4.3, i.e. not the recordings' limit; `megRs` 60k; the rest near e.txt.

| | TB-303b (e.txt) | f.txt (fit_f.sh) | TB-303c (g.txt) |
|---|---|---|---|
| band levels, the 320 fitted / 80 test notes | 1.27 / 1.30 dB | 1.28 / 1.34 dB | 1.27 / 1.32 dB |
| level difference sd | 1.13 dB | 1.11 dB | 1.10 dB |
| the level's course, 40 ... 450 ms, mean difference (all notes / accented) | 0.98 / 0.76 dB | 0.96 / 0.75 dB | 0.94 / 0.71 dB |
| SET-A5 at Accent 100 %, accented less the plain note's error, at 4 ... 40 / 40 ... 80 / 80 ... 130 / 130 ... 200 / 200 ... 300 / 300 ... 450 ms | +4.3 +0.5 -1.5 -1.3 -0.5 -0.2 dB | +2.7 +1.4 +0.1 -0.8 -0.8 -0.3 dB | +2.1 0.0 -0.9 -0.9 -1.0 -0.4 dB |
| SET-E5 at Accent 100 % (open cutoff), the first harmonic accented over plain at 0 / 50 / 100 / 150 ms (the recording: +8.5 / +5.4 / +2.4 / +0.3) | | +8.3 / +5.7 / +2.9 / +0.1 dB | +8.4 / +5.8 / +3.1 / +0.5 dB |
| accent: 4 ... 40 ms, mean difference to the recordings (peak / energy) | 0.97 / 0.71 dB | 1.20 / 0.84 dB | 0.92 / 0.66 dB |
| Accent 25 %, 4 ... 40 ms accented over plain (the recordings: -0.2 / -0.3) | -0.3 / -0.2 dB | +0.9 / +0.4 dB | -0.1 / -0.3 dB |
| ring decay, mean of the cells | 0.557 octaves | 0.568 octaves | 0.562 octaves |
| brightness, each period's centroid, mean difference | 0.167 octaves | 0.171 octaves | 0.173 octaves |
| the TB-03's patterns (not fitted), test and test-hiq | | 3.92 dB | 3.84 dB |
| QEMU, instructions a frame (test_qemu, knobs at 0.5) | 18.1k | 18.1k | 18.1k |

TB-303c is TB-303b's equal on the band levels (the test notes 0.02 dB behind) and closer on the accent: its first
40 ms, Accent 25 %, the level's course after it. SET-A5's accent no longer breaks in after 100 ms; what is left
there (-2.5 dB) is the level of every note at Cutoff 0, plain ones as well (below).

Open:

- **The level at Cutoff 0.** Every note of base A (SET-A*, the Cutoff pot down) is 2.5 ... 3.5 dB too quiet in both
  characters, plain or accented, with the right spectral shape: a gain at a low cutoff, not a shift of the cutoff (a
  flatter floor of the cutoff makes the spectrum too bright). The recordings' Cutoff 25 % has the harmonics of the
  model's Cutoff 0 (within 0.2 dB, `fit/potlaw.py`) and 3.7 dB more level, 50 % those of the model's 32.5 % and
  2.8 dB more, from 75 % up the two agree: the pot does little in its lower part, and there the 303 is louder at
  the same shape. A pot law to those points breaks the bases C and D (the sweeps on a steeper law), a gain that
  follows a low cutoff did not help in a fit: the part that makes it is not in the model. On a x0xb0x alike
  (`fit/lvlshape.py`, its held notes through the Cutoff pot, the model's cutoff scaled to each one's shape): +4.3 dB
  at 310 Hz falling to 0 at 1.5 kHz, so the 303's, not the recordings' chain. With the circuit's capacitors (d.txt,
  v5) that level is flat over the cutoff on both; it is the fitted ones (`cap1` 1.72, `topG` 0.33, which give the
  treble's slope and the lift above the peak) that bend it. Fitted with the circuit's held (`fit/fit_j.sh`): 1.40 dB
  on the band levels and SET-A still -2.1 ... -2.3 dB, the lowest positions of the Cutoff pot out of reach. The fix
  wants both: the circuit's poles, the pot's lower part that does little, and the treble and the lift from a part the
  model does not have yet.
- **The level with Reso at a low cutoff.** From Reso 0 to 1 the 303 gets +1.3 dB louder at a low cutoff (the x0xb0x,
  `ab_reso1`), +1.8 dB at Cutoff 0 (SET-A2); the model +5.5 ... +7.0 dB (with the circuit's capacitors as well). At a
  high cutoff the two agree within 1.5 dB. The x0xb0x again on 2026-11-05 (`fit/x0xcut.py`, the Cutoff pot at the
  left stop, 9, 12, 3 o'clock, the right stop, Reso 0 and full, clean notes): +5.4 / +3.6 / +1.1 / -5.8 / -7.1 dB,
  TB-303c at the same shape +12.1 / +9.9 / +7.2 / -3.5 / -5.9: the model's resonance some 6.5 dB too strong where
  the cutoff is low ("the filter will only self oscillate at mid and high frequencies").
- **The Cutoff pot's law** (the same takes, the model's cutoff matched to each position's shape at Reso 0): 95 / 125 /
  203 / 872 / 1277 Hz, flat at the bottom, steep from 12 to 3 o'clock, flat at the top, as dinsync's (268 / 393 /
  1684 / 3036 Hz at 25 ... 100 %). The level at the same shape, x0xb0x less model, +4.9 ... -1.5 dB with the fitted
  capacitors, +4.9 ... +2.1 with the circuit's. A taper of the pot fitted with the circuit's capacitors held went
  back to 0.07 (1.44 dB): the band levels do not take one of these parts alone.
- **The accent's sweep at full Reso and a low cutoff** (`fit/x0xaccent.py`, Cutoff 12 h, Reso and Accent full, 16ths):
  on the x0xb0x the first accent is already bright (the centroid of H1 ... H30 368 Hz, H6 ... H20 within 1 dB of
  H1 ... H3), the run rises to some 450 Hz, a plain note after accents stays bright (215 ... 254 Hz, -3 dB; alone
  162 ... 176 Hz, -20 dB). TB-303c with its plain notes matched (`fit/x0xaccent_model.py`): the first accent 203 Hz,
  -11 dB, then rising over the run: its sweep's capacitor C13 charges too slowly at full Reso. dinsync's sets have no
  accent at a high Reso and a low cutoff, so the fits never saw it.
- **Both units in one fit** (`fit/x0xmeas.py`, `fitva.py --xw`, `fit/fit_l.sh`): the x0xb0x's takes beside dinsync's
  notes, the x0xb0x's own calibration of its cutoff (an offset, the pot's range) from its shapes at Reso 0 (-1.40
  octaves, 1.03). Left free, that calibration moved and took the takes' errors with it; held, the fit did not move the
  model (runs/l.txt: 1.283 dB, the Reso's growth still +7.0 ... +1.2 dB too much over the pot, the accents 0.62
  octaves too dark): no value of the parts it has gives both units. By hand: a higher `fbHp`, more drive in the ladder
  or at its input, a heavier top node (`topLeak` 12 ... 35k) each trade one error for another; `resoCut` 0 (Reso not
  lowering the cutoff) gives the x0xb0x's full Reso (its shapes 6.2 -> 2.9 dB, the accents 0.69 -> 0.35 octaves) and
  breaks dinsync's base E (2.06 dB on the band levels, 4.3 at E). So the two units disagree where Reso moves the
  cutoff: dinsync's at a high cutoff, the x0xb0x's at a low one. Next: the calibration without the floor (TM3 is in
  the Cutoff pot's way only; the floor is a bias of its own), and Reso's shift of the cutoff with the cutoff.
- **The ring at Reso 75 %** decays twice as fast as the recordings'. A linear Resonance pot (`resoCurve` 0, as the
  circuit's VR4a) gives their decay within 0.6 dB/ms, but the fit with it ends at 1.43 dB on the band levels: with
  that ring the model's resonance is louder at a middle Reso than the 303's. Something that lengthens the ring
  without raising the peak is missing.
- **The Accent pot's load** on its wiper, in place of the raised D24 threshold.
- **SET-B5 at Accent 100 %** falls too soon after 150 ms (+1.1 dB over plain where the recording has +2.6).
- **The output's limit:** the fits keep `vcaSat` / `satAsym` near e.txt's; the recordings stop every accented note at
  -0.35 ... -0.39 and +0.35 ... +0.39 of full scale.

## The recordings' phases (2026-11-04)

The band levels see magnitudes only. The harmonics' phases relative to the first, `theta_k = arg H_k - k arg H_1`
(independent of where the window starts), show what the waveform looks like:

- **dinsync's recordings are phase-rotated.** Against the model's notes (`fit/rotation.py`, the model's output
  negated) every harmonic of every note is turned by the same angle: `theta_rec - theta_mod = phi (1 - k)` with `phi`
  +46 degrees (quartiles +44 / +50) over 50 saw notes of all 25 sets, 7 degrees left over, whatever the Cutoff,
  Reso, Env Mod and Decay. No circuit does that; a phase rotator does, as mastering uses to make a waveform's peaks
  symmetric. So the recordings' peaks and waveforms are not the 303's output (their limit above); magnitudes and the
  ratios of two notes (`fit/pulse.py`: the same rotation in both) are unaffected. The angle follows the model's
  couplings: TB-303g +36 degrees (6 left over), TB-303h +74 (12; its `hp2` 13.7 Hz for TB-303g's 37 turns H1 against
  the rest by some 25 degrees).
- **The x0xb0x's are not.** Its held notes at Cutoff 0 (`ab_cut1`, through the OCT41's line-in) against the engine
  of the same take (digital, d.txt): within 17 degrees at H2 ... H4; against the model rendered as dark as the x0xb0x
  (`fit/capphase.py`, the cutoff scaled until H2 ... H8 over H1 match) the difference is that of a one-pole highpass
  near 20 Hz, the line-in's coupling, within 6 degrees with the circuit's capacitors (d.txt), within 15 with the
  fitted ones (g.txt: `cap1` 1.72, `topG` 0.33). A hint that the fitted capacitors are not the ladder's poles but
  stand in for something else; the line-in's corner is assumed, not measured.
  - **Measured (2026-11-06):** the OCT41's line-in (a slow square from a DAC output, DC-coupled, through the line-in
    and T1 GND/Scope to the USB audio, the averaged step's response at k 66.9 Hz) turns `theta_k` by -3.4 / -6.1 /
    -8.7 / -10.9 / -13.4 degrees at H2 ... H6, a one-pole highpass at 2.7 Hz (the step's decay 46 ms): not 20 Hz.
    With it none of the parameter sets matches: the circuit's capacitors (d.txt) are 56 degrees off, TB-303g 44,
    TB-303h 78, and each wants a further one-pole highpass on the 303's side, 17.8 / 13.5 / 24.2 Hz (then 2 ... 4
    degrees). So the test does not favour the circuit's capacitors; it shows a coupling the model lacks or sets
    otherwise (C38, the VCA's output capacitor, is not modelled; `hp1`/`hp2` are fitted: TB-303h 154 / 13.7 Hz, the
    circuit's 44.5 / 24), which takes the poles' share of the phases with it.
    Fitted with the circuit's `hp1` and capacitors as start (`fit/fit_zn.sh` ... `fit_zq.sh`, `hp2` free for C27 and
    C38 together: on a held note one), the phases of both follow: the x0xb0x's at Reso 0 at all five Cutoff
    positions 15 degrees off (TB-303h 109), dinsync's rotation 4 ... 7 degrees left over (TB-303h 12, phi +37 ...
    +48), `hp2` at 110 ... 117 Hz, the capacitors held or free (then 0.87 / 0.87). But dinsync's band levels end at
    1.22 ... 1.32 dB and the ring at 0.13 ... 0.22 octaves (TB-303h 1.13, 0.046), so the fitted capacitors stay:
    they buy the magnitudes, the phases are what they cost. A separate C38 would add nothing a held note can tell
    from `hp2`.
- **The accents in a row** at full Reso (`ab_accrun`, `fit/accrun.py`, period-synchronous: the centroid of H1 ...
  H30 and H6 ... H20 over H1 ... H3, 5 ms after the onset, three periods): the x0xb0x's centroid 339 Hz at the first
  of eight accents, 330 Hz at the last, the upper harmonics -4.7 dB at both, so no rising "wow" in this take; the
  model's flat too (TB-303b 399 -> 386 Hz, TB-303c 360 -> 419). The 673 -> 856 Hz noted before came from a centroid
  over windows not locked to the period. The take is at Cutoff 0, where the accent's sweep does not open the filter
  (SET-A5): a take at a middle cutoff would show the sweep's capacitor C13. That take (`accrun_12h_v12`, Cutoff 12 h,
  the fit's part `acc` since TB-303f) has it: the x0xb0x's centroid 360 Hz at the first accent, 570 Hz from the
  third on (+0.67 octaves); TB-303g and TB-303h rise as much (+0.65) but lie 0.35 / 0.5 octaves lower (2026-11-06).
  Fits that take them up (fit/README.md, "The accents at full Reso") slow the ring at full Reso again or lose the
  x0xb0x's pattern at full Cutoff and Reso: the x0xb0x's sweep opens more at a middle cutoff than the model's.

## TB-303d (`va303_tb303d.h`, fit/runs/n.txt)

Fitted to dinsync's notes and to the user's x0xb0x's takes of 2026-11-05 together (`fit/fit_n.sh`, `fit/x0xmeas.py`,
`fitva.py --xw 40`: the x0xb0x weighed about as much as all of dinsync's notes), from TB-303c with `resoCutHz` free.

- **Why both.** dinsync's sets have no accent at a high Reso and a low cutoff, and their Reso sweeps say little about
  the resonance's level at a low cutoff; the x0xb0x's takes have both: its Cutoff pot at five positions with Reso 0
  and full, a run of accents at Cutoff 12 h and full Reso. The x0xb0x is another unit: its cutoff is mapped by three
  values of the fit, not of the voice (`#x0xCutOff` -2.60 octaves on the converter's part, `#x0xCutScale` 1.35 on the
  pot's range, `#x0xFloor` 0.63 on the floor; from its shapes at Reso 0, 0.55 dB, and held: left free, an offset alone
  moved and took the takes' errors with it, the model unchanged).
- **The two units disagree** where Reso moves the cutoff: at some 700 Hz dinsync's SET-C2 wants -0.7 octaves, the
  x0xb0x at 3 o'clock none. With the x0xb0x weighed as before (`--xw 12`) the fit kept dinsync's (TB-303c's sound); the
  user chose the x0xb0x's. Weighed so, the fit left its earlier minimum and found one better for dinsync's notes too.
- **What it made of the parts:** the Cutoff pot steeper with its bottom lower (`cutLow` 5.59, `cutRange` 6.19, the floor
  213 Hz; TB-303c 6.80 / 4.66 / 130), Reso's shift strong only at a high cutoff (`resoCut` -2.63 with `resoCutHz` 4.9
  kHz: -0.1 octave at 1 kHz, -0.7 at 3 kHz), `resoCurve` 0.87, `resoGain` 0.20, the output's limit nearly symmetric
  (`vcaSat` 1.64, `satAsym` 1.26), the accent's sweep deeper (`accDepth` 15.2) through a smaller R46 (`accR1` 0.33),
  `cap1` 2.99 and `topG` 0.26 (still not the circuit's), `driveLadder` at its bound 0.05 (the ladder linear),
  `topLeak` 114k.

| | TB-303b (e.txt) | TB-303c (g.txt) | TB-303d (n.txt) |
|---|---|---|---|
| band levels, the 320 fitted / 80 test notes | 1.27 / 1.30 dB | 1.27 / 1.32 dB | 1.11 / 1.03 dB |
| level difference sd | 1.13 dB | 1.10 dB | 1.01 dB |
| accent: 4 ... 40 ms, mean difference to the recordings (peak / energy) | 0.97 / 0.71 dB | 0.92 / 0.66 dB | 0.94 / 0.64 dB |
| Accent 25 % (the recordings: -0.2 / -0.3) | -0.3 / -0.2 dB | -0.1 / -0.3 dB | -0.3 / -0.4 dB |
| ring decay, mean of the cells | 0.557 octaves | 0.562 octaves | 0.477 octaves |
| brightness, each period's centroid, mean difference | 0.167 octaves | 0.173 octaves | 0.144 octaves |
| the level's course, 40 ... 450 ms, mean difference (all / accented) | 0.98 / 0.76 dB | 0.94 / 0.71 dB | 0.93 / 0.71 dB |
| the x0xb0x: shapes at full Reso / accents' centroid (x0xmeas.py, its calibration held) | | 4.97 dB / 0.83 oct | 1.64 dB / 0.32 oct |
| the TB-03's patterns (not fitted), test and test-hiq | | 3.84 dB | 4.08 dB |
| QEMU, instructions a frame (test_qemu, knobs at 0.5) | 18.1k | 18.1k | 18.1k |

Open: the accent at Cutoff 0 fades sooner again than TB-303c's (SET-A5 at Accent 100 %, the fundamental accented over
plain +9.1 / +5.3 dB at 50 / 100 ms, the recording +11.0 / +6.7); Reso's growth at the x0xb0x's lowest positions still
+3.8 ... +4.8 dB too much, at its right stop -4.5 dB too little; the capacitors and the drive at their odd values.

## TB-303e (`va303_tb303e.h`, fit/runs/r.txt)

TB-303d with the x0xb0x's Accent pot (`fit/fit_r.sh`, `fit/x0xmeas.py` part `accpot`, weighed three times on top of
`--xw 40`): the pot at 9, 12, 3 o'clock and its right stop, at Cutoff left and at 12 h, plain and accented notes in turn
(the takes of 2026-11-05, noon). The x0xb0x's accent has dinsync's shape over the pot (nothing in its first third, a step
between 12 and 3 o'clock) but is stronger and longer: at Cutoff left +25.0 / +16.5 / +11.3 / +5.1 dB over the plain note
on the fundamental at 0 / 50 / 100 / 200 ms with the pot at its stop (SET-A5 at 100 %: +15.1 / +11.0 / +6.7 / +2.4), at
12 h +17.8 / +12.7 / +8.5 / +3.6.

- **The way there.** The one take at Cutoff 0 (ab_acc0) weighed as much as the rest (`fit_o.sh`) moved TB-303d little:
  dinsync's many accent pairs outweigh it. Weighed three times (`fit_p.sh`) the accent into the amp grew long and strong,
  and at its end the switch at D27's threshold cut it off at once (SET-A5: +7.0 -> +2.5 dB from 150 to 200 ms). D27 as a
  soft diode (`accVcaKnee`, `fit_q.sh`) helped little while the fit kept the threshold high for dinsync's Accent 25 / 50
  %; the Accent pot's takes (`fit_r.sh`) give the threshold and the knee from the x0xb0x itself.
- **What it made of them:** `accVcaThr` 0.56 with a knee of 0.074, `accVca` 8.0, `accVcaTau` 201 ms, `accDepth` 25,
  `accC36` 5.2 ms; the output's limit lower (`vcaSat` 0.85, `satAsym` 1.42).
- The x0xb0x's Accent pot (`x0xmeas.py`, model less x0xb0x at 0 / 100 / 200 ms): at Cutoff left 9 h 0 / 0 / 0, 12 h
  -2 / 0 / 0, 3 h +2 / -1 / -2, the stop -1 / -3 / -3 dB (TB-303d at 3 h and the stop -6 / -5 / -3 and -3 / -7 / -5).
  From 150 to 200 ms the model's accent falls 3.3 dB, the x0xb0x's 2.3: what is left of the switch.

| | TB-303d (n.txt) | TB-303e (r.txt) |
|---|---|---|
| band levels, the 320 fitted / 80 test notes | 1.11 / 1.03 dB | 1.21 / 1.08 dB |
| accent: 4 ... 40 ms, mean difference to dinsync (peak / energy) | 0.94 / 0.64 dB | 0.62 / 1.19 dB |
| dinsync's accents' course, 40 ... 450 ms | 0.71 dB | 1.18 dB (stronger, as the x0xb0x's) |
| Accent 25 / 50 % (dinsync: -0.2 / -0.3, +4.2 / +3.7) | -0.3 / -0.4, +4.1 / +3.9 dB | -0.2 / -0.1, +3.1 / +3.3 dB |
| ring decay, mean of the cells | 0.477 octaves | 0.586 octaves |

More of the x0xb0x (the afternoon of 2026-11-05, `x0xmeas.py` parts `reso2`, `env`): Reso at 9 / 12 / 3 o'clock at Cutoff
12 and 3 h, Env Mod full (Decay 12 h) at Cutoff 3 / 12 / 9 o'clock. TB-303e meets them within some 2 dB: Reso's growth over
9 o'clock 0.6 ... 1.3 dB too much, the shapes 2.1 dB off, Env Mod's shapes 1.9 dB, the first harmonic's course 0.7 dB, the
level at Cutoff 9 h with Env Mod full 2.4 dB low. Fitted with them (`fit/fit_s.sh`, runs/s.txt) nothing moved more than a
few tenths (the top node's load gone, `topLeak` 1.75 M, `envR61` 0.18 as the circuit's 0.2): not taken.

A/B in a pattern (2026-11-05 evening, the user's OCT41 patch: 303-Patterns #PAT 2 by MIDI to the x0xb0x and to VA-303
TB-303e, Accent, Cutoff, Reso and Decay full, Env Mod 0; 34 cycles of 1.82 s averaged):
- Plain notes: the same level within 0.7 dB, the slides alike.
- The resonance at full Cutoff and Reso: the x0xb0x's at 1.3 kHz (0.8 kHz in the morning's `cut5_right` without
  accents), the model's at 2.1 kHz; dinsync's 303 (SET-E3, Env Mod 25 %) at 1.6 ... 2 kHz, which TB-303e meets. So the
  x0xb0x's top is darker; VA-303 Cutoff 94 of 127 matches its right stop. `x0xmeas.py`'s part `cut` sees H1 ... H10 of
  65 Hz only (up to 650 Hz), not where the resonance is at a high cutoff: its calibration puts the x0xb0x's stop at the
  model's 0.93, the resonance at 0.74.
- The x0xb0x's tuning: in tune at C3, sharp below (G2 +62, C1 +157 cents, some +3.5 Hz throughout): its trimming.
- With Cutoff 94 (and the accent's two changes above): accents start alike (+8 ... +9 dB, the slid one too), then
  the model's stay 2 ... 4 dB louder from 40 to 160 ms. On H1 (the amp) the x0xb0x's accent falls from its start (9 ->
  1.6 dB over a plain note in 100 ms), the model's holds for 20 ms and falls slower (8.9 -> 3.7); the upper harmonics
  (the sweep) start weaker in the model (+3.4 dB against +6.2 on A#2; on the slid note +0.2 against +2.8, rising to +6 at
  60 ms) and last longer: C13 at full Reso as in `accrun_12h_v12`. Fitted with it: TB-303f (below).

With TB-303e, TB-303b and TB-303c were left out of the app (three flash sectors). The new code of `accVcaKnee` changed
the compiler's rounding around it: TB-303 and TB-303d differ from before in 47 of 96000 samples of test_qemu's render
by one 16-bit step (-90 dBFS), so their checksums changed, their sound not.

## TB-303f (fit/runs/t.txt; left out of the app on 2026-11-19)

Left out with TB-303g on 2026-11-19, their headers removed: the steps from TB-303e to TB-303h, which on dinsync's
notes is within 0.02 dB of them and on the x0xb0x's pattern and acidvoice.com's TB-303 as close or closer; their
sounds some 1 dB from TB-303e's or TB-303h's on the band measure. What they brought is in TB-303h.

TB-303e fitted again with the user's pattern on the x0xb0x (`x0xmeas.py` part `pat`, `fit/fit_t.sh`, the default
character since the evening of 2026-11-05):
- **The measure:** the pattern (303-Patterns PAT 2: 16 steps, 7 accents, 4 of them in a row, 8 slides, two of them
  into an accent) by MIDI to the x0xb0x, Accent, Cutoff, Reso and Decay at the right stop, Env Mod at the left one;
  two takes of 34 cycles (`patch_ab2`, `patch_ab3`). Every 10 ms of the cycle, the cycles' mean: the power below 300
  Hz and above 500 Hz over two whole periods of the local pitch, rectangular (a Hann window weighs the low notes'
  bursts by their phase: the deterministic model swung 5 dB from frame to frame, the averaged takes not), each over
  its median, where the x0xb0x sounds. The model plays it at the x0xb0x's pitches (in tune at C3, +57 cents at G2,
  +168 at C1), the slides with a trigger as by MIDI.
- **The Cutoff pot's right stop:** the calibration's line (offset, scale, floor, from the shapes at Reso 0) put it
  where the resonance at full Reso would be 2 kHz; the x0xb0x's is at 817 Hz with Decay 0 (`cut5_right`) and 1.27 kHz
  with Decay 1 (the pattern). One point on that line gives both, 0.865 of the pot (`#x0xTop`): the pot's last fifth is
  flatter. With it the cut part's shapes at full Reso (now up to H20, the resonance) are 1.25 dB off instead of 2.41,
  Reso's growth at the stop -0.2 dB instead of -3.6. 3 o'clock stays: 611 Hz in the model, 632 in the x0xb0x.
- **What it made of it:** the accent's drive into the amp falls with 159 ms (TB-303e 201), C36 3.7 ms (5.2), `accVca`
  7.1 (8.0); `vegAttack` 2.9 ms, the schematic's 3 ms (0.4).

| | TB-303e (r.txt) | TB-303f (t.txt) |
|---|---|---|
| band levels, the 320 fitted / 80 test notes | 1.21 / 1.08 dB | 1.17 / 1.05 dB |
| accent: 4 ... 40 ms, mean difference to dinsync (peak / energy) | 0.62 / 1.19 dB | 0.58 / 1.15 dB |
| dinsync's accents' course, 40 ... 450 ms | 1.18 dB | 0.94 dB |
| Accent 50 % (dinsync +4.2 / +3.7) | +3.1 / +3.3 dB | +3.6 / +3.7 dB |
| ring decay, mean of the cells | 0.586 octaves | 0.565 octaves |
| the pattern, below 300 / above 500 Hz | 1.74 / 1.81 dB | 1.37 / 1.35 dB |
| the pattern's accents, model less x0xb0x | +2.8 / +2.2 dB | +1.6 / +1.0 dB |
| the x0xb0x's Accent pot at the stop, Cutoff left (0 / 100 / 200 ms) | -1 / -3 / -3 dB | -1 / -5 / -4 dB |
| the TB-03's patterns (not fitted) | 4.28 dB | 4.30 dB |

Closer on dinsync's notes and on the pattern; a long accent at Cutoff 0 (the Accent pot's takes, notes of 0.4 s) now
falls 1 ... 2 dB earlier than the x0xb0x's: at an open cutoff the x0xb0x's accent is shorter than at a closed one,
which the model's one time for the amp's share does not give. Reso's growth with the stop 0.5 dB more. With the fourth
character the app has four flash sectors (12.2 KB). test_qemu: TB-303, TB-303d and TB-303e unchanged, TB-303f
2D3A3BF9 / 98034B7D (saw / square), 19.9k instructions a frame as TB-303e.

## TB-303g (fit/runs/x.txt; left out of the app on 2026-11-19)

TB-303f with the accent sweep lifting the floor (`accFloor`, `fit/fit_x.sh`, the default character on 2026-11-06 until TB-303h):
- **What was missing:** at the x0xb0x's Cutoff 0 the cutoff is mostly the floor (126 Hz, the converter's part 7.5
  Hz). The sweep, octaves of the converter's part only, brightened TB-303f's accented note for less than 50 ms: at 50
  ms its H1 ... H8 were all +11 dB over the plain note, a level and no brightness. The x0xb0x's stays brighter for
  some 300 ms (`acc_c0_right`, accented less plain at 100 ms: H1 +11, H5 +20 dB; at 200 ms +5 / +10). So the fits
  held the accent into the amp long instead (`accVcaTau` 159 ... 205 ms), which made the accents at an open cutoff
  (the pattern) too long, and moved the sweep's parts far from the circuit (R46 at its bound 1 k, D24 0.43 of the
  envelope's peak, C13 140 ms, R72 0.9, 26 octaves). The envelope had this way already (`envFloor`, fitted to 3).
- **The fits:** from TB-303f with `accFloor` 15 (`fit_w.sh`) and from TB-303f with the sweep's parts at the circuit's
  values (`fit_x.sh`, as `fit_u.sh`, which without `accFloor` gave back to the long amp share); `fit_y.sh` (w with
  `driveLadder` held at 0.05: w had let it down to 0.012 and its ring at full Reso slowed, 0.742 octaves) and `fit_v.sh`
  (TB-303f with `driveLadder` and `cap1` free: nothing moved) for comparison. x has the lowest cost (1.219e7; w
  1.241e7, y 1.242e7, TB-303f 1.537e7 on the same measure).
- **What it made of it:** `accFloor` 12.5; R46 1.18 (the circuit's 0.94), R72 2.3 (2.0), C13 42 ms (50), 11.6 octaves
  (12), D24 0.26, C36 0.6 ms (1.55); the amp's share `accVca` 8.0 over a threshold of 0.76 with `accVcaTau` 178 ms.

| | TB-303f (t.txt) | TB-303g (x.txt) |
|---|---|---|
| band levels, the 320 fitted / 80 test notes | 1.17 / 1.05 dB | 1.14 / 1.04 dB |
| accent: 4 ... 40 ms, mean difference to dinsync (peak / energy) | 0.58 / 1.15 dB | 1.13 / 1.17 dB |
| Accent 50 % (dinsync +4.2 / +3.7) | +3.6 / +3.7 dB | +2.1 / +1.6 dB |
| dinsync's accents' course, 40 ... 450 ms (all / accented / set A) | 0.97 / 0.94 / 1.56 dB | 0.95 / 0.86 / 1.50 dB |
| ring decay, mean of the cells | 0.565 octaves | 0.567 octaves |
| the pattern, below 300 / above 500 Hz | 1.37 / 1.35 dB | 1.04 / 1.01 dB |
| the pattern's accents, model less x0xb0x | +1.6 / +1.0 dB | +0.1 / -0.2 dB |
| the x0xb0x's accent at Cutoff 0 over plain (0 / 50 / 100 / 200 ms) | +1.5 / -6.5 / -4.9 / -4.3 dB | +1.0 / -0.4 / -1.2 / -0.4 dB |
| the x0xb0x's Accent pot at the stop, Cutoff left (0 / 100 / 200 ms) | -1 / -5 / -4 dB | -1 / -1 / -0 dB |
| the TB-03's patterns (not fitted) | 4.30 dB | 4.34 dB |

Closer on nearly every measure; the accent's first 40 ms at Accent 50 % are 2 dB weaker than dinsync's (the x0xb0x's
Accent pot at 12 h is +2 dB over the model: the two units differ there). Left as before: Reso's growth at a low
cutoff 4 ... 5 dB more than the x0xb0x's (the cut part, 0 -> full Reso at the left stop and 9 h), the level with Env
Mod full at Cutoff 9 h 2.6 dB low, `cap1` / `topG` / `driveLadder` far from the circuit. test_qemu: TB-303 ... TB-303f
unchanged (accFloor 0), TB-303g 953BE750 / D0AE19A7 (saw / square), 20.0k instructions a frame; the app 12.6 KB, four
flash sectors.

## TB-303h (`va303_tb303h.h`, fit/runs/zf.txt)

TB-303g with the Reso pot's S (`resoS`) and the ring at full Reso (`fit/fit_zf.sh`, the default character since
2026-11-06):
- **What was missing:** the ring after each reset of the saw. At full Reso dinsync's TB-303's falls 2.4 dB/ms at 600
  Hz and 1.1 at 10 kHz (`fit/ring.py`), TB-303g's 1.5 and 1.0; at 75 % 4.95 rising to 15, TB-303g's 8 to 16; at 50 %
  17.6, TB-303g none to see. The x0xb0x's falls 11.5 dB from 1 to 4 ms after the reset at Cutoff 3 h and the right
  stop with full Reso (~3.6 dB/ms; `x0xmeas.py` part `ring`), TB-303g's 6.3. And the Reso pot's law: the x0xb0x's
  resonance peak (the strongest of H6 ... H20 over H1 ... H3, Cutoff 3 h) is -18.7 / -5.8 / -0.5 / +0.7 dB at Reso 9 h
  / 12 h / 3 h / full, TB-303g's -17.4 / -8.0 / -2.1 / +4.7: TB-303g's convex law (`resoCurve` 0.91: k 0.08 / 0.28 /
  0.58 / 1.01 of `kOsc` at a quarter / half / three quarters / full) rose steeply in the last quarter, held the ring
  long and the peak high there, and the cut part's "Reso's growth" at a low cutoff 4 ... 5 dB over the x0xb0x's.
- **The fits:** `fit_z.sh` (TB-303g with `fbHp2` alone: the growth 1 dB less, the ring slower, 0.996 octaves),
  `fit_zc.sh` (that with `cap1` and `topG` at the circuit's: 1.30 dB on the notes, given up). With the ring part and
  `topLeak` at the circuit's 100k, a concave law (`fit_za.sh`, `fit_zb.sh`): dinsync's ring at 75 % within 3 %, but
  at 100 % it fell faster the higher the frequency (resoMax 0.9), and the fits went back to the convex law: dinsync's
  band levels at Reso 25 and 50 % (sets B, C) want little resonance there, the ring at 75 and 100 % a linear law up to
  just past the oscillation point. Both: an S. `resoS` -1.12 with `resoCurve` 0.56, `resoMax` 1.0 and `topLeak` 50k
  had dinsync's ring at 0.22 octaves for +0.04 dB on the notes, unfitted; fitted (`fit_zd.sh`, `fit_ze.sh` with
  `fbHp2`) the S stayed, but the ring at 100 % slowed again (the notes' thousands of band levels outweigh it at `--rw
  60`); `fit_zf.sh` (from ze) and `fit_zg.sh` (from zd, no `fbHp2`) weigh the rings 200 an octave (dinsync) and 4
  (the x0xb0x's). zf is the closer: zg's ring 0.126 octaves and the x0xb0x's growth unchanged (+3.5 / +4.6 dB).
- **What it made of it:** `resoS` -0.99 with `resoCurve` 0.50 and `resoMax` 1.013: k 0.06 / 0.38 / 0.76 / 1.01 of
  `kOsc`; `topLeak` 122k (the circuit's 100k: dinsync's ring at full Reso a fixed loss against a feedback a little past
  the linear ladder's point, as the paragraph on the top node's load has it); `fbHp` 62.7 Hz, `fbHp2` 27.3 Hz.

| | TB-303g (x.txt) | TB-303h (zf.txt) |
|---|---|---|
| band levels, the 320 fitted / 80 test notes | 1.14 / 1.04 dB | 1.14 / 1.09 dB |
| ring decay, mean of the cells | 0.567 octaves | 0.046 octaves |
| ring at 100 %, 600 Hz / 10 kHz (dinsync 2.39 / 1.12 dB/ms) | 1.54 / 0.96 | 2.17 / 1.07 |
| ring at 75 %, 600 Hz (dinsync 4.95 dB/ms) | 7.99 | 4.92 |
| the x0xb0x's peak at Reso 9 h / 12 h / 3 h / full (-18.7 / -5.8 / -0.5 / +0.7 dB) | -17.4 / -8.0 / -2.1 / +4.7 | -18.3 / -5.4 / -1.5 / +1.8 |
| the x0xb0x's Reso's growth, model less x0xb0x (left stop ... right stop) | +3.8 / +4.8 / +3.6 / +1.3 / +0.1 dB | +2.1 / +3.3 / +2.3 / +0.3 / -0.9 dB |
| the x0xb0x's ring, its drop 1 ... 4 ms at full Reso, Cutoff 3 h / right (11.5 / 11.7 dB) | 6.5 / 6.2 dB | 8.4 / 8.1 dB |
| the x0xb0x's Reso 12 / 3 h shapes | 2.0 dB | 0.9 dB |
| accent: 4 ... 40 ms, mean difference to dinsync (peak / energy) | 1.13 / 1.17 dB | 1.46 / 1.01 dB |
| Accent 50 % (dinsync +4.2 / +3.7) | +2.1 / +1.6 dB | +1.8 / +1.8 dB |
| dinsync's accents' course, 40 ... 450 ms (all / accented / set A) | 0.95 / 0.86 / 1.50 dB | 0.93 / 0.85 / 1.48 dB |
| the pattern, below 300 / above 500 Hz | 1.04 / 1.01 dB | 0.94 / 1.01 dB |
| the pattern's accents, model less x0xb0x | +0.1 / -0.2 dB | +0.3 / -0.5 dB |
| the x0xb0x's accent at Cutoff 0 over plain (0 / 50 / 100 / 200 ms) | +1.0 / -0.4 / -1.2 / -0.4 dB | +1.4 / -0.2 / -0.3 / +0.5 dB |
| the x0xb0x's Env Mod full at Cutoff 3 / 12 / 9 h, the levels | +0.7 / +1.9 / -2.6 dB | +1.4 / +2.1 / -3.5 dB |
| the TB-03's patterns (not fitted) | 4.34 dB | 4.14 dB |

The ring and the Reso pot's law of both units, the x0xb0x's resonance within 1 dB over its Reso pot; the notes
as close (the test notes 0.05 dB further). Left: the x0xb0x's ring still falls faster than the model's (3.4 ... 3.6
dB/ms at 660 ... 800 Hz by `ring.py`, TB-303h 2.1; dinsync's TB-303 2.4: the units differ), its growth at a low
cutoff 2 ... 3 dB over the model's, the level with Env Mod full at Cutoff 9 h 3.5 dB low, `cap1` / `topG` /
`driveLadder` far from the circuit (3.3 / 0.34 / 0.05; the circuit's 0.545 / 1 / 1). test_qemu: `fbHp2`'s code
moves TB-303 ... TB-303g by one LSB in some 80 of 96000 samples (the compiler's rounding; `resoS` 0 changes nothing):
50B0C792 / 5D9BB275, E177F4E6 / A96F15C3, 1883F7B2 / E9FA113B, 4B09CF66 / CB17C7C5, 10F85D6B / B04DBDB0 (saw /
square); TB-303h E7BDBA13 / EA68C4C8, 20.5k instructions a frame; the app 13.0 KB, four flash sectors.

**Above 12 kHz** (2026-11-19, not taken): the fits measured to 12 kHz; above it TB-303d, e and h fall 9 dB under
dinsync's bright notes and accents (12 ... 15.4 kHz, re 100 ... 400 Hz; TB-303i 2 ... 3 dB), from the bent capacitors:
with TB-303i's `cap1` (0.545) alone TB-303h's top is right, its notes 3 dB off. `fit/fit_zz.sh` (the measure with two
bands to 15.4 kHz, `fit/measure.py` `$VA303_TOP_BANDS`, weighed 3, `fitva.py` `$VA303_TOP_W`; dinsync's recordings
fall from 16 kHz on) stalled at runs/zf.txt; `fit/fit_zz2.sh` (cap1 / topG pinned at 3.33 / 0.67, 2 / 0.5, 2 / 0.336,
the rest refitted) gave dinsync's notes 1.28 / 1.23 / 1.19 dB (TB-303h 1.14), the ring 0.11 / 0.10 / 0.06 octaves
(0.04), and the rest took the top back down (12 ... 15.4 kHz -7 / -12 / -10 dB): in this ladder the band levels to
12 kHz and the top pull apart. The top is TB-303i's, whose circuit ladder has it.

## The x0xb0x's takes of 2026-11-06 (late; `extern/x0x_reference/x0xb0x-2026-11-06`, its README)

Against TB-303h, the model rendered with the same notes and the x0xb0x's cutoff calibration:

- **The ring over the pitch** (Cutoff right, its ring at 760 ... 780 Hz, notes at 47 ... 173 Hz): the decay does not
  depend on the pitch, on the x0xb0x nor in the model. Reso 3 h: 4.3 dB/ms, TB-303h 4.2; full: 3.6, TB-303h 2.15
  (dinsync's 303 2.2 ... 2.4: the units differ, as above). At Cutoff 12 h and left the ring lies below 600 Hz, within
  the saw's harmonics.
- **The slide:** tau 23.0 ... 24.3 ms on the x0xb0x (after the MIDI's 2 ... 3 ms), up and down, 6.6 to 22 semitones;
  TB-303h 22 ms (`slideTau`, 100k x 0.22 uF). Left as it is: within the parts' tolerance.
- **The saw at an open cutoff:** H2 ... H8 over H1 within 0.7 dB at 66 ... 173 Hz. The phases want a one-pole
  highpass of some 40 Hz more than TB-303h has, at every pitch (the conflict of "The recordings' phases").
- **The square** is an ideal pulse against the saw (`pw_measure.py`'s residual below 1 %), its width 0.526 at 38 Hz,
  0.472 at 66, 0.432 at 125, 0.392 at 241, 0.342 at 464, 0.324 at 579 Hz: about -0.047 an octave, as on 2026-11-03.
  TB-303h's is no pulse at the low notes (residual 8 % at 65 Hz: `sqSag` 50 Hz, `sqRise` 166 Hz) and its law
  `sqWidth + sqWidthK (1 - 65.4 / f)` flattens above 200 Hz, where the x0xb0x's keeps falling. dinsync's magnitudes
  still want the shaped one (above, "Square").
- **Accents with pauses** (Accent right, notes of 0.1 s; `fit/x0xmeas.py` part `accgap`): an accent leaves the next
  plain note louder, at Cutoff 0 by +6.6 / +2.9 / +0.6 / +0.3 dB 0.125 / 0.25 / 0.5 / 1 s after it (TB-303h +4.3 /
  +1.7 / -0.8 / -0.8), at 12 h +3.9 / +2.3 / +0.6 (+2.7 / +0.7 / -0.7), at full Reso not at all (both). In the model
  that is the accent's sweep (C13, R46, Rm, D24, the MEG's R117); the accent into the amp (`accVcaTau`) does not move
  it. But the x0xb0x's accents do not add up as its plain notes do: two accents 0.25 s apart at Cutoff 0 are as loud
  as one (the model +1.2 dB at 50 ms), 0.125 s apart the second +3.9 (the model +1.5), and at 12 h the other way
  round. Fits with the part (`fit_zr.sh` ... `fit_zu.sh`, weight 2 / 8 / 20, the sweep and the MEG free) take the
  plain notes after an accent half the way (+5.4 / +2.8 at Cutoff 0) and the part from 0.73 / 0.75 / 0.53 to 0.65 /
  0.72 / 0.55 dB, but dinsync's band levels to 1.152 (1.143), the accents' attack to 1.51 / 1.10 (1.46 / 1.01) and
  Accent 50 % further off. Not taken: what an accent leaves behind on the x0xb0x saturates in a way the model's sweep
  (a capacitor charged through a diode and discharged with the MEG) does not.

## TB-303i (`va303_tb303i.h`, fit/runs/zsi.txt)

The circuit's ladder. TB-303b ... TB-303h bend the capacitors (TB-303h: `cap1` 3.33, `topG` 0.34; the circuit 0.545 /
1) and their phases are 80 ... 123 degrees off on both units. What the ladder is, asked of the notes directly
(`fit/xladder2.py`, `xladder_d.py`, `xlp_d.py`, 2026-11-07): the linear ladder of va303.h (its node matrix) fitted to
the held saws at Reso 0, the harmonics' magnitudes and phases and the level over the Cutoff positions, all positions at
once (one `cap1`, `topG`, two couplings; a cutoff each):

- **The x0xb0x** (cut1 ... cut5): the circuit's capacitors 0.60 rms (dB, 10 degrees), free ones no better, TB-303h's
  5.3; the couplings two highpasses near 87 Hz besides the line-in; the level over the positions within 1 dB.
- **dinsync's TB-303** (SET-A1 and SET-A5's plain notes, their constant rotation free): any `cap1` from 0.15 to 1.16
  alike (0.41; at 0.545 with `topG` 0.67), TB-303h's 0.78; two couplings near 67 Hz; to H40 (2.6 kHz) 0.32, a fixed
  lowpass at 2.2 kHz after the ladder 0.29. So the capacitors are the circuit's; the couplings, some 140 ... 170 Hz of
  first-order highpass in all, are what every fit found (TB-303h 154 + 14, the circuit's couplings fit 44.5 + 116).
  The schematic has the missing one: the filter's output into the VCA (IC15, BA662A) through R121 220k and C21 0.01 uF
  into 2.2k, 72 Hz (R122 / C22: 156 Hz), besides `hp1` (Open303's 44.5 Hz) and C27.
- **The Cutoff pot:** the cutoffs those fits give dinsync's positions, 122 / 143 / 246 / 587 / 967 Hz at 0 ... 100 %,
  rise 0.72 octaves over the pot's last quarter and 1.26 over the one before; the x0xb0x's right stop sits at 0.865 of
  its line (`#x0xTop`, TB-303f). A converter's top (`cutTop`) would stop the envelope's sweeps too, which reach 10 kHz
  and more. Without it the full model with the circuit's capacitors turned the drives up for the treble at Cutoff
  100 % (SET-A1 1.2 dB fitted on the A sets alone); with `cutPotTop` (0.897) and the drives low SET-A1 is 0.7 (TB-303h
  0.9), A4 0.5, A5 0.6.

The fits (fit/README.md, "The ladder"): with that start and everything else free 1.17 dB (zx, `cap1` held; zy with the
capacitors free 1.21), the ring at full Reso then too slow above 2.4 kHz; zxr weighs it three times, only the ladder,
the resonance and the couplings free (0.122 octaves). A lowpass in the feedback loop, tried for the high rings, lowers
the ladder's oscillation threshold (the loop's 180 degrees come at a lower frequency): the filter oscillated, removed
again. **The ring** (2026-11-07 afternoon): dinsync's decay at full Reso falls nearly linearly with the frequency, 2.39
dB/ms at 0.7 kHz to 1.12 at 11.6: a fixed loss a millisecond (the top node's load) less a growth a period (the feedback
just past the threshold), some 2.47 dB/ms and 0.12 dB a period; zxr had 2.63 and 0.15. Not the engine's numerics (the
reference build, double, 8x, iterated, rings the same, 0.125 octaves). A scan of `resoMax` and `topLeak` puts it at
1.015 and 62k (zxr 1.024 / 39.5k): the ring 0.069 octaves and the band levels 1.16 dB, both better; the fit from there
(`fit_zxs.sh`, all free) stays (zxs: 0.071, 1.16): the fits had stopped short of it. **The square** (user,
2026-11-07): the ideal pulse both units have, with their law (`sqWidthOct` -0.05, "Square" above), `fit_zsi.sh`: the
band levels 1.23 dB (zxs 1.16; the square notes 1.27 against 1.13), the rest as zxs.

| | TB-303h (zf.txt) | TB-303i (zsi.txt) |
|---|---|---|
| `cap1` / `topG`; couplings into / out of the filter | 3.33 / 0.34; 154 / 13.7 Hz | 0.545 / 0.67; 46 / 112 Hz |
| `resoMax`; `resoS` | 1.013; -0.99 | 1.016; -1.24 |
| the pulse: width at C2, its law; `sqSag` / `sqRise` | 0.564, -0.075 (1 - 65.4 / f); 50 / 166 Hz | 0.539, -0.05 an octave; 8.7 Hz / none |
| `cutPotTop`; `driveLadder`; `topLeak` | -; 0.05; 122k | 0.884; 0.59; 62k |
| band levels, 400 notes (fitted / test) | 1.13 (1.14 / 1.09) dB | 1.23 (1.24 / 1.19) dB |
| saw / square notes | 1.16 / 1.10 dB | 1.20 / 1.27 dB |
| per base A / B / C / D / E | 0.68 / 0.86 / 0.98 / 1.56 / 1.59 | 0.71 / 0.88 / 1.10 / 1.73 / 1.74 |
| dinsync's ring at full Reso, mean of the cells | 0.046 octaves | 0.072 octaves |
| ring at 100 %, 600 Hz / 10 kHz (dinsync 2.39 / 1.12 dB/ms) | 2.17 / 1.07 | 2.31 / 1.04 |
| accent: 4 ... 40 ms (peak / energy) | 1.46 / 1.01 dB | 1.19 / 1.14 dB |
| dinsync's accents' course (all / set A) | 0.93 / 1.48 dB | 0.88 / 0.91 dB |
| the x0xb0x's phases at Reso 0, H2 ... H6 by position | 80 / 101 / 119 / 122 / 123 deg | 10 / 11 / 21 / 35 / 18 deg |
| the x0xb0x's shapes at Reso 0 / full | 0.65 / 1.20 dB | 0.95 / 1.38 dB |
| the x0xb0x's Reso's growth, model less x0xb0x | +2.1 / +3.3 / +2.3 / +0.3 / -0.9 dB | -0.0 / +1.4 / +0.9 / +0.6 / -0.4 dB |
| the x0xb0x's Env Mod full at Cutoff 3 / 12 / 9 h, the levels | +1.4 / +2.1 / -3.5 dB | +0.7 / +1.2 / -1.9 dB |
| the pattern's accents, model less x0xb0x | +0.3 / -0.5 dB | +0.4 / -0.3 dB |
| the TB-03's patterns (not fitted) | 4.14 dB | 4.25 dB |

Right where the ladder shows (the phases, the resonance's growth, Env Mod), in the square's shape over the pitch and
in the accents' attack and course, behind on dinsync's band levels (0.1 dB, most of it the square notes) and its ring
(0.025 octaves). TB-303h stays the default; TB-303i to listen against it. test_qemu: TB-303 ... TB-303h unchanged
(`cutPotTop`, `sqWidthOct` 0), TB-303i E765BAEF / 575C618D (saw / square; before: zxr F9CD29E0 / ED17C078, zxs
7322C8E4 / D02C9346), 20.5k / 22.8k instructions a frame (saw / square; 25.6k at most), -22.9 / -24.7 LUFS; the app
13.8 KB, four flash sectors.

## test_qemu (2026-11-08, after the review: `fit/qemu_chars.sh`)

The engine after the review of 2026-11-08 (the note delay back at 4 ms, the cutoff's top with the Pade tanh, the
square's width at the control's rate, the AUX input interpolated, the slide running on after the gate; the app: the
firmware's soft limit in place of its own from 0.8, a bare trigger held 62.5 ms): every checksum changed (the delay
alone moves the notes by 8 blocks), the levels did not. 1.2k instructions a frame fewer (TB-303h 20 984 -> 19 772:
`calcControl()` only when a knob changes, the feedback's 1 / (1 + g) once a character, the Pade tanh at the top, no
soft knee in the app), the square 1.7k fewer (its width no longer divides each core sample).

| character | saw: insn a frame / LUFS / CRC | square: insn / LUFS / CRC |
|---|---|---|
| TB-303d | 17896 / -21.5 / CD13699F | 19405 / -23.3 / 412ABAD2 |
| TB-303e | 19271 / -21.7 / 8A4200D4 | 20780 / -23.5 / 95DA13BD |
| TB-303f (left out 2026-11-19) | 19271 / -21.9 / 2856E99E | 20781 / -23.6 / 502C15F8 |
| TB-303g (left out 2026-11-19) | 19295 / -21.6 / FF492423 | 20805 / -23.5 / 89AE273B |
| TB-303h (default) | 19772 / -22.1 / 471CE274 | 21287 / -24.0 / 604EA5CC |
| TB-303i | 19776 / -21.7 / A5D7548B | 21332 / -23.9 / 90E0E9B4 |
| AUX (TB-303h) | 19199 / -29.6 / 7C758C8E | |
| Overdrive 0.5 | 19799 / -10.4 / CC1D4D64 | |
| Overdrive 1 | 19622 / -5.2 / A5FABB31 | |
| Muffler SOFT | 20445 / -22.1 / 471CE274 | |
| Muffler HARD | 20447 / -22.1 / 97BA9FE0 | |
| BassMod ON | 20101 / -20.5 / B7211CC1 | |
