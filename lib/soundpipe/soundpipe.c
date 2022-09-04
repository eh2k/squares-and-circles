#include "soundpipe.h"
#include <stdlib.h>

uint32_t sp_rand(sp_data *sp)
{
    uint32_t val = (1103515245 * sp->rand + 12345) % SP_RANDMAX;
    sp->rand = val;
    return val;
}