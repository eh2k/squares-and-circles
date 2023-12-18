
#include "../squares-and-circles-api.h"

#include "faust/ui.hxx"
#include "faust/djembe.dsp.h"

void UIGlue::addButton(const char *ui, const char *label, float *zone)
{
    dsp_param_f(TRIG, zone);
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
        dsp_param_f2(label, zone, min, max);
        //dsp_param_step_f(label, step);
    }
}

static FAUSTCLASS dsp;

static float outputL[FRAME_BUFFER_SIZE];

DSP_SETUP
void setup()
{
    initmydsp(&dsp, SAMPLE_RATE);

    UIGlue ui;
    buildUserInterfacemydsp(&dsp, &ui);

    dsp_frame_f(OUTPUT_L, outputL);
}

DSP_PROCESS
void process()
{
    float *outputs[] = {outputL, nullptr};
    computemydsp(&dsp, FRAME_BUFFER_SIZE, nullptr, &outputs[0]);
}