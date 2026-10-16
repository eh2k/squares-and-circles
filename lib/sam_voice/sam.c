/* sam_voice, see sam.h and README.md.
 *
 * The phoneme rules (Parser1() .. InsertBreath()) and Render()'s frame setup are lib/SAM's code,
 * transcribed with the 6502 registers A, X, Y as locals and with the same 8 bit arithmetic; the
 * labels (pos41457, ...) and function names in the comments are lib/SAM's, so both can be read side
 * by side. Render()'s output loop and RenderSample() are a state machine here (step()), so that the
 * caller can take the samples a few at a time.
 */
#include <string.h>
#include <stdlib.h>

#include "sam.h"
#include "sam_tables.h"

/* lib/SAM indexes its phoneme lists with unsigned char and lets the index wrap. Where it computed
 * an index such as pos - 1 in int, it could read before the array (pos 0); here that index wraps to
 * 255 as well. */
#define PH(i) s->ph[(uint8_t)(i)]
#define LEN(i) s->len[(uint8_t)(i)]
#define STR(i) s->str[(uint8_t)(i)]

/* lib/SAM reads flags[255] for the end marker in three places, past the end of the table (in the
 * S&C build the high byte of a float, so the result depended on the build and on the engine's
 * state). The end marker has no flags here. */
static inline uint8_t flags_of(uint8_t i)
{
    return i < sizeof(flags) ? flags[i] : 0;
}

static inline uint8_t flags2_of(uint8_t i)
{
    return i < sizeof(flags2) ? flags2[i] : 0;
}

enum
{
    ST_DONE,        // nothing (more) to render
    ST_MAIN,        // pos48078: the next frame output
    ST_FRAME_END,   // after a frame: the next frame or the end of the chunk
    ST_PULSE,       // pos48155: count down the glottal pulse
    ST_PULSE_START, // pos48159: start a glottal pulse
    ST_UNVOICED,    // RenderSample(): unvoiced consonant, one output per bit
    ST_VOICED,      // RenderSample(): voiced consonant, one output per bit
};

void sam_voice_init(sam_voice_t *s)
{
    memset(s, 0, sizeof(*s));
    s->speed = 72;
    s->pitch = 64;
    s->mouth = 128;
    s->throat = 128;
}

/* Insert() (Code41014). lib/SAM moved the entries up to 253 and lost the 255 at 254 when the list
 * was full, and then ran past the end, mostly forever. Here a full list stops the parsing
 * (overflow, sam_voice_say() returns 0). */
SAM_VOICE_COLD static void insert(sam_voice_t *s, uint8_t position, uint8_t index, uint8_t length, uint8_t stress)
{
    if (s->count >= 254)
    {
        s->overflow = 1;
        return;
    }
    if (position <= s->count)
        s->count++;
    if (position < 254) // ML : always keep last safe-guarding 255
    {
        memmove(s->ph + position + 1, s->ph + position, 254 - position);
        memmove(s->len + position + 1, s->len + position, 254 - position);
        memmove(s->str + position + 1, s->str + position, 254 - position);
    }
    s->ph[position] = index;
    s->len[position] = length;
    s->str[position] = stress;
}

/* Parser1(): the phoneme names and stress digits of the input to ph[] and str[]. Returns 0 for
 * a character that is neither. The input ends at SAM_VOICE_END, at 0 or after 254 characters (lib/SAM
 * copied at most 254 characters, and failed at a 0). */
SAM_VOICE_COLD static int parser1(sam_voice_t *s, const uint8_t *input)
{
    uint8_t x = 0, position = 0, y;
    memset(s->str, 0, sizeof(s->str));
    for (;;)
    {
        const uint8_t sign1 = x < 254 ? input[x] : 0;
        if (sign1 == SAM_VOICE_END || sign1 == 0)
        {
            s->ph[position] = 255;
            s->count = position;
            return 1;
        }
        x++;
        const uint8_t sign2 = x < 254 ? input[x] : 0;

        // two-character names
        for (y = 0; y != 81; y++)
            if (signInputTable1[y] == sign1 && signInputTable2[y] != '*' && signInputTable2[y] == sign2)
                break;
        if (y != 81)
        {
            s->ph[position++] = y;
            x++;
            continue;
        }

        // one-character names (wildcard '*')
        for (y = 0; y != 81; y++)
            if (signInputTable2[y] == '*' && signInputTable1[y] == sign1)
                break;
        if (y != 81)
        {
            s->ph[position++] = y;
            continue;
        }

        // stress of the phoneme before
        y = 8;
        while ((sign1 != stressInputTable[y]) && (y > 0))
            y--;
        if (y == 0)
            return 0;
        STR(position - 1) = y;
    }
}

/* Parser2(): the rewrite rules, see sam.c for the list. */
SAM_VOICE_COLD static void parser2(sam_voice_t *s)
{
    uint8_t pos = 0, mem58 = 0, A, X, Y;

    for (;;)
    {
        if (s->overflow)
            return;
        X = pos;
        A = PH(pos);
        if (A == 0)
        {
            pos++;
            continue;
        }
        if (A == 255)
            return;
        Y = A;

        // <DIPHTONG ENDING WITH WX> -> <DIPHTONG ENDING WITH WX> WX
        // <DIPHTONG NOT ENDING WITH WX> -> <DIPHTONG NOT ENDING WITH WX> YX
        if ((flags_of(A) & 16) == 0)
            goto pos41457;
        mem58 = STR(pos);
        A = flags_of(Y) & 32;
        if (A == 0)
            A = 20; // 'WX'
        else
            A = 21; // 'YX'
        insert(s, pos + 1, A, 0, mem58);
        X = pos;
        goto pos41749;

    pos41457:
        // UL -> AX L
        A = PH(X);
        if (A != 78)
            goto pos41487;
        A = 24;
    pos41466:
        mem58 = STR(X);
        PH(X) = 13; // 'AX'
        insert(s, X + 1, A, 0, mem58);
        pos++;
        continue;

    pos41487:
        // UM -> AX M
        if (A != 79)
            goto pos41495;
        A = 27;
        goto pos41466;

    pos41495:
        // UN -> AX N
        if (A != 80)
            goto pos41503;
        A = 28;
        goto pos41466;

    pos41503:
        // <STRESSED VOWEL> <SILENCE> <STRESSED VOWEL> -> <STRESSED VOWEL> <SILENCE> Q <VOWEL>
        Y = A;
        A = flags_of(A) & 128;
        if (A != 0)
        {
            A = STR(X);
            if (A != 0)
            {
                X++;
                A = PH(X);
                if (A == 0)
                {
                    X++;
                    Y = PH(X);
                    if (Y == 255)
                        A = 0; // 65 & 128
                    else
                        A = flags_of(Y) & 128;
                    if (A != 0)
                    {
                        A = STR(X);
                        if (A != 0)
                        {
                            insert(s, X, 31, 0, 0); // 'Q'
                            pos++;
                            continue;
                        }
                    }
                }
            }
        }

        // T R -> CH R, D R -> J R, <VOWEL> R -> <VOWEL> RX
        X = pos;
        A = PH(pos);
        if (A != 23) // 'R'
            goto pos41611;
        X--;
        A = PH(pos - 1);
        if (A == 69) // 'T'
        {
            PH(pos - 1) = 42;
            goto pos41779;
        }
        if (A == 57) // 'D'
        {
            PH(pos - 1) = 44;
            goto pos41788;
        }
        A = flags_of(A) & 128;
        if (A != 0)
            PH(pos) = 18; // 'RX'
        pos++;
        continue;

    pos41611:
        // <VOWEL> L -> <VOWEL> LX
        if (A == 24) // 'L'
        {
            if ((flags_of(PH(pos - 1)) & 128) == 0)
            {
                pos++;
                continue;
            }
            PH(X) = 19; // 'LX'
            pos++;
            continue;
        }

        // G S -> G Z
        if (A == 32) // 'S'
        {
            if (PH(pos - 1) != 60)
            {
                pos++;
                continue;
            }
            PH(pos) = 38; // 'Z'
            pos++;
            continue;
        }

        // K <VOWEL OR DIPHTONG NOT ENDING WITH IY> -> KX <VOWEL OR DIPHTONG NOT ENDING WITH IY>
        if (A == 72) // 'K'
        {
            Y = PH(pos + 1);
            if (Y == 255)
                PH(pos) = 75; // ML : prevents an index out of bounds problem
            else
            {
                A = flags_of(Y) & 32;
                if (A == 0)
                    PH(pos) = 75; // 'KX'
            }
        }
        else
            // G <VOWEL OR DIPHTONG NOT ENDING WITH IY> -> GX <VOWEL OR DIPHTONG NOT ENDING WITH IY>
            if (A == 60) // 'G'
            {
                const uint8_t index = PH(pos + 1);
                if (index == 255)
                {
                    pos++;
                    continue;
                }
                else if ((flags_of(index) & 32) != 0)
                {
                    pos++;
                    continue;
                }
                PH(pos) = 63; // 'GX'
                pos++;
                continue;
            }

        // S P -> S B, S T -> S D, S K -> S G, S KX -> S GX
        Y = PH(pos);
        A = flags_of(Y) & 1;
        if (A == 0)
            goto pos41749;
        A = PH(pos - 1);
        if (A != 32) // 'S'
        {
            A = Y;
            goto pos41812;
        }
        PH(pos) = Y - 12;
        pos++;
        continue;

    pos41749:
        // <ALVEOLAR> UW -> <ALVEOLAR> UX
        A = PH(X);
        if (A == 53) // 'UW'
        {
            Y = PH(X - 1);
            A = flags2_of(Y) & 4;
            if (A == 0)
            {
                pos++;
                continue;
            }
            PH(X) = 16;
            pos++;
            continue;
        }

    pos41779:
        // CH -> CH CH'
        if (A == 42)
        {
            insert(s, X + 1, A + 1, 0, STR(X));
            pos++;
            continue;
        }

    pos41788:
        // J -> J J'
        if (A == 44)
        {
            insert(s, X + 1, A + 1, 0, STR(X));
            pos++;
            continue;
        }

    pos41812:
        // <UNSTRESSED VOWEL> T <PAUSE> -> <UNSTRESSED VOWEL> DX <PAUSE>
        // <UNSTRESSED VOWEL> D <PAUSE> -> <UNSTRESSED VOWEL> DX <PAUSE>
        if (A != 69)    // 'T'
            if (A != 57) // 'D'
            {
                pos++;
                continue;
            }
        if ((flags_of(PH(X - 1)) & 128) == 0)
        {
            pos++;
            continue;
        }
        X++;
        A = PH(X);
        if (A != 0)
        {
            if ((flags_of(A) & 128) == 0)
            {
                pos++;
                continue;
            }
            if (STR(X) != 0)
            {
                pos++;
                continue;
            }
            PH(pos) = 30; // 'DX'
        }
        else
        {
            A = PH(X + 1);
            if (A == 255)
                A = 0; // 65 & 128
            else
                A = flags_of(A) & 128;
            if (A != 0)
                PH(pos) = 30; // 'DX'
        }
        pos++;
    }
}

/* CopyStress(): a voiced consonant before a stressed vowel gets the vowel's stress + 1. */
SAM_VOICE_COLD static void copy_stress(sam_voice_t *s)
{
    uint8_t pos = 0, Y;
    for (;;)
    {
        Y = PH(pos);
        if (Y == 255)
            return;
        if ((flags_of(Y) & 64) == 0)
        {
            pos++;
            continue;
        }
        Y = PH(pos + 1);
        if (Y == 255)
        {
            pos++;
            continue;
        }
        else if ((flags_of(Y) & 128) == 0)
        {
            pos++;
            continue;
        }
        Y = STR(pos + 1);
        if (Y == 0)
        {
            pos++;
            continue;
        }
        if ((Y & 128) != 0)
        {
            pos++;
            continue;
        }
        STR(pos) = Y + 1;
        pos++;
    }
}

/* SetPhonemeLength(): the length from the stressed or the unstressed length table. */
SAM_VOICE_COLD static void set_phoneme_length(sam_voice_t *s)
{
    for (int position = 0; position < 256 && s->ph[position] != 255; position++)
    {
        const uint8_t index = s->ph[position];
        if (index >= sizeof(phonemeLengthTable))
            continue; // UL, UM, UN: Parser2() replaced them
        const uint8_t A = s->str[position];
        if ((A == 0) || ((A & 128) != 0))
            s->len[position] = phonemeLengthTable[index];
        else
            s->len[position] = phonemeStressedLengthTable[index];
    }
}

/* Code41240(): the plosives B D G K P T get their two following phonemes (index + 1, + 2). */
SAM_VOICE_COLD static void code41240(sam_voice_t *s)
{
    uint8_t pos = 0;
    while (PH(pos) != 255 && !s->overflow)
    {
        uint8_t X = pos, A;
        const uint8_t index = PH(pos);
        if ((flags_of(index) & 2) == 0)
        {
            pos++;
            continue;
        }
        else if ((flags_of(index) & 1) == 0)
        {
            insert(s, pos + 1, index + 1, phonemeLengthTable[index + 1], STR(pos));
            insert(s, pos + 2, index + 2, phonemeLengthTable[index + 2], STR(pos));
            pos += 3;
            continue;
        }

        do
        {
            X++;
            A = PH(X);
        } while (A == 0);

        if (A != 255)
        {
            if ((flags_of(A) & 8) != 0)
            {
                pos++;
                continue;
            }
            if ((A == 36) || (A == 37)) // '/H' '/X'
            {
                pos++;
                continue;
            }
        }

        insert(s, pos + 1, index + 1, phonemeLengthTable[index + 1], STR(pos));
        insert(s, pos + 2, index + 2, phonemeLengthTable[index + 2], STR(pos));
        pos += 3;
    }
    // SAMMain() then cut the list at the first index > 80, starting where this loop left X. The
    // rules produce no such index (77 at most), so that never cut anything.
}

/* AdjustLengths(): see sam.c for the rules. */
SAM_VOICE_COLD static void adjust_lengths(sam_voice_t *s)
{
    uint8_t X = 0, index, loopIndex = 0, A, mem56;

    // lengthen the voiced phonemes and fricatives between a vowel and punctuation by 1.5
    for (;;)
    {
        index = PH(X);
        if (index == 255)
            break;
        if ((flags2_of(index) & 1) == 0)
        {
            X++;
            continue;
        }
        loopIndex = X;
    pos48644:
        X--;
        if (X == 0)
            break;
        index = PH(X);
        if (index != 255)
            if ((flags_of(index) & 128) == 0)
                goto pos48644;
        do
        {
            index = PH(X);
            if (index != 255)
                if (((flags2_of(index) & 32) == 0) || ((flags_of(index) & 4) != 0))
                {
                    A = LEN(X);
                    A = (A >> 1) + A + 1;
                    LEN(X) = A;
                }
            X++;
        } while (X != loopIndex);
        X++;
    }

    loopIndex = 0;
    for (;;)
    {
        X = loopIndex;
        index = PH(X);
        if (index == 255)
            return;

        A = flags_of(index) & 128;
        if (A != 0)
        {
            X++;
            index = PH(X);
            if (index == 255)
                mem56 = 65;
            else
                mem56 = flags_of(index);

            if ((flags_of(index) & 64) == 0)
            {
                if ((index == 18) || (index == 19)) // 'RX' & 'LX'
                {
                    X++;
                    index = PH(X);
                    // <VOWEL> <RX | LX> <CONSONANT>: vowel - 1
                    if ((flags_of(index) & 64) != 0)
                        LEN(loopIndex)--;
                    loopIndex++;
                    continue;
                }
                loopIndex++;
                continue;
            }

            if ((mem56 & 4) == 0)
            {
                if ((mem56 & 1) == 0)
                {
                    loopIndex++;
                    continue;
                }
                // <VOWEL> <UNVOICED PLOSIVE>: vowel - 1/8
                X--;
                mem56 = LEN(X) >> 3;
                LEN(X) -= mem56;
                loopIndex++;
                continue;
            }

            // <VOWEL> <VOICED CONSONANT>: vowel * 5/4 + 1
            A = LEN(X - 1);
            LEN(X - 1) = (A >> 2) + A + 1;
            loopIndex++;
            continue;
        }

        // <NASAL> <STOP CONSONANT>: nasal = 5, consonant = 6
        if ((flags2_of(index) & 8) != 0)
        {
            X++;
            index = PH(X);
            if (index == 255)
                A = 0; // 65 & 2
            else
                A = flags_of(index) & 2;
            if (A != 0)
            {
                LEN(X) = 6;
                LEN(X - 1) = 5;
            }
            loopIndex++;
            continue;
        }

        // <VOICED STOP CONSONANT> {optional silence} <STOP CONSONANT>: both length / 2 + 1
        if ((flags_of(index) & 2) != 0)
        {
            do
            {
                X++;
                index = PH(X);
            } while (index == 0);

            if (index == 255 || (flags_of(index) & 2) == 0) // lib/SAM: 65 & 2 for the end marker
            {
                loopIndex++;
                continue;
            }
            LEN(X) = (LEN(X) >> 1) + 1;
            LEN(loopIndex) = (LEN(loopIndex) >> 1) + 1;
            loopIndex++;
            continue;
        }

        // <LIQUID CONSONANT> after a stop consonant: - 2
        if ((flags2_of(index) & 16) != 0)
        {
            index = PH(X - 1);
            if ((flags_of(index) & 2) != 0)
                LEN(X) -= 2;
        }
        loopIndex++;
    }
}

/* InsertBreath(): a chunk marker (254) after each punctuation, and at the last pause before 232
 * frames are reached. Without a pause since the last marker, lib/SAM went back to that pause again
 * and again, forever; here the marker goes before the phoneme that reaches the 232 frames. */
SAM_VOICE_COLD static void insert_breath(sam_voice_t *s)
{
    uint8_t mem54 = 255, mem55 = 0, mem66 = 0, X, index;
    int breath = -1; // where the last marker went
    for (;;)
    {
        if (s->overflow)
            return;
        X = mem66;
        index = PH(X);
        if (index == 255)
            return;
        mem55 += LEN(X);

        if (mem55 < 232)
        {
            if (index != 254) // ML : Prevents an index out of bounds problem
            {
                if ((flags2_of(index) & 1) != 0)
                {
                    X++;
                    mem55 = 0;
                    insert(s, X, 254, 0, 0);
                    breath = X;
                    mem66++;
                    mem66++;
                    continue;
                }
            }
            if (index == 0)
                mem54 = X;
            mem66++;
            continue;
        }

        if (mem54 == 255 || mem54 < breath)
        {
            // no pause since the last marker (lib/SAM: endless loop): the marker before this
            // phoneme, or after it if it is the first one since the last marker
            if (X == breath + 1)
                X++;
            insert(s, X, 254, 0, 0);
            breath = X;
            mem55 = 0;
            mem66 = X + 1;
            continue;
        }
        X = mem54;
        PH(X) = 31; // 'Q*' glottal stop
        LEN(X) = 4;
        STR(X) = 0;
        X++;
        mem55 = 0;
        insert(s, X, 254, 0, 0);
        breath = X;
        X++;
        mem66 = X;
    }
}

/* trans() of render.c: the mouth/throat scaling, (m * f) >> 7 with the lowest bit 0 */
static inline uint8_t trans(uint8_t m, uint8_t f)
{
    return (uint8_t)((((unsigned)m * f) >> 8) << 1);
}

/* F1 and F2 of a phoneme: SetMouthThroat() scaled those of the voiced phonemes in freq1data[] and
 * freq2data[] in place. */
static void sam_voice_formants(const sam_voice_t *s, uint8_t index, uint8_t *f1, uint8_t *f2)
{
    if (index >= 5 && index < 30)
    {
        *f1 = trans(s->mouth, mouthFormants5_29[index]);
        *f2 = trans(s->throat, throatFormants5_29[index]);
    }
    else if (index >= 48 && index < 54)
    {
        *f1 = trans(s->mouth, mouthFormants48_53[index - 48]);
        *f2 = trans(s->throat, throatFormants48_53[index - 48]);
    }
    else
    {
        *f1 = freq1data[index];
        *f2 = freq2data[index];
    }
}

/* AddInflection(): a rising (inflection 1) or falling (255) pitch over the 30 frames before frame x.
 * Returns the last pitch it wrote: lib/SAM left it in the register A, which Render() tested next. */
SAM_VOICE_COLD static uint8_t add_inflection(sam_voice_t *s, uint8_t X, uint8_t inflection)
{
    uint8_t *pitches = s->frame[0];
    const uint8_t mem49 = X;
    uint8_t A, phase1;

    A = X <= 30 ? 0 : X - 30;
    X = A;
    // ML : A =, fixes a problem with invalid pitch with '.'
    // (lib/SAM looped forever when all 256 pitches were 127, the engine's default pitch)
    for (int n = 0; (A = pitches[X]) == 127 && n < 256; n++)
        X++;
    for (;;)
    {
        A += inflection;
        phase1 = A;
        pitches[X] = A;
        do
        {
            X++;
            if (X == mem49)
                return A;
        } while (pitches[X] == 255);
        A = phase1;
    }
}

/* Render() up to its output loop: the frames of the chunk at ph[c] (ended by 255), their
 * transitions, the pitch contour. Returns 0 if the chunk is empty. */
SAM_VOICE_COLD static int render_frames(sam_voice_t *s, uint8_t c)
{
    const uint8_t *phonemeIndexOutput = s->ph + c;
    const uint8_t *phonemeLengthOutput = s->len + c;
    const uint8_t *stressOutput = s->str + c;
    uint8_t *pitches = s->frame[0];
    uint8_t phase1 = 0, phase2, phase3, mem38, mem40, mem44, mem48, mem49, mem50, mem51, mem53, mem56;
    uint8_t speedcounter, A, X, Y;

    if (phonemeIndexOutput[0] == 255)
        return 0;

    // CREATE FRAMES: each phoneme <length> times
    X = 0;
    mem44 = 0;
    do
    {
        Y = mem44;
        A = phonemeIndexOutput[mem44];
        mem56 = A;
        if (A == 255)
            break;
        if (A == 1) // period: rising inflection
            A = add_inflection(s, X, 1);
        if (A == 2) // question mark: falling inflection (and after a period that ended on pitch 2)
            add_inflection(s, X, 255);

        phase1 = tab47492[stressOutput[Y] + 1];
        phase2 = phonemeLengthOutput[Y];
        Y = mem56;

        uint8_t f1, f2;
        sam_voice_formants(s, Y, &f1, &f2);
        do
        {
            s->frame[1][X] = f1;
            s->frame[2][X] = f2;
            s->frame[3][X] = freq3data[Y];
            s->frame[4][X] = ampl1data[Y];
            s->frame[5][X] = ampl2data[Y];
            s->frame[6][X] = ampl3data[Y];
            s->cons[X] = sampledConsonantFlags[Y];
            pitches[X] = s->pitch + phase1;
            X++;
            phase2--;
        } while (phase2 != 0);
        mem44++;
    } while (mem44 != 0);

    // CREATE TRANSITIONS: linear from the end of one phoneme into the next, the pitch from the
    // middle of one phoneme to the middle of the next. See render.c for the details.
    mem44 = 0;
    mem49 = 0;
    X = 0;
    for (;;)
    {
        Y = phonemeIndexOutput[X];
        A = phonemeIndexOutput[X + 1];
        X++;
        if (A == 255)
            break;

        X = A;
        mem56 = blendRank[A];
        A = blendRank[Y];
        if (A == mem56)
        {
            phase1 = outBlendLength[Y];
            phase2 = outBlendLength[X];
        }
        else if (A < mem56)
        {
            phase1 = inBlendLength[X];
            phase2 = outBlendLength[X];
        }
        else
        {
            phase1 = outBlendLength[Y];
            phase2 = inBlendLength[Y];
        }

        Y = mem44;
        A = mem49 + phonemeLengthOutput[mem44];
        mem49 = A;
        A = A + phase2;
        speedcounter = A;
        phase3 = mem49 - phase1;
        A = phase1 + phase2;
        mem38 = A;

        X = A;
        X -= 2;
        if ((X & 128) == 0)
            for (int table = 0; table < 7; table++) // mem47 168 .. 174
            {
                uint8_t *t = s->frame[table];
                mem40 = mem38;
                if (table == 0) // pitch
                {
                    uint8_t mem36 = phonemeLengthOutput[mem44] >> 1;
                    uint8_t mem37 = phonemeLengthOutput[mem44 + 1] >> 1;
                    mem40 = mem36 + mem37;
                    mem37 += mem49;
                    mem36 = mem49 - mem36;
                    A = t[mem37];
                    mem53 = A - t[mem36];
                }
                else
                {
                    A = t[speedcounter];
                    mem53 = A - t[phase3];
                }

                // change per frame, remainder and sign. A division by 0 gives 0 on the
                // Cortex-M (no trap), and the remainder (a - q * b) the dividend.
                const signed char m53 = (signed char)mem53;
                mem50 = mem53 & 128;
                const uint8_t m53abs = abs(m53);
                if (mem40 != 0)
                {
                    mem51 = m53abs % mem40;
                    mem53 = (uint8_t)(m53 / mem40);
                }
                else
                {
                    mem51 = m53abs;
                    mem53 = 0;
                }

                X = mem40;
                Y = phase3;
                mem56 = 0;
                for (;;)
                {
                    A = t[Y] + mem53;
                    mem48 = A;
                    Y++;
                    X--;
                    if (X == 0)
                        break;
                    mem56 += mem51;
                    if (mem56 >= mem40)
                    {
                        mem56 -= mem40;
                        if ((mem50 & 128) == 0)
                        {
                            if (mem48 != 0)
                                mem48++;
                        }
                        else
                            mem48--;
                    }
                    t[Y] = mem48;
                }
            }
        mem44++;
        X = mem44;
    }

    // the frames
    mem48 = mem49 + phonemeLengthOutput[mem44];

    // ASSIGN PITCH CONTOUR: pitch - F1 / 2 (all 256 frames, like lib/SAM)
    if (!s->sing)
        for (int i = 0; i < 256; i++)
            pitches[i] -= (s->frame[1][i] >> 1);

    // RESCALE AMPLITUDE: linear to dB (all 256 frames, like lib/SAM)
    for (int i = 0; i < 256; i++)
    {
        s->frame[4][i] = amplitudeRescale[s->frame[4][i]];
        s->frame[5][i] = amplitudeRescale[s->frame[5][i]];
        s->frame[6][i] = amplitudeRescale[s->frame[6][i]];
    }

    // the output loop starts
    s->frames = mem48;
    s->y = 0;
    s->phase1 = s->phase2 = s->phase3 = 0;
    s->speedcounter = 72; // sam standard speed
    A = pitches[0];
    s->glottal = A;
    s->mem38 = A - (A >> 2);
    s->voiced_pos = 0;
    s->state = ST_MAIN;
    return 1;
}

/* PrepareOutput(): the phonemes from ph[next_chunk] up to the next 254 (breath) or 255 (end),
 * without the pauses, are a chunk for Render(). They are moved together in place (lib/SAM copied
 * them to phonemeIndexOutput[60] and the others, without a bound). Returns 0 when there is no
 * chunk left. */
SAM_VOICE_COLD static int start_chunk(sam_voice_t *s)
{
    while (s->more)
    {
        const uint8_t c = s->next_chunk;
        uint8_t x = c, w = c, a;
        for (;;)
        {
            a = s->ph[x];
            if (a == 255 || a == 254)
                break;
            if (a != 0)
            {
                s->ph[w] = a;
                s->len[w] = s->len[x];
                s->str[w] = s->str[x];
                w++;
            }
            if (++x == 0)
            {
                a = 255; // no end marker (lib/SAM: endless loop)
                break;
            }
        }
        s->ph[w] = 255;
        s->more = a == 254 && x != 255;
        s->next_chunk = x + 1;
        if (render_frames(s, c))
            return 1;
    }
    return 0;
}

SAM_VOICE_COLD int sam_voice_say(sam_voice_t *s, const char *phonemes)
{
    // Init(), and the state of the previous utterance (lib/SAM kept the frames of the last
    // utterance and the last timetable index, which could leak into the next one)
    memset(s->str, 0, sizeof(s->str));
    memset(s->len, 0, sizeof(s->len));
    memset(s->frame, 0, sizeof(s->frame));
    memset(s->cons, 0, sizeof(s->cons));
    memset(s->ring, SAM_VOICE_SILENCE, sizeof(s->ring));
    s->ph[255] = 32; // SAMMain(): to prevent buffer overflow
    s->state = ST_DONE;
    s->more = 0;
    s->last_kind = 0;
    s->bufferpos = 0;
    s->out_pos = 0;
    s->emitted = 0;
    s->written = 0;

    s->overflow = 0;
    if (!parser1(s, (const uint8_t *)phonemes))
        return 0;
    parser2(s);
    copy_stress(s);
    set_phoneme_length(s);
    adjust_lengths(s);
    code41240(s);
    insert_breath(s);
    if (s->overflow)
        return 0;

    s->next_chunk = 0;
    s->more = 1;
    start_chunk(s);
    return 1;
}

/* Output8Bit()/Output8BitAry(): 5 samples from the new position on */
static inline void output(sam_voice_t *s, uint8_t kind, const uint8_t *v)
{
    s->bufferpos += timetable[s->last_kind][kind];
    s->last_kind = kind;
    const uint32_t p = s->bufferpos / 50;
    for (int k = 0; k < 5; k++)
        s->ring[(p + k) & 15] = v[k];
    s->out_pos = p;
    s->written = p + 5;
}

static inline void output1(sam_voice_t *s, uint8_t kind, uint8_t value)
{
    const uint8_t v[5] = {value, value, value, value, value};
    output(s, kind, v);
}

/* RenderSample(): the consonant's bits from sampleTable */
static void sample_start(sam_voice_t *s)
{
    const uint8_t x = (s->mem39 & 7) - 1;
    s->sample_y = s->y;
    s->sample_table = x;
    s->sample_off = tab48426[x];
    s->sample_bits = 8;
    if (s->mem39 & 248)
    {
        s->sample_pos = (s->mem39 & 248) ^ 255;
        s->state = ST_UNVOICED;
    }
    else
    {
        s->sample_count = (s->frame[0][s->y] >> 4) ^ 255;
        s->sample_pos = s->voiced_pos;
        s->state = ST_VOICED;
    }
    s->sample_byte = sampleTable[s->sample_table * 256 + s->sample_pos];
}

/* Render()'s output loop up to and including its next output. Returns 0 at the end of the chunk. */
static int step(sam_voice_t *s)
{
    for (;;)
    {
        switch (s->state)
        {
        case ST_MAIN:
        {
            const uint8_t y = s->y;
            s->mem39 = s->cons[y];
            if (s->mem39 & 248)
            {
                // unvoiced sampled phoneme
                sample_start(s);
                continue;
            }

            // the glottal pulse and the formants: F1 and F2 sine, F3 square
            const int a1 = s->frame[4][y] & 15, a2 = s->frame[5][y] & 15, a3 = s->frame[6][y] & 15;
            const unsigned f1 = s->frame[1][y] * 64, f2 = s->frame[2][y] * 64, f3 = s->frame[3][y] * 64;
            unsigned p1 = s->phase1 * 256, p2 = s->phase2 * 256, p3 = s->phase3 * 256;
            uint8_t v[5];
            for (int k = 0; k < 5; k++)
            {
                const int rect = (p3 & 0x8000) ? 112 : -112; // rectangle[] of render.c
                int mux = sinus[(p1 >> 8) & 255] * a1 + sinus[(p2 >> 8) & 255] * a2 + rect * a3;
                mux /= 32;
                v[k] = (uint8_t)(mux + 128);
                p1 += f1;
                p2 += f2;
                p3 += f3;
            }
            output(s, 0, v);

            if (--s->speedcounter != 0)
            {
                s->state = ST_PULSE;
                return 1;
            }
            s->y++;
            s->frames--;
            s->state = ST_FRAME_END;
            return 1;
        }

        case ST_FRAME_END:
            if (s->frames == 0)
            {
                s->state = ST_DONE;
                return 0;
            }
            s->speedcounter = s->speed;
            /* fall through */

        case ST_PULSE:
            if (--s->glottal == 0)
            {
                s->state = ST_PULSE_START;
                continue;
            }
            if (--s->mem38 != 0 || s->mem39 == 0)
            {
                // the formants go on
                const uint8_t y = s->y;
                s->phase1 += s->frame[1][y];
                s->phase2 += s->frame[2][y];
                s->phase3 += s->frame[3][y];
                s->state = ST_MAIN;
                continue;
            }
            // voiced sampled phoneme: the sample in the last quarter of the pulse
            sample_start(s);
            continue;

        case ST_PULSE_START:
        {
            const uint8_t a = s->frame[0][s->y];
            s->glottal = a;
            s->mem38 = a - (a >> 2);
            s->phase1 = s->phase2 = s->phase3 = 0;
            s->state = ST_MAIN;
            continue;
        }

        case ST_UNVOICED:
        {
            // 0 bit: tab48426[] (never 0, so no second output), 1 bit: 5
            if (s->sample_byte & 128)
                output1(s, 2, 5 * 16);
            else
                output1(s, 1, (s->sample_off & 15) * 16);
            s->sample_byte <<= 1;
            if (--s->sample_bits == 0)
            {
                s->sample_bits = 8;
                if (++s->sample_pos == 0)
                {
                    // two frames. lib/SAM went from 1 frame left to 255 and on through the frames
                    // of earlier chunks, often forever; here the chunk ends.
                    s->glottal = 1;
                    s->y = s->sample_y + 2;
                    s->frames = s->frames > 2 ? s->frames - 2 : 0;
                    s->state = ST_FRAME_END;
                    return 1;
                }
                s->sample_byte = sampleTable[s->sample_table * 256 + s->sample_pos];
            }
            return 1;
        }

        case ST_VOICED:
        {
            if (s->sample_byte & 128)
                output1(s, 3, (26 & 15) * 16);
            else
                output1(s, 4, (6 & 15) * 16);
            s->sample_byte <<= 1;
            if (--s->sample_bits == 0)
            {
                s->sample_bits = 8;
                s->sample_pos++;
                if (++s->sample_count == 0)
                {
                    s->voiced_pos = s->sample_pos;
                    s->state = ST_PULSE_START;
                    return 1;
                }
                s->sample_byte = sampleTable[s->sample_table * 256 + s->sample_pos];
            }
            return 1;
        }

        default:
            return 0;
        }
    }
}

int sam_voice_render(sam_voice_t *s, uint8_t *out, int n)
{
    int k = 0;
    for (;;)
    {
        // the samples before the last output's position are final, at the end all of them
        const uint32_t end = s->state == ST_DONE ? s->written : s->out_pos;
        while (s->emitted < end && k < n)
            out[k++] = s->ring[s->emitted++ & 15];
        if (k == n || s->state == ST_DONE)
            return k;
        if (!step(s) && !start_chunk(s))
            s->state = ST_DONE;
    }
}
