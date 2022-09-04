#include "machine.h"
#include "stmlib/dsp/dsp.h"
#include "stmlib/dsp/units.h"

struct dsp
{
};

struct Meta
{
    void declare(const char *key, const char *value)
    {
    }
};

struct Soundfile
{
};

struct UI
{
    virtual void openTabBox(const char *label){};
    virtual void openVerticalBox(const char *title) = 0;
    virtual void openHorizontalBox(const char *title) = 0;
    virtual void closeBox() = 0;

    virtual void declare(float *zone, const char *key, const char *val) = 0;
    virtual void addNumEntry(const char *label, float *zone, float init, float min, float max, float step) = 0;
    virtual void addHorizontalSlider(const char *label, float *zone, float init, float min, float max, float step) = 0;
    virtual void addVerticalSlider(const char *label, float *zone, float init, float min, float max, float step) = 0;
    virtual void addButton(const char *label, float *zone) = 0;
    virtual void addCheckButton(const char *label, float *zone) = 0;

    // -- passive widgets

    virtual void addHorizontalBargraph(const char *label, float *zone, float min, float max){};
    virtual void addVerticalBargraph(const char *label, float *zone, float min, float max){};

    // -- soundfiles

    virtual void addSoundfile(const char *label, const char *filename, Soundfile **sf_zone){};
};

using namespace machine;

template <class T, uint32_t engine_props>
class FaustEngine : public machine::Engine, UI
{
    static_assert(std::is_base_of<dsp, T>::value, "T must derive from dsp");

    float bufferL[FRAME_BUFFER_SIZE];
    float bufferR[FRAME_BUFFER_SIZE];

    float _pitch = 0.5f;

    T _faust;

    float *_trigger = nullptr;

    void openVerticalBox(const char *title) override
    {
    }
    void openHorizontalBox(const char *title) override
    {
    }
    void closeBox() override
    {
    }
    void declare(float *zone, const char *key, const char *val) override
    {
    }

    virtual void addButton(const char *label, float *zone)
    {
        if (_trigger == nullptr)
            _trigger = zone;
    }

    virtual void addNumEntry(const char *label, float *zone, float init, float min, float max, float step)
    {
    }

    virtual void addCheckButton(const char *label, float *zone)
    {
        addHorizontalSlider(label, zone, 0, 0, 1, 1);
    }

    virtual void addVerticalSlider(const char *label, float *zone, float init, float min, float max, float step) override
    {
        addHorizontalSlider(label, zone, init, min, max, step);
    }

    void addHorizontalSlider(const char *label, float *zone, float init, float min, float max, float step) override
    {
        for (size_t i = 0; i < LEN_OF(param); i++)
        {
            if (param[i].name == nullptr)
            {
                param[i].init(label, zone, init, min, max);
                return;
            }
        }
    }

public:
    FaustEngine() : machine::Engine(engine_props)
    {
        // static_assert(sizeof(T) < 150000, "");

        _faust.init(48000);
        _faust.buildUserInterface(this);
    }

    ~FaustEngine() override
    {
    }

    float base_frequency = 440.f;
    float base_pitch = 69.f;

    float note_to_frequency(float note)
    {
        return base_frequency * powf(2.f, (note - base_pitch) / 12.f);
    }

    void process(const machine::ControlFrame &frame, OutputFrame &of) override
    {
        if (_trigger != nullptr)
            *_trigger = frame.trigger ? 1.f : 0.f;

        // auto note = (float)frame.midi.key + _pitch * 12.f + (frame.midi.pitch / 128) + frame.cv_voltage * 12;
        // auto f = note_to_frequency(note + 12);

        float *outputs[] = {bufferL, bufferR};
        float *ins[] = {machine::get_aux(AUX_L), machine::get_aux(AUX_R)};

        _faust.compute(FRAME_BUFFER_SIZE, &ins[0], &outputs[0]);

        of.out = bufferL;

        if (_faust.getNumOutputs() > 1)
            of.aux = bufferR;
    }
};