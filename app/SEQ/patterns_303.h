#include <inttypes.h>

constexpr uint8_t REST = 0x0;

/* between 0x0 and 0xA, the VCO voltage pins, so these notes arent really
 * 'effective' in that they all sound the same.
 */

// lowest octave
constexpr uint8_t C1 = 0x0B;
constexpr uint8_t C1_SHARP = 0x0C;
constexpr uint8_t D1 = 0x0D;
constexpr uint8_t D1_SHARP = 0x0E;
constexpr uint8_t E1 = 0x0F;
constexpr uint8_t F1 = 0x10;
constexpr uint8_t F1_SHARP = 0x11;
constexpr uint8_t G1 = 0x12;
constexpr uint8_t G1_SHARP = 0x13;

// middle octave
constexpr uint8_t A1 = 0x14;
constexpr uint8_t A1_SHARP = 0x15;
constexpr uint8_t B1 = 0x16;
constexpr uint8_t C2 = 0x17;
constexpr uint8_t C2_SHARP = 0x18;
constexpr uint8_t D2 = 0x19;
constexpr uint8_t D2_SHARP = 0x1A;
constexpr uint8_t E2 = 0x1B;
constexpr uint8_t F2 = 0x1C;
constexpr uint8_t F2_SHARP = 0x1D;
constexpr uint8_t G2 = 0x1E;
constexpr uint8_t G2_SHARP = 0x1F;

// high octave
constexpr uint8_t A2 = 0x20;
constexpr uint8_t A2_SHARP = 0x21;
constexpr uint8_t B2 = 0x22;
constexpr uint8_t C3 = 0x23;
constexpr uint8_t C3_SHARP = 0x24;
constexpr uint8_t D3 = 0x25;
constexpr uint8_t D3_SHARP = 0x26;
constexpr uint8_t E3 = 0x27;
constexpr uint8_t F3 = 0x28;
constexpr uint8_t F3_SHARP = 0x29;
constexpr uint8_t G3 = 0x2A;
constexpr uint8_t G3_SHARP = 0x2B;

constexpr uint8_t A3 = 0x2C;
constexpr uint8_t A3_SHARP = 0x2D;
constexpr uint8_t B3 = 0x2E;
constexpr uint8_t C4 = 0x2F;
constexpr uint8_t C4_SHARP = 0x30;
constexpr uint8_t D4 = 0x31;
constexpr uint8_t D4_SHARP = 0x32;
constexpr uint8_t E4 = 0x33;
constexpr uint8_t F4 = 0x34;
constexpr uint8_t F4_SHARP = 0x35;
constexpr uint8_t G4 = 0x36;
constexpr uint8_t G4_SHARP = 0x37;

constexpr uint8_t A4 = 0x38;
constexpr uint8_t A4_SHARP = 0x39;
constexpr uint8_t B4 = 0x3A;
constexpr uint8_t C5 = 0x3B;
constexpr uint8_t C5_SHARP = 0x3C;
constexpr uint8_t D5 = 0x3D;
constexpr uint8_t D5_SHARP = 0x3E;
// no more notes!

constexpr uint8_t NOTE_MASK = 0x3F;
constexpr uint8_t SLIDE = 0x80;
constexpr uint8_t ACCENT = 0x40;

//"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"

// Daft Punk - Da Funk
const uint8_t da_funk[16] = {A2_SHARP, F4, D3 | SLIDE, C5, A3_SHARP | SLIDE | ACCENT, G3, F2_SHARP | SLIDE | ACCENT, G3_SHARP,
                             A4_SHARP | SLIDE, A2, C4 | SLIDE, D4 | SLIDE | ACCENT, F3, D3 | SLIDE, F2 | SLIDE, D3_SHARP | SLIDE};