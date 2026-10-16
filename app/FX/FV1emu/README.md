# FX/FV1emu

Emulator of the Spin Semiconductor FV-1 effects DSP. It runs FV-1 programs (reverbs, delays, choruses ...) from a
bank stored as a flash blob. The FX knob picks the program, A, B and C are the chip's pots POT0..POT2.

## Engines

Every flash blob starting with `!FV1` shows up as its own menu entry, named after the blob without extension
(with `FX/` in front if the name has no category). The FV1emu editor saves its bank as `FX/FX_BANK.fv1`, which
gives FX/FX_BANK. Without such a blob the engine is not in the menu.

To make a bank, use the FV1emu editor ([lib/fv1/web-app](../../../lib/fv1/web-app)), linked from the web app's
Library page. It builds a bank from SpinASM programs (its factory list: [factory.json](../../../lib/fv1/factory.json))
and sends it to the module as `FX/FX_BANK.fv1`.

## Parameters

| Knob | Range | Default | |
|---|---|---|---|
| **D/W** | 0 ... 1 | wet | dry/wet. At 0 the program doesn't run |
| **FX** | program names | first | program in the bank |
| **A** | 0 ... 1 | 0.5 | POT0 |
| **B** | 0 ... 1 | 0.5 | POT1 |
| **C** | 0 ... 1 | 0.5 | POT2 |

Left encoder: D/W, A, C. Right encoder: FX, B. A, B and C are renamed to the program's pot names; a pot with no
name is hidden.

## Usage

Input comes from the I/O page as usual: **AUX-INPUT** (a jack or `USB`, level via **INSERT-AUX**) or
**INSERT-&lt;engine&gt;** for other tracks. L goes to ADCL, R to ADCR; the output is stereo from DACL/DACR.

## Implementation

[lib/fv1/FV1.hpp](../../../lib/fv1/FV1.hpp): 24-bit fixed point with saturation, 32 registers, 2 sine and 2 ramp
LFOs, 32,768 words of delay memory (stored as 16 bit). When FX changes, the program is decoded into a table of
handlers ([lib/fv1/FV1_DASM.hpp](../../../lib/fv1/FV1_DASM.hpp)) that runs once per sample.

The program runs at 48 kHz, while most FV-1 programs are written for 32.768 kHz. So delay times come out 0.68x
as long (0.68 s max) and LFO rates and filter frequencies 1.46x higher than on the chip.

Notes:

- Decoding stops at the first `SKP 0,0` (the NOP assemblers fill unused slots with), so a program with an explicit
  NOP in the middle gets cut there.
- Switching programs clears the registers but not the delay memory or the LFOs.
- If the program table can't be allocated, the input passes dry and the screen shows OUT OF MEMORY.
- FX/Gated-Reverb and FX/Reverb-HP-LP run built-in FV-1 programs on the same emulator.

Bank format: `!FV1`, byte 4 = number of programs, then from byte 8 one 640-byte record per program (1 byte props,
32 bytes name, 3 x 16 bytes pot names, 47 reserved, 512 bytes of program as 128 big-endian words).

Code 6.3 KB, RAM 67 KB (plus 2 KB heap for the decoded program).

## License and credits

- `FV1emu.cpp`: © 2021 E.Heidt, MIT license (see [License](../../../README.md#license)).
- FV-1 emulator (`../../../lib/fv1`) of the Spin Semiconductor FV-1; the Spin Semiconductor programs under the Spin Semiconductor Open Reverb License (see [lib/fv1/spn/Readme.txt](../../../lib/fv1/spn/Readme.txt)).
- `../../../lib/msfa/exp2.h` (used by the FV-1 emulator) from [music-synthesizer-for-android](https://github.com/google/music-synthesizer-for-android), © Google Inc., Apache License 2.0.
