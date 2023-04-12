#include "machine.h"
#include <stdio.h>

#ifndef FLASHMEM
#include "pgmspace.h"
#endif

#include "fv1/FV1.h"

using namespace machine;

template <int PROG, int inputGainDiv = 10>
struct FXEngine : public Engine
{
    const float inputGain = 0.8f / inputGainDiv * 10;
    float raw = 0;

    uint8_t program = 0;
    uint8_t loaded_program = -1;

    float pot0, pot1, pot2;
    const char *names[7];
    bool inited = false;
    float bufferL[FRAME_BUFFER_SIZE];
    float bufferR[FRAME_BUFFER_SIZE];

    FV1 *fv1;

    FXEngine(float pp0 = 1.f, float pp1 = 0.5f, float pp2 = 0.5f, float pp3 = 0.5f,
             const char *n0 = "D/W", const char *n1 = "P0", const char *n2 = "P1", const char *n3 = "P3")
        : Engine(AUDIO_PROCESSOR)
    {
        fv1 = fv1_init(machine::malloc);

        param[0].init(n0, &raw, pp0);
        program = PROG;
        param[1].init(n1, &pot0, pp1);
        param[2].init(n2, &pot1, pp2);
        param[3].init(n3, &pot2, pp3);
    }

    ~FXEngine() override
    {
        machine::mfree(fv1);
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        if (loaded_program != program)
        {
            loaded_program = program;
            fv1_set_fx(fv1, program);
        }

        float *ins[] = {machine::get_aux(AUX_L), machine::get_aux(AUX_R)};

        for (int i = 0; i < FRAME_BUFFER_SIZE; ++i)
        {
            ins[0][i] *= inputGain;
            ins[1][i] *= inputGain;
        }

        fv1_process(fv1, ins[0], ins[1], pot0, pot1, pot2, bufferL, bufferR, FRAME_BUFFER_SIZE);

        for (int i = 0; i < FRAME_BUFFER_SIZE; ++i)
        {
            ins[0][i] /= inputGain;
            ins[1][i] /= inputGain;
            bufferL[i] = raw * bufferL[i] + (1 - raw) * ins[0][i];
            bufferR[i] = raw * bufferR[i] + (1 - raw) * ins[1][i];
        }

        of.out = bufferL;
        of.aux = bufferR;
    }

    void display() override
    {
        if (PROG < 0)
        {
            static char tmp[16];
            sprintf(tmp, "> PROG-%.2x", program);
            param[1].name = tmp;
        }

        gfx::drawEngine(this, fv1 ? nullptr : machine::OUT_OF_MEMORY);
    }
};

void init_fv1()
{
    machine::add<FXEngine<1, 15>>(machine::FX, "Gated-Reverb", 1.f, 0.5f, 0.5f, 0.5f, "D/W", "PreD", "G-Time", "Damp");
    machine::add<FXEngine<0>>(machine::FX, "Reverb-HP-LP", 1.f, 0.5f, 0.5f, 0.5f, "D/W", "Reverb", "HP", "LP");
}

MACHINE_INIT(init_fv1);