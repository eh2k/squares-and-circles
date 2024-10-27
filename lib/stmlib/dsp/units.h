// Copyright 2014 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Conversion from semitones to frequency ratio.

#ifndef STMLIB_DSP_UNITS_H_
#define STMLIB_DSP_UNITS_H_

#include "stmlib/stmlib.h"
#include "stmlib/dsp/dsp.h"

namespace stmlib {

// https://pichenettes.github.io/mutable-instruments-documentation/tech_notes/exponential_conversion_in_digital_oscillators/

// Computes 2^x by using a polynomial approximation of 2^frac(x) and directly
// incrementing the exponent of the IEEE 754 representation of the result
// by int(x). Depending on the use case, the order of the polynomial
// approximation can be chosen.
template<int order>
inline float Pow2Fast(float x) {
  union {
    float f;
    int32_t w;
  } r;


  if (order == 1) {
    r.w = float(1 << 23) * (127.0f + x);
    return r.f;
  }

  int32_t x_integral = static_cast<int32_t>(x);
  if (x < 0.0f) {
    --x_integral;
  }
  x -= static_cast<float>(x_integral);

  if (order == 1) {
    r.f = 1.0f + x;
  } else if (order == 2) {
    r.f = 1.0f + x * (0.6565f + x * 0.3435f);
  } else if (order == 3) {
    r.f = 1.0f + x * (0.6958f + x * (0.2251f + x * 0.0791f));
  }
  r.w += x_integral << 23;
  return r.f;
}

#ifdef SemitonesToRatioFast

inline float SemitonesToRatio(float semitones){
  return Pow2Fast<1>(semitones / 12.f);
}

#else 

extern const float lut_pitch_ratio_high[257];
extern const float lut_pitch_ratio_low[257];

inline float SemitonesToRatio(float semitones) {
  float pitch = semitones + 128.0f;
  MAKE_INTEGRAL_FRACTIONAL(pitch)

  return lut_pitch_ratio_high[pitch_integral] * \
      lut_pitch_ratio_low[static_cast<int32_t>(pitch_fractional * 256.0f)];
}

#endif

inline float SemitonesToRatioSafe(float semitones) {
  float scale = 1.0f;
  while (semitones > 120.0f) {
    semitones -= 120.0f;
    scale *= 1024.0f;
  }
  while (semitones < -120.0f) {
    semitones += 120.0f;
    scale *= 1.0f / 1024.0f;
  }
  return scale * SemitonesToRatio(semitones);
}


inline float Exp2Safe(float value) {
  return SemitonesToRatioSafe(value * 12.0f);
}

}  // namespace stmlib

#endif  // STMLIB_DSP_UNITS_H_
