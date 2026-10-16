
/* 
 Crackle originally released under CC0-1.0.
 Source: https://github.com/mhetrick/hetrickcv

 Adapted from:
 https://github.com/mhetrick/hetrickcv/blob/master/docs/Modules/Crackle.md
 https://github.com/mhetrick/hetrickcv/blob/master/src/Crackle.cpp
*/

/*
    ┌────────┐    crackle
    │        │ ┌◖◖◖◖◖◖◖◖◖◖◖◖┐
    │        │ │            │
    │ ┌◅◅◅◅◅◅◅◅│◅◅◅◅┐       │
  ──┼─┼───┬───┬─┬───┼─▶     │
    │ │▫▫▫│▫▫▫│▫│▫▫▫│─.     │
 ┌──┼─┴───┼──┐│▫│▫▫▫│  `.   │
 │◓◓│◓◓◓◓◓│◓◓││▫├───┴────╳──┼─┐
 │◓◓│◓◓◓┌─┼─┬┼┼─┼─┐◘◘◘◘◘◘◘◘◘│◘│
 │◓◓├───┼─┘◓│││▫│◘│◘◘◘◘◘▲◘◘◘│◘│
 │◓◓│◓◓◓│◓◓◓│├┼─┤◘│◘◘◘◘◘│◘◘◘│◘│
 │◓◓│◓◓◓◓◓◓◓│││ │◘│◘◘◘◘◘└───┘◘│
 │◓◓│◓◓◓◓◓◓◓│││ │◘│◘◘◘◘◘◘◘◘◘◘◘│
 └──┼─╳─────┼┘│ │◘│◘◘◘◘◘◘◘◘◘◘◘│
  ▲ │  `.   ▴ │ ├─┼───────────┘
  │ │  ───────┘ │ │,─' ▲
  └─┘       ▴───┼─│    │
            └───┼─┴────┴──▶
                │
                └─────────────▶
*/

#include <stdio.h>
#include <cmath>
#include <algorithm>

struct Crackle
{
    float lastDensity = 1.0;
    float densityScaled = 1.0;
    float y1 = 0.2643;
    float y2 = 0.0;
    float lasty1 = 0.2643f;

    float broken(float density)
    {
        if (lastDensity != density)
        {
            densityScaled = std::clamp(density, 0.0f, 2.0f) / 2.0f;
            densityScaled = powf(densityScaled, 3.0f) + 1.0f;
            lastDensity = density;
        }

        const float y0 = std::fabs(y1 * densityScaled - y2 - 0.05f);
        y2 = y1;
        y1 = lasty1;
        lasty1 = std::clamp(y0, -1.0f, 1.0f);
        return lasty1;
    }

    float classic(float density)
    {
        if (lastDensity != density)
        {
            densityScaled = std::clamp(density, 0.0f, 2.0f) / 2.0f;
            densityScaled = powf(densityScaled, 3.0f) + 1.0f;
            lastDensity = density;
        }

        const float y0 = std::fabs(y1 * densityScaled - y2 - 0.05f);
        y2 = y1;
        y1 = y0;
        return std::clamp(y0, -1.f, 1.f);
    }

};