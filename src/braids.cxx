#include "stmlib/stmlib.h"
#include "stmlib/dsp/dsp.h"
#include "machine.h"
#include "braids/macro_oscillator.h"
#include "braids/envelope.h"
#include "braids/quantizer.h"
#include "braids/quantizer_scales.h"
#include "braids/settings.h"
#include "braids/vco_jitter_source.h"

using namespace braids;
using namespace machine;

struct BraidsEngine : public Engine
{
    MacroOscillator osc;
    Envelope envelope;
    VcoJitterSource jitter_source;

    int16_t audio_samples[FRAME_BUFFER_SIZE];
    uint8_t sync_samples[FRAME_BUFFER_SIZE];

    float _pitch;
    uint16_t _shape;
    uint16_t _timbre;
    uint16_t _color;
    uint16_t _attack;
    uint16_t _decay;

    uint16_t timbre;
    uint16_t color;

    float buffer[FRAME_BUFFER_SIZE];

    BraidsEngine()
    {
        settings.Init();
        osc.Init();
        jitter_source.Init();
        envelope.Init();

        memset(audio_samples, 0, sizeof(audio_samples));
        memset(sync_samples, 0, sizeof(sync_samples));

        uint16_t defaults[] = {float_range_to_uint16_t(0), braids::MACRO_OSC_SHAPE_CSAW, INT16_MAX, INT16_MAX, INT16_MAX, 0};
        SetParams(defaults);
        timbre = _timbre;
        color = _color;

        // settings.SetValue(SETTING_AD_VCA, true);
        settings.SetValue(SETTING_SAMPLE_RATE, 5);
        settings.SetValue(SETTING_PITCH_OCTAVE, 4);
        settings.SetValue(SETTING_PITCH_RANGE, PITCH_RANGE_EXTERNAL);
    }

    void Process(const ControlFrame &frame, float **out, float **aux) override
    {
        envelope.Update(_attack / 512, _decay / 512);

        if (frame.trigger)
        {
            osc.Strike();
            envelope.Trigger(braids::ENV_SEGMENT_ATTACK);
        }

        if (frame.midi.on > 0)
        {
            envelope.Trigger(braids::ENV_SEGMENT_ATTACK);
        }

        uint32_t ad_value = envelope.Render();

        osc.set_shape((braids::MacroOscillatorShape)_shape);

        float pitchV = (_pitch - 1) + frame.cv_voltage;
        int32_t pitch = (pitchV * 12.0 + frame.midi.key + 24) * 128 + frame.midi.pitch;

        // if (!settings.meta_modulation())
        // {
        //     pitch += settings.adc_to_fm(adc_3);
        // }

        pitch += jitter_source.Render(settings.vco_drift());
        pitch += ad_value * settings.GetValue(SETTING_AD_FM) >> 7;

        CONSTRAIN(pitch, 0, 16383);

        if (settings.vco_flatten())
            pitch = stmlib::Interpolate88(braids::lut_vco_detune, pitch << 2);

        ONE_POLE(timbre, _timbre, 0.005f);
        ONE_POLE(color, _color, 0.005f);

        osc.set_parameters(timbre >> 1, color >> 1);

        osc.set_pitch(pitch + settings.pitch_transposition());

        osc.Render(sync_samples, audio_samples, FRAME_BUFFER_SIZE);

        float gain = _decay < UINT16_MAX ? ad_value : 65535;
        gain /= UINT16_MAX;

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
            buffer[i] = ((float)audio_samples[i]) / INT16_MAX * gain;

        *out = buffer;
    }

    void OnEncoder(uint8_t param_index, int16_t inc, bool pressed) override
    {
        if (param_index == 0)
        {
            if (inc > 0)
                _pitch += !pressed ? 1 : 1.f / 12;
            else
                _pitch -= !pressed ? 1 : 1.f / 12;

            CONSTRAIN(_pitch, -4, 4);
        }
        else if (param_index == 1)
        {
            if (inc > 0)
                _shape += 1;
            else if (_shape > 0)
                _shape -= 1;

            CONSTRAIN(_shape, braids::MACRO_OSC_SHAPE_CSAW, braids::MACRO_OSC_SHAPE_QUESTION_MARK);
        }
        else
        {
            Engine::OnEncoder(param_index, inc, pressed);
        }
    }

    void SetParams(const uint16_t *params) override
    {
        _pitch = float_range_from_uint16_t(params[0]);
        _shape = params[1];
        CONSTRAIN(_shape, braids::MACRO_OSC_SHAPE_CSAW, braids::MACRO_OSC_SHAPE_QUESTION_MARK);
        _timbre = params[2];
        _color = params[3];
        _decay = params[4];
        _attack = params[5];
    }

    const char **GetParams(uint16_t *values) override
    {
        static const char *names[]{"Freq.", "", "Timbre", "Color", "Decay", "Attack", nullptr};
        values[0] = float_range_to_uint16_t(_pitch);
        static char shape[12];
        CONSTRAIN(_shape, braids::MACRO_OSC_SHAPE_CSAW, braids::MACRO_OSC_SHAPE_QUESTION_MARK);
        sprintf(shape, ">  %s", braids::settings.metadata(braids::Setting::SETTING_OSCILLATOR_SHAPE).strings[_shape]);
        names[1] = shape;

        values[1] = _shape;
        values[2] = _timbre;
        values[3] = _color;

        if (_decay < UINT16_MAX)
        {
            values[4] = _decay;
            values[5] = _attack;
        }
        else
        {
            names[4] = "VCA-off";
            values[4] = _decay;
            names[5] = nullptr;
        }

        return names;
    }
};

void init_braids()
{
    machine::add<BraidsEngine>(M_OSC, "Waveforms");
}

MACHINE_INIT(init_braids);