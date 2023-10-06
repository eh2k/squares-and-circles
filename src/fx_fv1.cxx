#include "machine.h"
#include <stdio.h>

#include "fv1/FV1.h"

#ifndef XZ

#include "xz_lzma2/xz.h"

extern "C"
{
    void *xz_malloc(size_t s)
    {
        return machine::malloc(s);
    }
    void xz_free(void *p)
    {
        machine::mfree(p);
    }
}

struct xz
{
    xz_dec_lzma2 *lzma2 = nullptr;
    xz_buf b;
    xz_ret status;
    xz(const uint8_t *in, size_t size)
    {

        b.in = in;
        b.in_pos = 0;
        b.in_size = size;

        lzma2 = xz_dec_lzma2_create(XZ_PREALLOC, 4096); // xz needs about 32K of heap...
        status = xz_dec_lzma2_reset(lzma2, 0);
        if (status != XZ_OK)
            machine::message("XZ ERR 0x%x", status);
    }

    ~xz()
    {
        end();
    }

    void end()
    {
        if (lzma2)
        {
            xz_dec_lzma2_end(lzma2);
            lzma2 = nullptr;
        }
    }

    void decode(uint8_t *out, size_t size)
    {
        b.out_pos = 0;
        b.out = out;
        b.out_size = size;

        status = xz_dec_lzma2_run(lzma2, &b);
        if (status > 1)
            machine::message("XZ ERR 0x%x %d/%d", status, b.in_pos, b.in_size);
    }
};

#endif

using namespace machine;

struct FV1_ARGS
{
    uint32_t props;
    int32_t PROG;
    float inputGain;
    float dryWet;
    const char *n0;
    const char *n1;
    const char *n2;
    const char *n3;
};

struct FV1_BANK_ENTRY
{
    char name[32];
    const uint8_t *bank;
    int index;
};

struct FV1_ROM
{
    uint8_t props;
    char name[32];
    char pot0[16];
    char pot1[16];
    char pot2[16];
    char dummy[128 - sizeof(name) - sizeof(pot0) - sizeof(pot1) - sizeof(pot2) - sizeof(props)];
    uint8_t program[512];
};

static_assert(sizeof(FV1_ROM) == 128 + 512, "");

struct FV1_Engine : public Engine
{
    float inputGain = 1.f;
    float raw = 0;

    uint8_t program = 0;
    uint8_t loaded_program = -1;

    float pot0, pot1, pot2;
    const char *names[7];
    bool inited = false;
    float bufferL[FRAME_BUFFER_SIZE];
    float bufferR[FRAME_BUFFER_SIZE];
    float ins[2][machine::FRAME_BUFFER_SIZE] = {};

    FV1 *fv1 = nullptr;
    const uint8_t *xzrom = nullptr;
    void *ram = nullptr;

    FV1_Engine(const FV1_ARGS &args) : Engine(AUDIO_PROCESSOR)
    {
        fv1 = fv1_init(machine::malloc);

        inputGain = args.inputGain;
        raw = args.dryWet;
        pot0 = 0.5f;
        pot1 = 0.5f;
        pot2 = 0.5f;

        param[0].init(args.n0, &raw, raw);
        program = args.PROG;

        param[1].init(args.n1, &pot0, pot0);
        param[2].init(args.n2, &pot1, pot1);
        param[3].init(args.n3, &pot2, pot2);
    }

    FV1_Engine(const FV1_BANK_ENTRY *bnk_entry) : Engine(AUDIO_PROCESSOR | TRIGGER_INPUT)
    {
        this->xzrom = bnk_entry->bank;

        inputGain = 1.f;
        pot0 = 0.5f;
        pot1 = 0.5f;
        pot2 = 0.5f;
        raw = 1.f;

        decode_rom(bnk_entry->index);
    }

    char paramNames[4][16] = {};

    void decode_rom(int index = -1)
    {
        machine::mfree(this->ram);
        machine::mfree(this->fv1);

        xz xz(&this->xzrom[3], *((uint16_t *)&this->xzrom[1]));

        if (index >= 0)
            program = index;
        else
            index = 0;

        FV1_ROM rom;

        for (int i = 0; i <= program && xz.status == XZ_OK; i++)
        {
            xz.decode((uint8_t *)&rom, sizeof(FV1_ROM));
        }

        xz.end();

        this->fv1 = fv1_init(machine::malloc);
        if (this->fv1 != nullptr) // nullptr == out_of_memory
        {
            this->ram = fv1_load(fv1, rom.program, machine::malloc);

            snprintf(paramNames[0], sizeof(paramNames[0]), rom.name);
            snprintf(paramNames[1], sizeof(paramNames[1]), rom.pot0);
            snprintf(paramNames[2], sizeof(paramNames[2]), rom.pot1);
            snprintf(paramNames[3], sizeof(paramNames[3]), rom.pot2);

            param[1].name = rom.name;
            if (rom.props & TRIGGER_INPUT)
            {
                inputGain = __FLT_EPSILON__;
                param[0].init("LEVEL", &raw, raw); // No Dry/Wet
            }
            else
            {
                inputGain = 1.f;
                param[0].init("D/W", &raw, raw);
            }

            int i = 1;

            if (index < 0)
                param[i++].init(paramNames[0], &program, 0, 0, this->xzrom[0] - 1);

            param[i++].init(paramNames[1], &pot0, pot0);
            param[i++].init(paramNames[2], &pot1, pot1);
            param[i++].init(paramNames[3], &pot2, pot2);
        }
    }

    ~FV1_Engine() override
    {
        machine::mfree(ram);
        machine::mfree(fv1);
    }

    void process(const ControlFrame &frame, OutputFrame &of) override
    {
        if (loaded_program != program)
        {
            if (this->xzrom)
            {
                decode_rom();
                loaded_program = program;
            }
            else
            {
                loaded_program = program;
                fv1_set_fx(fv1, program);
            }
        }

        if (this->io->tr > 0)
        {
            fv1_fake_cv_trig(frame.trigger, ins[0], FRAME_BUFFER_SIZE);
            fv1_fake_cv_trig(frame.trigger, ins[1], FRAME_BUFFER_SIZE);
        }
        else
        {
            memset(ins, 0, sizeof(ins));
        }

        machine::get_audio(AUX_L, ins[0], inputGain);
        machine::get_audio(AUX_R, ins[1], inputGain);

        fv1_process(fv1, ins[0], ins[1], pot0, pot1, pot2, bufferL, bufferR, FRAME_BUFFER_SIZE);

        if (this->io->tr > 0)
        {
            // without fake trigger peak
            memset(ins, 0, sizeof(ins));
            machine::get_audio(AUX_L, ins[0], inputGain);
            machine::get_audio(AUX_R, ins[1], inputGain);
        }

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
        gfx::drawEngine(this, fv1 ? nullptr : machine::OUT_OF_MEMORY);
    }
};

void machine_add_fv1_bank(const char *machine, const char *engine, const uint8_t *bank, int i)
{
    static FV1_BANK_ENTRY fv1_engins[32];
    snprintf(fv1_engins[i].name, 32, engine);

    if (i < (int)LEN_OF(fv1_engins))
    {
        fv1_engins[i].bank = bank;
        fv1_engins[i].index = i;

        for (size_t j = 0; j < sizeof(fv1_engins[i].name) && fv1_engins[i].name[j] != 0; j++)
        {
            if (fv1_engins[i].name[j] == '/')
            {
                fv1_engins[i].name[j] = 0;
                machine::add<FV1_Engine, const FV1_BANK_ENTRY *>(fv1_engins[i].name, &fv1_engins[i].name[j + 1], &fv1_engins[i]);
                return;
            }
        }

        machine::add<FV1_Engine, const FV1_BANK_ENTRY *>(machine, fv1_engins[i].name, &fv1_engins[i]);
    }
}

void init_fv1()
{
    machine::add<FV1_Engine, FV1_ARGS>(machine::FX, "Gated-Reverb", {AUDIO_PROCESSOR, 1, 0.5f, 1.f, "D/W", "PreD", "G-Time", "Damp"});
    machine::add<FV1_Engine, FV1_ARGS>(machine::FX, "Reverb-HP-LP", {AUDIO_PROCESSOR, 0, 1.f, 1.f, "D/W", "Reverb", "HP", "LP"});

    if (const uint8_t *FV1_BNK0 = machine::flash_read("FV1_BNK0"))
    {
        xz xz(&FV1_BNK0[3], *((uint16_t *)&FV1_BNK0[1]));

        FV1_ROM tmp;

        for (int i = 0; i < FV1_BNK0[0] && xz.status == XZ_OK; i++)
        {
            xz.decode((uint8_t *)&tmp, sizeof(FV1_ROM));
            machine_add_fv1_bank("FV1emu", tmp.name, FV1_BNK0, i);
        }

        xz.end();
    }
}

MACHINE_INIT(init_fv1);