/* sam_voice's reciter: English text to SAM phonemes, see sam.h.
 *
 * lib/SAM/reciter.c's TextToPhonemes(), with the 6502 registers A, X, Y and the two buffers as
 * locals instead of globals, and the text copied in as lib/SAM's callers did. The labels are
 * reciter.c's.
 */
#include <string.h>

#include "sam.h"
#include "sam_reciter_tables.h"

static unsigned char GetRuleByte(unsigned short mem62, unsigned char Y)
{
    unsigned int address = mem62;
    if (mem62 >= 37541)
    {
        address -= 37541;
        return rules2[address + Y];
    }
    address -= 32000;
    return rules[address + Y];
}

// Code37055 and Code37066: the flags of the character before / after a position
#define CODE37055(m)                    \
    do                                  \
    {                                   \
        X = (unsigned char)((m) - 1);   \
        A = inputtemp[X];               \
        Y = A;                          \
        A = tab36376[Y];                \
    } while (0)
#define CODE37066(m)                    \
    do                                  \
    {                                   \
        X = (unsigned char)((m) + 1);   \
        A = inputtemp[X];               \
        Y = A;                          \
        A = tab36376[Y];                \
    } while (0)

/* TextToPhonemes(): input is the text ending with '[', output (the same buffer) the phonemes ending
 * with 155. Returns 0 for a character without a rule. */
SAM_VOICE_COLD static int sam_voice_reciter(unsigned char *input)
{
    unsigned char inputtemp[256]; // secure copy of input tab36096
    unsigned char A, X, Y;
    unsigned char mem56; // output position for phonemes
    unsigned char mem57;
    unsigned char mem58;
    unsigned char mem59;
    unsigned char mem60;
    unsigned char mem61;
    unsigned short mem62; // memory position of current rule
    unsigned char mem64;  // position of '=' or current character
    unsigned char mem65;  // position of ')'
    unsigned char mem66;  // position of '('

    inputtemp[0] = 32;

    // secure copy of input, upper case
    X = 1;
    Y = 0;
    do
    {
        A = input[Y] & 127;
        if (A >= 112)
            A = A & 95;
        else if (A >= 96)
            A = A & 79;
        inputtemp[X] = A;
        X++;
        Y++;
    } while (Y != 255);

    X = 255;
    inputtemp[X] = 27;
    mem61 = 255;

    mem56 = 255;

pos36554:
    while (1)
    {
        mem61++;
        X = mem61;
        A = inputtemp[X];
        mem64 = A;
        if (A == '[')
        {
            mem56++;
            X = mem56;
            input[X] = 155;
            return 1;
        }

        if (A != '.')
            break;
        X++;
        Y = inputtemp[X];
        A = tab36376[Y] & 1;
        if (A != 0)
            break;
        mem56++;
        X = mem56;
        A = '.';
        input[X] = '.';
    }

    A = mem64;
    Y = A;
    A = tab36376[A];
    mem57 = A;
    if ((A & 2) != 0)
    {
        mem62 = 37541;
        goto pos36700;
    }

    A = mem57;
    if (A != 0)
        goto pos36677;
    A = 32;
    inputtemp[X] = ' ';
    mem56++;
    X = mem56;
    if (X > 120)
    {
        // pos36654: the output is full
        input[X] = 155;
        return 1;
    }
    input[X] = A;
    goto pos36554;

pos36677:
    A = mem57 & 128;
    if (A == 0)
        return 0;

    // go to the right rules for this character.
    X = mem64 - 'A';
    mem62 = tab37489[X] | (tab37515[X] << 8);

    // -------------------------------------
    // go to next rule
    // -------------------------------------
pos36700:
    // find next rule
    Y = 0;
    do
    {
        mem62 += 1;
        A = GetRuleByte(mem62, Y);
    } while ((A & 128) == 0);
    Y++;

    // find '('
    while (1)
    {
        A = GetRuleByte(mem62, Y);
        if (A == '(')
            break;
        Y++;
    }
    mem66 = Y;

    // find ')'
    do
    {
        Y++;
        A = GetRuleByte(mem62, Y);
    } while (A != ')');
    mem65 = Y;

    // find '='
    do
    {
        Y++;
        A = GetRuleByte(mem62, Y);
        A = A & 127;
    } while (A != '=');
    mem64 = Y;

    X = mem61;
    mem60 = X;

    // compare the string within the bracket
    Y = mem66;
    Y++;
    while (1)
    {
        mem57 = inputtemp[X];
        A = GetRuleByte(mem62, Y);
        if (A != mem57)
            goto pos36700;
        Y++;
        if (Y == mem65)
            break;
        X++;
        mem60 = X;
    }

    // the string in the bracket is correct: check the left context
    A = mem61;
    mem59 = mem61;

pos36791:
    while (1)
    {
        mem66--;
        Y = mem66;
        A = GetRuleByte(mem62, Y);
        mem57 = A;
        if ((A & 128) != 0)
            goto pos37180;
        X = A & 127;
        A = tab36376[X] & 128;
        if (A == 0)
            break;
        X = mem59 - 1;
        A = inputtemp[X];
        if (A != mem57)
            goto pos36700;
        mem59 = X;
    }

    A = mem57;
    if (A == ' ')
        goto pos36895;
    if (A == '#')
        goto pos36910;
    if (A == '.')
        goto pos36920;
    if (A == '&')
        goto pos36935;
    if (A == '@')
        goto pos36967;
    if (A == '^')
        goto pos37004;
    if (A == '+')
        goto pos37019;
    if (A == ':')
        goto pos37040;
    return 0;

pos36895:
    CODE37055(mem59);
    A = A & 128;
    if (A != 0)
        goto pos36700;
pos36905:
    mem59 = X;
    goto pos36791;

pos36910:
    CODE37055(mem59);
    A = A & 64;
    if (A != 0)
        goto pos36905;
    goto pos36700;

pos36920:
    CODE37055(mem59);
    A = A & 8;
    if (A == 0)
        goto pos36700;
pos36930:
    mem59 = X;
    goto pos36791;

pos36935:
    CODE37055(mem59);
    A = A & 16;
    if (A != 0)
        goto pos36930;
    A = inputtemp[X];
    if (A != 72)
        goto pos36700;
    X--;
    A = inputtemp[X];
    if ((A == 67) || (A == 83))
        goto pos36930;
    goto pos36700;

pos36967:
    CODE37055(mem59);
    A = A & 4;
    if (A != 0)
        goto pos36930;
    A = inputtemp[X];
    if (A != 72)
        goto pos36700;
    if ((A != 84) && (A != 67) && (A != 83))
        goto pos36700;
    mem59 = X;
    goto pos36791;

pos37004:
    CODE37055(mem59);
    A = A & 32;
    if (A == 0)
        goto pos36700;
pos37014:
    mem59 = X;
    goto pos36791;

pos37019:
    X = mem59;
    X--;
    A = inputtemp[X];
    if ((A == 'E') || (A == 'I') || (A == 'Y'))
        goto pos37014;
    goto pos36700;

pos37040:
    CODE37055(mem59);
    A = A & 32;
    if (A == 0)
        goto pos36791;
    mem59 = X;
    goto pos37040;

    //---------------------------------------
    // the right context

pos37077:
    X = mem58 + 1;
    A = inputtemp[X];
    if (A != 'E')
        goto pos37157;
    X++;
    Y = inputtemp[X];
    X--;
    A = tab36376[Y] & 128;
    if (A == 0)
        goto pos37108;
    X++;
    A = inputtemp[X];
    if (A != 'R')
        goto pos37113;
pos37108:
    mem58 = X;
    goto pos37184;
pos37113:
    if ((A == 83) || (A == 68)) // 'S' 'D'
        goto pos37108;
    if (A != 76) // 'L'
        goto pos37135;
    X++;
    A = inputtemp[X];
    if (A != 89)
        goto pos36700;
    goto pos37108;

pos37135:
    if (A != 70)
        goto pos36700;
    X++;
    A = inputtemp[X];
    if (A != 85)
        goto pos36700;
    X++;
    A = inputtemp[X];
    if (A == 76)
        goto pos37108;
    goto pos36700;

pos37157:
    if (A != 73)
        goto pos36700;
    X++;
    A = inputtemp[X];
    if (A != 78)
        goto pos36700;
    X++;
    A = inputtemp[X];
    if (A == 71)
        goto pos37108;
    goto pos36700;

pos37180:
    A = mem60;
    mem58 = A;

pos37184:
    Y = mem65 + 1;
    if (Y == mem64)
        goto pos37455;
    mem65 = Y;
    A = GetRuleByte(mem62, Y);
    mem57 = A;
    X = A;
    A = tab36376[X] & 128;
    if (A == 0)
        goto pos37226;
    X = mem58 + 1;
    A = inputtemp[X];
    if (A != mem57)
        goto pos36700;
    mem58 = X;
    goto pos37184;
pos37226:
    A = mem57;
    if (A == 32)
        goto pos37295; // ' '
    if (A == 35)
        goto pos37310; // '#'
    if (A == 46)
        goto pos37320; // '.'
    if (A == 38)
        goto pos37335; // '&'
    if (A == 64)
        goto pos37367; // '@'
    if (A == 94)
        goto pos37404; // '^'
    if (A == 43)
        goto pos37419; // '+'
    if (A == 58)
        goto pos37440; // ':'
    if (A == 37)
        goto pos37077; // '%'
    return 0;

pos37295:
    CODE37066(mem58);
    A = A & 128;
    if (A != 0)
        goto pos36700;
pos37305:
    mem58 = X;
    goto pos37184;

pos37310:
    CODE37066(mem58);
    A = A & 64;
    if (A != 0)
        goto pos37305;
    goto pos36700;

pos37320:
    CODE37066(mem58);
    A = A & 8;
    if (A == 0)
        goto pos36700;
pos37330:
    mem58 = X;
    goto pos37184;

pos37335:
    CODE37066(mem58);
    A = A & 16;
    if (A != 0)
        goto pos37330;
    A = inputtemp[X];
    if (A != 72)
        goto pos36700;
    X++;
    A = inputtemp[X];
    if ((A == 67) || (A == 83))
        goto pos37330;
    goto pos36700;

pos37367:
    CODE37066(mem58);
    A = A & 4;
    if (A != 0)
        goto pos37330;
    A = inputtemp[X];
    if (A != 72)
        goto pos36700;
    if ((A != 84) && (A != 67) && (A != 83))
        goto pos36700;
    mem58 = X;
    goto pos37184;

pos37404:
    CODE37066(mem58);
    A = A & 32;
    if (A == 0)
        goto pos36700;
pos37414:
    mem58 = X;
    goto pos37184;

pos37419:
    X = mem58;
    X++;
    A = inputtemp[X];
    if ((A == 69) || (A == 73) || (A == 89))
        goto pos37414;
    goto pos36700;

pos37440:
    CODE37066(mem58);
    A = A & 32;
    if (A == 0)
        goto pos37184;
    mem58 = X;
    goto pos37440;

pos37455:
    // the rule matches: its phonemes to the output
    Y = mem64;
    mem61 = mem60;

pos37461:
    A = GetRuleByte(mem62, Y);
    mem57 = A;
    A = A & 127;
    if (A != '=')
    {
        mem56++;
        X = mem56;
        input[X] = A;
    }
    if ((mem57 & 128) == 0)
        goto pos37485;
    goto pos36554;
pos37485:
    Y++;
    goto pos37461;
}

SAM_VOICE_COLD int sam_voice_text_to_phonemes(const char *text, char *out, int size)
{
    unsigned char buf[256];
    int i, n;

    memset(buf, 0, sizeof(buf));
    for (i = 0; i < 253 && text[i] != 0 && text[i] != '['; i++)
        buf[i] = text[i];
    buf[i] = '[';

    if (!sam_voice_reciter(buf))
        return -1;

    for (n = 0; n < 256 && buf[n] != SAM_VOICE_END; n++)
        ;
    if (n == 256 || n + 2 > size)
        return -1;
    memcpy(out, buf, n + 1);
    out[n + 1] = 0;
    return n + 1;
}
