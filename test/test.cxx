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
    return random() % howbig;
}

namespace gfx
{
    void drawPixel(uint8_t *buffer, int16_t x, int16_t y) {}
    void drawLine(uint8_t *buffer, int x1, int y1, int x2, int y2) {}
    void drawRect(uint8_t *buffer, int x1, int y1, int w, int h) {}
    void drawXbm(uint8_t *buffer, int16_t x, int16_t y, int16_t width, int16_t height, const uint8_t *xbm) {}
    void drawString(uint8_t *buffer, int16_t x, int16_t y, const char *text, uint8_t font) {}
    void drawEngine(uint8_t *buffer, machine::Engine *engine) {}
    void DrawKnob(unsigned char*, int, int, char const*, unsigned short, bool) {}
}

namespace machine
{
    static struct : MidiHandler
    {
        void midiReceive(uint8_t midiByte) override {}
        void midiReset() override {}
        bool getMidiNote(int midi_channel, int midi_voice, MidiNote *note) override { return false; }
    } _dummy;

    MidiHandler *midi_handler = &_dummy;

    float get_bpm()
    {
        return 120;
    }

    uint32_t trigger[4] = {}; // millis()
    float cv_voltage[4] = {};

    template <>
    float get_cv<float>(int src)
    {
        return cv_voltage[src];
    }

    uint32_t get_trigger(int src)
    {
        return trigger[src];
    }

    float audio_in[2][FRAME_BUFFER_SIZE];

    template <>
    float *get_aux<float>(int src)
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

    struct EngineDef
    {
        const char* engine;
        std::function<Engine *()> init;
    };

    static std::vector<EngineDef> registry;

    void add(const char *machine, const char *engine, std::function<Engine *()> createFunc)
    {
        registry.push_back({ engine, createFunc });
    }

    void add_modulation_source(const char *name, std::function<void(Parameter *)> createFunc)
    {
    }

    void add_quantizer_scale(const char *name, const QuantizerScale& scale)
    {        
    }

    void *malloc(size_t size)
    {
        auto ptr = ::malloc(size);
        memset(ptr, 0, size);
        return ptr;
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

int main()
{
    MACHINE_INIT(init_voltage);
    MACHINE_INIT(init_peaks);
    MACHINE_INIT(init_braids);
    MACHINE_INIT(init_plaits);
    MACHINE_INIT(init_samples_tr909);
    MACHINE_INIT(init_samples_tr707);
    MACHINE_INIT(init_clap);
    MACHINE_INIT(init_reverb);
    MACHINE_INIT(init_faust);
    MACHINE_INIT(init_rings);
    MACHINE_INIT(init_speech);
    MACHINE_INIT(init_sam);
    MACHINE_INIT(init_delay);

    MACHINE_INIT(init_modulations);

    //return;

    std::map<const char *, bool> machines;

    for (int j = 0; j < machine::registry.size(); j++)
    {
        auto audio_in = machine::registry[0].init();

        std::vector<int16_t> buffer;

        auto &r = machine::registry[j];

        printf("* **%s**\n", r.engine);
        // if (j != 1)
        //      continue;

        auto engine = r.init();

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

        for (float v = -3; v < 3; v += 1.f)
        {
            for (int i = 0; i < 48000 * 2;)
            {
                frame.trigger = i == 0;
                frame.cv_voltage = v;

                float *ins[] = {machine::audio_in[0], machine::audio_in[1]};

                audio_in->Process(frame, &ins[0], &ins[1]);

                float *out = nullptr;
                float *aux = nullptr;
                engine->Process(frame, &out, &aux);

                if (out == nullptr)
                    break;

                const int n = 2000;
                // if (buffer.size() > n * 2 &&
                //     !memcmp(&buffer[buffer.size() - n * 2], &buffer[buffer.size() - n], n))
                //     break;

                for (int k = 0; k < machine::FRAME_BUFFER_SIZE; k++)
                {
                    auto v = (out[k]) * INT16_MAX;
                    if (aux != nullptr)
                        v += (aux[k]) * INT16_MAX;

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