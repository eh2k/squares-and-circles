#include "../../squares-and-circles-api.h"

#define INLINE __attribute__((always_inline))

#include "fv1/FV1.hpp"
#include "fv1//spn/OEM_1_4.hpp"

static float raw = 0;
static float pot0, pot1, pot2;

static uint32_t mem[sizeof(FV1) / sizeof(uint32_t)];
static FV1 *fv1 = nullptr;

void engine::setup()
{
    fv1 = new (mem) FV1(); // Issue with static constructor !?
    fv1->setFx(program);

    raw = 1.f;
    pot0 = 0.5f;
    pot1 = 0.5f;
    pot2 = 0.5f;

    engine::addParam("D/W", &raw);

    engine::addParam("G-Time", &pot0); // OEM1_4.spn: pot0 = gate time, pot1 = predelay
    engine::addParam("PreD", &pot1);
    engine::addParam("Damp", &pot2);
}

void engine::process()
{
    auto inputL = engine::inputBuffer<0>();
    auto inputR = engine::inputBuffer<1>();
    auto outputL = engine::outputBuffer<0>();
    auto outputR = engine::outputBuffer<1>();

    fv1->execute(inputL, inputR, pot0, pot1, pot2, outputL, outputR, FRAME_BUFFER_SIZE);

    for (int i = 0; i < FRAME_BUFFER_SIZE; ++i)
    {
        outputL[i] = raw * outputL[i] + (1 - raw) * inputL[i];
        outputR[i] = raw * outputR[i] + (1 - raw) * inputR[i];
    }
}