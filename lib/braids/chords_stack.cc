//https://github.com/boourns/eurorack/blob/master/braids/stack.cc

#include "braids/digital_oscillator.h"

#include <algorithm>
#include <cstdio>

#include "stmlib/utils/dsp.h"
#include "stmlib/utils/random.h"

#include "braids/parameter_interpolation.h"
#include "braids/resources.h"
#include "braids/quantizer.h"

extern braids::Quantizer quantizer;

namespace braids {

using namespace stmlib;

const int kStackSize = 6;

#define CALC_SINE(phase) Interpolate88(ws_sine_fold, (Interpolate824(wav_sine, phase) * gain >> 15) + 32768);

inline void renderChordSine(
  DigitalOscillatorState& state_,
  int16_t parameter_[2],
  const uint8_t *sync, 
  int16_t *buffer, 
  size_t size, 
  uint32_t *phase_increment, 
  uint8_t noteCount) {
  
  uint32_t phase_0, phase_1, phase_2, phase_3, phase_4;
  int16_t gain = 2048 + (parameter_[0] * 30720 >> 15);

  phase_0 = state_.stack.phase[0];
  phase_1 = state_.stack.phase[1];
  phase_2 = state_.stack.phase[2];
  phase_3 = state_.stack.phase[3];
  phase_4 = state_.stack.phase[4];

  while (size) {
    int32_t sample = 0;
    
    phase_0 += phase_increment[0];
    phase_1 += phase_increment[1];
    phase_2 += phase_increment[2];
    phase_3 += phase_increment[3];
    phase_4 += phase_increment[4];

    sample = CALC_SINE(phase_0);

    sample += CALC_SINE(phase_1);
    sample += CALC_SINE(phase_2);
    sample += CALC_SINE(phase_3);

    if (noteCount > 4) {
      sample += CALC_SINE(phase_4);
    }

    sample = (sample >> 3) + (sample >> 5);
    CLIP(sample)
    *buffer++ = sample;
    
    phase_0 += phase_increment[0];
    phase_1 += phase_increment[1];
    phase_2 += phase_increment[2];
    phase_3 += phase_increment[3];
    phase_4 += phase_increment[4];

    sample = CALC_SINE(phase_0);

    sample += CALC_SINE(phase_1);
    sample += CALC_SINE(phase_2);
    sample += CALC_SINE(phase_3);

    if (noteCount > 4) {
      sample += CALC_SINE(phase_4);
    }

    sample = (sample >> 3) + (sample >> 5);
    CLIP(sample)
    *buffer++ = sample;

    size -= 2;
  }
  
  state_.stack.phase[0] = phase_0;
  state_.stack.phase[1] = phase_1;
  state_.stack.phase[2] = phase_2;
  state_.stack.phase[3] = phase_3;
  state_.stack.phase[4] = phase_4;
}

inline void renderChordSaw(
  DigitalOscillatorState& state_,
  int16_t parameter_[2],
  const uint8_t *sync, 
  int16_t *buffer, 
  size_t size, 
  uint32_t *phase_increment, 
  uint8_t noteCount) {

  uint32_t phase_0, phase_1, phase_2, phase_3, phase_4, phase_5;

  uint32_t detune = 0;

  for (int i = 0; i < 2; i++) {
    phase_0 = state_.stack.phase[(i*6)+0];
    phase_1 = state_.stack.phase[(i*6)+1];
    phase_2 = state_.stack.phase[(i*6)+2];
    phase_3 = state_.stack.phase[(i*6)+3];
    phase_4 = state_.stack.phase[(i*6)+4];
    phase_5 = state_.stack.phase[(i*6)+5];

    if (i == 1) {
      detune = parameter_[0]<<3;
    }

    int16_t *b = buffer;
    size_t s = size;

    while (s) {
      int32_t sample = 0;
      
      phase_0 += phase_increment[0] + detune;
      phase_1 += phase_increment[1] - detune;
      phase_2 += phase_increment[2] + detune;
      phase_3 += phase_increment[3] - detune;
      phase_4 += phase_increment[4] + detune;
      phase_5 += phase_increment[5] - detune;

      sample += (1 << 15) - (phase_0 >> 16);
      sample += (1 << 15) - (phase_1 >> 16);
      sample += (1 << 15) - (phase_2 >> 16);
      sample += (1 << 15) - (phase_3 >> 16);

      if (noteCount > 4) {
        sample += (1 << 15) - (phase_4 >> 16);
      }
      if (noteCount > 5) {
        sample += (1 << 15) - (phase_5 >> 16);
      }

      sample = (sample >> 2) + (sample >> 5);
      CLIP(sample)
      if (i == 0) {
        *b++ = sample >> 1;
      } else {
        *b += sample >> 1;
        b++;
      }
      
      phase_0 += phase_increment[0] + detune;
      phase_1 += phase_increment[1] - detune;
      phase_2 += phase_increment[2] + detune;
      phase_3 += phase_increment[3] - detune;
      phase_4 += phase_increment[4] + detune;
      phase_5 += phase_increment[5] - detune;

      sample = (1 << 15) - (phase_0 >> 16);
      sample += (1 << 15) - (phase_1 >> 16);
      sample += (1 << 15) - (phase_2 >> 16);
      sample += (1 << 15) - (phase_3 >> 16);

      if (noteCount > 4) {
        sample += (1 << 15) - (phase_4 >> 16);
      }
      if (noteCount > 5) {
        sample += (1 << 15) - (phase_5 >> 16);
      }

      sample = (sample >> 2) + (sample >> 5);
      CLIP(sample)
      if (i == 0) {
        *b++ = sample >> 1;
      } else {
        *b += sample >> 1;
        b++;
      }

      s -= 2;
    }
    state_.stack.phase[(i*6)+0] = phase_0;
    state_.stack.phase[(i*6)+1] = phase_1;
    state_.stack.phase[(i*6)+2] = phase_2;
    state_.stack.phase[(i*6)+3] = phase_3;
    state_.stack.phase[(i*6)+4] = phase_4;
    state_.stack.phase[(i*6)+5] = phase_5;
  }
}

// #define CALC_TRIANGLE_RAW(x) ((int16_t) ((((x >> 16) << 1) ^ (x & 0x80000000 ? 0xffff : 0x0000))) + 32768)
#define CALC_TRIANGLE(x) Interpolate88(ws_tri_fold, (calc_triangle_raw(x) * gain >> 15) + 32768)

inline int16_t calc_triangle_raw(uint32_t phase) {
  uint16_t phase_16 = phase >> 16;
  int16_t triangle = (phase_16 << 1) ^ (phase_16 & 0x8000 ? 0xffff : 0x0000);
  return triangle + 32768;
}

inline void renderChordTriangle(
  DigitalOscillatorState& state_,
  int16_t parameter_[2],
  const uint8_t *sync, 
  int16_t *buffer, 
  size_t size, 
  uint32_t *phase_increment, 
  uint8_t noteCount) {
  uint32_t phase_0, phase_1, phase_2, phase_3, phase_4, phase_5;

  phase_0 = state_.stack.phase[0];
  phase_1 = state_.stack.phase[1];
  phase_2 = state_.stack.phase[2];
  phase_3 = state_.stack.phase[3];
  phase_4 = state_.stack.phase[4];
  phase_5 = state_.stack.phase[5];

  int16_t gain = 2048 + (parameter_[0] * 30720 >> 15);

  while (size) {
    int32_t sample = 0;
    
    phase_0 += phase_increment[0];
    phase_1 += phase_increment[1];
    phase_2 += phase_increment[2];
    phase_3 += phase_increment[3];
    phase_4 += phase_increment[4];
    phase_5 += phase_increment[5];

    sample = CALC_TRIANGLE(phase_0);
    sample += CALC_TRIANGLE(phase_1);
    sample += CALC_TRIANGLE(phase_2);
    sample += CALC_TRIANGLE(phase_3);

    if (noteCount > 4) {
      sample += CALC_TRIANGLE(phase_4);
    }
    if (noteCount > 5) {
      sample += CALC_TRIANGLE(phase_5);
    }

    sample = (sample >> 3) + (sample >> 5);
    CLIP(sample)
    *buffer++ = sample;

    phase_0 += phase_increment[0];
    phase_1 += phase_increment[1];
    phase_2 += phase_increment[2];
    phase_3 += phase_increment[3];
    phase_4 += phase_increment[4];
    phase_5 += phase_increment[5];

    sample = CALC_TRIANGLE(phase_0);
    sample += CALC_TRIANGLE(phase_1);
    sample += CALC_TRIANGLE(phase_2);
    sample += CALC_TRIANGLE(phase_3);

    if (noteCount > 4) {
      sample += CALC_TRIANGLE(phase_4);
    }
    if (noteCount > 5) {
      sample += CALC_TRIANGLE(phase_5);
    }

    sample = (sample >> 3) + (sample >> 5);
    CLIP(sample)
    *buffer++ = sample;

    size -= 2;
  }
  
  state_.stack.phase[0] = phase_0;
  state_.stack.phase[1] = phase_1;
  state_.stack.phase[2] = phase_2;
  state_.stack.phase[3] = phase_3;
  state_.stack.phase[4] = phase_4;
  state_.stack.phase[5] = phase_5;
}

#define CALC_SQUARE(x, width) ((x > width) ? 5400 : -5400)

inline void renderChordSquare(
  DigitalOscillatorState& state_,
  int16_t parameter_[2],
  const uint8_t *sync, 
  int16_t *buffer, 
  size_t size, 
  uint32_t *phase_increment, 
  uint8_t noteCount) {

  uint32_t phase_0, phase_1, phase_2, phase_3, phase_4, phase_5;
  uint32_t pw = parameter_[0] << 16;

  phase_0 = state_.stack.phase[0];
  phase_1 = state_.stack.phase[1];
  phase_2 = state_.stack.phase[2];
  phase_3 = state_.stack.phase[3];
  phase_4 = state_.stack.phase[4];
  phase_5 = state_.stack.phase[5];

  while (size) {
    int32_t sample = 0;
    
    phase_0 += phase_increment[0];
    phase_1 += phase_increment[1];
    phase_2 += phase_increment[2];
    phase_3 += phase_increment[3];

    sample = CALC_SQUARE(phase_0, pw);
    sample += CALC_SQUARE(phase_1, pw);
    sample += CALC_SQUARE(phase_2, pw);
    sample += CALC_SQUARE(phase_3, pw);

    if (noteCount > 4) {
      sample += CALC_SQUARE(phase_4, pw);
    }
    if (noteCount > 5) {
      sample += CALC_SQUARE(phase_5, pw);
    }

    CLIP(sample)
    *buffer++ = sample;

    phase_0 += phase_increment[0];
    phase_1 += phase_increment[1];
    phase_2 += phase_increment[2];
    phase_3 += phase_increment[3];

    sample = CALC_SQUARE(phase_0, pw);
    sample += CALC_SQUARE(phase_1, pw);
    sample += CALC_SQUARE(phase_2, pw);
    sample += CALC_SQUARE(phase_3, pw);

    if (noteCount > 4) {
      sample += CALC_SQUARE(phase_4, pw);
    }
    if (noteCount > 5) {
      sample += CALC_SQUARE(phase_5, pw);
    }

    CLIP(sample)
    *buffer++ = sample;

    size -= 2;
  }
  
  state_.stack.phase[0] = phase_0;
  state_.stack.phase[1] = phase_1;
  state_.stack.phase[2] = phase_2;
  state_.stack.phase[3] = phase_3;
  state_.stack.phase[4] = phase_4;
  state_.stack.phase[5] = phase_5;
}

const uint8_t mini_wave_line[] = {
  157, 161, 171, 188, 189, 191, 192, 193, 196, 198, 201, 234, 232,
  229, 226, 224, 1, 2, 3, 4, 5, 8, 12, 32, 36, 42, 47, 252, 254, 141, 139,
  135, 174
};

#define SEMI * 128

const uint16_t chords[17][3] = {
  { 2, 4, 6 },
  { 16, 32, 48 },
  { 2 SEMI, 7 SEMI, 12 SEMI },
  { 3 SEMI, 7 SEMI, 10 SEMI },
  { 3 SEMI, 7 SEMI, 12 SEMI },
  { 3 SEMI, 7 SEMI, 14 SEMI },
  { 3 SEMI, 7 SEMI, 17 SEMI },
  { 7 SEMI, 12 SEMI, 19 SEMI },
  { 7 SEMI, 3 + 12 SEMI, 5 + 19 SEMI },
  { 4 SEMI, 7 SEMI, 17 SEMI },
  { 4 SEMI, 7 SEMI, 14 SEMI },
  { 4 SEMI, 7 SEMI, 12 SEMI },
  { 4 SEMI, 7 SEMI, 11 SEMI },
  { 5 SEMI, 7 SEMI, 12 SEMI },
  { 4, 7 SEMI, 12 SEMI },
  { 4, 4 + 12 SEMI, 12 SEMI },
  { 4, 4 + 12 SEMI, 12 SEMI },
};

inline void renderChordWavetable(
  DigitalOscillatorState& state_,
  int16_t parameter_[2],
  const uint8_t *sync, 
  int16_t *buffer, 
  size_t size, 
  uint32_t *phase_increment, 
  uint8_t noteCount) {

  const uint8_t* wave_1 = wt_waves + mini_wave_line[parameter_[0] >> 10] * 129;
  const uint8_t* wave_2 = wt_waves + mini_wave_line[(parameter_[0] >> 10) + 1] * 129;
  uint16_t wave_xfade = parameter_[0] << 6;
  uint32_t phase_0, phase_1, phase_2, phase_3, phase_4;

  phase_0 = state_.stack.phase[0];
  phase_1 = state_.stack.phase[1];
  phase_2 = state_.stack.phase[2];
  phase_3 = state_.stack.phase[3];
  phase_4 = state_.stack.phase[4];

  while (size) {
    int32_t sample = 0;
    
    phase_0 += phase_increment[0];
    phase_1 += phase_increment[1];
    phase_2 += phase_increment[2];
    phase_3 += phase_increment[3];
    phase_4 += phase_increment[4];

    sample = Crossfade(wave_1, wave_2, phase_0 >> 1, wave_xfade);
    sample += Crossfade(wave_1, wave_2, phase_1 >> 1, wave_xfade);
    sample += Crossfade(wave_1, wave_2, phase_2 >> 1, wave_xfade);
    sample += Crossfade(wave_1, wave_2, phase_3 >> 1, wave_xfade);
    if (noteCount > 4) {
      sample += Crossfade(wave_1, wave_2, phase_4 >> 1, wave_xfade);
    }

    sample = (sample >> 2);
    CLIP(sample)
    *buffer++ = sample;
    
    phase_0 += phase_increment[0];
    phase_1 += phase_increment[1];
    phase_2 += phase_increment[2];
    phase_3 += phase_increment[3];
    phase_4 += phase_increment[4];

    sample = Crossfade(wave_1, wave_2, phase_0 >> 1, wave_xfade);
    sample += Crossfade(wave_1, wave_2, phase_1 >> 1, wave_xfade);
    sample += Crossfade(wave_1, wave_2, phase_2 >> 1, wave_xfade);
    sample += Crossfade(wave_1, wave_2, phase_3 >> 1, wave_xfade);
    if (noteCount > 4) {
      sample += Crossfade(wave_1, wave_2, phase_4 >> 1, wave_xfade);
    }

    sample = (sample >> 2);
    CLIP(sample)
    *buffer++ = sample;

    size -= 2;
  }
  
  state_.stack.phase[0] = phase_0;
  state_.stack.phase[1] = phase_1;
  state_.stack.phase[2] = phase_2;
  state_.stack.phase[3] = phase_3;
  state_.stack.phase[4] = phase_4;
}

// without the attribute this gets build as-is AND inlined into RenderStack :/
void DigitalOscillator::renderChord(
  const uint8_t *sync, 
  int16_t *buffer, 
  size_t size, 
  const uint8_t* noteOffset, 
  uint8_t noteCount) {

  int32_t fm = 0;

  if (strike_) {
    for (size_t i = 0; i < kStackSize; ++i) {
      state_.stack.phase[i] = Random::GetWord();
    }
    strike_ = false;
  }

  // Do not use an array here to allow these to be kept in arbitrary registers.
  uint32_t phase_increment[6];

  if (quantizer.enabled()) {
    int8_t index = 0;
    int8_t root = 0;
    quantizer.Process(pitch_, 0, &root);
    fm = pitch_ - quantizer.Lookup(root);

    phase_increment[0] = phase_increment_;
    for (size_t i = 1; i < noteCount; i++) {
      if(custom_chord != nullptr)
        phase_increment[i] = DigitalOscillator::ComputePhaseIncrement(quantizer.Process(pitch_ + (noteOffset[i-1]<<7)));
      else {
        index = (root + noteOffset[i-1]);
        phase_increment[i] = DigitalOscillator::ComputePhaseIncrement(quantizer.Lookup(index) + fm);
      }        
    }
  } else {
    noteCount = 4;
    uint16_t chord_integral = parameter_[1] >> 11;
    uint16_t chord_fractional = parameter_[1] << 5;
    if (chord_fractional < 30720) {
      chord_fractional = 0;
    } else if (chord_fractional >= 34816) {
      chord_fractional = 65535;
    } else {
      chord_fractional = (chord_fractional - 30720) * 16;
    }

    phase_increment[0] = phase_increment_;
    for (size_t i = 0; i < 3; ++i) {
      uint16_t detune_1 = chords[chord_integral][i];
      uint16_t detune_2 = chords[chord_integral + 1][i];
      uint16_t detune = detune_1 + ((detune_2 - detune_1) * chord_fractional >> 16);
      phase_increment[i+1] = DigitalOscillator::ComputePhaseIncrement(pitch_ + detune);
    }
  }

  if (shape_ == OSC_SHAPE_STACK_SAW || shape_ == OSC_SHAPE_CHORD_SAW) {
    renderChordSaw(state_, parameter_, sync, buffer, size, phase_increment, noteCount);
  } else if (shape_ == OSC_SHAPE_STACK_WAVETABLE || shape_ == OSC_SHAPE_CHORD_WAVETABLE) {
    renderChordWavetable(state_, parameter_, sync, buffer, size, phase_increment, noteCount);
  } else if (shape_ == OSC_SHAPE_STACK_TRIANGLE || shape_ == OSC_SHAPE_CHORD_TRIANGLE) {
    renderChordTriangle(state_, parameter_, sync, buffer, size, phase_increment, noteCount);
  } else if (shape_ == OSC_SHAPE_STACK_SQUARE || shape_ == OSC_SHAPE_CHORD_SQUARE) {
    renderChordSquare(state_, parameter_, sync, buffer, size, phase_increment, noteCount);
  } else {
    renderChordSine(state_, parameter_, sync, buffer, size, phase_increment, noteCount);
  }
}

//number of notes, followed by offsets
const uint8_t diatonic_chords[8][6] = {
  {0, 2, 4, 7, 0, 0},  // octave
  {0, 2, 4, 5, 7, 0},  // octave add6
  {0, 2, 4, 6, 0, 0},  // 7th
  {0, 2, 4, 5, 6, 0},  // 7th add6
  {0, 2, 4, 6, 8, 0},  // 9th
  {0, 2, 4, 6, 8, 10}, // 11th
  {0, 2, 4, 5, 7, 10}, // 11th add6
  {0, 2, 4, 8, 0, 0},  // add9
};

const uint8_t* custom_chord = nullptr;

void DigitalOscillator::RenderDiatonicChord(
    const uint8_t* sync,
    int16_t* buffer,
    size_t size) {
  
  const uint8_t* offsets = custom_chord;
  
  if(offsets == nullptr) 
    offsets = diatonic_chords[(parameter_[1] >> 12) & 0x7];
    
  uint8_t len = 4;
    
  for (size_t i = 4; i < 6; i++) {
    if(offsets[i] > 0)
      len ++;
  }

  renderChord(sync, buffer, size, &offsets[1], len);
}

void DigitalOscillator::RenderStack(
    const uint8_t* sync,
    int16_t* buffer,
    size_t size) {
  
  uint8_t span = 1 + (parameter_[1] >> 11);
  uint8_t offsets[kStackSize];
  uint8_t acc = 0;
  uint8_t count = kStackSize-1;
  uint8_t i = 0;

  for (; i < count; i++) {
    acc += span;
    offsets[i] = acc;
  }

  // don't pass in kStackSize or gcc will render a second, optimized version of renderChord that
  // knows noteCount is static.
  renderChord(sync, buffer, size, offsets, i);
}

}
