#include <math.h>

struct MetaGlue
{
    const char *metaInterface;
    void declare(const char *ui, const char *key, const char *value)
    {
    }
};

struct UIGlue
{
    const char *uiInterface;
    void openTabBox(const char *ui, const char *label){};
    void openVerticalBox(const char *ui, const char *title){};
    void openHorizontalBox(const char *ui, const char *title){};
    void closeBox(const char *ui){};

    void declare(const char *ui, float *zone, const char *key, const char *val) {}
    void addNumEntry(const char *ui, const char *label, float *zone, float init, float min, float max, float step);
    void addHorizontalSlider(const char *ui, const char *label, float *zone, float init, float min, float max, float step);
    void addVerticalSlider(const char *ui, const char *label, float *zone, float init, float min, float max, float step);
    void addButton(const char *ui, const char *label, float *zone);
    void addCheckButton(const char *ui, const char *label, float *zone);

    // -- passive widgets

    void addHorizontalBargraph(const char *ui, const char *label, float *zone, float min, float max){};
    void addVerticalBargraph(const char *ui, const char *label, float *zone, float min, float max){};

    // -- soundfiles

    //void addSoundfile(const char *label, const char *filename, Soundfile **sf_zone){};
};

constexpr float base_frequency = 440.f;
constexpr float base_pitch = 69.f;

float note_to_frequency(float note)
{
    return base_frequency * powf(2.f, (note - base_pitch) / 12.f);
}