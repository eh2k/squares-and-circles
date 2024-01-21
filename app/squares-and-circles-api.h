
// Copyright (C)2023 - Eduard Heidt
//
// Author: Eduard Heidt (eh2k@gmx.de)
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

#if 0
#define WASM_EXPORT extern "C" __attribute__((used)) __attribute__((visibility("default")))
#define WASM_EXPORT_AS(NAME) WASM_EXPORT __attribute__((export_name(NAME)))
#define WASM_IMPORT(MODULE, NAME) __attribute__((import_module(MODULE))) __attribute__((import_name(NAME)))
#define WASM_CONSTRUCTOR __attribute__((constructor))
#else
#define WASM_EXPORT extern "C"
#define WASM_EXPORT_AS(NAME) WASM_EXPORT
#define WASM_IMPORT(MODULE, NAME)
#define WASM_CONSTRUCTOR
#endif

#ifndef FLASHMEM
#define FLASHMEM __attribute__((section(".text")))
#endif
#define LEN_OF(x) (sizeof(x) / sizeof(x[0]))
#define MOD(x, y) ((x) % (y))

#ifndef ONE_POLE
#define ONE_POLE(out, in, coefficient) out += (coefficient) * ((in)-out);
#endif
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

#define V_OCT "_V_OCT"
#define IO_STEREOLIZE ".IO_STEREO"

#ifdef __cplusplus
#ifdef __GNUC__
/* poision memory functions */
// #   pragma GCC poison _ZSt17__throw_bad_allocv
// #   pragma GCC poison new delete
// #   pragma GCC poison malloc new
#endif

#ifndef EMSCRIPTEN

#include <stdlib.h>
#include <new>

void *operator new(std::size_t sz)
{
    void *out = malloc(sz);
    return out;
}

void *operator new[](std::size_t sz)
{
    void *out = malloc(sz);
    return out;
}

void operator delete(void *ptr) noexcept
{
    free(ptr);
}

void operator delete(void *ptr, std::size_t size) noexcept
{
    free(ptr);
}

void operator delete[](void *ptr) noexcept
{
    free(ptr);
}

void operator delete[](void *ptr, std::size_t size) noexcept
{
    free(ptr);
}

extern "C" uint32_t micros();
extern "C" uint32_t millis();

#endif
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

#ifndef engine

extern "C"
{
    extern uint32_t *__t;
    extern uint8_t *__clock;
    extern uint8_t *__trig;
    extern uint8_t *__gate;
    extern uint8_t *__accent;
    extern float *__cv;

    extern float *__output_l_fp;
    extern float *__output_r_fp;
    extern int16_t *__output_l_i16p;
    extern int16_t *__output_r_i16p;

    extern float **__audio_in_l_fpp;
    extern float **__audio_in_r_fpp;

    extern uint8_t *__display_buffer_u8_p;
    extern uint32_t *__engine_props;

    extern float *__adc_voltage_f_p;
    extern float **__dac_voltage_fp_p;
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
}

namespace engine
{
    inline uint32_t t() { return *__accent; }
    inline uint8_t clock() { return *__clock; }
    inline uint8_t trig() { return *__trig; }
    inline uint8_t gate() { return *__gate; }
    inline uint8_t accent() { return *__accent; }
    inline float cv() { return *__cv; }

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

    extern "C" void setup();
    extern "C" void process();
    extern "C" void draw();
    extern "C" void screensaver();

    /////
    typedef bool (*uiHandler)(uint16_t type, uint16_t control, int16_t value, uint16_t mask);

    inline void setUIHandler(uiHandler handler)
    {
        __ui_event_handler_ptr = nullptr;
        __ui_event_handler();
        __ui_event_handler_ptr = handler;
    }
    extern "C" void addParam_f32(const char *name, float *value, float min = 0.f, float max = 1.f);                  // min...max
    extern "C" void addParam_i32(const char *name, int32_t *value, int32_t min, int32_t max, const char **valueMap); // 0...max

    void addParam(const char *name, int32_t *value, int32_t min, int32_t max, const char **valueMap = nullptr) // 0...max
    {
        addParam_i32(name, value, min, max, &valueMap[0]);
    }

    void addParam(const char *name, float *value, float min = 0.f, float max = 1.f) // 0...max
    {
        addParam_f32(name, value, min, max);
    }

    inline void setUIMode(uint32_t mode) // 1: compact_mode, 2: sample_view
    {
        if (mode == 1)
            *__engine_props |= 32;
    }

    constexpr uint32_t PARAM_SELECTED = 0x1;
    constexpr uint32_t PARAM_MODULATION = 0x2;

    extern "C" uint32_t getParamFlags(const void *valuePtr);
    inline uint32_t isParamSelected(const void *valuePtr)
    {
        return getParamFlags(valuePtr) & PARAM_SELECTED;
    }

    extern "C"
    {
        void *dsp_sample_u8(const uint8_t *data, int len, int sample_rate, int addr_shift);
        void *dsp_sample_Am6070(const uint8_t *data, int len, int sample_rate, int amp_mul);
        void dsp_process_sample(void *smpl, float start, float end, float pitch, float output[FRAME_BUFFER_SIZE]);
        void dsp_process_hihats(void *ch, void *oh, float ch_vol, float ch_dec, float oh_dec, float output[FRAME_BUFFER_SIZE]);
    }
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

namespace machine
{
    extern "C" const uint8_t *fs_read(const char *blobName);

    inline uint32_t midi_bpm()
    {
        return *__bpm;
    }
    inline uint32_t raw_digital_inputs()
    {
        return *__digital_inputs_u32_p;
    }

    inline float raw_adc_voltage(uint32_t index)
    {
        return __adc_voltage_f_p[index];
    }

    inline float *raw_dac_buffer(uint32_t index)
    {
        return __dac_voltage_fp_p[index];
    }
};
#else
using namespace ui;
using namespace UI;
#endif

#ifndef __no_gfx
namespace gfx
{
    extern "C" void drawCircle(int x, int y, int r);
    extern "C" void fillCircle(int x, int y, int r);
    extern "C" void clearCircle(int x, int y, int r);
    extern "C" void drawRect(int x, int y, int w, int h);
    extern "C" void fillRect(int x, int y, int w, int h);
    extern "C" void clearRect(int x, int y, int w, int h);
    extern "C" void invertRect(int x, int y, int w, int h);
    extern "C" void drawString(int32_t x, int32_t y, const char *s, int32_t font = 1);
    extern "C" void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    extern "C" void drawXbm(int x, int y, int width, int height, const uint8_t *xbm);
    extern "C" void message(const char *msg);

    inline uint8_t *displayBuffer()
    {
        return __display_buffer_u8_p;
    }
}
#endif