
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

#define DSP_PROCESS WASM_EXPORT_AS("_process")
#define DSP_SETUP WASM_EXPORT_AS("_setup")
#define GFX_DISPLAY WASM_EXPORT_AS("_display")
#define GFX_SCREENSAVER WASM_EXPORT_AS("_screensaver")

#define FRAME_BUFFER_SIZE 24

extern "C"
{
    WASM_IMPORT("gfx", "clear")
    void gfx_clear();

    WASM_IMPORT("gfx", "draw_xbm")
    void gfx_draw_xbm(int x, int y, int width, int height, const uint8_t *xbm);

    WASM_IMPORT("gfx", "draw_circle")
    void gfx_draw_circle(int x, int y, int r);

    WASM_IMPORT("gfx", "draw_rect")
    void gfx_draw_rect(int x, int y, int w, int h);

    WASM_IMPORT("gfx", "fill_rect")
    void gfx_fill_rect(int x, int y, int w, int h);

    WASM_IMPORT("gfx", "draw_string")
    void gfx_draw_string(int32_t x, int32_t y, const char *s, int32_t font = 1);

    WASM_IMPORT("gfx", "draw_line")
    void gfx_draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

    WASM_IMPORT("gfx", "message")
    void gfx_message(const char *msg);

    WASM_IMPORT("dsp", "frame_f")
    void dsp_frame_f(const char *name, float frame[FRAME_BUFFER_SIZE]); // len == 24

    WASM_IMPORT("dsp", "frame_i16")
    void dsp_frame_i16(const char *name, const int16_t out[FRAME_BUFFER_SIZE]); // len == 24

    WASM_IMPORT("dsp", "param_f")
    void dsp_param_f(const char *name, float *value); // 0...1

    WASM_IMPORT("dsp", "param_f2")
    void dsp_param_f2(const char *name, float *value, float min, float max); // min...max

    WASM_IMPORT("dsp", "param_u8")
    void dsp_param_u8(const char *name, uint8_t *value, int32_t min, int32_t max); // 0...max

    WASM_IMPORT("fs", "fs_read")
    const uint8_t *fs_read(const char *blobName);

    WASM_IMPORT("dsp", "sample_u8")
    void *dsp_sample_u8(const uint8_t *data, int len, int sample_rate, int addr_shift);
    void *dsp_sample_Am6070(const uint8_t *data, int len, int sample_rate, int amp_mul);

    WASM_IMPORT("dsp", "process_sample")
    void dsp_process_sample(void *smpl, float start, float end, float pitch, float output[FRAME_BUFFER_SIZE]);

    WASM_IMPORT("dsp", "process_hihats")
    void dsp_process_hihats(void *ch, void *oh, float ch_vol, float ch_dec, float oh_dec, float output[FRAME_BUFFER_SIZE]);
}

extern uint8_t *__display_buffer_u8_p;
extern float *__adc_voltage_f_p;
extern float **__dac_voltage_fp_p;
extern uint32_t *__digital_inputs_u32_p;

#define gfx_display_buffer __display_buffer_u8_p
#define adc_voltage __adc_voltage_f_p
#define dac_buffer __dac_voltage_fp_p
#define digital_inputs __digital_inputs_u32_p

#define FLASHMEM __attribute__((section(".text")))
#define LEN_OF(x) (sizeof(x) / sizeof(x[0]))
#define ONE_POLE(out, in, coefficient) out += (coefficient) * ((in)-out);
#define DEFAULT_NOTE 60
#define SAMPLE_RATE 48000

#define BPM ".BPM"
#define CV ".CV"
#define TRIG ".TRIG"
#define GATE ".GATE"
#define V_OCT "_V_OCT"
#define IO_STEREOLIZE ".IO_STEREO"

#define OUTPUT_L ".OUTPUT_L"
#define OUTPUT_R ".OUTPUT_R"
#define INPUT_L ".INPUT_L"
#define INPUT_R ".INPUT_R"
#define INPUT_CV0 ".CV0"

#ifdef __cplusplus
#ifdef __GNUC__
/* poision memory functions */
// #   pragma GCC poison _ZSt17__throw_bad_allocv
// #   pragma GCC poison new delete
// #   pragma GCC poison malloc new
#endif

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
#endif

extern "C"
{
    uint32_t micros();
    uint32_t millis();

    enum EventType : uint16_t
    {
        EVENT_NONE,
        EVENT_BUTTON_UP = 2,
        EVENT_BUTTON_HOLD,
        EVENT_ENCODER,
    };

#define BUTTON_L (1 << 0)
#define BUTTON_R (1 << 1)
#define ENCODER_L (1 << 8)
#define ENCODER_R (1 << 9)

    extern struct
    {
        uint16_t type;
        uint16_t control;
        int16_t value;
        uint16_t mask;
        bool handled;
    } *__ui_event;
}

#define UI_HANDLER                                                                 \
    bool uiHandler(uint16_t type, uint16_t control, int16_t value, uint16_t mask); \
    WASM_EXPORT_AS("_ui_handler")                                                  \
    void __ui_event_handler() { __ui_event->handled = uiHandler(__ui_event->type, __ui_event->control, __ui_event->value, __ui_event->mask); }
