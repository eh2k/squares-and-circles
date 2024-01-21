
#include "../squares-and-circles-api.h"

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
}