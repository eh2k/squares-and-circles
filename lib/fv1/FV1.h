/*
 * Copyright (C)2021 - Eduard Heidt
 * Licensed for personal, non-commercial use only
 */

extern "C"
{
    struct FV1;
    FV1 *fv1_init(void *(*malloc)(size_t size));
    void fv1_set_fx(FV1 *fv1, int program);
    void fv1_process(FV1 *fv1, const float *inL, const float *inR, float pot0, float pot1, float pot2, float *outL, float *outR, unsigned int size);
    void fv1_fake_cv_trig(bool trig, float *in, unsigned int size);

    typedef void (*FV1FX)(FV1 *fv1);
    extern FV1FX fv1_programs[256];
    void *fv1_load(FV1 *fv1, const uint8_t *prog, void *(*malloc)(size_t size));
}