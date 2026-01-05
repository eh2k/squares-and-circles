
#include <algorithm>
#include <cstring>
#include <fstream>
#include <inttypes.h>
#include <iostream>
#include <map>
#include <vector>

#include "../lib/misc/noise.hxx"
#include "../lib/plaits/dsp/oscillator/oscillator.h"
#include "../lib/plaits/resources.h"

#define ASSERT_EQ(val1, val2)                                                                                          \
    if (((val1) != (val2)))                                                                                            \
    {                                                                                                                  \
        std::cout << "ASSERT FAILED: " << __FILE__ << ":" << __LINE__ << ": " << #val1 << " != " << val2 << std::endl; \
        exit(1);                                                                                                       \
    }                                                                                                                  \
    else                                                                                                               \
        std::cout << "ASSERT OK: " << __FILE__ << ":" << __LINE__ << ": " << #val1 << " == " << val2 << std::endl;

#define ASSERT_NEQ(val1, val2)                                                                                         \
    if (((val1) == (val2)))                                                                                            \
    {                                                                                                                  \
        std::cout << "ASSERT FAILED: " << __FILE__ << ":" << __LINE__ << ": " << #val1 << " != " << val2 << std::endl; \
        exit(1);                                                                                                       \
    }                                                                                                                  \
    else                                                                                                               \
        std::cout << "ASSERT OK: " << __FILE__ << ":" << __LINE__ << ": " << #val1 << " == " << val2 << std::endl;

void write_wav(const std::vector<int16_t> &buffer, int nchannels, const std::string &fileName)
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
    wav.NumOfChan = nchannels;
    wav.blockAlign *= nchannels;
    wav.ChunkSize = fsize + sizeof(wav_hdr) - 8;
    wav.Subchunk2Size = fsize + sizeof(wav_hdr) - 44;

    std::ofstream out(fileName, std::ios::binary);
    out.write(reinterpret_cast<const char *>(&wav), sizeof(wav));
    out.write(reinterpret_cast<const char *>(&buffer[0]), fsize);
}

int nchannels = 1;
std::vector<int16_t> buffer;

#define clock scclock

extern void (*__midi_event_handler_ptr)();
extern void (*__ui_event_handler_ptr)();

void __ui_event_handler()
{
}

#define NO_UI_EVENT_HANDLER
#define NO_MIDI_EVENT_HANDLER
#include "squares-and-circles-api.h"

UI_EVENT_t *__ui_event = nullptr;
MIDI_EVENT_t *__midi_event = nullptr;
IO_t *__io = new IO_t();

float *__audio_in_l_fp = new float[FRAME_BUFFER_SIZE];
float *__audio_in_r_fp = new float[FRAME_BUFFER_SIZE];
float **__audio_in_l_fpp = &__audio_in_l_fp;
float **__audio_in_r_fpp = &__audio_in_r_fp;
float *__output_l_fp = new float[FRAME_BUFFER_SIZE];
float *__output_r_fp = new float[FRAME_BUFFER_SIZE];
int16_t *__output_l_i16p = new int16_t[FRAME_BUFFER_SIZE];
int16_t *__output_r_i16p = new int16_t[FRAME_BUFFER_SIZE];

uint32_t *__engine_props = new uint32_t[1];
uint32_t *__trig = new uint32_t[1];
uint32_t *__gate = new uint32_t[1];
uint32_t *__accent = new uint32_t[1];
int32_t *__cv = new int32_t[1];
uint32_t *__t = new uint32_t[1]{0};
uint32_t *__samples_per_step = new uint32_t[1]{3000 / 2}; // 120BPM

int32_t *_paramP = nullptr;
int32_t _paramMax = 0;

EXTERN_C
{
    void addParam_f32(const char *name, float *value, float min = 0.f, float max = 1.f) // min...max
    {
    }
    void addParam_i32(const char *name, int32_t *value, int32_t min, int32_t max, const char **valueMap) // 0...max
    {
        if (_paramP == nullptr)
        {
            _paramP = value;
            _paramMax = max;
        }
    }
    void setParamName(const void *valuePtr, const char *name)
    {
    }
    void drawRect(int x, int y, int w, int h)
    {
    }
    void fillRect(int x, int y, int w, int h)
    {
    }
    void drawCircle(int x, int y, int r)
    {
    }
    void drawXbm(int x, int y, int width, int height, const uint8_t *xbm)
    {
    }
    void clearRect(int x, int y, int w, int h)
    {
    }

    bool qz_enabled()
    {
        return true;
    }
    int16_t qz_lookup(int8_t note)
    {
        // return ((int32_t)note - 64) << 7;
        return ((int32_t)note - DEFAULT_NOTE) * (PITCH_PER_OCTAVE / 12);
    }

    int32_t qz_process(int32_t pitch, int8_t *note)
    {
        return pitch;
    }
}

void write_frame()
{
    if (__output_l_i16p[0] != 0x7FFF)
    {
        for (int k = 0; k < FRAME_BUFFER_SIZE; k++)
        {
            auto v = (__output_l_i16p[k]);
            buffer.push_back(v);

            if (__output_r_i16p[0] != 0xFFFF)
            {
                nchannels = 2;
                v = (__output_r_i16p[k]);
                buffer.push_back(v);
            }
        }
    }
    else
    {
        for (int k = 0; k < FRAME_BUFFER_SIZE; k++)
        {
            CONSTRAIN(__output_l_fp[k], -1, 1);
            auto v = (__output_l_fp[k]) * INT16_MAX;
            buffer.push_back(v);

            if (__output_r_fp[0] > -10000)
            {
                nchannels = 2;
                CONSTRAIN(__output_l_fp[k], -1, 1);
                v = (__output_r_fp[k]) * INT16_MAX;
                buffer.push_back(v);
            }
        }
    }
}

int main()
{
    engine::setup();

    std::fill_n(__output_l_i16p, FRAME_BUFFER_SIZE, 0x7FFF);
    std::fill_n(__output_r_i16p, FRAME_BUFFER_SIZE, 0x7FFF);
    std::fill_n(__output_l_fp, FRAME_BUFFER_SIZE, -10000);
    std::fill_n(__output_r_fp, FRAME_BUFFER_SIZE, -10000);

    // engine::process();

    WhiteNoise noise;
    plaits::Oscillator osc;
    osc.Init();
    float phase_inc_ = 440.f / 2 / SAMPLE_RATE;

    __cv[0] = -PITCH_PER_OCTAVE * 2;
    if (_paramP)
        _paramP[0]--;

    for (size_t i = 0; i < SAMPLE_RATE * 128; i += FRAME_BUFFER_SIZE)
    {
        for (size_t j = 0; j < FRAME_BUFFER_SIZE; j++)
        {
            __audio_in_l_fp[j] = noise.nextf(-1, 1);
            __audio_in_r_fp[j] = noise.nextf(-1, 1);
        }

        osc.Render<plaits::OSCILLATOR_SHAPE_SAW>(phase_inc_, 0.5f, __audio_in_l_fp, FRAME_BUFFER_SIZE);
        memcpy(__audio_in_r_fp, __audio_in_l_fp, sizeof(float) * FRAME_BUFFER_SIZE);

        __trig[0] = (i % (SAMPLE_RATE / 2)) == 0;
        __gate[0] = (i % (SAMPLE_RATE / 2)) < SAMPLE_RATE / 3;

        if (__trig[0])
            __cv[0] += PITCH_PER_OCTAVE / 12;

        if (i % (SAMPLE_RATE) == 0)
        {
            if (_paramP)
            {
                _paramP[0]++;
                if (_paramP[0] > _paramMax)
                    break;
            }

            __cv[0] = -PITCH_PER_OCTAVE * 2;
        }

        engine::process();
        __t[0]++;

        write_frame();
    }
    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
        printf("%x ", buffer[i]);
    printf("\n");
    write_wav(buffer, nchannels, "test.wav");
    return 0;
}