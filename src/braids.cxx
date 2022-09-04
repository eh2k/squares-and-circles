#include "stmlib/stmlib.h"
#include "stmlib/dsp/dsp.h"
#include "machine.h"
#include "braids/macro_oscillator.h"
#include "braids/envelope.h"
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
    uint8_t _shape;
    uint16_t _timbre;
    uint16_t _color;
    uint16_t _attack;
    uint16_t _decay;

    float buffer[FRAME_BUFFER_SIZE];

    BraidsEngine() : Engine(TRIGGER_INPUT|VOCT_INPUT)
    {
        settings.Init();
        osc.Init();
        jitter_source.Init();
        envelope.Init();

        memset(audio_samples, 0, sizeof(audio_samples));
        memset(sync_samples, 0, sizeof(sync_samples));

        // settings.SetValue(SETTING_AD_VCA, true);
        settings.SetValue(SETTING_SAMPLE_RATE, 5);
        settings.SetValue(SETTING_PITCH_OCTAVE, 4);
        settings.SetValue(SETTING_PITCH_RANGE, PITCH_RANGE_EXTERNAL);

        param[0].init_v_oct("Freq", &_pitch);
        param[1].init("Shape", &_shape, braids::MACRO_OSC_SHAPE_CSAW, braids::MACRO_OSC_SHAPE_CSAW, braids::MACRO_OSC_SHAPE_LAST - 1);
        param[2].init("Timbre", &_timbre, INT16_MAX);
        param[3].init("Color", &_color, INT16_MAX);
        param[4].init("Decay", &_decay, INT16_MAX);
        param[5].init("Attack", &_attack, 0);
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        envelope.Update(_attack / 512, _decay / 512);

        if (frame.trigger)
        {
            osc.Strike();
            envelope.Trigger(braids::ENV_SEGMENT_ATTACK);
        }

        if (frame.gate)
        {
            //Not working witch Attack > 0
            //envelope.Trigger(braids::ENV_SEGMENT_DECAY);
        }

        uint32_t ad_value = envelope.Render();

        osc.set_shape((braids::MacroOscillatorShape)_shape);

        float pitchV = (_pitch - 1) + frame.cv_voltage();
        int32_t pitch = (pitchV * 12.0 + machine::DEFAULT_NOTE + 24) * 128;

        // if (!settings.meta_modulation())
        // {
        //     pitch += settings.adc_to_fm(adc_3);
        // }

        pitch += jitter_source.Render(settings.vco_drift());
        pitch += ad_value * settings.GetValue(SETTING_AD_FM) >> 7;

        CONSTRAIN(pitch, 0, 16383);

        if (settings.vco_flatten())
            pitch = stmlib::Interpolate88(braids::lut_vco_detune, pitch << 2);

        osc.set_parameters(_timbre >> 1, _color >> 1);

        osc.set_pitch(pitch + settings.pitch_transposition());

        osc.Render(sync_samples, audio_samples, FRAME_BUFFER_SIZE);

        uint32_t gain = _decay < UINT16_MAX ? ad_value : UINT16_MAX;

        for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
            audio_samples[i] = (gain * audio_samples[i]) / UINT16_MAX;

        of.push(audio_samples, LEN_OF(audio_samples));
    }

    void onDisplay(uint8_t *buffer) override
    {
        param[1].name = braids::settings.metadata(braids::Setting::SETTING_OSCILLATOR_SHAPE).strings[_shape];

        if (_decay < UINT16_MAX)
        {
            param[4].name = "Decay";
            param[5].name = "Attack";
        }
        else
        {
            param[4].name = "VCA-off";
            param[5].name = nullptr;
        }

        gfx::drawEngine(buffer, this);
    }
};

void init_braids()
{
    machine::add<BraidsEngine>(M_OSC, "Waveforms");
}

MACHINE_INIT(init_braids);