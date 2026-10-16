# SYNTH/DxFM

<img align="right" src="DxFM.png" width=256px />

A Yamaha DX7 voice (six operators, 32 algorithms, envelopes, LFO) that plays DX7 32-voice SysEx banks. Built on
msfa, the DX7 engine from Google's Music Synthesizer for Android, as used in Dexed. Two voices alternate, so the
release of one note rings on under the next, optionally spread across the stereo field.

## Engines

| Engine | |
|---|---|
| SYNTH/DxFM | the 32 voices of a bank you upload (flash blob `DXFMSYX0`) |
| SYNTH/DxFM_BNK1-3 | the 96 voices of the three FM banks built into Plaits, no upload needed |

## Parameters

| Knob | Range | Default | |
|---|---|---|---|
| **V_OCT** | -4 ... +4 octaves | 0 (C4) | pitch, 0 plays MIDI 60 |
| *voice name* | 0 ... 31 (DxFM), 0 ... 95 (BNK1-3) | 0 | the program; the label shows the voice name |
| **Rate** | 0 ... 127 | 64 (middle) | scales envelope rates 2-4: left shorter (x1.5), right longer (x0.5). Left of the middle it also lowers velocity, down to 64 |
| **Hold** | 0 ... 1 s | 0 | keeps the key down this long after the gate falls |
| **Stereo** | 0 ... 1 | 0 | 0: both voices in both channels, 1: one voice left, the other right |

Left encoder: V_OCT, Rate, Stereo. Right encoder: voice, Hold.

Rate takes effect at the next note. Velocity is otherwise always 127; there is no accent input.

## Notes

- SYNTH/DxFM needs a bank in flash. Upload a DX7 32-voice bulk dump (`.syx`) with the web app: LIBRARY, "DXFM".
  It stores bytes 6 ... 4101 of the file (the 32 packed voices without the SysEx header) as `DXFMSYX0`.
- Without the blob it plays its built-in voice MARIMBA as program 0 and shows "NO DXFM.SYX FOUND!".
- A voice with any byte >= 0x80 is not loaded: it shows "INVALID" and the previous voice keeps playing.
- The voices follow V/OCT changes while they sound (via msfa's master tune). The patch's TRANSPOSE is not applied.
- Changing the program releases both voices.

msfa renders 64-sample blocks at 48 kHz into a small ring buffer per voice.

Code 48 KB, RAM 5.3 KB.

## License and credits

- `DxFM.cpp`: © 2023 E.Heidt, Apache License 2.0 (see the header of the file).
- FM synthesis engine of [music-synthesizer-for-android](https://github.com/google/music-synthesizer-for-android) (Google Inc.) as in Dexed (Pascal Gauthier, Jean Pierre Cimalando), Apache License 2.0.
- [lib/plaits](../../../lib/plaits): Mutable Instruments Plaits, © Emilie Gillet, MIT license.
