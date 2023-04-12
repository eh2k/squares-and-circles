#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>
#include <map>

void write_wav(const std::vector<int16_t> &buffer, const std::string &fileName)
{
    typedef struct WAV_HEADER
    {
        /* RIFF Chunk Descriptor */
        uint8_t RIFF[4] = {'R', 'I', 'F', 'F'}; // RIFF Header Magic header
        uint32_t ChunkSize;                     // RIFF Chunk Size
        uint8_t WAVE[4] = {'W', 'A', 'V', 'E'}; // WAVE Header
        /* "fmt" sub-chunk */
        uint8_t fmt[4] = {'f', 'm', 't', ' '}; // FMT header
        uint32_t Subchunk1Size = 16;           // Size of the fmt chunk
        uint16_t AudioFormat = 1;              // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM
                                               // Mu-Law, 258=IBM A-Law, 259=ADPCM
        uint16_t NumOfChan = 1;                // Number of channels 1=Mono 2=Sterio
        uint32_t SamplesPerSec = 48000;        // Sampling Frequency in Hz
        uint32_t bytesPerSec = 48000 * 2;      // bytes per second
        uint16_t blockAlign = 2;               // 2=16-bit mono, 4=16-bit stereo
        uint16_t bitsPerSample = 16;           // Number of bits per sample
        /* "data" sub-chunk */
        uint8_t Subchunk2ID[4] = {'d', 'a', 't', 'a'}; // "data"  string
        uint32_t Subchunk2Size;                        // Sampled data length
    } wav_hdr;

    static_assert(sizeof(wav_hdr) == 44, "");

    auto fsize = buffer.size() * sizeof(int16_t);
    std::string in_name = "test.bin"; // raw pcm data without wave header

    wav_hdr wav;
    wav.ChunkSize = fsize + sizeof(wav_hdr) - 8;
    wav.Subchunk2Size = fsize + sizeof(wav_hdr) - 44;

    std::ofstream out(fileName, std::ios::binary);
    out.write(reinterpret_cast<const char *>(&wav), sizeof(wav));
    out.write(reinterpret_cast<const char *>(&buffer[0]), fsize);
}

#include "machine.h"
#include "stmlib/dsp/dsp.h"

uint32_t random(uint32_t howbig)
{
    if (howbig == 0)
        return 0;
    return std::rand() % howbig;
}

namespace gfx
{
    uint8_t* display_buffer = nullptr;
    void setColor(uint8_t color) {}
    void drawPixel(int x, int y, uint8_t color) {}
    void drawLine(int x1, int y1, int x2, int y2) {}
    void drawRect(int x1, int y1, int w, int h) {}
    void drawXbm(int x, int y, int width, int height, const uint8_t *xbm) {}
    void drawString(int x, int y, const char *text, uint8_t font) {}
    void drawEngine(machine::Engine *engine, const char* infoMsg) {}
    void DrawKnob(int, int, char const *, unsigned short, bool) {}
}

namespace machine
{
    bool MidiHandler::enabled() { return false; }

    static struct : MidiHandler
    {
        void midiReceive(uint8_t midiByte) override {}
        void midiReset() override {}
    } _dummy;

    MidiHandler *midi_handler = &_dummy;

    Screensaver* screensaver = nullptr;

    uint32_t &get_bpm()
    {
        static uint32_t bpm = 120 * 100;
        return bpm;
    }

    uint32_t digital_inputs; // millis()
    float cv_voltage[4] = {};

    template <>
    float get_cv<float>(int src)
    {
        return cv_voltage[src];
    }

    int get_io_info(int type, int index, char *name)
    {
        return 0;
    }

    bool get_trigger(int src)
    {
        return digital_inputs & (1 << src);
    }
    bool get_gate(int src)
    {
        return digital_inputs & (8 << src);
    }

    float audio_in[2][FRAME_BUFFER_SIZE];

    void get_audio(int src, float* mix_buffer, float gain)
    {
        
    }

    float *get_aux(AUX src)
    {
        if (src == -1)
        {
            return audio_in[1];
        }

        if (src == -2)
        {
            return audio_in[2];
        }

        return nullptr;
    }

    float *tmp_buff()
    {
        static float __tmp[machine::FRAME_BUFFER_SIZE * 12];
        static int __tmpP = 0;

        __tmpP += machine::FRAME_BUFFER_SIZE;
        __tmpP %= LEN_OF(__tmp);
        return &__tmp[__tmpP];
    }

    template <typename T>
    void _push(T *buff, size_t len, float f, OutputFrame *out)
    {
        auto tmp = tmp_buff();

        if (out->out == nullptr)
            out->out = tmp;
        else if (out->aux == nullptr)
            out->aux = tmp;

        T *buff2 = buff;
        for (size_t i = 0; i < len; i++)
        {
            for (size_t j = 0; j < (machine::FRAME_BUFFER_SIZE / len); j++)
                *tmp++ = (float)*buff2 * f;

            ++buff2;
        }
    }

    template <>
    void OutputFrame::push(float *buff, size_t len)
    {
        _push(buff, len, 1.f, this);
    }

    template <>
    void OutputFrame::push(int16_t *buff, size_t len)
    {
        _push(buff, len, 1.f / INT16_MAX, this);
    }

    template <>
    void OutputFrame::push(int32_t *buff, size_t len)
    {
        _push(buff, len, 1.f / machine::PITCH_PER_OCTAVE, this);
    }

    struct EngineDef
    {
        const char *engine;
        std::function<Engine *()> init;
    };

    static std::vector<EngineDef> registry;

    void add(const char *machine, const char *engine, std::function<Engine *()> createFunc)
    {
        registry.push_back({engine, createFunc});
    }

    void ModulationSource::display(int x, int y)
    {
    }

    void add_modulation_source(const char *name, std::function<void(Parameter *)> createFunc)
    {
    }

    std::pair<const char *, const machine::QuantizerScale *> quantizer_scales[UINT8_MAX] = {};

    void add_quantizer_scale(const char *name, const QuantizerScale &scale)
    {
        auto p = &quantizer_scales[0];

        while (p->first != nullptr)
            p++;

        if (p < (&quantizer_scales[0] + LEN_OF(quantizer_scales)))
        {
            p->first = name;
            auto s = &scale;
            p->second = s;
        }
    }

    const std::pair<const char *, const QuantizerScale *> &get_quantizer_scale(uint8_t index)
    {
        if (index < LEN_OF(quantizer_scales))
            return quantizer_scales[index];
        else
            return quantizer_scales[0];
    }

    void *malloc(size_t size)
    {
        printf("MALLOC %d\n", size);
        auto ptr = ::malloc(size);
        memset(ptr, 0, size);
        return ptr;
    }

    void mfree(void* p)
    {
        ::free(p);
    }

    void free(machine::Engine *&ptr)
    {
        if (ptr != nullptr)
        {
            ptr->~Engine();
            ::free(ptr);
            ptr = nullptr;
        }
    }
}

#undef MACHINE_INIT
#define MACHINE_INIT(init_fun) \
    extern void init_fun();    \
    init_fun();

#include "plaits/dsp/voice.h"

int test_seq()
{
    machine::registry.clear();

    MACHINE_INIT(init_quantizer);
    //MACHINE_INIT(init_tb_3po);
    //MACHINE_INIT(init_acid_sequencer);
    MACHINE_INIT(init_plaits);

    machine::Engine *seq = nullptr;

    for (int j = 0; j < machine::registry.size(); j++)
    {
        std::vector<int16_t> buffer;

        auto &r = machine::registry[j];

        auto engine = r.init();
        engine->init();

        if (engine->props & machine::SEQUENCER_ENGINE)
        {
            seq = r.init();
            seq->init();
        }

        int16_t d;
        machine::ControlFrame frame;
        float tmp[machine::FRAME_BUFFER_SIZE] = {};

        for (int t = 0; t < 16; t++)
        {
            for (int i = 0; i < 48000 / 6;)
            {
                frame.trigger = i == 0;
                frame.gate = i < 48000 / 12;
                frame.cv_voltage_ = 0;
                frame.accent = 0;

                int n = (48000 / 6 / 24);

                frame.clock = 1 + ((buffer.size() / n) % 96);
                if ((buffer.size() % n) > 1)
                    frame.clock = 0;

                const float *ins[] = {machine::audio_in[0], machine::audio_in[1]};

                if (seq != nullptr && !(engine->props & machine::SEQUENCER_ENGINE))
                {
                    machine::OutputFrame of;
                    seq->process(frame, of);
                    frame.cv_voltage_ = (-2.f + of.out[0]) * machine::PITCH_PER_OCTAVE;
                    static bool last = false;

                    frame.trigger = !last && of.aux[0] > 0.1f;
                    frame.accent = false; // frame.trigger;
                    last = frame.trigger;
                }

                machine::OutputFrame of;
                engine->process(frame, of);

                frame.t++;

                for (int k = 0; k < machine::FRAME_BUFFER_SIZE; k++)
                {
                    auto v = (-of.out[k]) * INT16_MAX;
                    // if (of.aux != nullptr)
                    //      v += (-of.aux[k]) * INT16_MAX / 5;

                    CONSTRAIN(v, INT16_MIN, INT16_MAX);
                    buffer.push_back(v);
                    i++;
                }
            }
        }

        free(engine);

        if (buffer.size() > 0)
        {
            char name[128];
            sprintf(name, "%.2d_%s_seq.wav", j, r.engine);
            write_wav(buffer, name);
        }
    }

    return 0;
}

int main()
{
    //test_seq();

    machine::registry.clear();

    // MACHINE_INIT(init_midi_clock);
    // MACHINE_INIT(init_acid_sequencer);
    // MACHINE_INIT(init_voltage);
    MACHINE_INIT(init_noise);
    // MACHINE_INIT(init_padsynth);
    // MACHINE_INIT(init_open303);
    MACHINE_INIT(init_braids);
    MACHINE_INIT(init_plaits);
    MACHINE_INIT(init_peaks);
    MACHINE_INIT(init_sample_roms);
    MACHINE_INIT(init_clap);
    MACHINE_INIT(init_faust);
    MACHINE_INIT(init_rings);
    MACHINE_INIT(init_speech);
    MACHINE_INIT(init_sam);
    // MACHINE_INIT(init_reverb);
    // MACHINE_INIT(init_delay);
    // MACHINE_INIT(init_modulations);

    // return;

    machine::Engine *seq = nullptr;
    machine::IOConfig ioconf { 1, 1, 0, 0 };

    std::map<const char *, bool> machines;

    for (int j = 0; j < machine::registry.size(); j++)
    {
        std::vector<int16_t> buffer;

        auto &r = machine::registry[j];

        printf("* **%s**\n", r.engine);
        // if (j != 1)
        //      continue;

        auto engine = r.init();
        engine->io = &ioconf;
        engine->init();

        printf("````\n");
        printf("Parameters:\n");

        for (int n = 0; n < LEN_OF(engine->param) && engine->param[n].name != nullptr; n++)
            printf(" - %s\n", engine->param[n].name);

        printf("````\n");

        // params[4] = UINT16_MAX / 3;
        // params[5] = INT16_MAX;
        // engine->SetParams(params);

        int16_t d;
        machine::ControlFrame frame;
        float tmp[machine::FRAME_BUFFER_SIZE] = {};

        // for (float v = -6; v < 6; v += 1.f)

        for (float v = -3; v < 3; v += 1.f)
        {
            for (int i = 0; i < 48000;)
            {
                frame.trigger = i == 0;
                frame.cv_voltage_ = v * machine::PITCH_PER_OCTAVE;
                frame.accent = 0;

                int n = (48000 / 6 / 24);

                frame.clock = 1 + ((buffer.size() / n) % 96);
                if ((buffer.size() % n) > 1)
                    frame.clock = 0;

                const float *ins[] = {machine::audio_in[0], machine::audio_in[1]};

                machine::OutputFrame of;
                engine->process(frame, of);

                frame.t++;

                if (of.out == nullptr)
                    break;

                // const int n = 2000;
                //  if (buffer.size() > n * 2 &&
                //      !memcmp(&buffer[buffer.size() - n * 2], &buffer[buffer.size() - n], n))
                //      break;

                for (int k = 0; k < machine::FRAME_BUFFER_SIZE; k++)
                {
                    auto v = (-of.out[k]) * INT16_MAX;
                    // if (of.aux != nullptr)
                    //      v += (-of.aux[k]) * INT16_MAX / 5;

                    CONSTRAIN(v, INT16_MIN, INT16_MAX);
                    buffer.push_back(v);
                    i++;
                }
            }

            for (int k = 0; k < LEN_OF(engine->param) && engine->param[k].name != nullptr; k++)
            {
                auto val = engine->param[k].to_uint16();
                engine->param[k].from_uint16(UINT16_MAX);
                engine->param[k].from_uint16(val);
            }

            // engine->param[1].apply_encoder(1, false);
            // break;
        }

        free(engine);

        if (buffer.size() > 0)
        {
            char name[128];
            sprintf(name, "%.2d_%s.wav", j, r.engine);
            write_wav(buffer, name);
        }
    }

    return 0;
}