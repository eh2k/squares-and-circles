# DRUMKITS/MFB-512

An emulation of the MFB-512, an analog drum machine by MFB (Berlin), for [DrumSynth](../DrumSynth/README.md): its
9 instruments, synthesized from measurements of MFB-512 recordings.

| Instrument | Note | Instrument | Note |
|---|---|---|---|
| BD512 bass drum | 36 | CP512 hand clap | 39 |
| SD512 snare drum | 38 | CH512 closed hi-hat | 42 |
| LT512 low tom | 41 | OH512 open hi-hat | 46 |
| MT512 mid tom | 45 | CR512 crash | 49 |
| HT512 high tom | 48 | | |

The kit is the flash blob `DRUMKITS/MFB-512.rc8`. On average it is about 4 dB off the recordings (spectra and
envelopes), a bit more than the other kits; the largest gaps are in the toms and the attacks.

## How it was made

Recordings from <https://github.com/geikha/tidal-drum-machines>, folder `machines/MFB512` (44.1 kHz, one hit per
instrument). The files are short and end while the sounds still decay (bass drum 80 ms, crash 794 ms), and the open
hat and crash carry 50 Hz mains hum. The tails past the end of the files are extrapolated.

Same method as the [TR-808](../TR-808/README.md#method). Three things were needed beyond it:

- Harmonic layers for the toms: their harmonics are strong in the attack and weak later. The tom is split into a
  fundamental and copies of it that only carry harmonics and fade out early. Sines with the same pitch envelope stay
  in phase, so this gives harmonics that change over time.
- A soft clipper for the bass drum: its first positive half wave is flat at the top.
- Clipped noise for the hats and the crash: their waveform is far less peaky than filtered noise. Noise through a
  band pass into a soft clipper gets close; the filter has to come before the clipper, since a narrow filter after
  it makes the noise peaky again.

Instruments:

- BD512: sine about 60 Hz, starting at 110 Hz and dropping within the first half waves, into an asymmetric soft
  clipper that flattens only the first positive half wave. A noise click in the first few ms. A short drum: after
  25 ms it loses about 5 dB per half wave.
- SD512: a 186 Hz tone with a waveshaper for its harmonics, a second tone at 281 Hz, and two noise parts (a low
  body and a broad part with a high shelf). The noise spectrum stays the same over the whole sound, so the VCF
  didn't help.
- LT512 / MT512 / HT512: falling pitch (LT 128 -> 92 Hz, MT 146 -> 107 Hz, HT 187 -> 126 Hz), the tone plus two
  harmonic layers, attack noise with a resonance at 460-720 Hz. LT adds one gliding sine (414 -> 390 Hz); MT and HT
  two inharmonic modes at a fixed ratio to the fundamental (about 2.5x and 3.5x).
- CP512: noise bursts at 0, 12.8 and 26.8 ms, then a tail from 40 ms that decays to -50 dB at 190 ms. Mostly
  600 Hz-3 kHz.
- CH512 / OH512 / CR512: clipped band noise (CH and OH around 7.5 kHz, CR around 3.9 kHz) with the recording's
  envelope, plus a low band and a resonance at 4-4.9 kHz. OH and CR also have their stable spectral lines as sines
  (e.g. OH 4120 Hz, CR 2745 Hz). With their low crest they peak at only about 0.5.

## License and credits

- The kit is played by [DrumSynth](../DrumSynth/README.md) (`lib/drumsynth`, © E.Heidt, MIT license, see [License](../../../README.md#license)).
