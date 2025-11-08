#ifndef GlobalDefinitions_h
#define GlobalDefinitions_h

#include <float.h>
#include <inttypes.h>

#define INLINE inline

#ifndef REAL_T_IS_DOUBLE
#define REAL_T_IS_FLOAT
#endif

#ifdef REAL_T_IS_FLOAT
typedef float real_t; //[eh2k]
#else
typedef double real_t; 
#endif
//_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON)

constexpr int SAMPLE_RATE = 48000;

//-------------------------------------------------------------------------------------------------
// mathematical constants:

#define PI 3.1415926535897932384626433832795
#define EULER 2.7182818284590452353602874713527
#define SQRT2 1.4142135623730950488016887242097
#define ONE_OVER_SQRT2 0.70710678118654752440084436210485
#define LN10 2.3025850929940456840179914546844
#define ONE_OVER_LN10 0.43429448190325182765112891891661
#define LN2 0.69314718055994530941723212145818
#define ONE_OVER_LN2 1.4426950408889634073599246810019
#define SEMITONE_FACTOR 1.0594630943592952645618252949463

//-------------------------------------------------------------------------------------------------
// type definitions:

// unsigned 64 bit integers:
#ifdef _MSC_VER
typedef unsigned __int64 UINT64;
#else
typedef unsigned long long UINT64;
#endif

// signed 64 bit integers:
#ifdef _MSC_VER
typedef signed __int64 INT64;
#else
typedef signed long long INT64;
#endif

// unsigned 32 bit integers:
#ifdef _MSC_VER
typedef unsigned __int32 UINT32;
#else
typedef unsigned long UINT32;
#endif

// ...constants for numerical precision issues, denorm, etc.:
#define TINY FLT_MIN
#define EPS DBL_EPSILON

// define infinity values:

inline real_t dummyFunction(real_t x) { return x; }
#define INF (1.0/dummyFunction(0.0))
#define NEG_INF (-1.0/dummyFunction(0.0))

//-------------------------------------------------------------------------------------------------
// debug stuff:

// this will try to break the debugger if one is currently hosting this app:
#ifdef _DEBUG

#ifdef _MSC_VER
#pragma intrinsic (__debugbreak)
#define DEBUG_BREAK __debugbreak();
#else
#define DEBUG_BREAK {}
#endif

#else

#define DEBUG_BREAK {}  // evaluate to no op in release builds

#endif

// an replacement of the ASSERT macro
#define rassert(expression)  { if (! (expression)) DEBUG_BREAK }

//-------------------------------------------------------------------------------------------------
// bit twiddling:

//extract the exponent from a IEEE 754 real_ting point number (single and double precision):
#ifdef REAL_T_IS_FLOAT
#define EXPOF(value) (((*((reinterpret_cast<UINT32 *>(&value)))&0x7FFFFFFF)>>23)-127)
#else
#define EXPOF(value) (((*((reinterpret_cast<UINT64 *>(&value)))&0x7FFFFFFFFFFFFFFFULL)>>52)-1023)
#endif
// ULL indicates an unsigned long long literal constant

#endif
