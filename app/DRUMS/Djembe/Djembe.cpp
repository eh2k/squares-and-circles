
#include "../../squares-and-circles-api.h"

#include "faust/ui.hxx"
#include "faust/djembe.dsp.h"

static float *_trig = nullptr;

void UIGlue::addButton(const char *ui, const char *label, float *zone)
{
    if (_trig == nullptr)
        _trig = zone;
}

void UIGlue::addNumEntry(const char *ui, const char *label, float *zone, float init, float min, float max, float step)
{
}

void UIGlue::addCheckButton(const char *ui, const char *label, float *zone)
{
    addHorizontalSlider(ui, label, zone, 0, 0, 1, 1);
}

void UIGlue::addVerticalSlider(const char *ui, const char *label, float *zone, float init, float min, float max, float step)
{
    addHorizontalSlider(ui, label, zone, init, min, max, step);
}

void UIGlue::addHorizontalSlider(const char *ui, const char *label, float *zone, float init, float min, float max, float step)
{
    if (zone)
    {
        *zone = init;
        engine::addParam(label, zone, min, max);
    }
}

static FAUSTCLASS dsp;

// A single strike peaks at ~0.8 (gain = 3 in djembe.dsp), but the modes ring for up to 0.6 s: strikes 250 ms apart
// add up to ~2 and the DAC would clip them hard. Below 0.8 the signal stays as it is, above it a soft knee runs
// towards +-1.
static inline float soft_knee(float x)
{
    constexpr float knee = 0.8f;
    const float a = fabsf(x);
    if (a <= knee)
        return x;
    const float y = knee + (1.f - knee) * tanhf((a - knee) / (1.f - knee));
    return x < 0 ? -y : y;
}

void engine::setup()
{
    initmydsp(&dsp, SAMPLE_RATE);

    UIGlue ui;
    buildUserInterfacemydsp(&dsp, &ui);
}

void engine::process()
{
    if (_trig != nullptr)
        *_trig = engine::trig();
    auto outputL = engine::outputBuffer<0>();
    float *outputs[] = {outputL, nullptr};
    computemydsp(&dsp, FRAME_BUFFER_SIZE, nullptr, &outputs[0]);
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        outputL[i] = soft_knee(outputL[i]);
}