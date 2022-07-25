

extern "C"
{
    struct FV1;
    FV1 *fv1_init();
    void fv1_free(FV1 *);
    void fv1_set_fx(FV1 *fv1, int program);
    void fv1_process(FV1 *fv1, const float *inL, const float *inR, float pot0, float pot1, float pot2, float *outL, float *outR, unsigned int size);
}