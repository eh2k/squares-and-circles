
/*
 Adapted from:
 https://github.com/mhetrick/hetrickcv/blob/master/docs/Modules/Dust.md
 https://github.com/mhetrick/hetrickcv/blob/master/src/Dust.cpp
*/

#include <stdio.h>
#include <cmath>
#include <algorithm>

struct Dust
{
    float lastDensity = 0.0;
    float densityScaled = 0.0;
    float threshold = 0.0;

    float process(float densityInput, bool bipolar)
    {
        if (lastDensity != densityInput)
        {
            densityScaled = std::clamp(densityInput, 0.0f, 4.0f) / 4.0f;
            densityScaled = SAMPLE_RATE * powf(densityScaled, 3.0f);
            lastDensity = densityInput;
            threshold = (1.0 / SAMPLE_RATE) * densityScaled;
        }

        const float noiseValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        if (noiseValue < threshold)
        {
            if (bipolar)
            {
                const float scale = (threshold > 0.0f) ? 2.0f / threshold : 0.0f;
                return std::clamp((noiseValue * scale - 1.0f), -1.f, 1.0f);
            }
            else
            {
                const float scale = (threshold > 0.0f) ? 1.0f / threshold : 0.0f;
                return std::clamp(noiseValue * scale, 0.0f, 1.f);
            }
        }
        else
        {
            return 0;
        }
    }
};
