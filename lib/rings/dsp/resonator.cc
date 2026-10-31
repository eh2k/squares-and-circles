// Copyright 2015 Emilie Gillet.
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
// Resonator.

#include "rings/dsp/resonator.h"

#include "stmlib/dsp/dsp.h"
#include "stmlib/dsp/cosine_oscillator.h"
#include "stmlib/dsp/parameter_interpolator.h"

#include "rings/resources.h"

namespace rings {

using namespace std;
using namespace stmlib;

void Resonator::Init() {
  for (int32_t i = 0; i < kMaxModes; ++i) {
    // [eh2k] as stmlib::Svf::Init()
    f_[i].g = OnePole::tan<FREQUENCY_DIRTY>(0.01f);
    f_[i].r = 1.0f / 100.0f;
    f_[i].h = 1.0f / (1.0f + f_[i].r * f_[i].g + f_[i].g * f_[i].g);
    f_[i].state_1 = f_[i].state_2 = 0.0f;
  }

  set_frequency(220.0f / kSampleRate);
  set_structure(0.25f);
  set_brightness(0.5f);
  set_damping(0.3f);
  set_position(0.999f);
  previous_position_ = 0.0f;
  set_resolution(kMaxModes);
  filters_resolution_ = -1;
  num_modes_ = 0;
  at_rest_ = true; // the states 0
  ringing_ = 0;
}

int32_t Resonator::ComputeFilters() {
  // [eh2k] Only when an input changed (two divisions a mode), not in every block of a held note
  if (resolution_ == filters_resolution_ &&
      frequency_ == filters_frequency_ &&
      structure_ == filters_structure_ &&
      brightness_ == filters_brightness_ &&
      damping_ == filters_damping_) {
    return num_modes_;
  }
  filters_resolution_ = resolution_;
  filters_frequency_ = frequency_;
  filters_structure_ = structure_;
  filters_brightness_ = brightness_;
  filters_damping_ = damping_;

  float stiffness = Interpolate(lut_stiffness, structure_, 256.0f);
  float harmonic = frequency_;
  float stretch_factor = 1.0f; 
  float q = 500.0f * Interpolate(
      lut_4_decades,
      damping_,
      256.0f);
  float brightness_attenuation = 1.0f - structure_;
  // Reduces the range of brightness when structure is very low, to prevent
  // clipping.
  brightness_attenuation *= brightness_attenuation;
  brightness_attenuation *= brightness_attenuation;
  brightness_attenuation *= brightness_attenuation;
  float brightness = brightness_ * (1.0f - 0.2f * brightness_attenuation);
  float q_loss = brightness * (2.0f - brightness) * 0.85f + 0.15f;
  float q_loss_damping_rate = structure_ * (2.0f - structure_) * 0.1f;
  int32_t num_modes = 0;
  for (int32_t i = 0; i < min(kMaxModes, resolution_); ++i) {
    float partial_frequency = harmonic * stretch_factor;
    if (partial_frequency >= 0.49f) {
      partial_frequency = 0.49f;
    } else {
      num_modes = i + 1;
    }
    // as stmlib::Svf::set_f_q<FREQUENCY_FAST>(partial_frequency, 1.0f + partial_frequency * q)
    ResonatorMode& m = f_[i];
    m.g = OnePole::tan<FREQUENCY_FAST>(partial_frequency);
    m.r = 1.0f / (1.0f + partial_frequency * q);
    m.h = 1.0f / (1.0f + m.r * m.g + m.g * m.g);
    stretch_factor += stiffness;
    if (stiffness < 0.0f) {
      // Make sure that the partials do not fold back into negative frequencies.
      stiffness *= 0.93f;
    } else {
      // This helps adding a few extra partials in the highest frequencies.
      stiffness *= 0.98f;
    }
    // This prevents the highest partials from decaying too fast.
    q_loss += q_loss_damping_rate * (1.0f - q_loss);
    harmonic += frequency_;
    q *= q_loss;
  }
  
  num_modes_ = num_modes;
  return num_modes;
}

// [eh2k] n modes from first over the block, each as stmlib::Svf::Process<FILTER_MODE_BAND_PASS>() did it: the 1st,
// 3rd, ... into out, the 2nd, 4th, ... into aux, times its amplitude (varying: the next of each sample's oscillator)
template<int32_t n, bool varying>
inline void Resonator::ProcessModes(
    int32_t first,
    const float* amplitudes,
    CosineOscillator* oscillators,
    const float* in,
    float* out,
    float* aux,
    size_t size) {
  float g[n], r[n], h[n], state_1[n], state_2[n], a[n];
  for (int32_t k = 0; k < n; ++k) {
    const ResonatorMode& m = f_[first + k];
    g[k] = m.g;
    r[k] = m.r;
    h[k] = m.h;
    state_1[k] = m.state_1;
    state_2[k] = m.state_2;
    a[k] = varying ? 0.0f : amplitudes[k];
  }
  for (size_t j = 0; j < size; ++j) {
    const float input = in[j];
    float odd = out[j];
    float even = aux[j];
    for (int32_t k = 0; k < n; ++k) {
      if (varying) {
        a[k] = oscillators[j].Next();
      }
      float hp = (input - r[k] * state_1[k] - g[k] * state_1[k] - state_2[k]) * h[k];
      float bp = g[k] * hp + state_1[k];
      state_1[k] = g[k] * hp + bp;
      float lp = g[k] * bp + state_2[k];
      state_2[k] = g[k] * bp + lp;
      if (k & 1) {
        even += a[k] * bp;
      } else {
        odd += a[k] * bp;
      }
    }
    out[j] = odd;
    aux[j] = even;
  }
  for (int32_t k = 0; k < n; ++k) {
    f_[first + k].state_1 = state_1[k];
    f_[first + k].state_2 = state_2[k];
  }
}

// [eh2k] No input (below -240 dB) and every mode below -120 dB: the modes at rest (states 0), nothing to compute
// until an input comes, the output 0. What they would still have rung is at most ~1 LSB of 16 bits, and decays from
// there. True if so (no Process()). The first sample first (a sound or its tail goes on), then the states (while the
// modes ring, the first one says so), then the other samples.
bool Resonator::Rest(const float* in, float* out, float* aux, size_t size) {
  if (!size || fabsf(in[0]) > 1e-12f) {
    at_rest_ = false;
    return false;
  }
  if (!at_rest_) {
    // all the modes the resolution can run, also those above the highest frequency (not run now, their state kept),
    // from the one that rang the last time
    for (int32_t n = 0; n < resolution_; ++n) {
      int32_t i = ringing_ + n;
      if (i >= resolution_) {
        i -= resolution_;
      }
      if (fabsf(f_[i].state_1) > 1e-6f || fabsf(f_[i].state_2) > 1e-6f) {
        ringing_ = i;
        return false;
      }
    }
  }
  for (size_t j = 1; j < size; ++j) {
    if (fabsf(in[j]) > 1e-12f) {
      at_rest_ = false;
      return false;
    }
  }
  if (!at_rest_) {
    for (int32_t i = 0; i < resolution_; ++i) {
      f_[i].state_1 = f_[i].state_2 = 0.0f;
    }
    at_rest_ = true;
  }
  ParameterInterpolator position(&previous_position_, position_, size);
  for (size_t j = 0; j < size; ++j) {
    position.Next(); // the position goes on as in Process()
    out[j] = 0.0f;
    aux[j] = 0.0f;
  }
  return true;
}

void Resonator::Process(const float* in, float* out, float* aux, size_t size) {
  int32_t num_modes = ComputeFilters();
  if (!size) {
    return;
  }
  // [eh2k] Modes outer, samples inner (was: all modes for each sample), 4 modes at a time: their coefficients and
  // state stay in registers over the block. Each sample sums the same products in the same order as before, so the
  // output is the same. The modes ran in pairs: an odd count one more.
  num_modes += num_modes & 1;

  float input[kMaxBlockSize];
  float position[kMaxBlockSize];
  ParameterInterpolator interpolator(&previous_position_, position_, size);
  for (size_t j = 0; j < size; ++j) {
    input[j] = in[j] * 0.125f;
    position[j] = interpolator.Next();
    out[j] = 0.0f;
    aux[j] = 0.0f;
  }

  // The amplitudes of the modes: a cosine over them at the position of each sample. The interpolated position is
  // monotonic: equal first and last values are the position all along, one set of amplitudes for the block.
  int32_t i = 0;
  if (position[0] == position[size - 1]) {
    CosineOscillator oscillator;
    oscillator.Init<COSINE_OSCILLATOR_APPROXIMATE>(position[0]);
    float amplitudes[kMaxModes];
    for (int32_t k = 0; k < num_modes; ++k) {
      amplitudes[k] = oscillator.Next();
    }
    for (; i + 4 <= num_modes; i += 4) {
      ProcessModes<4, false>(i, &amplitudes[i], NULL, input, out, aux, size);
    }
    if (i < num_modes) {
      ProcessModes<2, false>(i, &amplitudes[i], NULL, input, out, aux, size);
    }
  } else {
    CosineOscillator oscillators[kMaxBlockSize];
    for (size_t j = 0; j < size; ++j) {
      oscillators[j].Init<COSINE_OSCILLATOR_APPROXIMATE>(position[j]);
    }
    for (; i + 4 <= num_modes; i += 4) {
      ProcessModes<4, true>(i, NULL, oscillators, input, out, aux, size);
    }
    if (i < num_modes) {
      ProcessModes<2, true>(i, NULL, oscillators, input, out, aux, size);
    }
  }
}

}  // namespace rings
