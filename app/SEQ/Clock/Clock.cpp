// Copyright (C)2021 - Eduard Heidt
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

#include "../../squares-and-circles-api.h"
#include <stdio.h>

ENGINE_NAME("SEQ/Clock#v2");       // v2: Source first, restore()
ENGINE_ALIAS("SEQ/Clock", "bbbb");  // BPM, MODE (now PPQN), Impulse, Offset
ENGINE_ALIAS("MIDI/Clock", "bbbb"); // the same, saved before the app moved to SEQ/Clock

// SYNC (0): the clock of the track's CLOCK-SOURCE (I/O page), BPM shows its tempo. FREE (1): an own clock at BPM.
// Not CLK and INT: those are the I/O page's global clock and the settings page's internal one
static int32_t source = 0;
static int32_t bpm = 120;
static int32_t ppqn = 0;
static const char *ppqn_names[]{
    "4", "8", "24"};
static int32_t offset = 0;
static int32_t impulse = 1;
// Source and BPM as text, as FX/Delay's Time: the names, which draw() writes from the last process(). snprintf: a
// longer text is cut, not written over the variables after the buffer (impulse came after source_name)
static char source_name[16] = ">MODE\nSYNC";
static char bpm_name[16] = ">>120 BPM"; // ">>": the tempo is in the name, no value shown while it is turned
static bool shown_free = false;
static int32_t shown_bpm = 120;
constexpr int32_t BPM_MIN = 30; // as the TEMPO page
constexpr int32_t BPM_MAX = 255;
constexpr uint32_t SAMPLES_PER_MS = SAMPLE_RATE / 1000;
// the samples the CLK and RST pulses still last: a count, not an end time, whose signed difference to the sample
// counter would turn positive again after 12.4 h without a pulse
static uint32_t clk_left = 0;
static uint32_t rst_left = 0;
// the samples the pending pulses are due at: with an offset longer than a tick, the next tick no longer replaces the
// pulse of the one before (24 ppqn at 120 BPM: CLK fell silent from ~21 ms)
static uint32_t pending[32] = {};
static uint8_t pending_in = 0, pending_out = 0;
// FREE: 24 ticks a quarter note. The phase grows by BPM x 100 a sample, a tick at FREE_TICK (120 BPM: 1000 samples,
// 20.8 ms): no drift, and the pulse begins on its sample, since the CV outputs play every sample of the buffer
constexpr uint32_t FREE_TICK = 60 * SAMPLE_RATE / 24 * 100;
static uint32_t free_phase = 0;
static uint8_t free_pos = 0; // 1-96; 0: stopped, the next FREE frame starts with a reset
static int32_t free_bpm = 0;
static int16_t *out_clk;
static int16_t *out_reset;

// the patches of before: SEQ/Clock and MIDI/Clock with BPM, MODE, Impulse, Offset (SYNC, BPM only shown). SEQ/Clock
// with Source as the fifth (only on the OCT40, 2026-10-27) loads as it is, in the order of then
void engine::restore(uint32_t id, const engine::patch_param *old, int n, engine::patch_param *params)
{
    if (old != nullptr && n == 4)
    {
        params[2] = old[1]; // MODE, now PPQN
        params[3] = old[2]; // Impulse
        params[4] = old[3]; // Offset
    }
    else if (old == nullptr && id == ENGINE_ID("SEQ/Clock"))
    {
        const engine::patch_param p[] = {params[4], params[0], params[1], params[2], params[3]};
        for (size_t i = 0; i < LEN_OF(p); i++)
            params[i] = p[i];
    }
}

// SYNC: turning BPM (the right encoder) makes the clock FREE, from the tempo shown; the firmware turns it on from
// there. The encoder only, not the value: a modulation of BPM would switch too
static bool on_ui(uint16_t type, uint16_t control, int16_t value, uint16_t mask)
{
    if (type == EVENT_ENCODER && control == ENCODER_R && source == 0 && engine::isParamSelected(&bpm))
    {
        bpm = shown_bpm > 0 ? shown_bpm : 120;
        source = 1;
    }
    return false;
}

void engine::setup()
{
    engine::addParam(source_name, &source, 0, 1);
    engine::addParam(bpm_name, &bpm, BPM_MIN, BPM_MAX);
    // 24, 8 or 4 ppqn
    engine::addParam(">PPQN\n%s", &ppqn, 0, 2, ppqn_names);
    engine::addParam(">Impulse\n%dms", &impulse, 1, 5);
    engine::addParam(">Offset\n%dms", &offset, 0, 127);

    out_clk = engine::outputBuffer_i16(OUT_TRIG "CLK");
    out_reset = engine::outputBuffer_i16(OUT_TRIG "RST");

    engine::setUIHandler(on_ui);
}

static uint8_t last_clk = 0;
static uint8_t clk_change(uint8_t clk)
{
    if (last_clk != clk)
    {
        last_clk = clk;
        return last_clk;
    }
    else
        return 0;
}

// the clock at position clk (1-96, CLOCK_RESET) in sample `at`: a CLK pulse on PPQN's positions, RST on a reset
static void tick(uint8_t clk, uint32_t at)
{
    const uint8_t div = ppqn == 0 ? 6 : (ppqn == 1 ? 3 : 1);

    if ((clk % div) == 1 || div == 1)
    {
        if ((uint8_t)(pending_in - pending_out) < LEN_OF(pending))
            pending[pending_in++ % LEN_OF(pending)] = at + offset * SAMPLES_PER_MS;
    }

    if (clk == engine_sync::CLOCK_RESET)
        rst_left = impulse * SAMPLES_PER_MS; // as long as a CLK pulse, not one frame
}

void engine::process()
{
    const uint32_t now = engine::t() * FRAME_BUFFER_SIZE; // the sample counter at the frame's first sample
    const uint8_t clk = clk_change(engine_sync::ppn());  // also with FREE: back on SYNC, no stale position is a tick

    if (source == 0)
    {
        free_pos = 0;
        bpm = (engine_sync::clk_bpm() + 50) / 100;
        if (clk)
            tick(clk, now);
    }
    else
    {
        // FREE starts at the tempo SYNC showed, at 120 without one (CLOCK-SOURCE "-": 0)
        if (bpm < BPM_MIN)
            bpm = 120;
        CONSTRAIN(bpm, BPM_MIN, BPM_MAX);
        free_bpm = bpm;
    }
    shown_free = source != 0;
    shown_bpm = bpm;

    for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        if (source != 0)
        {
            if (free_pos == 0)
            {
                free_pos = 1;
                free_phase = 0;
                tick(engine_sync::CLOCK_RESET, now + i);
            }
            else if ((free_phase += free_bpm * 100) >= FREE_TICK)
            {
                free_phase -= FREE_TICK;
                free_pos = free_pos % 96 + 1;
                tick(free_pos, now + i);
            }
        }

        while (pending_out != pending_in && (int32_t)(now + i - pending[pending_out % LEN_OF(pending)]) >= 0)
        {
            ++pending_out;
            clk_left = impulse * SAMPLES_PER_MS;
        }

        out_clk[i] = clk_left > 0 ? (5 * PITCH_PER_OCTAVE) : 0;
        out_reset[i] = rst_left > 0 ? (5 * PITCH_PER_OCTAVE) : 0;
        clk_left -= clk_left > 0;
        rst_left -= rst_left > 0;
    }
}

void engine::draw()
{
    snprintf(source_name, sizeof(source_name), shown_free ? ">MODE\nFREE" : ">MODE\nSYNC");
    if (shown_bpm > 0)
        snprintf(bpm_name, sizeof(bpm_name), ">>%d BPM", (int)shown_bpm);
    else
        snprintf(bpm_name, sizeof(bpm_name), ">>--- BPM"); // SYNC without a clock
}
