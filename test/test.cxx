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

namespace machine
{
    float midi_bpm = 0;

    static std::vector<engine_def> registry;


    void add(const engine_def &def)
    {
        registry.push_back(def);
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

#define MACHINE_INIT(init_fun)             \
    static struct dummy_##init_fun         \
    {                                      \
        dummy_##init_fun() { init_fun(); } \
    } dummy_##init_fun

//#include "../src/peaks.cxx"
//#include "../src/braids.cxx"
#include "../src/rings.cxx"

// #include "../src/tr909_samples.cxx"
// #include "../src/clap.cxx"
// #include "../src/plaits.cxx"
// #include "../src/resonator.cxx"
// #include "../src/fx_reverb.cxx"
// #include "../src/fx_delay.cxx"
// #include "../src/voltage.cxx"
// #include "../src/speech.cxx"

int main()
{
    std::map<const char *, bool> machines;

    for (int j = 0; j < machine::registry.size(); j++)
    {
        auto audio_in = machine::registry[0].init();

        std::vector<int16_t> buffer;

        auto &r = machine::registry[j];

        if (machines[r.machine] == false)
        {
            printf("### %s\n", r.machine);
            machines[r.machine] = true;
        }

        printf("* **%s**\n", r.engine);
        // if (j != 1)
        //      continue;

        auto engine = r.init();

        printf("````\n");
        printf("Parameters:\n");

        uint16_t params[8];
        auto names = engine->GetParams(params);
        for (int n = 0; names[n] != nullptr; n++)
            printf(" - %s\n", names[n]);

        printf("````\n");

        // params[4] = UINT16_MAX / 3;
        // params[5] = INT16_MAX;
        // engine->SetParams(params);

        int16_t d;
        machine::ControlFrame frame;
        float tmp[machine::FRAME_BUFFER_SIZE] = {};

        for (float v = -3; v < 3; v += 1.f)
        {
            for (int i = 0; i < 48000*2;)
            {
                frame.trigger = i == 0;
                frame.cv_voltage = v;

                frame.audio_in[0] = tmp;
                frame.audio_in[1] = tmp;

                audio_in->Process(frame, &frame.audio_in[0], &frame.audio_in[0]);

                if (frame.audio_in[1] == nullptr)
                    frame.audio_in[1] = frame.audio_in[0];

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
                    if(aux != nullptr)
                        v += (aux[k]) * INT16_MAX;

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
            sprintf(name, "%.2d_%s.wav", j, r.engine);
            write_wav(buffer, name);
        }
    }

    return 0;
}