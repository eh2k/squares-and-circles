/* sam_voice: SAM (Software Automatic Mouth, lib/SAM) as a streaming renderer.
 *
 * lib/SAM renders a whole utterance at once into a sound buffer (22050 Hz, 8 bit): ten seconds of
 * buffer, all of the work in one call. sam_voice produces the same samples in the order they are played:
 * sam_voice_say() parses the phonemes and prepares the first frames, sam_voice_render() then makes as many
 * samples as the caller asks for. The state is one struct of about 2.9 KB, there are no globals, and
 * several voices can run side by side.
 *
 * The output is bit for bit the one of lib/SAM (SAMMain() + Render()) for every utterance, see
 * README.md for the test and for the few places where lib/SAM's result depended on undefined
 * behaviour and sam_voice had to pick one.
 *
 *   sam_voice_t sam;
 *   sam_voice_init(&sam);
 *   char ph[256];
 *   sam_voice_text_to_phonemes("HELLO", ph, sizeof(ph)); // sam_reciter.c, English text to phonemes
 *   sam.speed = 72;                                  // the voice is read by sam_voice_say()
 *   sam_voice_say(&sam, ph);
 *   uint8_t buf[64];
 *   int n;
 *   while ((n = sam_voice_render(&sam, buf, 64)) > 0)
 *       ...                                          // n samples, 22050 Hz, unsigned, 128 = silence
 */
#ifndef SAM_VOICE_H
#define SAM_VOICE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* For the code that runs once per utterance or in setup: small rather than fast (the apps are built with
 * -O3, which makes it a third larger and saves little time there). */
#ifndef SAM_VOICE_COLD
#define SAM_VOICE_COLD __attribute__((optimize("Os")))
#endif

#define SAM_VOICE_SAMPLE_RATE 22050
#define SAM_VOICE_SILENCE 128

/* the end of a phoneme string, as the reciter writes it */
#define SAM_VOICE_END 0x9B

typedef struct
{
    /* the voice, read by sam_voice_say(); defaults are SAM's: 72, 64, 128, 128, 0 */
    uint8_t speed;  /* frame length: 72 = SAM's normal speed, lower is faster */
    uint8_t pitch;  /* glottal pulse length: lower is higher */
    uint8_t mouth;  /* F1 scale of the voiced phonemes, 128 = 1.0 */
    uint8_t throat; /* F2 scale of the voiced phonemes, 128 = 1.0 */
    uint8_t sing;   /* 1: pitch without the F1 contour (SAM's sing mode) */

    /* the phonemes: index, length (frames), stress. 255 ends the list, 254 starts a new chunk
     * (InsertBreath()). The indices are 8 bit and wrap like SAM's, hence 256 entries. */
    uint8_t ph[256], len[256], str[256];
    uint8_t count;    /* where the 255 is, while parsing */
    uint8_t overflow; /* the rules made more than 254 phonemes */

    /* the frames of the current chunk: pitch, F1, F2, F3, A1, A2, A3 (render.c's pitches[] ..
     * amplitude3[], tables 168..174), and the sampled consonant flags */
    uint8_t frame[7][256];
    uint8_t cons[256];

    /* the next chunk: whether there is one, where it starts in ph[] */
    uint8_t more, next_chunk;

    /* Render()'s main loop */
    uint8_t state;
    uint8_t y;            /* frame */
    uint8_t frames;       /* frames left (mem48) */
    uint8_t speedcounter; /* outputs left in this frame */
    uint8_t glottal;      /* outputs left in this glottal pulse (mem44) */
    uint8_t mem38;        /* outputs left until the voiced sample (3/4 of the pulse) */
    uint8_t mem39;        /* sampled consonant flags of the frame */
    uint8_t phase1, phase2, phase3;
    /* RenderSample() */
    uint8_t sample_y;     /* frame to return to */
    uint8_t sample_pos;   /* position in sampleTable */
    uint8_t sample_count; /* bytes left (voiced) */
    uint8_t sample_bits;  /* bits left in sample_byte */
    uint8_t sample_byte;
    uint8_t sample_table; /* which 256 bytes of sampleTable */
    uint8_t sample_off;   /* value of an unvoiced 0 bit */
    uint8_t voiced_pos;   /* where the voiced sample goes on (mem66) */

    /* the output: lib/SAM writes each output to 5 samples from bufferpos/50 on, and the next output
     * overwrites those from its own position on. A sample is final once the position has passed it. */
    uint8_t last_kind;  /* the previous output's timetable index */
    uint32_t bufferpos; /* in 1/50 samples */
    uint32_t out_pos;   /* bufferpos / 50: the samples before it are final */
    uint32_t written;   /* out_pos + 5 */
    uint32_t emitted;   /* samples handed out */
    uint8_t ring[16];   /* samples [emitted, written) */
} sam_voice_t;

/* SAM's default voice, nothing to say */
void sam_voice_init(sam_voice_t *s);

/* Starts an utterance: phonemes (e.g. "/HEH3LOW" or the reciter's output), ending with SAM_VOICE_END
 * (or 0). The voice is read now. Returns 0 if the phonemes don't parse (then there is nothing to
 * render). Can be called any time, a running utterance stops. */
int sam_voice_say(sam_voice_t *s, const char *phonemes);

/* Renders up to n samples (22050 Hz, unsigned 8 bit, SAM_VOICE_SILENCE = 0). Returns how many it wrote:
 * fewer than n only at the end of the utterance. */
int sam_voice_render(sam_voice_t *s, uint8_t *out, int n);

/* sam_reciter.c: English text (A-Z, digits, punctuation; lower case is upper cased) to phonemes ending
 * with SAM_VOICE_END and 0. The text ends at 0 or '['. Returns the length without the 0, or -1 if the
 * text has a character the reciter has no rule for, or if out is too small. */
int sam_voice_text_to_phonemes(const char *text, char *out, int size);

#ifdef __cplusplus
}
#endif

#endif
