# The model behind lib/va909

The voices of a Roland TR-909 built from its circuit: every block below is a part of the schematic with its parts'
values; what the parts leave open (their tolerances, the pots' tapers, the transistors' gains, the levels the CPU
writes to the ACCENT line) is fitted to recordings of a TR-909. The constants are the fields of the voices' `Params`
(`bd.h`: `VA909_BD_PARAMS`, each with its comment, and so on); the fitted sets are `va909::bdFit`, `sdFit`, `cpFit`,
`hhFit`, `crFit`, `rdFit`, `ltFit` / `mtFit` / `htFit` / `tnFit`, `rsFit` (the `va909_*_fit.h`). This file
says where each comes from. The fit's tools are on the branch `va909` only (`fit/README.md` there).

No code, renders or values of the TR-09 or of `lib/btq` / `experimental/boutique` are in it, not even as a
cross-check; nor the kit DRUMKITS/TR-909, which is measured against the same references for comparison only.

## Sources

- **Schematic**: the TR-909 Service Notes, first edition (JUN. 15, 1984), p. 11 "CIRCUIT DIAGRAM VOICING BOARD"
  (VG 73133090, PCB 2291084903 3/3), the 400 dpi scan of archive.org (`roland_TR-909_SERVICE_NOTES`); its p. 5/6
  (circuit descriptions: TRIG a 5 V pulse of 2 ms, ACCENT a 6 bit DAC, the noise generator), p. 9 (the voices' scope
  traces with and without accent), p. 12 (the Multi Jack Board), the change information (C9 0.22 -> 0.33 uF at SN
  381500). Part designators below are the schematic's. All NPN 2SC2603-F, all PNP 2SA1115-F, all diodes 1SS133,
  IC11 ... IC15 M5218L on +-15 V; Q6 and Q12 are selected parts ("(S)").
- **Recordings**: the Rob Roy Recordings' TR-909 set (Jason Baker, 1995, machines.hyperreal.org, `909-BD.zip` and
  `TR909all.zip`, free, not for profit, not to be modified): 24 bass drums `BT<tune>A<attack>D<decay>` with the knobs at 0, 3, 7, 10
  (attack 0 and 10 only), 44.1 kHz, mono, from the 909's mix out (all levels at their top) into an E-mu ESI-32,
  normalised. Not in the repository: `.build/va909/ref/robroy` of the main checkout. The tidal-drum-machines set
  (`machines/RolandTR909`, knobs unknown) was looked at, not fitted.
- **SPICE**: ngspice 45.2 with the schematic as a netlist (`fit/spice/bd.cir`) and generic models of the parts
  (`fit/spice/models.lib`): the check of the model's structure (below).

## The bass drum (`bd.h`)

```
 TRIG ─Q1─┬─D4/R3─Q2── TP ──D7,R21──► C9 (ENV3) ──R26──┐            R27 (+15 V) ──┐
          │                   │      R57+TUNE ┘          ├─ IC12a ─ IC12b (+-1, Q10) ─R59─► IC13a (C14) ─► triangle
          ├─C1─Q7── reset ────┼─────────────────────────────────────── Q11 (R51) shorts C14, D12 forces IC13b ┘   │
          │                   └─D6,R20─► C7 ─R18─► Q3 ─R32─► Q4 ┐                                   IC13b (R49, R50) ◄┘
 ACCENT ─IC11b (x3.13)── accb ─────────────────────────────────┴─R35,D9─► C8 (ENV1) ─R54─► Q12 base     │
          │                                       D8 ─► R36/R22 ┘     DECAY+R58 ┘                       │
          └─Q5 (while Q1 is on) ─R41─► C12 (ENV2) ─R42─► Q6 base                 triangle ─R52─ D10/D11 ─R53─ R43 ─► Q12 coll.
 TP ─R5─Q8 (off while TP) ─C5,R14─ C2 ─R7─ Sallen-Key (R16, C3, C4, Q9) ─C11,R44─┐                       │
 NOISE ─R10/R9─C15─R46─C13─R45────────────────────────────────────────────────── mix (R56) ─ATTACK─ R55 ─► Q6 coll.
                                                                    Q6, Q12 emitters ─► IC11a (LEVEL) ─► out
```

### The trigger

- TRIG (5 V, 2 ms; the latch IC1) turns on Q1 (R1 22k, R2 10k); Q1's collector turns on Q2 (R3 100k, R4 47k, D4):
  the trigger pulse **TP** at Q2's collector, 14.9 V for the trigger's length (`vtpHi`), 0.31 V at rest (Q8's base
  current through R5 into R19 2.2k, `vtpLo`). `trigMs` 2.
- Q1's fall also reaches Q7 through C1 (0.068 uF, R12 100k, R13 47k): Q7 is on while TRIG is high and resets the VCO.
- The accent: the ACCENT line (the CPU's 6 bit DAC, RM0621: 5k ... 160k from the latch's outputs, 0 ... 5 V) into
  IC11b, gain 1 + R37/R38 = 3.13 (`accGain`), its output (accb, at most the op amp's swing `vsat`) feeds Q5 and Q4.
  The line holds the voltage until the next trigger: the voice's `trigger(acc)` takes it in volts.

### ENV3, the pitch envelope, and the VCO

- TP charges C9 (0.33 uF) through D7 and R21 (2.2k) for the trigger's length; R57 (22k) + TUNE (VR2, 100k A, a
  rheostat) and R26 (330k, into IC12a's summing node) discharge it. Not the accent: TP is the full 15 V on every hit,
  so the sweep is the same at any accent.
- IC12a sums ENV3 through R26 and +15 V through R27 (1.5M), R28 (150k) its feedback: |V1| = R28/R26 V(C9) + R28/R27
  15 V. IC12b passes V1 with +1 or -1 (R29, R30, R31 100k; Q10 shorts its (+) input), switched by the Schmitt
  trigger IC13b (R49 100k, R50 47k: thresholds +-vsat R50/(R49+R50)) through R23/R24. The integrator IC13a (R59 47k,
  C14 0.033 uF) ramps between the thresholds: a triangle at f = |V1| / (4 Vth R59 C14), linear in ENV3. Q10 must be
  the PNP (on while IC13b is low): with the NPN the comparator would latch.
- While TRIG is high, Q7 turns on Q11 (R47 330k), which shorts C14 through R51 (2.2k, 73 us), and forces IC13b high
  through R48/D12: the triangle stands at |V1| R51/R59 and starts rising when TRIG ends. The tone starts 2 ms after
  the trigger, always in the same phase; the VCO runs free between hits (the VCA keeps it silent).
- The model: C9 integrated exactly per sample (a piecewise linear ODE: the charge while TP is high and C9 is below
  its target, the discharge always), the triangle as a phase that turns at the thresholds with a polyBLAMP on the
  corner (the output one sample late), the reset as a one-pole towards |V1| R51/R59.

### The shaper and the VCA Q12

- The triangle (+-4.3 V) through R52 (10k) into the antiparallel diodes D10/D11, loaded by R53 (2.2k) + R43 (47):
  linear to some +-2.5 V of the triangle, the tops rounded by the diodes' log law (`dIs`, `dN`). One table over the
  triangle's voltage, solved by Newton at setup.
- Q12's collector sits at R43 (47 ohm): a few mV of the shaped wave. Its emitter is in IC11a's summing node, its base
  is fed from C8 (ENV1) through R54 (470k). Q12 is saturated: the emitter current, Ebers-Moll with both junctions on,
  is ie = ib (1 + 1/bf - u) / (1/bf + u/br), u = exp(-vce/vt): about ib br vce / vt for small vce, limited to
  -br ib on the negative side, so even harmonics and a DC part that follows the envelope (`qBf`, `qBr`, `qIs`).
- The base current ib = (V(C8) - vbe) / R54: the level follows ENV1 above Q12's Vbe and dies where ENV1 meets it. In
  the decay, when V(C8) is little above Vbe, the base's swing with the collector, Vbe lower by L = vt ln((1/bf +
  u/br) / (1/bf + 1/br)) (Ebers-Moll; checked against ngspice, `fit/vcabase.py`), modulates ib: linearised around
  the base current (`BaseCurrent::at`, d ib / d v = 1 / (R54 + vt/ib)), Vbe solved every 16 samples. The collector
  current loads the 47 ohm node (one Newton step on vce).
- LEVEL (VR4, 100k B) is IC11a's feedback: out = -VR4 (ie12 + ie6). The output is DC coupled (the Multi Jack
  Board's R500 1k / C500 0.01 uF, R501 12k to the mix bus).

### ENV1 and the hold

- TP also charges C7 (0.033 uF) through D6 and R20 (6.8k); C7 discharges through R18 (470k) into Q3's base (R17 470k
  to ground): Q3 holds Q4 on (R32 47k, R33 1M) until C7 has fallen to some 2 Vbe, 60 ... 80 ms. While Q4 is on, accb
  charges C8 (0.33 uF) through R35 (1.8k) and D9; D8 into the divider R36 (47k from +15 V) / R22 (10k) takes part of
  the current once C8 is above the divider's 2.6 V: C8 settles at some 0.82 (accb - 0.7 V) + 0.47 V. DECAY (VR5, 1M
  A) + R58 (47k) and Q12's base discharge it.
- The model: Q4's current limited by Q3's and Q4's gains (`bf3`, `bf4`) from Q3's base current (C7 above `vbe3`
  (R17+R18)/R17), D8's share as a divider, C8 integrated per sample.

### The click (ATTACK) and ENV2

- While TP is high Q8 (R5 100k, R6 22k) is off: its collector falls through R14 (22k) with C5 (6.8 nF), and at TP's
  end it steps back to 15 V. C2 (3.3 nF), R7 (100k), R15 (100k) and the Sallen-Key low pass of the follower Q9 (R16
  39k, C3 4.7 nF, C4 100 pF: 5.3 kHz, Q 3.4) make two ringing clicks of it, a weak one at the start and the strong one
  at TP's end; C11 (0.47 uF) and R44 (22k) put them into the mix node (R56 330 ohm). The network is four states
  (C5, C2, C3, C4), its transition matrix for the sample (Q8 on and off) the matrix exponential at setup.
- The noise: the noise bus (IC32/IC33 4006, 32 stages, clocked by IC31a/b at some 450 kHz: R184 10k, C47 100 pF;
  0/15 V) through R10/R9 (12k/4.7k), C15 (0.47 uF), R46 (4.7k) into C13 (0.1 uF) and R45 (4.7k) into the mix node:
  noise low passed at some 500 Hz, a thump. In the model white noise of the bus's density at its clock (`noiseClk`),
  the circuit's 450 kHz in all voices (one bus; the fits wanted it 18 dB apart until the snare's and the clap's other
  parts were free).
- ATTACK (VR3, 500 B) in series to R55 (47 ohm) takes the mix node to Q6's collector; Q6 is Q12's twin, its base fed
  from C12 (ENV2) through R42 (470k). ENV2: accb charges C12 (0.033 uF) through Q5 and R41 (22k) while TRIG is high
  (Q5's base through R34 100k and D5 to Q1's collector); it falls through Q6's base (some 15 ms). So the accent sets
  the click's level, and both clicks pass the VCA. On units up to SN 371499 R495 (150k) loads Q6's base (`r495`).

### What it does

- **Accent**: the same pitch sweep; ENV1 and ENV2 charged higher: louder, a slightly longer decay (ENV1 has further
  to fall to Q12's Vbe), a stronger click. With the fitted constants, against the recordings' 2.55 V on the ACCENT
  line: 1 V -9 dB, 1.5 V -4.8 dB, 3.5 V +2.7 dB, 4.5 V and more +4.4 dB (IC11b's output at its swing), tone and
  click alike (x1.66), the decay to -40 dB (the last sample over it) 216 -> 226 ms. Roland's traces (p. 9) show some
  2.5 times the amplitude with accent at noon (the low tom there some 2.1 times: the model's ACCENT voltages, not a
  part, are open; see below).
- **Rolls and flams**: each trigger resets the VCO's phase and charges C9, C7, C8, C12 from where they are: a second
  hit 10 ms after the first starts at the first's sweep, its ENV1 already up (no new attack in the level), its click
  again; nothing else is reset.

## The snare drum (`sd.h`)

```
 TRIG ─Q39─┬─Q40── TP ─D56,R247─► C66 (ENV1) ─R248─┐                       ┌─► 4069 (x3) ─R261─► IC37b (C69) ─► VCO-1
           │       │                 TUNE (VR6) ─R249─┴─► N1 ─IC35a/b─► the 4069's rails +-V(N1)  └─ IC37a (R262/R260, D62/R275 from TP)
           │       ├── D62/D63 (the comparators' lower thresholds), Q44/Q45 (the resets), Q46/Q49/Q43 (VCAs held)
           │       └───────────────────────────────────────────────── the same for VCO-2: IC38a, 4069 (x3), IC38b (C71)
           ├─D58─Q41: ga (the accent gated) ─D68─► C83 ─R291─► C70 ─R290─► Q50 ◄── VCO-1 ─R283─ D65/D66 ─R288─ R289
           │                                  ─D67─► C82 ─R295─► C79 ─R293─► Q51 ◄── VCO-2 ─R305─ D69/D70 ─R304─ R303
           │                                  ─D64─► C73 ─R280─► Q47 ◄── high pass IC39a (4 kHz) ─R278─ R279     ┐
           └─D61─C68/R256─Q42─D60─► C67 (ENV4) ─R297─► Q48 ◄── C80 ─R299─ R298 ◄── low pass IC40a (8.5 kHz) ◄─ NOISE
                          TONE (VR7) + R254 to IC34a (-0.1 accb) ┘                       Q47, Q48 ─► IC39b (SNAPPY) ─► R294 ┐
                                                                          Q50, Q51 ──────────────────► IC40b (LEVEL) ◄──┘
```

### The VCOs

- Each VCO is a Schmitt trigger (IC37a / IC38a, R262/R260 100k: the upper threshold +6.6 V), a 4069 inverter (three in
  parallel) supplied by IC35a/b at +-V(N1), and an integrator (IC37b, R261 47k, C69 0.01 uF / IC38b, R264 47k, C71
  6800 pF): a triangle whose slope is V(N1) / (R C). N1 is TUNE's wiper (VR6 10k B between R287 100k to +15 V and
  R284 10k: 1.25 ... 2.5 V) through R249 (100k) and ENV1 (C66, charged from TP through D56/R247, 47k) through R248
  (470k): on a hit the rails go up to some +-4 V and bend back in some 10 ms, both VCOs alike.
- The lower threshold is no resistor's: D62/R275 (D63/R274) tie the comparator's (+) input to the TP line, which at
  rest is held only by the base dividers of Q46, Q49, Q43; a low comparator pulls it below 0 V. So the lower
  threshold is -3.6 V while the other VCO's comparator is high and -4.4 V while it is low as well (SPICE): the VCOs
  are coupled through the TP line. In the model: the two thresholds by the other VCO's state; a threshold that moves
  past the triangle turns it where it is.
- While TP is high Q44 / Q45 short the integrators through R263 / R268 (10k) and D62/D63 force the comparators high:
  the triangles stand at R263 (V(N1) / R261 - Ib), Ib the reset transistor's base current from TP (R258 100k: VCO-1
  sits at -0.6 V, R267 1M: VCO-2 at +0.7 V), and start rising when TRIG ends.
- The comparators and the 4069 switch late (`cmpDelay`: the triangles overshoot the thresholds by their slope times
  it; some 40 us in SPICE).

### The drum's VCAs, ENV2, ENV3

- Each triangle through a 10k into antiparallel diodes (D65/D66, D69/D70), then R288 1.5k / R304 2.2k into 47 ohm:
  the VCAs Q50 and Q51 as the bass drum's Q12 (`VcaTable`, `vcaCurrent`).
- ENV3 (VCO-1): the gated accent ga (Q41, its base through R257 and D58 to Q39's collector) charges C83 (0.1 uF)
  through D68; C83 empties through R291 (220k) into C70 (4700 pF), which Q46 holds at 0 V while TP is high, and R290
  (22k) feeds Q50's base. ENV2 (VCO-2): C82 (0.047 uF) through D67, R295 (220k) into C79 (0.01 uF, Q49), R293 (220k)
  to Q51. So the tones fade in over a ms after TRIG and the high one dies sooner.

### The snappy

- The noise through R286/R285 (10k/3.3k), C85/R302 (a high pass at 48 Hz) and IC40a, a Sallen-Key low pass (R301,
  R300 22k, C81 3300 pF, C84 220 pF: 8.5 kHz, Q 1.9); then two ways:
  - C80 (0.022 uF), R299 (4.7k) into R298 (47 ohm): Q48, the body of the snappy, its base from ENV4 through R297 (220k).
    ENV4 is C67 (0.47 uF): Q42 charges it through D60 while C68 (6800 pF) lets Q42's base current through R256 (1M)
    flow, some 7 ms after TRIG; TONE (VR7, 500k B) and R254 (100k) to IC34a's output (-0.1 of the accent) empty it,
    so TONE is the snappy's length.
  - IC39a, a Sallen-Key high pass (C76, C75 2200 pF, R276 6.8k, R277 47k: 4 kHz, Q 1.3), R278 (2.2k) into R279 (47
    ohm): Q47, the snap, its base from C73 (470 pF), which ga charges through D64 while Q43 holds Q47's base at 0 V;
    when TRIG ends C73 empties through R280 (220k) into it: a burst of some 0.1 ms.
- IC39b sums Q47 and Q48, SNAPPY (VR9, 10k B) its feedback; C78, R294 (47k) into IC40b's summing node, with Q50 and
  Q51. LEVEL (VR8, 50k B) is IC40b's feedback.

### The model against SPICE

The tones (`fit/cmp_sd.py`, knobs 0 / 5 / 10, the model's noise off as SPICE's: `--p909 nonoise.txt`;
2026-11-14): the lower tone's spectral peak within 1 ... 4 %, the upper's within 1.5 % of SPICE's, ENV1 ... ENV4 within
a few %, the band measure 2.4 ... 2.7 dB, the level -0.3 ... -0.4 dB (the tones' beating is sensitive to their phases;
before the onset's Newton steps below 3.0 ... 3.3 dB, with the base current's old sign 3.1 ... 3.6 dB). The noise
path (`fit/cmp_sd_noise.py`, four seeds in SPICE against as many renders): 1.8 dB of the mean band levels per window,
-0.7 dB on average (before: 2.3, -0.6).

The onset against SPICE (2026-11-14, `fit/sd_onset.py`: `spice/sd.cir` at 1 us, the noise off, the circuit's
constants, TUNE 0/5/10, ACCENT 2 ... 4 V): at TRIG's end C70 rises 0.1 V (4 vt) a sample, and between the VCAs' Newton
steps (every 16 samples) `BaseCurrent::track` followed Q50's base current only along its slope (1 + dv / vt where
e^(dv / vt) is due): the tone came 44 ... 90 us (2 ... 4 samples) late at 2 ... 20 % of its peak, C70, unloaded,
overshot by 0.4 V, and the next update, 16 samples after TRIG's end (333 us), jumped 0.48 ... 0.70 of the peak in one
sample (SPICE's steepest 0.04 ... 0.06). `stepBase()`: a Newton step where track's would raise the base current by
more than a quarter (25 ... 65 a hit, all by 1.5 ms after TRIG's end; natively within the noise, 224 B of code). Now
0 ... 30 us early at 2 ... 90 % (SPICE's 10 %: 122 ... 196 us after TRIG's end), the first trough within 15 us, the
steepest step 0.04 ... 0.09; the lead of about a sample is the step convention (the first sample after TRIG holds a
step of C70's charge). The fit's measure without a refit 3.50 -> 3.48 dB, 19.1 cents. With the fitted constants (ACC
2.55 V) the tone's 10 % moved only 4 ... 56 us earlier (0.26 ... 0.32 ms after TRIG's end, the circuit's 0.14 ...
0.18 ms in SPICE), its 90 % later (the jump gone): ENV3 as fitted (R291 x2.75, C83 x0.38) is slower than its parts,
so the late tone against the recordings is mostly the fit's (fitted with the jump; a refit may move ENV3). The voice
no longer sleeps while C83 / C82 still charge C70 / C79 (only reachable in the kit at MIDI velocity under some 30).

Found on the way, not changed: at ACCENT 0.5 V the snare is 5.1 ... 5.7 dB under SPICE and some 0.2 ms late (the
fixed diode drop `vd`: C83 / C82 charge to ga - 0.6 V where SPICE's D68 / D67 drop 0.35 ... 0.45 V); at 4 V +1.4 ...
+1.6 dB (`vcaCurrent2` for Q50 / Q51 would give -0.15 / -0.06 / +0.03 dB at 2 / 2.55 / 4 V, not applied); the snap
lasts longer in SPICE (Q41's C_jc times its gain in the generic PNP model keeps feeding ga some 0.5 ms; open whether
the real Q41 does: a candidate for the recordings' steep rise at TRIG's end); C67 (ENV4) starts at 0 V on the first
hit instead of SPICE's rest, -0.25 ... -0.32 V.

### The snare's fit

Against the Rob Roy Recordings' 52 snares (`ST<tune>T<tone>S<snappy>`, the knobs at 0, 3, 7, 10, tone not varied with
snappy 0; `fit/robroy_sd.py`), the recordings' chain the bass drum's (the mix out's high pass, the outputs' low
passes, the sampler's saturation, ACCENT 2.55 V: one 909, one chain). First the tones' parts on their mean
instantaneous frequencies (Hilbert, in band passes at the recordings' two peaks, 30 ... 120 ms), then held while the
rest was fitted on the band measure.

| | band measure | tones | |
|---|---|---|---|
| the model, 52 hits | 3.48 dB | 19 cents rms | (`sd3`: `sd2`, 3.47 dB, with R280 / C73 at their parts; before: 3.57 dB, 15 cents with the noise fitted; the code of 2026-11-14 3.48 dB as well, 3.50 between the base current's sign and the onset) |
| the kit DRUMKITS/TR-909, its snare | 7.38 dB | 191 cents | |

- The tones: C69 x1.13, C71 x1.15, TUNE's track 19 % at 3 and 76 % at 7 of 10 (an S of a B taper), ENV1 slower (R248
  x1.9, C66 x1.7: the bend lasts longer), the switching delay at its floor (10 us).
- The recordings at snappy 0 were made at another tuning (T0: 131 / 187 Hz against 120 / 171 Hz with snappy).
- The noise bus at the circuit's 450 kHz (`sd2`, refitted with it: 3.47 dB; with the bus fitted, x0.09, 3.57 dB):
  TONE, R254, R280, C67 and the snappy VCAs' reverse gain (`nBr`) came closer to their parts.
- Fitted far from the parts, so carrying what the model misses (`sd3`): the shaper's diodes (Is x0.28, N x0.75),
  ENV3's and ENV2's capacitors and resistors (C83 x0.38, C82 x0.58, R291 x2.75, R295 x1.04), IC40a's low pass on the
  snappy's noise (C81 x0.22, C84 x1.38: 8.5 -> 15.2 kHz), ENV4 (C67 x1.51); the snappy VCAs' reverse gain 4.5 (the
  circuit's 5).
- ENV5, the snap (`sd3`, 2026-11-10): R280 and C73 at their parts (220k, 470 pF; `sd2` had 105k and 404 pF). At TRIG's
  end C73 empties into Q47's base through R280 and lets a burst of IC39a's high passed noise through: as fitted 42 us
  (two samples) at twice the current, a one- or two-sample spike in the app's output up to the hit's peak (snappy 10;
  the fit's chain smooths it: the outputs' 15.9 kHz low pass, the sampler's saturation; the app has neither); at its
  parts 103 us, five samples, 0.65 of the peak at snappy 10 (0.22 at 3). The band measure the same (3.47 / 3.48 dB;
  `sd_detail.py` 3.51 / 3.44 dB, without ENV5 3.50). Why the fit took it so short: the recordings rise steeply at
  TRIG's end at every snappy (3 as 10), the model's tones (ENV3 / ENV2 into Q50 / Q51) some 0.4 ms later; the snap
  (the snappy's path) filled it in. The tones' start: the onset follows SPICE since 2026-11-14 (above); against the
  recordings it is still late with the fitted ENV3.


## The hand clap (`cp.h`)

- **The bursts.** IC29 is an AN6912, four comparators with open collectors between 0 V and -15 V; the ladder R219,
  R217, R214 (4.7k each) gives -5 V and -10 V. While TRIG is high Q36 gates the accent (IC26a, x3.13) through R213 to
  comparator a, which pulls C62 (0.027 uF) to -15 V; R229 (1M) lets it back to 0 V (27 ms). Comparator b sinks while
  C62 is below -5 V (some 30 ms): it sets d's threshold (R227/R228 of -14.8 V: -10 V) and lets comparator c go,
  whose R220 (82k) then charges C65 (0.47 uF) through D54. Comparator d compares C65 (through R225 1M; C63 220 pF from
  its output: positive feedback) with the threshold: once C65 has risen past it, d lets go, Q40 turns on and empties
  C65 through R223 (330 ohm) at once, d sinks again. So C65 is a sawtooth, one tooth per clap (R220 C65 sets the
  period), as long as b sinks; then C65 rises slowly through R224/R238 (485k): the last, long clap.
- **The OTA.** Q38, a PNP pair fed by the accent through R235 (15k), steers its current into the BA662's bias by
  C65's voltage (-15 V while Q40 is on) on the left base (R224 470k / R238 15k) against the TM trimmer on the right
  (R494 47k / R230 2.2k; the Service Notes' adjustment: 2 Vpp at the multi out with LEVEL at noon): every clap starts
  loud and dies as C65 rises. The noise band passed by IC26b (multiple feedback: R208 47k, R209 10k, R207 150k, C42,
  C43 4700 pF: 960 Hz, Q 2.1) goes through C56, R202/R201 (39k/1k) into the OTA; its output current into R237 (82k)
  clips at the OTA's swing (the accent's current, some 0.4 mA, would make 30 V of it: the clap's noise is clipped),
  C58 (1000 pF) puts it into IC28a's summing node (a high pass at 1.9 kHz).
- **The tail.** The band passed noise through C55 (0.47 uF), R204 (1k) into C57 (0.47 uF, a low pass at some 440
  Hz), R205 (3.3k) into R203 (47 ohm): Q37, its base from ENV2: ga charges C59 (0.1 uF) through D52, R233 (470k)
  into C61 (0.1 uF), R234 (470k) to Q37's base: it swells for some 30 ms and fades over a few hundred.
- LEVEL (VR20, 50k B) is IC28a's feedback.

The model against SPICE (the circuit's values, ACCENT 2 V; `fit/cpcmp.py`: `cpspice.py` against `bin/cpnative`,
2026-11-14): C62 within 0.02 V, C61 within 0.06 V, C65 (the sawtooth)
within 0.15 V in 99 % of the samples (one tooth 0.3 ms late), the output's level per 2 ms over the first 80 ms within
3 dB in 27 of 40 windows (mean |.| 2.3 dB; the noise is another), the peak 9.2 V against 9.2 ... 9.9 V (four seeds of
SPICE's noise).

### The clap's fit

Against the Rob Roy Recordings' two claps (`HANDCLP1`, `HANDCLP2`, one ACCENT level each, unknown), the same chain;
the band measure and the level per 2 ms over the first 80 ms (the bursts' rhythm, `--envw 2`); C65 held at 0.376 uF,
which gives the recordings' rhythm (bursts at 0, 10, 22, 31 ms; the circuit's 0.47 uF: every 14 ms). Its cost is
stepwise (the bursts start on a sample), the fit went by small steps.

| | band measure |
|---|---|
| the model, 2 hits | 4.16 dB (`cp3`; 4.48 dB with the noise fitted; the code since 2026-11-13 4.17 dB) |
| the kit DRUMKITS/TR-909, its clap | 6.42 dB (5.7 ... 6.5: its noise differs from render to render) |

- The noise bus at the circuit's 450 kHz (`cp3`, refitted with it: 4.16 dB; fitted x6 before, 4.48 dB).
- Far from the parts: Q37's reverse gain (x6, the limit), the tail's capacitors (C61 x0.69, R234 x1.9).


## The hi-hat and the cymbals (`hh.h`)

The 909's digital voices: ROMs of sampled cymbals (HN61256P, 32 KB each: `909_HIGH`, `909_CRASH`, `909_RIDE`, the
blobs the kit DRUMKITS/TR-909 reads too) played by counters into 6-bit resistor DACs, each into a transistor VCA
whose envelope gives back what the samples were compressed by (the Service Notes, p. 5: "the Hi-Hat sounds have been
compressed before being digitalized ... in order to have greater S/N ratio").

```
 TRIG ─C159/R491 (edge)─► reset IC70/IC71 ─► IC69 ─► IC68 latch ─► RA9 DAC ─C157─► R482 22 ohm ◄─ Q85 (S) ─► IC67b ─►
 clock IC72c/d ÷2 (31.57 kHz) ┘                                                    base ◄─ Q84 pair ◄─ IC67a ◄─ R453 ◄─ C135
 TRIG ─R454─► Q74: C135 held for 2 ms; C135 ─► R453 (220k) │ R452 + VR23 OH DECAY │ (closed) Q73 ─ R451 + VR21 CH DECAY
 ACCENT ─R481 4.7k─► Q84's base (R489 150, R488 2.2k from R482)
```

- **The ROMs and the clocks.** The hats: the open one 0x0000 ... 0x5FFF, the closed one 0x6000 ... 0x7FFF (its
  first 2 ms silent: the recordings' closed hats start there too); the counters stop at the end (IC72a). The ROM's
  byte holds the DAC's six bits in its top six (the open hat's tail has the low two set: noise, not data), code 32 the
  rest (the ROMs' first and last bytes). The clock: the recordings' ROM staircase correlated with them (0.9 in every
  file, `fit/romrate.py`): 31570 Hz at every DECAY (+-10 Hz; the Service Notes "about 60 kHz divided by two"; the kit
  plays the ROM at 30540 Hz; DRUMS/TR909-HiHat at 32000 Hz until 2026-11, now at this clock). The cymbals' clocks
  (IC59/IC60, IC57 ... with R443 / R441 6.8k, VR25 / VR27 10k B, C131 / C130 470 pF, halved) over TUNE: the crash
  27.6 ... 55.8 kHz, the ride 26.7 ... 55.5 kHz; a 4011 oscillator's period k R C + t0 with TUNE's track straight
  between 0, 3, 7, 10: k 1.95 / 2.06, t0 2.76 / 2.42 us, within 12 / 5 cents of the ten clocks measured.
- **The staircase.** Rendered band limited: each step the integral of a Kaiser windowed sinc cut at 20 kHz, 4
  samples either side (`va909_blep.h`, `fit/gen_blep.py`; 6 were 0.07 ... 0.11 dB closer to the recordings and dearer
  on the DSM). The staircase's images (fclk - f, fclk + f, ...) are in the
  recordings below 24 kHz, the ones above it must not fold back: against a 4x oversampled render polyBLEP was 0.3
  ... 0.6 dB further from the recordings. The ROM interpolated linearly instead (a sample player's way, refitted):
  2.77 / 1.95 / 2.25 dB (hat / crash / ride) against the staircase's 2.30 / 1.72 / 1.90 with 6 samples either side,
  2.37 / 1.79 / 2.01 with 4 (the A/B in `fit/README.md`).
- **The VCA.** Q85 (Q71, Q69) saturated between the DAC's 22 ohm node and an op-amp's summing node, its base current
  given by a current source: Ebers-Moll with ib given, ie = ib F(vce / vt), the node v0 = v + 22 ohm (ie - ib): the
  transistor takes its share of the DAC's current (the Service Notes: "a voltage controlled potentiometer"). Solved
  for each step of the DAC (the VCA sees the staircase: its output is one), one Newton step from the tangent at 0
  (F is convex there: within 2e-3 of the root for the DACs' swings), F and F' from a table over vce. The output: R483
  (R425, R424) 47k times the emitter's current, at the op-amp's swing (13.5 V) at most.
- **The hat's envelope.** Q74 holds C135 at its start while TRIG is high (2 ms); C135 discharges through R453 (220k
  into IC67a: the reference current of the exponential converter Q84) and OH DECAY's R452 + VR23, while the hat is
  closed also through CH DECAY's R451 + VR21 (Q73): one time constant, its current through R453 the VCA's base
  current (Q84's ratio fixed but for the accent and R488), smoothed by IC67a (R453 C156: 0.22 ms). The open and the
  closed hat are one voice: a hit of either restarts the counters and C135. What the scan leaves unclear (Q74's and
  Q73's supplies, which way C135 runs) the recordings settle: one exponential each, its time constant over the knobs.
- **The cymbals' envelope.** The top six address bits (RA11 / RA13 into IC52b / IC52a, R432 270k / R439 470k, C126
  / C125 1000 pF) as the VCA's base current: "the ROM addresses as the envelope data". The recordings' envelope over
  the address is one curve for every TUNE (`fit/romenv.py`): (1 - a / aEnd)^aPow of the address a (0 ... 63), smoothed
  by IC52's integrator; the ROM's end stops the VCA (IC59d).
- **The accent.** Q84's (Q70's, Q68's) base: the ACCENT line through R481 4.7k into R489 150 (R426 4.7k into R428
  220): 0.029 (0.045) of its volts, the base current times exp of that over vt. From the circuit only (the
  recordings: one level): from the normal 2.55 V to 4.51 V (Total Accent 0.8) the hat +14 dB, the cymbals +18 ... 22
  dB, their op-amps at their swing (the crash a square for half a second). DRUMS/VA-909 gives them 0.3 of the
  ACCENT line's excursion from the normal level (`ROM_ACCENT`): the hat +5 dB, the cymbals +8 dB at Total Accent
  0.8, +6 / +10 dB at 1, the op-amps short of their swing.

### The hi-hat's and the cymbals' fit

Against the Rob Roy Recordings' hats (`HHCD0 ... A`, `HHOD0 ... A`), crashes (`CSHD0 ... A`) and rides
(`RIDED0 ... A`, `RIDED4` = `RIDED6`): each aligned on its ROM (the staircase correlated with it, to 1/50 sample),
the model on its own; the band measure, the level per 5 ms, and the hits' level held at 1 V of 10 ms RMS at the
normal accent: the recordings carry none, and without it the fit drives the base currents up to 20 ... 30 V at the
op-amps (the VCA's compression), for 0.03 dB.

| constant | circuit | fitted | |
|---|---|---|---|
| `c135` | 1 uF | 1.13 uF | OH: 78, 102, 125, 145, 177, 200 ms at DECAY 0, 2, ... 10 (the recordings' 86 ... 201 ms) |
| `vr23`, `oh3`, `oh7` | 1M A | 0.8M (limit); 10 %, 31 % at 3, 7 | OH DECAY |
| `vr21`, `ch3`, `ch7` | 100k B | 80k (limit); 25 %, 85 % | CH: 10, 21, 32, 43, 48, 50 ms (OH DECAY at 1.7 in the recordings) |
| `cPast` | | 0.008 | C135 aims a little past 0: the hat ends after 4.8 time constants |
| `vNode`, `i0`, `qBr` | | 16 mV, 6.4 uA, 23 | the hat's VCA: the DAC's swing at the 22 ohm, the base current at a normal hit |
| crash `vNode`, `i0`, `qBr` | | 18 mV, 26 uA, 4.6 | |
| crash `aEnd`, `aPow` | 64, 1 | 61, 0.91 | the address envelope: 0 at 61/64 of the ROM |
| ride `vNode`, `i0`, `qBr` | | 22 mV, 3.3 uA, 29 | |
| ride `aEnd`, `aPow` | 64, 1 | 64, 1.18 | |

| | band measure | the kit DRUMKITS/TR-909 |
|---|---|---|
| hi-hat, 12 hits | 2.37 dB (1.63 ... 3.26) | 6.00 dB |
| crash, 6 hits | 1.79 dB (1.46 ... 2.06) | 5.21 dB |
| ride, 5 hits | 2.01 dB (1.46 ... 2.83) | 6.28 dB |

- What is left: the closed hats at DECAY 0 and 2 (3.2, 3.0 dB: their first ms), the ride at TUNE 10 (2.5 dB: its
  clock 55 kHz, the ROM's own content over 24 kHz folds back). The VCA's reverse gains go where the fit puts them
  (4.6 ... 29: the three are alike parts), the DECAY pots to their tolerances' end.
- The choke (`OPCL1-4`, `CLOP1-4`: a closed hat into an open one and the other way round): not fitted; a first look
  (the knobs unknown) has the two hats' levels within some 3 dB of the recordings'.

## The toms (`tt.h`)

The low, mid and high tom are one circuit with other capacitors (the low tom's designators below; `ttCircuit(tom)`):

| | LT | MT | HT |
|---|---|---|---|
| VCO-1, VCO-2, VCO-3 | C18 0.022, C19 0.033, C20 0.012 uF | C32 0.018, C33 0.027, C34 0.01 uF | C97 0.015, C102 0.022, C103 8200 pF |
| ENV1, ENV2 (two), ENV3 | C22 0.22, C21 / C23 0.68, C25 0.056 uF | C36 0.15, C35 / C37 0.47, C39 0.056 uF | C99 0.15, C98 / C100 0.47, C106 0.047 uF |
| Q18's C24 (the attack) | 0.01 uF | C38 6800 pF | C101 5600 pF |

- **The trigger.** TRIG turns on Q16; Q16 turns on Q15 (TP: +15 V through R91 100 ohm, 2 ms) and Q17, which gates
  the accent (IC18b: 1 + R119 / R120 + R119 / R118, R118 220k to -15 V: x3.17 + 0.68 V) onto the envelopes (ga).
  R120 is printed "47K" (p. 11), but the mid and the high tom's identical stages read 4.7K clearly (R180 with R179 10K,
  R178 220K, IC23b; R383 with R382, R381, IC45b): 4.7k, the print's decimal point lost. (47k would make it x1.26 and
  the toms some 9 dB quieter at a normal hit. Roland's traces, p. 9, do not decide it: the low tom with accent some
  2.1 times its amplitude without, the model 1.47 with 4.7k, 1.58 with 47k -- and the bass drum is as far off there,
  2.5 against some 1.45 (3.8 V): the ACCENT voltages are what is open, not R120.)
- **The rails.** IC14a buffers TUNE (VR10 10k B between R113 68k and R112 10k: 1.7 ... 3.4 V): the 4069's upper rail.
  IC14b makes the lower: -(the upper) - R64 (v16 / R60 + v17 / R61), C16 (0.1 uF, R60 2.2M: 0.22 s) and C17 (0.047 uF,
  R61 470k: 22 ms) charged to TP through D15 / D14. RD18JB2 (D16) between them.
- **The VCOs.** As the snare's: IC15a a Schmitt trigger (R67 / R69: +-vsat / 2, 6.75 V), two 4069 inverters (IC16) at
  those rails, R73 47k into the integrator IC15b: the triangle falls at the upper rail's slope and rises at the
  lower's, so ENV4 bends it up and makes it lopsided at the hit (the low tom: 81 Hz settles from some 2 semitones
  up). TP forces the comparators low (D13 through R62) and shorts the capacitors through Q13 and R71 10k: the three
  start together, falling. The thresholds are symmetric (no resistor from the TP line, unlike the snare's).
- **The attack.** TP charges C24 (D17); R101 1M empties it into Q18's base: Q18 lets VCO-1 into its shaper through R102
  1k besides R103 10k (as much as its base current lets through) for some 10 ... 30 ms: a harder, louder start (the
  Service Notes' SN 426700 change to the tom noise "emphasizes the attack" too). Q18's collector is at VCO-1 through
  R102, its emitter at the diodes' node: forward up to its gain times its base current, the other way only its reverse
  gain's (5): the attack hardens the positive half waves. In the model the shaper's drive grows by R103 times Q18's
  current, over a companded table axis (`atComp`). 2026-11-14: Q18 (the attack) is no switch. With its base current
  forced, Ebers-Moll gives ie/ib = -br + (bf+1+br)/(1+e^-z), z = (vce - vt ln(bf/br))/vt: a logistic step in vce from
  -br ib to (bf+1) ib around vt ln(bf/br) (0.1 V). Below its limit vce ~ vt ln(1 + ie/(br ib)): at 2 uA of base current
  29 mV for 20 uA (20 mV across R102), 74 mV for 0.12 mA, and the knees towards both limits are as soft. Before, Q18 was
  saturated at vce = 0 (R103 || R102 into the diodes, the table `shaperSat`) and clamped at bf ib / -br ib. Against
  SPICE (`fit/ttvca.py`, `fit/ttq18.py`) that was up to 15 % of the peak off at 0.74 uA (9 ... 10 % at 2 uA, 4 ... 5.5 %
  at 20 uA), node a 40 ... 57 mV high; the law alone is within 0.6 ... 1.0 % (node a 3 ... 4 mV). Now `q18Current`
  solves the law at setup into `TomTables::bend`: 11 rows over the base current (0, then 54 nA ... 27.5 uA doubling), 64
  points over the triangle, R103 times the current. The rows' share is linear in ib from the float's mantissa (exact at
  Q18's limit), and the bend branch costs what it did (two lerps for the clamps, no division; 2.5 KB more). The blend of
  two rows softens the moving knee: within 3.2 % of SPICE (0.54 uA, `ttq18.py --vth 1 --n 121`; 2 uA 2.2 %, 20 uA 2.1 %,
  vth 8 V the VCA's own 3 %), ib18 = 0 bit-identical. The fit's measure without a refit 3.806 -> 3.811 dB (the exact law
  per sample 3.815; level per 5 ms 3.35 dB, tones 50 cents). Tried, not taken: rows of 2^(1/2) (a quarter of the blend's
  miss for twice the bytes), rows aligned on the knee (worse at high ib), the companded coordinate in the table instead
  of atComp's division (4.0 % against 3.4 %). (Before 2026-11-06 Q18 passed both half waves alike, from the triangle's
  voltage over R102 alone: against SPICE up to 52 % of the peak off, the fit's C24 and Q18's gain at their limits.)
- **The VCAs.** Each triangle through 10k into antiparallel diodes, 2.2k into 47 ohm: Q19 / Q20 / Q21 (selected),
  saturated, their emitters in IC18a's summing node (LEVEL VR12 50k B). Q19's base: ENV1 (C22, R107 470k) and ENV2
  (R108 220k) as one source; Q20's: ENV2 (C21 through R100 100k into C23, R109 220k; DECAY VR11 500k B + R111 56k
  empty C21: ENV2 rises for some 50 ms); Q21's: ENV3 (C25, R110 470k). The shaper and the VCA in one table over the
  triangle (VCO-1 and VCO-2), the noise added at Q21's collector (VCO-3: two tables).
- **The tom noise** (IC27, Q33, Q34; one for the three toms): the noise bus through R190 / C161 (1.5 kHz), IC27b's
  Sallen-Key high pass (C51 / C52 4700 pF, R191 10k, R192 220k: 720 Hz, Q 2.3), C53 / R195 into Q33's 47 ohm
  collector; Q33's base gets a burst at each tom's TRIG (Q34, C54: what the scan shows of it is fitted, `nIb`,
  `nTau`); IC27a sums Q33 and R194's share of the filtered noise; R114 22k into each tom's Q21.
- Left out: IC18a's swing (the low tom at LEVEL's top reaches it: clipped at 13.5 V), the 4069's output resistance
  as the snare's (`rOn`, fitted).

### The toms' fit

Against the Rob Roy Recordings' 48 toms (`LT/MT/HT<tune>D<decay>`, TUNE and DECAY at 0, 3, 7, A), the three toms at
once (`fit/fit_tom.py`: the shared constants one set, each tom's capacitors its own): the band measure, the level
per 5 ms, the tones (the mean instantaneous frequencies in band passes at each recording's three strongest peaks,
30 ... 150 ms).

| constant | circuit | fitted | |
|---|---|---|---|
| the VCOs' capacitors | | x1.06 ... x1.13 | the tones (the 4069's `rOn` 81 ohm) |
| `tune3`, `tune7` | 0.3, 0.7 | 0.31, 0.79 | TUNE's track |
| `decay3`, `decay7` | 0.3, 0.7 | 0.25, 0.72 | DECAY's |
| `c16`, `c17`, `r61` | | x1.06, x1.05, x1.09 | ENV4 |
| `c21`, `c22`, `c25` (three each) | | x0.75 ... x1.17 | the envelopes' capacitors |
| `c23` (three) | 0.68, 0.47, 0.47 uF | x1.21, x1.30, x1.21 | ENV2's second capacitor |
| `r100`, `r108`, `r109`, `r110` | 100k, 220k, 220k, 470k | 80k (limit), 264k (limit), 240k, 433k | the envelopes |
| `c24` (three), `bf18` | 0.01 uF, 6800, 5600 pF; 250 | x0.75 (limits); 656 | the attack (Q18) |
| `qIs`, `qN`, `qBr`, `dIs` | 1e-14, 1, 5, 2.5 nA | 2.1e-13, 1.36, 4.9, 55 nA | the VCAs, the shapers |
| `nIb`, `nTau` | | 29 uA, 1.1 ms | the tom noise's burst |

| | band measure | the kit DRUMKITS/TR-909 |
|---|---|---|
| low tom, 16 hits | 3.78 dB | 7.12 dB |
| mid tom, 16 hits | 3.90 dB | 7.77 dB |
| high tom, 16 hits | 3.82 dB | 8.37 dB |
| all 48 | 3.83 dB (tones 50 cents rms; `tt4`) | 7.75 dB |

- The code of 2026-11-14 with `tt4`'s constants (the tom optimisation, the base current's sign, Q18's law; not
  refitted; `fit/fit_tom.py --report`): 3.81 dB (LT 3.77, MT 3.87, HT 3.79), the level per 5 ms 3.35 dB, tones 50 cents.
- 2026-11-06: `diodeShaper` (va909.h) left the shapers' node volts too high where Q18's attack drives them past some
  17 V (the table goes to 96 V): the start of a hit too loud, the more the higher TUNE (a tom grew 12 dB from TUNE 0 to
  10; the app's toms clipped). Now a tom is about as loud at every TUNE, an accent +4 dB. `qN`, the VCAs'
  base-emitter emission coefficient where the envelopes reach Vbe: the decays' end (1 the ideal junction). Then
  against SPICE (`fit/ttvca.py`): Q18 one way only (its reverse gain, `br18`) and saturated through R103 || R102 (the
  table `shaperSat`; since 2026-11-14 Ebers-Moll's law, `TomTables::bend`, above), the VCAs' node with `vcaCurrent2`
  (its Newton step's derivative exact under a heavy load). Refitted: 4.29 (tt2) -> 4.19 (tt3) -> 3.83 dB (`tt_f`,
  `tt4`).
- What is left (`fit/tail_tom.py runs/tt4.txt`, the code of 2026-11-14): the level per 5 ms model - recording 0 ...
  +0.3 dB up to 30 ms after the peak, -0.3 ... -0.8 dB to 150 ms, -2.2 dB at 150 ... 300 ms, -3.6 dB at 300 ... 600 ms
  (as fitted, with the base current's old sign: -0.3 ... -0.7, -2.0, -3.4; tt2: -1 ... -1.8, -3.2, -5.7):
  the decays' last 10 ... 20 dB still a little fast (as the bass drum's), R100, R108 and C24 at their limits, the
  shapers' Is far from the parts. The three VCOs' beating (their phases differ) keeps the level per 5 ms some 3 dB
  rms apart.

## The rim shot (`rs.h`)

- **The trigger.** TRIG's edge through C111 (0.018 uF): its far end empties through D89 into R396 / R395 (22k each;
  the spike at their junction, 2.3 V, into the + inputs of IC48a, IC49a, IC49b) and through R399 (22k) into Q63's
  base, which holds it at its Vbe while it conducts (R398 10k to ground below). The model integrates that node (four
  Euler steps a sample while it lasts): the spike's area is SPICE's. Q63 (some 0.3 ms) turns on Q64, the accent
  (IC48b, 1 + R406 / R397) charges C119 (0.047 uF) through R401 2.7k: ENV.
- **F1 ... F3.** Each op amp a bridged T in its feedback: H(s) = 1 + s Rb Cb g / (s^2 Ca Cb Rb + s (Ca + Cb) + g), g =
  1 / Rs: F1 R407 470k, C113 / C112 0.01 uF, R394 2.2k: 495 Hz, Q 7.3; F2 R414 330k, C116 / C115 0.027 uF, R411
  2.2k: 219 Hz, Q 6.1; F3 R416 470k, C118 / C117 4700 pF, R404 2.2k: 1.05 kHz. Their gain at the resonance (Rb / 2
  Rs, some 100) drives them to the op amps' swing: the Service Notes draw them at 30 / 15 / 30 Vpp. The model: the
  two capacitors' voltages each, the op amp ideal while it follows (the - input the spike), at its swing the output
  held and the - input floating until it passes the spike again (the T rings on from the clipped output); each mode
  linear, the trapezoidal rule. (A biquad of the closed loop with its output and state clipped, as before, put F1
  and F3 11 ... 23 % under SPICE.)
- **The clip.** R408, R415 (12k), R417 (3.3k from SN 415300, 12k before) into D91 / D92, R423 12k into R418 680 ohm:
  Q65's collector (selected); its base from ENV through R403 1M; its emitter in IC50a's summing node (LEVEL VR19 100k
  B with C120 220 pF). One table for the diodes over the three outputs' Thevenin voltage; Q65's collector solved
  with up to 8 damped Newton steps from the last sample's solution (through R418's 680 ohm the transistor pulls it
  far, where the drums' 47 ohm hardly let it: one step there, the toms' with the exact derivative).
- **The output.** C121 / C122 (0.01 uF), R422 22k, R420 220k: IC50b's Sallen-Key high pass (230 Hz, Q 1.6); R421 /
  R419 hold its - input, where R422 feeds back, at half its output: the rim shot's out twice the filter's (the model
  had it halved). Q66 / Q67 (RIM SHOT / TRIG OUT, the tape sync) are not in the sound.

### The rim shot against SPICE

`fit/spice/rs.cir` (the parts as printed), `fit/rsspice.py` (the model with the circuit's constants against it, both
cut at the hit; probes of F1 ... F3, Q65's collector and current, IC50a, ENV): the band measure of the shapes 1.61
dB, correlated 0.988, the level -0.6 dB, each window of 0 ... 80 ms within 5 % (again on 2026-11-14). Before
(2026-11-06) 7.4 dB, correlated 0.42: `diodeShaper` (va909.h) started its Newton steps of at most 0.05 V at the
unloaded divider's voltage, which for the rim shot's light load (12.7k behind 2.1k) left the diodes' node volts too
high whenever F1 or F3 were at their swing; Q65's current up to 34 times SPICE's at the start, the output halved
instead of doubled (the two errors near cancelling for the first 15 ms), F1 / F3 clipped as biquads, the spike's area
0.71 of SPICE's.

### The rim shot's fit

Against the Rob Roy Recordings' `RIM63` (40 ms; `RIM127` is the same sound, correlated 1.000): the band measure, the
level per ms, the tones (3 ... 30 ms). R417 at 12k (before SN 415300) fits better than 3.3k (5.29 against 6.90 dB
with the circuit's other values): the recordings' 909 is an early one.

| | band measure | the kit DRUMKITS/TR-909 |
|---|---|---|
| the model (`rs3`, the corrected model) | 3.59 dB, tones 27 cents (the code since 2026-11-13: 3.60 dB, 28 cents) | 6.94 dB |
| before (the model with the errors above) | 4.68 dB | |

- Fitted (`fit_rs.py`, rs_c then rs_d with the limits the circuit's): the bridges' resistors and capacitors within
  12 %, R417 11k, tCharge 0.53 ms, R403 0.8 and C119 0.8 (limits); far from the parts: D91 / D92's Is 1e-10 A (the
  limit; 2.5 nA: their knee some 0.15 V higher) and Q65's reverse gain 1 (the limit).
- The recording's first 0.55 ms are not the 909's: 24 samples between exact zeros and 0.5 ... 0.85, the sampler's
  garbage at a sample's start, as in 117 of the 184 Rob Roy files (the 24 bass drums counted in both archives: 160
  distinct; two snares, `ST0T0S0` and `ST0T3SA`, up to 1.07
  ms; the measures leave out the first 0.625 ms).

## The bass drum against SPICE

`fit/grid_model.py`: knobs (0, 3, 5, 10 in six combinations) x ACCENT (0.7, 2, 4.5 V) x a second hit 2 ... 150 ms after
the first, the model with the circuit's constants against the netlist (2026-11-14; the model's noise off as SPICE's,
`--p909 nonoise.txt`, the line `noiseClk 0`): 1.2 ... 2.3 dB of the band measure
(below), 1.78 dB mean (the build before the base current's sign 1.79; with the model's noise on 1.9 ... 4.7 dB, mean
3.3: the 1.4 ... 2.4 dB, 1.95 dB mean noted before were the noise off as well, an older model's), the level within 0.5
dB at 2 and 4.5 V (-1.4 ... -2.3 dB at 0.7 V), the pitch at 10 and 40 ms within 0 ... 5 Hz; after the refinements of the
VCA (the base's swing, the node's load) the decay's harmonics agree with SPICE's within 2 dB (`fit/cmp_detail.py`, knobs
5 / 5 / 5, ACCENT 2 V: H2 ... H8, 120 ... 200 ms; noted before: 1 ... 4 dB).

## The bass drum's fit

The open constants against the Rob Roy recordings (`fit/README.md`): the band measure (1/3-octave bands in windows of
2.5 ... 50 ms, each side normalised to its loudest cell, floored 60 dB under it, the mean absolute difference) and the
first 24 periods of each of the 24 hits. The recordings are the 909's mix out as an E-mu sampler took it, so the
model's output goes through the same chain before it is compared: the mix out (C154 into IC66a: a first order high
pass, inverting), the outputs' RC low passes (R500/C500 held at 15.9 kHz, R530/C511 fitted: its printed 7.2 kHz went
to 23 kHz, free in its limits 3 ... 30 kHz), the sampler's input saturating softly (all 24
hits sit at full scale for their first 3 ... 30 ms), the normalisation, each hit aligned on its main click (the files
start 1.07 ... 1.48 ms before it, cut by hand). The Service Notes mark C500 ... C512 "SN 415300-UP" (p. 12; their
fault guide, p. 7, has owners of earlier units add 0.01 uF across the jacks), and R417 (11.0k fitted: 12k before SN
415300, 3.3k from it) dates the recorded 909 before that: the fit's mix-out pole at 23 kHz (-1.7 dB at 16 kHz where
the printed RC has -7.7 dB) fits a unit without them; the 15.9 kHz pole held in the chain then stands for the
sampler's input filter. The pitch envelope's parts were fitted alone on the periods first,
then held while the rest was fitted on the bands.

| constant | circuit | fitted | |
|---|---|---|---|
| `c14` | 33 nF | 37.4 nF | the rest pitch: 49.4 Hz in all 24 hits (the model 49.25 Hz; the circuit's values: 56 Hz); as well a smaller swing of IC13b |
| `c9` | 0.33 uF | 0.287 uF | the sweep's time constant: 7.4, 10.0, 17.6, 28.9 ms at TUNE 0, 3, 7, 10 (C9 against (R57 + TUNE) \|\| R26) |
| `r57` | 22k | 27.8k | R57 and VR2's end resistance (TUNE at 0) |
| `vr2`, `tune3`, `tune7` | 100k A | 116k; 9.5 %, 41 % at 3, 7 | TUNE's track: straight between 0, 3, 7, 10 |
| `r21` | 2.2k | 1.8k (limit) | C9's charge: the sweep starts higher than the circuit's values give (some 240 Hz at the click) |
| `r26`, `r27` | 330k, 1.5M | 341k, 1.51M | the sweep's depth and the rest |
| `c8`, `vr5`, `decay3`, `decay7` | 0.33 uF, 1M A | 0.35 uF, 0.80M (limit); 11 %, 39 % | DECAY: closer to linear than an A taper |
| `c7`, `vbe3` | 33 nF, 0.55 V | 36.8 nF, 0.57 V | the hold: 60 ... 80 ms |
| `qBr`, `qIs`, `r54` | 5, 1e-14, 470k | 4.3, 1e-12 (limit), 376k (limit) | the VCA: the decay's shape at its end |
| `dIs`, `dN` | 2.5 nA, 1.75 | 6.5 nA, 1.75 | the shaper's diodes |
| `vr3`, `r42`, `noiseClk` | 500, 470k, 450 kHz | 573, 376k (limit), 450 kHz | the click and the noise (the bus the circuit's in all voices; R46 x1.27, C13 x0.77 then: `bd3`, 2.27 dB as before) |
| ACC | | 2.55 V | the ACCENT line of the recordings: the app's normal level |
| HP | 128 Hz | 123 Hz | the mix out's high pass (the bus's 1.24k into C154 1 uF) |

The results, `fit/summary.py` (the model as the engine computes it; the pitch the mean of the hits' rms, `fit.py`'s
rms over all the hits' periods 16 cents):

| | band measure | pitch (24 periods) | first 5 ms (correlation) |
|---|---|---|---|
| the model, 24 hits, as fitted | 2.27 dB (1.79 ... 3.04) | 17 cents rms | 0.99 |
| the model, the code of 2026-11-14 (the base current's sign, not refitted) | 2.24 dB (1.83 ... 2.94) | 17 cents rms | 0.99 |
| the model fitted without the four D7 hits, on them | 2.12 dB | 9 cents | 0.99 |
| the kit DRUMKITS/TR-909, its bass drum | 5.95 dB (4.47 ... 8.15) | 318 cents | 0.95 |

What is left (`fit/diag.py`): the decay's last 20 dB fall a little faster than the recordings' (the fit presses the
VCA's constants to their limits there: R54, R42, Q12's Is); the click's band 2.5 ... 6 kHz some 3 dB strong at the
main click. The recordings carry no level (normalised) and no accent (all at one, unknown, ACCENT level): the
accent's range is the circuit's (ACCENT 2.55 V to 5 V: +4.4 dB, see above), Roland's traces (p. 9, at noon: some 2.5
times the amplitude with accent, +8 dB) suggest a lower normal level than the recordings' 2.55 V (1.7 V would give
it).

## Open

- The snare's and the clap's fits carry structure the model misses (above). (The noise bus is one now: 450 kHz.)
- SPICE: the rim shot whole (`fit/spice/rs.cir`); the toms' shaper and VCA with the attack (`fit/spice/tt_vca.cir`,
  `fit/ttvca.py`, a DC sweep: within 2 ... 12 % of the peak after `vcaCurrent2` and Q18's fix, 3 ... 10 % with the base
  current's sign fixed (2026-11-13; 3.0 ... 3.4 % without Q18), 2.1 ... 3.4 % with Q18 by Ebers-Moll (2026-11-14; at 2
  and 20 uA 2.1 ... 3.0 %, rms 0.5 ... 1.9 %; over its base current 3.2 % at worst, `fit/ttq18.py`), the toms' VCAs
  before 12 ... 21 % over at high base currents), the low tom's VCO-1 with TUNE's rails and ENV4
  (`fit/spice/tt_vco.cir`, `fit/ttvco.py`: within 1 ... 16 cents over 150 ms at TUNE 0, 5, 10, the peaks within 0.1 V),
  the ROM voices' VCA (`fit/spice/hh_vca.cir`, `fit/hhvca.py`: within 0.9 ... 4.3 %). Open: the hi-hat's and cymbals'
  envelope loop. The Service Notes: Q73 charges C135 through CH DECAY (from 0 V, where Q74 holds it during TRIG);
  IC67a's + input, Q84's bases (R489, R490) and the DECAY pots' far end show the same bar symbol. Read as one rail, the
  loop's reference current is (the rail - V(C135)) / R453, an exponential decay as the model has it; but no reading of
  that symbol (+15 V, ground, -15 V) gives an operating point: with the bases near ground Q84's right collector cannot
  push Q85's base (some 0.55 V) through R485 10k from emitters at some 0.66 V. A detail is misread or not in the scan;
  the model's envelope and level rest on the recordings.
- The cost (QEMU, instructions in a frame of a hit): the snare 9.8k, the bass drum 8.8k, a tom 9k, the clap 6.3k,
  the rim shot 4.6k, the hat and each cymbal 4.4k; all eleven every 20 ms in turn 64.5k (69.3k at most) against the
  kit DRUMKITS/TR-909's 73.9k (81.5k) in the same test. On the DSM (cycles, 2026-11-06) QEMU's count misleads:
  divisions (14 cycles), the FPU's latencies and the code's size weigh there. First measured: all eleven every 90 ms
  64.5k against the kit's 49.1k. Then (profiled with the DSM's PC sampler: the VCAs' base currents, the hats' and
  cymbals' VCA, the clap's OTA) the ROM VCA from a table of F and F' (one division a ROM step), the band limited step
  on 4 samples either side (the hats and cymbals 0.07 ... 0.11 dB further from the recordings), the base current's
  slope with one division, the clap's logistic and tanh from tables and its OTA left out once it takes under 1e-5,
  the app's silence without a pass over the mix: 56.3k against the kit's 57.6k in the same session (every 20 ms 108.6k
  against 102.5k). `-O2` instead of `-O3` (the code 8.6 KB smaller) was not faster. OCT40 not measured.
  After the rim shot's corrections (its resonators' two states, the spike's node, Q65's Newton steps) and the
  toms' (2026-11-06): QEMU all eleven every 20 ms 68.8k (73.1k at most); the DSM (adf081a6, the app in its RAM,
  `.build/va909/hw/hw_ram.py`) every 90 ms 58.7k against the kit's 58.5k, every 20 ms 118.6k against 103.2k; a tom
  alone every 90 ms 22k (before 20.8k; with two Newton steps in its VCAs 25.8k), the rim shot 17.8k (12.5k before
  its corrections; 20.8k before its Newton started from the last sample). The kit became DRUMS/VA-909 (2026-11-06,
  the user: an instrument a track, no multi-trig): QEMU, a hit every 250 ms, 4.1k (the hat, the cymbals) ... 10.9k
  (the snare, the toms) on average, 13k ... 19k at most; RAM 20.6 KB (all eleven voices, Instr changes at any time).
  Then a kit again without the selector (2026-11-07, the user: the knobs on pages, each instrument its own TRIG,
  stereo by the mix page): QEMU the eleven in turn every 250 ms 13.3k on average, 29.2k at most; every 40 ms
  68.2k / 73.0k, every 20 ms 70.4k / 75.0k (all eleven sounding); RAM 20.8 KB.
  Then one app of seven engines (2026-11-10, the user: the kit split, mods added): VA-909/Kit as before (the same
  samples as the kit of 2026-11-07, QEMU CRC ACE0CCEC, with ACCENT and other knobs as well) and the parts BD, SD,
  Toms, RS-CP, Hats, Cymbals with mods (app/DRUMS/VA-909/README.md), each allocating only its voices. QEMU, a hit
  every 250 ms (`-t 500`): the kit 13.7k / 29.6k (+0.7 %: the engine's dispatch), every 20 ms (`-t 40`) 68.1k / 73.3k
  against 67.8k / 73.0k of the kit before; the parts 4.9k (Hats) ... 18.4k (Toms) on average. RAM: the kit 23.8 KB,
  a part 5.2 ... 9.8 KB. Under `-ffast-math` the samples stayed the same only with the voices' `init()` on the fitted
  constants themselves (the compiler folds what it computes of them; a copy gave the cymbals 1-LSB and the ride a
  95-LSB difference) and `setParams(p, false)` as a template (an `if` on a run-time flag changed the tables' rounding).
  The bass drum's Drive (2026-11-10, the user, after the 909's Drive mods: "waveform adjust from normal to square"):
  a gain of 1 ... 16 before R52, the shaper's table filled again 4 of its 64 pairs of points a frame (`driveStep()`);
  not fitted (no recording of a modded 909; the mods' source, Colin Fraser's page "TR909 Sound Mods", which the
  MATRIXSYNTH post quotes, replaces R52 by a 10k pot with 1k in series: not these values). Other diodes for D10/D11
  (VA-909/BD's Diodes; the LEDs another of those mods, germanium and two 1N4148 in series swaps as in a pedal's
  clipper): typical, not measured, germanium Is 1.6e-7 A, N 1.5 (0.25 V at 100 uA), two 1N4148 the fitted Is with
  twice the fitted N, a red LED Is 4e-18 A, N 2 (1.5 V at 10 uA, 1.9 V at 20 mA); the shaper's table again
  (`shaperChanged()`, `driveStep()`). The
  body's level is kept at the 1N4148's (2026-11-10, the user: the LEDs "klingt irgendwie komisch": at Drive 0.25 they
  made the body +7.7 dB louder, into the output's limit, 17 ms flat a hit): `setLevelRef()`, Q12's current times the
  ratio of the AC parts of its gain F over the triangle's range (the native voice: within 0.3 dB of the 1N4148's body
  at Drive 0 ... 1). The ratio of the table as it is while it fills, its points in pairs from the ends in: a level set
  only at a pass's end had a switch during a hit peak x3 ... x5 for 16 ms (1N4148 -> LED, Ge -> LED at Drive 0.5),
  from the middle out still x1.9, from the ends in x1.16. What stays is the circuit's: the LEDs put three times the
  1N4148's voltage on Q12's collector, bf against br makes the body lopsided (second harmonic -9 dB against -19 dB).
  RS-CP split into VA-909/RS and VA-909/CP (2026-11-12, the user: engines of their own): one instrument each on the
  track's TRIG-INPUT, mono, the mods and Level on one page, Level as BD's (the TOTAL ACCENT while the ACCENT input is
  patched; the user first had it the level always, then as before). +3 dB and +4.5 dB over the kit's channel (LUFS -19.3
  and -17.8, among the MD rim shots and claps); QEMU 7.8k and 6.0k on average, RAM 6.1 and 5.8 KB; the other engines'
  samples unchanged (the same CRCs, with ACCENT as well).
  The knobs laid out as the released drums have them (2026-11-12, the user; DRUMS/808ish-BD, 909ish-BD: Pitch | Punch,
  Tone | Decay): the tune at the top left, the decay at the bottom right with the attack or snappy above it, Accent /
  Level at the bottom of the left column (BD, SD, RS, CP, the toms' page 2, the kit's SD column; the same samples, also
  with the parameters set by name).
  The engines registered as DRUMS/VA909-BD, -SD, -RS, -CP, -Toms (2026-11-13, the user: no kit and no hats for now;
  DRUMS/TR909-HiHat after the toms): the hats, the cymbals and the kit off by `VA909_ALL 0` (their code kept; with 1 the
  same samples as before), the five in the firmware's `engines_order` after DRUMS/Claps, before DRUMS/TR909-HiHat. The
  same samples (CRCs) under the new names.
  VA909-Toms one instrument (2026-11-13, the user: simpler, a selector LT / MT / HT as the second knob; Decay's middle
  the 909's longest): the track's TRIG-INPUT, mono, one page Tune | Tom, Bend | Noise, Level | Decay (Dec-Max gone).
  Tom picks the voice of the next hit; the three voices stay, so a tom rings on under another's hit. One Tune and one
  Decay turn the three toms' pots alike. Decay: DECAY 0 ... 1 in its lower half; above it C21 x1 ... x4. VR11 (the
  old Dec-Max) lengthened the toms little at DECAY's top: with 556k (VR11 + R111) C21 empties mostly through R100
  (80k) into C23, so VR11 x4 gave the low tom 0.6 -> 0.8 s to -40 dB; C21 x4 1.55 s (HT 0.4 -> 1.05 s), C21 and C23
  x4 2.25 s but ENV2's rise stretched to a swell (the attack -0.6 dB), C23 x4 alone 1.0 s. +6 dB over the kit's
  channel (LUFS -17.5 LT, -18.0 MT, -17.2 HT at Decay 0.5); QEMU 11.0k on average (one tom ringing), 9.8 KB RAM. The
  other engines' samples unchanged (BD, SD, RS, CP; Hats, Cymbals, Kit with `VA909_ALL 1`, the kit's toms as well).
  VA909-SD on one page as well (2026-11-13, the user): Tune | Snappy, Decay | Color, Level | Tone (the left column the
  tones, the right one the snappy). Tune has the 909's TUNE in its middle half and the Pitch mod (R287 x2 ... x0.5) at
  TUNE's ends in its outer quarters: the lower tone 69 Hz at 0, 119 Hz at 0.25, 228 Hz at 0.75, 382 Hz at 1. Decay is
  T-Dec (C83 / C82); Color N-Color and N-Body together (C81 / C84 as N-Color, C80 as N-Body at 1 - Color: darker and
  fuller down, brighter and thinner up). The same samples as the old knobs where they meet (Tune 0.625 = TUNE 0.75,
  Tune 0.125 = TUNE 0 with Pitch 0.25, Tune 0.875 = TUNE 1 with Pitch 0.75; Color c = N-Color c with N-Body 1 - c), at
  the defaults and in the other engines (the kit's SD with `VA909_ALL 1` as well).
  On the DSM (2026-11-13, the user: measure and improve VA-909's cycles on DSM0): the app (79 KB) ran from the QSPI
  flash: next to SYNTH/Plaits (184 KB) the D2 SRAM had 78.6 KB left for a copy (machine/src_DSM/code_ram.cpp), some
  0.6 KB short. Without `VA909_ALL` the ROM voices' and the kits' branches are constant false (`ROM_VOICES`, `MULTI`),
  so their code (the hat, the cymbals, kBlep, setupMultiTrigs, the mix page's levels, stereo) is not compiled: 62 KB,
  the same samples (CRCs; with `VA909_ALL 1` the same binary as before). From the SRAM (cycles a frame, a hit every
  250 ms, the engine on T4 over MIDI, Plaits + VA909-BD on the other tracks): BD 33.0k -> 11.7k, SD 57.3k -> 21.9k,
  RS 38.0k -> 16.2k, CP 34.3k -> 13.5k, Toms 56.9k -> 21.6k (LT MT HT in turn every 20 ms 93.1k -> 53.2k), idle 9 ...
  11k -> 4k (the firmware's part of a track). The user's patch then: the frame 93k -> 71k on average, its peak 273k ->
  89k, no underruns in a minute (before: 6 in the first 71 s after a reset, 735 in 9 min). What is left (a PC profile,
  one tom every 20 ms): Tom::process 16.3k of the app's 20.7k: three VCOs, three VCA lookups with a Newton step each
  and a division every 4 samples a sample.
  The toms then (2026-11-13, the user: optimise them; PC profile per source line: vcaCurrent2 17 %, the loop 15 %,
  BaseCurrent 13 %, the VCA tables 10 %, atComp 9 %, the tables 6 %, the tom noise 6 %; nothing dominates): VCO-3's
  VCA off at 40 dB under a hit's base current (2e-7 A, it was 60 dB: 2e-8; -46 dB under the tom's sum, VCA19 / VCA20
  at -11 / -16 dB then; 0.12 s instead of 0.25 s, the low tom at DECAY's top), C25 emptying on into the base without the VCA's load; the tom noise
  only while a tom's VCO-3 VCA is on (`wantsNoise()`); Q18's bend path (atComp, a division a sample) only while it moves
  VCO-1 by more than 0.1 mV (some 100 ms, it was the whole hit: q18 > 0 as long as C24 holds any charge); the
  comparators' thresholds once every 4 samples (bit exact). The fit's measure 3.83 -> 3.82 dB (tones 50 cents, level
  3.31 dB as before); the level per 5 ms of hits in a row within 0.6 dB (MT, the start of the next hit; LT, HT 0.24 dB).
  On the DSM a tom hit every 250 ms 21.6k -> 16.6k cycles a frame; every 20 ms (the whole model all the time) and three
  toms in turn as before (23.4k, 53.9k). Tried, not taken: a VCO standing still while its VCA is off (the next hit
  another phase: -30 dB rms of difference); 1/x by a bit seed and two Newton steps for the two divisions a sample (-107
  dB, but no faster on the M7: 23.8k / 55.0k).
  The base's swing had the wrong sign (2026-11-13, found writing the white paper, settled with ngspice, the user:
  take the fix without a new fit): Ebers-Moll's base current ib = is e^(vbe/vt) (1/bf + u/br) gives vbe =
  vt ln(ib/is') - L(vce); `BaseCurrent::at` added +L, so where a collector below the emitter raises ib it fell.
  `fit/vcabase.py`: one NPN (ideal Ebers-Moll), its base through 470k from 0.7 ... 2 V, the collector held at -0.15 ...
  +0.15 V: ngspice = Ebers-Moll exactly; the model with -L within 0.01 ... 1.8 % of ib, with +L up to 130 % off at 0.7 V.
  The toms' VCO-1 path (`fit/ttvca.py`, the netlist's QVCA with RB 50): -L 0.6 % of the peak at vth 0.7 ... 1 V
  without Q18, +L 5 ... 12 %; with Q18 bending 10 % either way (another, smaller miss of that path). The fits'
  measures with the fix and the constants as they were: BD 2.27 -> 2.24 dB (pitch 16 cents), SD 3.48 -> 3.50, CP
  4.16 -> 4.17, the toms 3.82 -> 3.81 (level per 5 ms 3.31 -> 3.35 dB), RS 3.59 -> 3.60: the swing matters in the
  deep decay, which the band measure weighs little. Not refitted.
  Q18 by Ebers-Moll and the snare's onset as SPICE (2026-11-14, found checking the two paths against SPICE again: the
  toms' attack and "The model against SPICE" of the snare above): the toms' measure 3.806 -> 3.811 dB, the snare's
  3.50 -> 3.48 dB, not refitted; BD, RS, CP bit-identical (the same QEMU CRCs). QEMU (`-e VA909 -s 4 -t 500` / `-t
  40`, average / at most): SD 10.9k / 11.7k (before 10.8k / 11.0k) and 10.5k / 11.7k (10.3k / 11.0k), Toms 9.1k /
  11.4k (9.0k / 11.1k) and 11.4k / 11.6k (11.1k / 11.4k), their RAM 11.7 KB (9.2 KB: `TomTables::bend`); the app 64 504
  bytes. The DSM not measured again. The SPICE checks again with the code as it is: BD 1.2 ... 2.3 dB (mean 1.78), the
  snare's tones 2.4 ... 2.7 dB, the clap's states and the rim shot as before. The voices asleep with the fitted
  constants after a normal hit at noon (`PROBE=idle`): BD 0.28 s, SD 0.41 s, CP 0.92 s.
- The ROM voices' accent: the circuit's exponential converters give +14 ... +22 dB over the normal level at Total
  Accent 0.8 (the bass drum +4.4 dB); no recording says how far the 909's goes. The app scales the excursion they
  get to 0.3 (+5 / +8 dB).
- Recordings with known accent (the 909's Total Accent at known positions), hits in a row (rolls), the individual
  out: none found free. The fit's accent range rests on the circuit.
- The real TRIG's length: 2 ms after the Service Notes; the recordings don't show it (their start is cut).
- R495 (units up to SN 371499): not known for the recordings' 909; left out.
