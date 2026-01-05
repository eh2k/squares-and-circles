#include "../squares-and-circles-api.h"

#include "faust/ui.hxx"
#include "faust/rev_dattorro.dsp.h"

void UIGlue::addButton(const char *ui, const char *label, float *zone)
{
    engine::addParam("TRIG", zone);
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
    float *tmp[4];
    tmp[0] = engine::inputBuffer<0>();
    tmp[1] = engine::inputBuffer<1>();
    tmp[2] = engine::outputBuffer<0>();
    tmp[3] = engine::outputBuffer<1>();
    computemydsp(&dsp, FRAME_BUFFER_SIZE, &tmp[0], &tmp[2]);
}