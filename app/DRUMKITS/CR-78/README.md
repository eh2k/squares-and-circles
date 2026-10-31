# DRUMKITS/CR-78

An emulation of the Roland CR-78 CompuRhythm for [DrumSynth](../DrumSynth/README.md): 14 instruments, all
synthesized from measurements of CR-78 recordings. It has nothing to do with Roland's own `CR78` kit for the RC-808.

| Instrument | Note | Instrument | Note |
|---|---|---|---|
| BD78 bass drum | 36 | TB78 tambourine | 54 |
| SD78 snare drum | 38 | GU78 short guiro | 73 |
| RS78 rim shot | 37 | GU78L long guiro | 74 |
| CH78 closed hi-hat | 42 | MB78 metallic beat | 80 |
| OH78 open hi-hat | 46 | BH78 high bongo | 60 |
| CB78 cowbell | 56 | BL78 low bongo | 61 |
| CL78 claves | 75 | LC78 low conga | 64 |

The CR-78's maracas are missing (no recording). The kit is the flash blob `DRUMKITS/CR-78.rc8`.

MIDI: the kit has seven notes outside 35 ... 59, one more than DrumSynth has slots for. Key 61 plays LC78 instead
of BL78 and key 64 falls back to OH78; BL78 is on key 72 (see [DrumSynth](../DrumSynth/README.md)).

On average the result is about 3.4 dB off the recordings (spectra and envelopes). The biggest gaps are in the late
part of the snare and the conga, where the recordings are mostly their own noise floor.

## How it was made

Recordings from <https://github.com/geikha/tidal-drum-machines>, folder `machines/RolandCompurhythm78` (44.1 kHz,
one hit per file). Several file names don't match the CR-78's voices (`Woodblock-02` is the rim shot,
`Woodblock-01` the claves, `Conga H` / `Conga M` the bongos, `Quid` the guiro, `Hit` the metallic beat), so each was
identified by measuring it. The recordings have a hum/noise floor about 40 dB down, and some are gated.

The method is the one described in [TR-808](../TR-808/README.md#method): measure, build from the engine's parts,
render and score, refine, set the levels.

- BD78: sine 60 Hz, the first period shorter (83 Hz), a weak waveshaper, LP 800 Hz and a noise click. Gone after
  about 200 ms.
- SD78: a 325 Hz tone that dies within 25 ms, noise HP ~1.8 kHz -> LP ~7.8 kHz gated after about 55 ms, and a low
  residue around 200 Hz after the gate.
- RS78: four sines (1523 Hz strongest, plus 1034, 2019, 697 Hz) and a noise click, 4 ms long.
- CH78 / OH78: plain noise in a band around 8 kHz, no partials. CH is gated after 45 ms, OH decays over 400 ms and
  starts with a short broadband click.
- CB78: two tones at 536 and 789 Hz with their harmonics and a sum tone at 1322 Hz that fades in (the CR-78 mixes
  the two oscillators nonlinearly). Flat for 30 ms, then a fast decay. The beat between the two tones is not in the
  same phase as the recording's, since the engine does not set an oscillator's start phase.
- CL78: a sine 2546 -> 2575 Hz with three weak sidebands, about 45 ms.
- TB78: narrow band noise at 4.8-5 kHz over a flat high floor, with two hits (the second at 65 ms).
- GU78 / GU78L: band noise around 6 kHz chopped into bursts by the envelope. The short one has 9 bursts every
  13.35 ms, the long one 53 bursts, the last 24 faster (every 8.1 ms). Burst times and heights follow the recordings,
  and a soft clipper makes the noise less spiky.
- MB78: 23 sines for the measured inharmonic partials (1.1-17.9 kHz), each with its own envelope, plus a little
  noise. The six-square metallic oscillator could not place the gaps in its spectrum.
- BH78 / BL78 / LC78: sines at 616, 402 and 207 Hz with no pitch change, faded in over a quarter period so the
  first half wave dips like the recording (a bridged-T resonator hit by a pulse). The conga adds weak 2nd and 3rd
  harmonics and a mode at 160 Hz; the bongos a quiet narrow residue around the tone.

## Engine behaviour worth knowing

- Envelopes can have many points: the guiros' envelopes carry every burst.
- The engine's sine starts at -1. A quarter-period fade in reproduces the start of a bridged-T voice; instruments
  made of many sines need a fade in, or they all start at -1 together.

## License and credits

- The kit is played by [DrumSynth](../DrumSynth/README.md) (`lib/drumsynth`, © E.Heidt, MIT license, see [License](../../../README.md#license)).
