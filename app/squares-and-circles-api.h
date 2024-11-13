
// Copyright (C)2023 - E.Heidt
//
// Author: E.Heidt (eh2k@gmx.de)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//

////////////////////////////////////////////////////////////////////////////////
// Work in Progress: API will have breaking changes !!!!
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define V_OCT "V_OCT"
#define V_QTZ "V_QTZ"
#define SEQ_SWING "$SWING"
#define MULTI_TRIGS ">TRIGS"
#define IO_STEREOLIZE ".IO_STEREO"

constexpr uint32_t ENGINE_MODE_COMPACT = 1 << 5;
constexpr uint32_t ENGINE_MODE_CV_OUT = 1 << 6;
constexpr uint32_t ENGINE_MODE_MIDI_IN = 1 << 7;
constexpr uint32_t ENGINE_MODE_STEREOLIZED = 1 << 9;

#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef MACHINE_INTERNAL

#define LEN_OF(x) (sizeof(x) / sizeof(x[0]))
#define MOD(x, y) ((x) % (y))

#ifndef CONSTRAIN
#define CONSTRAIN(var, min, max) \
    if (var < (min))             \
    {                            \
        var = (min);             \
    }                            \
    else if (var > (max))        \
    {                            \
        var = (max);             \
    }

#endif

#ifdef __cplusplus
#ifdef __GNUC__
/* poision memory functions */
// #   pragma GCC poison _ZSt17__throw_bad_allocv
// #   pragma GCC poison new delete
// #   pragma GCC poison malloc new
// #   pragma GCC poison virtual
#endif

EXTERN_C uint32_t micros();
EXTERN_C uint32_t millis();
EXTERN_C uint32_t crc32(uint32_t crc, const void *buf, size_t size);

#endif

#ifndef DEFAULT_NOTE
#define DEFAULT_NOTE 60
#endif
#ifndef SAMPLE_RATE
#define SAMPLE_RATE 48000
#endif
#ifndef FRAME_BUFFER_SIZE
#define FRAME_BUFFER_SIZE 24
#endif
#ifndef PITCH_PER_OCTAVE
#define PITCH_PER_OCTAVE (12 << 7)
#endif

#ifndef engine

EXTERN_C
{
    extern const char *__name;
    extern uint32_t *__t;
    extern uint8_t *__clock;
    extern uint8_t *__step;
    extern uint32_t *__step_changed;
    extern uint32_t *__samples_per_step;
    extern uint32_t *__trig;
    extern uint32_t *__gate;
    extern uint32_t *__accent;
    extern int32_t *__cv;

    extern float *__output_l_fp;
    extern float *__output_r_fp;
    extern int16_t *__output_l_i16p;
    extern int16_t *__output_r_i16p;

    extern uint8_t *__mixer_level;
    extern uint8_t *__mixer_pan;

    extern float **__audio_in_l_fpp;
    extern float **__audio_in_r_fpp;

    extern uint8_t *__display_buffer_u8_p;
    extern uint32_t *__engine_props;

    extern float *__adc_voltage_f_p;
    extern uint32_t __adc_count;
    extern float *__dac_voltage_fp_p;
    extern uint32_t __dac_count;
    extern uint32_t *__digital_inputs_u32_p;
    extern uint32_t *__bpm;

    extern struct
    {
        uint16_t type;
        uint16_t control;
        int16_t value;
        uint16_t mask;
        bool handled;
    } *__ui_event;

    bool (*__ui_event_handler_ptr)(uint16_t type, uint16_t control, int16_t value, uint16_t mask) = nullptr;
    void __ui_event_handler()
    {
        if (__ui_event_handler_ptr)
            __ui_event->handled = __ui_event_handler_ptr(__ui_event->type, __ui_event->control, __ui_event->value, __ui_event->mask);
    }

    extern struct
    {
        uint8_t type;
        uint8_t key;
        int16_t value;
    } *__midi_event;

    void (*__midi_event_handler_ptr)() = nullptr;
    void __midi_event_handler()
    {
        if (__midi_event_handler_ptr)
            __midi_event_handler_ptr();
    }

    extern struct
    {
        uint8_t tr;
        uint8_t ac;
        uint8_t cv;
        uint8_t qz;
        int8_t transpose;
        uint8_t aux;
        uint8_t ext[4] = {0, 0, 0, 0};
        uint8_t midi_channel;
        uint8_t dac;
        uint8_t level;
        uint8_t pan; // 128 == center
        uint8_t stereo;
        uint8_t aux_dry;
    } *__io;
}

namespace engine
{
    inline const char *name() { return __name; }

    inline uint32_t t() { return *__t; }
    inline uint8_t step() { return *__step; }
    inline bool stepChanged() { return *__step_changed != 0; }
    inline bool stepReset() { return *__step_changed > 1; }
    inline uint32_t trig() { return *__trig; }
    inline uint32_t gate() { return *__gate; }
    inline uint32_t accent() { return *__accent; }
    inline float cv() { return (float)*__cv / PITCH_PER_OCTAVE; }
    inline int32_t cv_i32() { return *__cv; }

    inline bool is_stereo() { return (__io->dac == 2 || __io->dac == 5); }

    template <int channel>
    inline float *outputBuffer();

    template <>
    inline float *outputBuffer<0>() { return __output_l_fp; }

    template <>
    inline float *outputBuffer<1>() { return __output_r_fp; }

    template <int channel>
    inline int16_t *outputBuffer_i16();

    template <>
    inline int16_t *outputBuffer_i16<0>() { return __output_l_i16p; }

    template <>
    inline int16_t *outputBuffer_i16<1>() { return __output_r_i16p; }

    template <int channel>
    inline float *inputBuffer();

    template <>
    inline float *inputBuffer<0>() { return *__audio_in_l_fpp; }

    template <>
    inline float *inputBuffer<1>() { return *__audio_in_r_fpp; }

    inline float mixLevel(int ch) { return ((4.f / UINT16_MAX) * __mixer_level[ch]) * __mixer_level[ch]; } // exp range 0 - 4
    inline float mixLevelL(int ch) { return cosf((float)__mixer_pan[ch] / 255.f * (float)M_PI_2) * mixLevel(ch); }
    inline float mixLevelR(int ch) { return sinf((float)__mixer_pan[ch] / 255.f * (float)M_PI_2) * mixLevel(ch); }

    EXTERN_C void setup();
    EXTERN_C void process();
    EXTERN_C void draw();
    EXTERN_C void screensaver();

    /////
    typedef bool (*uiHandler)(uint16_t type, uint16_t control, int16_t value, uint16_t mask);

    inline void setUIHandler(uiHandler handler)
    {
        __ui_event_handler_ptr = nullptr;
        __ui_event_handler();
        __ui_event_handler_ptr = handler;
    }

    void __attribute__((weak)) onMidiNote(uint8_t key, uint8_t velocity); // NoteOff: velocity == 0
    void __attribute__((weak)) onMidiPitchbend(int16_t pitch);
    void __attribute__((weak)) onMidiCC(uint8_t ccc, uint8_t value);
    void __attribute__((weak)) onMidiSysex(uint8_t byte);

    EXTERN_C void addParam_f32(const char *name, float *value, float min = 0.f, float max = 1.f);                  // min...max
    EXTERN_C void addParam_i32(const char *name, int32_t *value, int32_t min, int32_t max, const char **valueMap); // 0...max

    void addParam(const char *name, int32_t *value, int32_t min, int32_t max, const char **valueMap = nullptr) // 0...max
    {
        addParam_i32(name, value, min, max, &valueMap[0]);
    }

    void addParam(const char *name, float *value, float min = 0.f, float max = 1.f) // 0...max
    {
        addParam_f32(name, value, min, max);
    }

    EXTERN_C void setPatchStateEx(void *ptr, size_t size);

    inline void
    setMode(uint32_t mode) // 1: compact_mode, 2: sample_view
    {
        *__engine_props |= mode;
        if (mode & ENGINE_MODE_MIDI_IN)
        {
            __midi_event_handler_ptr = []()
            {
                switch (__midi_event->type)
                {
                case 0:
                    onMidiNote(__midi_event->key, __midi_event->value);
                    return;
                case 1:
                    onMidiPitchbend(__midi_event->value);
                    return;
                case 2:
                    onMidiCC(__midi_event->key, __midi_event->value);
                    return;
                case 3:
                    onMidiSysex(__midi_event->key);
                    return;
                }
            };
        }
    }

    constexpr uint32_t PARAM_SELECTED = 0x1;
    constexpr uint32_t PARAM_MODULATION = 0x2;

    EXTERN_C uint32_t getParamFlags(const void *valuePtr);
    inline uint32_t isParamSelected(const void *valuePtr)
    {
        return getParamFlags(valuePtr) & PARAM_SELECTED;
    }

    EXTERN_C void setParamName(const void *valuePtr, const char *name);

    EXTERN_C void *dsp_sample_u8(const uint8_t *data, int len, int sample_rate, int addr_shift);
    EXTERN_C void *dsp_sample_Am6070(const uint8_t *data, int len, int sample_rate, int amp_mul);
    EXTERN_C void dsp_set_sample_pos(void *smpl, float pos, float amplitude, float decay);
    EXTERN_C void dsp_process_sample(void *smpl, float start, float end, float pitch, float output[FRAME_BUFFER_SIZE]);
    EXTERN_C int32_t cv_quantize(int32_t cv);
}

enum EventType : uint16_t
{
    EVENT_NONE,
    EVENT_BUTTON_UP = 2,
    EVENT_BUTTON_HOLD,
    EVENT_ENCODER,
};

constexpr uint16_t BUTTON_L = (1 << 0);
constexpr uint16_t BUTTON_R = (1 << 1);
constexpr uint16_t ENCODER_L = (1 << 8);
constexpr uint16_t ENCODER_R = (1 << 9);

namespace clock
{
    constexpr uint8_t CLOCK_RESET = 97;

    inline uint8_t ppn() { return *__clock; }
    inline uint32_t samples_per_step() { return *__samples_per_step; }
}

namespace machine
{
    EXTERN_C const uint8_t *fs_read(const char *blobName);

    inline uint32_t clk_bpm() // BPM x 100
    {
        return *__bpm;
    }

    inline void clk_bpm(uint32_t bpm)
    {
        *__bpm = bpm;
    }
    inline uint32_t raw_digital_inputs()
    {
        return *__digital_inputs_u32_p;
    }

    inline float *raw_adc_voltage(uint32_t index)
    {
        if (index < __adc_count)
            return &__adc_voltage_f_p[index];
        else
            return nullptr;
    }

    inline float *raw_dac_buffer(uint32_t index)
    {
        if (index < __dac_count)
            return &__dac_voltage_fp_p[index];
        else
            return nullptr;
    }
}
#else
using namespace ui;
using namespace UI;
#endif

#ifndef GFX_API
namespace gfx
{
    EXTERN_C void drawCircle(int x, int y, int r);
    EXTERN_C void fillCircle(int x, int y, int r);
    EXTERN_C void clearCircle(int x, int y, int r);
    EXTERN_C void drawRect(int x, int y, int w, int h);
    EXTERN_C void fillRect(int x, int y, int w, int h);
    EXTERN_C void clearRect(int x, int y, int w, int h);
    EXTERN_C void invertRect(int x, int y, int w, int h);
    EXTERN_C void drawString(int x, int y, const char *s, uint8_t font = 1);
    EXTERN_C void drawStringCenter(int x, int y, const char *s, uint8_t font = 1);
    EXTERN_C void drawLine(int x1, int y1, int x2, int y2);
    EXTERN_C void setPixel(int x, int y);
    EXTERN_C void drawXbm(int x, int y, int width, int height, const uint8_t *xbm);
    EXTERN_C void drawSample(void *smpl);
    EXTERN_C void message(const char *msg);

    inline uint8_t *displayBuffer()
    {
        return __display_buffer_u8_p;
    }
}
#endif

#ifndef SERIAL_API
namespace machine
{
    EXTERN_C void serial_write(void const *buffer, uint32_t bufsiz);
    EXTERN_C uint32_t serial_read(void *buffer, uint32_t length);
    EXTERN_C uint32_t serial_available();

    EXTERN_C void get_device_id(uint8_t *mac);

    EXTERN_C float cpu_load_percent();

    EXTERN_C uint8_t *engine_malloc(uint32_t size);
    EXTERN_C void engine_start(uint32_t args);
    EXTERN_C void engine_stop(uint32_t result);
}
#endif

#endif // MACHINE_INTERNAL

// fatfs api -- ff.h

enum FRESULT : int
{
    FR_OK = 0,
    FR_DISK_ERR = 1,
    FR_NO_FILE = 4,
};

typedef struct
{
    void *impl;
} FIL;

typedef unsigned int UINT;  /* int must be 16-bit or 32-bit */
typedef unsigned char BYTE; /* char must be 8-bit */
typedef uint16_t WORD;      /* 16-bit unsigned integer */
typedef uint32_t DWORD;     /* 32-bit unsigned integer */
typedef uint64_t QWORD;     /* 64-bit unsigned integer */
typedef WORD WCHAR;         /* UTF-16 character type */
typedef DWORD FSIZE_t;

#define FA_READ 0x01
#define FA_WRITE 0x02
#define FA_OPEN_EXISTING 0x00
#define FA_CREATE_NEW 0x04
#define FA_CREATE_ALWAYS 0x08
#define FA_OPEN_ALWAYS 0x10
#define FA_OPEN_APPEND 0x30

EXTERN_C FRESULT f_open(FIL *f, const char *name, uint32_t mode);
EXTERN_C FRESULT f_read(FIL *f, void *p, UINT size, UINT *bytes_read);
EXTERN_C FSIZE_t f_tell(FIL *fp);
EXTERN_C FSIZE_t f_size(FIL *fp);
EXTERN_C FRESULT f_truncate(FIL *fp);
EXTERN_C FRESULT f_lseek(FIL *fp, FSIZE_t ofs);
EXTERN_C FRESULT f_close(FIL *f);
EXTERN_C FRESULT f_write(FIL *fp, const void *buff, UINT btw, UINT *bw);