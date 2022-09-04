#pragma once

#include <inttypes.h>
#include <string.h>

typedef float SPFLOAT;
#define SP_OK 1
#define SP_NOT_OK 0
#define SP_RANDMAX 2147483648

typedef struct sp_auxdata {
    size_t size;
    void *ptr;
} sp_auxdata;

typedef struct { 
    SPFLOAT *out;
    uint32_t sr;
    uint32_t rand;
    sp_auxdata aux;
} sp_data; 

uint32_t sp_rand(sp_data *sp);