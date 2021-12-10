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

template <class T>
class FaustEngine : public machine::Engine, UI
{
    static_assert(std::is_base_of<dsp, T>::value, "T must derive from dsp");

    float bufferL[FRAME_BUFFER_SIZE];
    float bufferR[FRAME_BUFFER_SIZE];

    float _pitch = 0.5f;

    T _faust;
    static constexpr int _params_n = 8;
    const char *_param_names[_params_n] = {};

    struct
    {
        float *value;
        float min;
        float max;
        float step;
        void from_uint16(uint16_t v)
        {
            *value = min + ((float)v / UINT16_MAX) * (max - min);
        }
        uint16_t to_uint16()
        {
            return (*value - min) / (max - min) * UINT16_MAX;
        }
    } _param_values[_params_n] = {};

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
        for (size_t i = 0; i < _params_n - 1; i++)
        {
            if (_param_names[i] == nullptr)
            {
                _param_names[i] = label;
                _param_values[i].value = zone;
                _param_values[i].min = min;
                _param_values[i].max = max;
                _param_values[i].step = step;
                *_param_values[i].value = init;

                _param_names[i + 1] = nullptr;
                _param_values[i + 1].value = nullptr;
                return;
            }
        }
    }

public:
    FaustEngine() : machine::Engine(machine::AUDIO_PROCESSOR)
    {
        // static_assert(sizeof(T) < 150000, "");

        memset(_param_values, 0, sizeof(_param_values));
        _faust.init(48000);
        _faust.buildUserInterface(this);
    }

    ~FaustEngine() override
    {
    }

    double base_frequency = 440.0;
    double base_pitch = 69.0;

    double note_to_frequency(double note)
    {
        return base_frequency * pow(2.0, (note - base_pitch) / 12.0);
    }

    void Process(const machine::ControlFrame &frame, float **out, float **aux) override
    {
        if (_trigger != nullptr)
            *_trigger = frame.trigger ? 1.f : 0.f;

        auto note = (float)frame.midi.key + _pitch * 12.f + (frame.midi.pitch / 128) + frame.cv_voltage * 12;

        // auto f = note_to_frequency(note + 12);

        float *outputs[] = {bufferL, bufferR};
        float *ins[] = {frame.audio_in[0], frame.audio_in[1]};
        _faust.compute(FRAME_BUFFER_SIZE, &ins[0], &outputs[0]);

        *out = bufferL;

        if (_faust.getNumOutputs() > 1)
            *aux = bufferR;
    }

    void SetParams(const uint16_t *params) override
    {
        //_pitch = float_range_from_uint16_t(params[0]);
        for (int i = 0; i < _params_n; i++)
        {
            if (_param_values[i].value != nullptr)
                _param_values[i].from_uint16(params[i]);
        }
    }

    const char **GetParams(uint16_t *values) override
    {
        for (int i = 0; i < _params_n; i++)
        {
            if (_param_names[i] != nullptr)
                values[i] = _param_values[i].to_uint16();
        }

        return _param_names;
    }
};


#include "faust/djembe.hxx"
#include "faust/rev_dattorro.hxx"

void init_faust()
{
    //(char[sizeof(mydsp)])"";

    machine::add<FaustEngine<djembe>>(machine::DRUM, "Djembe");
    machine::add<FaustEngine<rev_dattorro>>(machine::FX, "Rev-Dattorro");
}

MACHINE_INIT(init_faust);
