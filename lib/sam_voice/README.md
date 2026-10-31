# sam_voice

SAM (Software Automatic Mouth, [lib/SAM](../SAM), from https://github.com/s-macke/SAM) as a streaming
renderer for `SYNTH/SAM`.

lib/SAM renders a whole utterance in one call into a 220 KB sound buffer (the engine gave it 48 KB), and
keeps its state in globals. On a trigger, the engine spent 1.0 to 2.1 million instructions (3.4 to 7 frames
of 0.5 ms) in one `process()` call. sam_voice produces the same samples in the order they are played:

- `sam_voice_say()` parses the phonemes and makes the frames of the first chunk, `sam_voice_render()` then makes as many
  samples as the caller asks for (a chunk is the part up to a breath, at most 256 frames of 10 ms).
- All state is one `sam_voice_t` of 2876 bytes, there are no globals; several voices can run side by side.
- The tables are `const`, the F1/F2 scaling of mouth and throat is computed instead of written into
  the tables, F3's square wave is computed.
- The code that runs once per utterance (the phoneme rules, the frames) and the reciter are built
  with `-Os` (`SAM_VOICE_COLD`), the output loop with the apps' `-O3`: 3.3 KB less, same samples.

| `SYNTH/SAM` (test_qemu, 4 s, all 7 words) | lib/SAM | sam_voice |
|---|---:|---:|
| RAM (module + heap) | 53.2 KB | 3.6 KB |
| `BIN_SIZE` | 29.0 KB | 16.6 KB |
| instructions of the trigger frame | 1.03 to 2.10 M (344 to 699 %) | 19 to 35 k (6.3 to 11.7 %) |
| instructions per frame, average | 0.9 to 2.3 k | 1.8 to 1.9 k |

The average is a little higher because sam_voice renders while the word plays; lib/SAM rendered once and the
engine played the buffer again on the next trigger (except for RND_0-9).

## API

```c
#include "sam_voice/sam.c"     // in an extern "C" block of the app
#include "sam_voice/sam_reciter.c" // only for text -> phonemes

sam_voice_t sam;
sam_voice_init(&sam);                                   // SAM's voice: speed 72, pitch 64, mouth/throat 128
char ph[256];
sam_voice_text_to_phonemes("HELLO", ph, sizeof(ph));    // English -> " /HEHLOW\x9b"
sam.speed = 72;                                    // the voice is read by sam_voice_say()
sam_voice_say(&sam, ph);                                // 0: the phonemes don't parse (or are too many)
uint8_t buf[16];
int n = sam_voice_render(&sam, buf, sizeof(buf));       // 22050 Hz, unsigned, 128 = silence; < 16 at the end
```

See [sam.h](sam.h). The phonemes are SAM's (`/HEH3LOW`), see lib/SAM's README. `sam_voice_parse()` +
`sam_voice_start()` are `sam_voice_say()` in two calls (`SYNTH/SAM` parses in the trigger's frame and starts in the
next one: its trigger frame took up to 35k instructions).

## Same output as lib/SAM

The phoneme rules (`Parser1()` .. `InsertBreath()`), the frame setup of `Render()` and the reciter are
lib/SAM's code with the 6502 registers `A`, `X`, `Y` as locals and the same 8 bit arithmetic; the labels and
names in the comments are lib/SAM's. `Render()`'s output loop and `RenderSample()` are a state machine
(`step()`) that stops after each output. lib/SAM writes each output to 5 samples from `bufferpos/50` on and
lets the next output overwrite them from its own position on; sam_voice keeps the last 16 samples in a ring and
hands out a sample once the position has passed it.

lib/SAM's quirks that shape the sound are kept, for example: `AddInflection()` left its last pitch in the
register `A`, and `Render()` then tested `A == 2` for the question mark, so a period whose inflection ended
on pitch 2 got the falling one as well; the first frame always has speed 72; the pitch transition starts at
the blend position but takes its slope from the middles of the two phonemes.

Tested against lib/SAM on the host (2026-10-04): a copy of lib/SAM and sam_voice rendered the same utterance, and
every sample was compared. Texts: the engine's words, the digits, ~3300 words and ~480 sentences from the
repository's Markdown files; 20 000 random phoneme strings (all phonemes, stress digits, pauses,
punctuation); each with SAM's voice, the engine's voice and random speed, pitch, mouth, throat and sing mode,
rendered in random block sizes: ~120 000 utterances, ~3.4 billion samples, no difference. The reciter gave
the same phonemes for all texts.

In the engine (test_qemu, ARM), `SYNTH/SAM` with sam_voice gives the CRC of the lib/SAM build for every word, once
the start of the word is rendered the old way (see below). For RND_0-9 the CRCs agree as well when the build
also keeps lib/SAM's leftovers of the previous word.

## Where sam_voice differs

Only where lib/SAM's result was undefined, depended on the previous utterance, or where lib/SAM hung:

| lib/SAM | sam_voice |
|---|---|
| `flags[255]` for the end marker, past the end of the table (`AdjustLengths()` twice, `Parser2()`'s T/D rule). In the S&C build that byte was the high byte of the engine's `_r_throat`: 0xBF from the image, or 0x3F once it was written; with 0xBF a word that ends in `<vowel> T` (the digit 8, `EY4T`) ended in a flap `DX` | the end marker has no flags: `T` stays `T` |
| `phonemeindex[pos - 1]` at `pos` 0 read the byte before the array | the index wraps to 255 (32, as `SAMMain()` sets it) |
| a division by 0 in `Render()`'s pitch transition (two phonemes of length 0 or 1) | as on the Cortex-M: quotient 0, remainder the dividend |
| the frames of the previous utterance and the last output's timetable index carried over into the next utterance | each utterance starts from zero |
| the first 3 to 4 samples of an utterance were never written (the engine played 0, a click to −1) | silence (128) |
| `InsertBreath()`: without a pause in the last 232 frames it went back to the same pause again and again, forever | the breath goes before the phoneme that reaches 232 frames |
| an unvoiced consonant takes two frames: with one frame left, the frame counter went to 255 and the output loop on through the frames of earlier chunks, often forever | the chunk ends |
| `AddInflection()` (`.` and `?`) skipped frames of pitch 127 without a bound: forever when all 256 were 127 (pitch 127 is the engine's default; seen in a sentence with `!! ...`) | it stops after 256 |
| `Insert()` on a full list (255 at index 254) dropped the end marker, and the parser ran on, mostly forever | `sam_voice_say()` returns 0 |
| `phonemeIndexOutput[60]` had no bound | the chunk's phonemes are moved together in place in `ph[]` |
| the input ended at 155; at 0 or after 254 characters it failed | it ends at 155 or 0 or after 254 characters |
