/*
   Copyright 2013 Google Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef __CONTROLLERS_H
#define __CONTROLLERS_H

#include "synth.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MIDI_CONTROLLER_MODE_MAX 2

// State of MIDI controllers
const int kControllerPitch = 0;
const int kControllerPitchRange = 1;
const int kControllerPitchStep = 2;

class FmCore;

class FmMod {
  public:
    uint8_t range;
    bool pitch;
    bool amp;
    bool eg;
    uint8_t ctrl_mode;
    uint8_t _dummy_;

    FmMod()
    {
      range = 0;
      ctrl_mode = 0;
      pitch = false;
      amp = false;
      eg = false;
    }

    void setRange(uint8_t r)
    {
      range = r < 0 && r > 99 ? 0 : r;
    }

    void setTarget(uint8_t assign)
    {
      assign = assign < 0 && assign > 7 ? 0 : assign;
      pitch = assign & 1; // PITCH
      amp = assign & 2; // AMP
      eg = assign & 4; // EG
    }

    void setMode(uint8_t m)
    {
      ctrl_mode = m > MIDI_CONTROLLER_MODE_MAX ? 0 : m;
    }
};

class Controllers {
    void applyMod(int cc, FmMod &mod)
    {
      uint8_t total = 0;
      float range = mod.range / 100.f;

      switch (mod.ctrl_mode)
      {
        case 0:
          total = uint8_t(float(cc) * range); // LINEAR mode
          break;
        case 1:
          total = uint8_t(127.f * range - (float(cc) * range)); // REVERSE mode
          break;
        case 2:
          total = uint8_t(range * float(cc) + (1.f - range) * 127.f); // DIRECT BC mode by Thierry (opus.quatre)
          break;
      }

      if (mod.amp)
        amp_mod = max(amp_mod, total);

      if (mod.pitch)
        pitch_mod = max(pitch_mod, total);

      if (mod.eg)
        eg_mod = max(eg_mod, total);
    }

  public:
    int32_t values_[3];

    uint8_t amp_mod;
    uint8_t pitch_mod;
    uint8_t eg_mod;

    uint8_t aftertouch_cc;
    uint8_t breath_cc;
    uint8_t foot_cc;
    uint8_t modwheel_cc;
    int masterTune;

    uint8_t opSwitch;

    FmMod wheel;
    FmMod foot;
    FmMod breath;
    FmMod at;

    Controllers() {
      amp_mod = 0;
      pitch_mod = 0;
      eg_mod = 0;
    }

    void refresh() {
      amp_mod = pitch_mod = eg_mod = 0;

      applyMod(modwheel_cc, wheel);
      applyMod(breath_cc, breath);
      applyMod(foot_cc, foot);
      applyMod(aftertouch_cc, at);

      if ( ! ((wheel.eg || foot.eg) || (breath.eg || at.eg)) )
        eg_mod = 127;
    }
};

#endif  // __CONTROLLERS_H
