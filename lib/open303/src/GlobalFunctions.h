#ifndef GlobalFunctions_h
#define GlobalFunctions_h

#include <math.h>
#include <stdlib.h>
#include "GlobalDefinitions.h"

namespace rosic { //[eh2k] start namespace

  //[eh2k]>
  #ifdef REAL_T_IS_FLOAT
  
  INLINE real_t sin(real_t x)
  {
    return sinf(x);
  }

  INLINE real_t sinh(real_t x)
  {
    return sinhf(x);
  }

  INLINE real_t cos(real_t x)
  {
    return cosf(x);
  }

  INLINE real_t cosh(real_t x)
  {
    return coshf(x);
  }

  INLINE real_t log(real_t x)
  {
    return logf(x);
  }

  INLINE real_t fabs(real_t x)
  {
    return fabsf(x);
  }

  INLINE real_t floor(real_t x)
  {
    return floorf(x);
  }

  INLINE real_t sqrt(real_t x)
  {
    return sqrtf(x);
  }

  INLINE real_t exp(real_t x)
  {
    return expf(x);
  }

  INLINE real_t pow(real_t x, real_t y)
  {
    return powf(x, y);
  }

  INLINE real_t fmod(real_t x, real_t y)
  {
    return fmodf(x, y);
  }
  #endif
  //<[eh2k]

/** This file contains a bunch of useful macros and functions which are not wrapped into the
rosic namespace to facilitate their global use. */

/** Converts a raw amplitude value/factor to a value in decibels. */
INLINE real_t amp2dB(real_t amp);

/** Converts a raw amplitude value/factor to a value in decibels with a check, if the amplitude is
close to zero (to avoid log-of-zero and related errors). */
INLINE real_t amp2dBWithCheck(real_t amp, real_t lowAmplitude = 0.000001);

/** Returns the index of the maximum value in an array of doubles where the array should be of
length numValues. */
template <class T>
INLINE int arrayMaxIndex(T* theArray, int numValues);

/** Returns the index of the minimum value in an array of doubles where the array should be of
length numValues. */
template <class T>
INLINE int arrayMinIndex(T* theArray, int numValues);

/** Converts a time-stamp given in beats into seconds acording to a tempo measured in beats per
minute (bpm). */
INLINE real_t beatsToSeconds(real_t beat, real_t bpm);

/** Converts a value in decibels to a raw amplitude value/factor. */
INLINE real_t dB2amp(real_t x);

/** Converts an angle in degrees into radiant. */
INLINE real_t degreeToRadiant(real_t degrees);

/** Frees the memory associated with the pointer ans sets the poiter itself to NULL */
//INLINE void deleteAndNullifyPointer(void *pointer);

/** Returns the Euclidean distance between points at coordinates (x1,y1), (x2,y2). */
INLINE real_t euclideanDistance(real_t x1, real_t y1, real_t x2, real_t y2);

/** Calculates the exponential function with base 10. */
INLINE real_t exp10(real_t x);

/** Calculates the exponential function with base 2. */
INLINE real_t exp2(real_t x);

/** Calculates the factorial of some integer n >= 0. */
//INLINE int factorial(int n);

/** Converts a frequency in Hz into a MIDI-note value assuming A4 = 440 Hz. */
INLINE real_t freqToPitch(real_t freq);

/** Converts a frequency in Hz into a MIDI-note value for tunings different than the
default 440 Hz. */
INLINE real_t freqToPitch(real_t freq, real_t masterTuneA4);

/** Checks a pointer for nullity and if it is not NULL, it calls delete for the associated object
and then sets the pointer to NULL. */
INLINE void ifNotNullDeleteAndSetNull(void* pointer);

/** Maps an integer index in the range 0...numIndices-1 into a normalized floating point number in 
the range 0...1. */
INLINE float indexToNormalizedValue(int index, int numIndices);

/** Checks, if x is close to some target-value within some tolerance. */
INLINE bool isCloseTo(real_t x, real_t targetValue, real_t tolerance);

/** Checks, if x is even. */
INLINE bool isEven(int x);

/** Checks, if x is odd. */
INLINE bool isOdd(int x);

/** Checks, if x is a power of 2. */
INLINE bool isPowerOfTwo(unsigned int x);

/** Calculates the logarithm to base 2. */
INLINE real_t log2(real_t x);

/** Calculates logarithm to an arbitrary base b. */
INLINE real_t logB(real_t x, real_t b);

/** Converts a value between inMin and inMax into a value between outMin and outMax where the
mapping is linear for the input and the output. Example: y = linToLin(x, 0.0, 1.0, -96.0, 24.0)
will map the input x assumed to lie inside 0.0...1.0 to the range between -96.0...24.0. This
function is useful to convert between parameter representations between 0.0...1.0 and the
clear-text parameters. */
INLINE real_t linToLin(real_t in, real_t inMin, real_t inMax, real_t outMin, real_t outMax);

/** Converts a value between inMin and inMax into a value between outMin and outMax where the
mapping of the output is exponential. Example: y = linToExp(x, 0.0, 1.0, 20.0, 20000.0) will map
the input x assumed to lie inside 0.0...1.0 to the range between 20.0...20000.0 where equal
differences in the input lead to equal factors in the output. Make sure that the outMin value is
greater than zero! */
INLINE real_t linToExp(real_t in, real_t inMin, real_t inMax, real_t outMin, real_t outMax);

/** Same as linToExp but adds an offset afterwards and compensates for that offset by scaling the
offsetted value so as to hit the outMax correctly. */
INLINE real_t linToExpWithOffset(real_t in, real_t inMin, real_t inMax, real_t outMin,
                                 real_t outMax, real_t offset = 0.0);

/** The Inverse of "linToExp" */
INLINE real_t expToLin(real_t in, real_t inMin, real_t inMax, real_t outMin, real_t outMax);

/** The Inverse of "linToExpWithOffset" */
INLINE real_t expToLinWithOffset(real_t in, real_t inMin, real_t inMax, real_t outMin,
                                 real_t outMax, real_t offset = 0.0);

/** Returns a power of two which is greater than or equal to the input argument. */
template <class T>
INLINE T nextPowerOfTwo(T x);

/** Maps a normalized floating point number in the range 0...1 into an integer index in the range 
0...numIndices-1. */
INLINE int normalizedValueToIndex(float normalizedValue, int numIndices);

/** Converts a picth-offset in semitones value into a frequency multiplication factor. */
INLINE real_t pitchOffsetToFreqFactor(real_t pitchOffset);

/** Converts a MIDI-note value into a frequency in Hz assuming A4 = 440 Hz. */
INLINE real_t pitchToFreq(real_t pitch);

/** Converts a MIDI-note value into a frequency in Hz for arbitrary master-tunings of A4. */
INLINE real_t pitchToFreq(real_t pitch, real_t masterTuneA4);

/** Converts an angle in radiant into degrees. */
INLINE real_t radiantToDegree(real_t radiant);

/** Generates a random number that is uniformly distributed between min and max (inclusive). The
underlying integer pseudo random number generator is a linear congruential with period length of 
2^32. It is based on Numerical Recipies in C (2nd edition), page 284. You may pass a seed to the 
first call to initialize it - otherwise it will use 0 as seed. A negative number (as in the default 
argument) will indicate to not initialize the state and just generate a random number based on the 
last state (which is the case for a typical call). */
INLINE real_t randomUniform(real_t min = 0.0, real_t max = 1.0, int seed = -1);

/** Returns the nearest integer (as real_t, without typecast). */
INLINE real_t round(real_t x);

/** Converts a time value in seconds into a time value measured in beats. */
INLINE real_t secondsToBeats(real_t timeInSeconds, real_t bpm);

/** Returns the sign of x as real_t. */
INLINE real_t sign(real_t x);

/** Converts a time-stamp given in whole notes into seconds according to a tempo measured in
beats per minute (bpm). */
INLINE real_t wholeNotesToSeconds(real_t noteValue, real_t bpm);

//=================================================================================================
//implementation:

INLINE real_t amp2dB(real_t amp)
{
  return 8.6858896380650365530225783783321 * log(amp);
  //return 20*log10(amp); // naive version
}

INLINE real_t amp2dBWithCheck(real_t amp, real_t lowAmplitude)
{
  if( amp >= lowAmplitude )
    return 8.6858896380650365530225783783321 * log(amp);
  else
    return 8.6858896380650365530225783783321 * log(lowAmplitude);
}

template <class T>
INLINE int arrayMaxIndex(T* theArray, int numValues)
{
  int    maxIndex = 0;
  real_t maxValue = theArray[0];
  for(int i=0; i<numValues; i++)
  {
    if( theArray[i] > maxValue )
    {
      maxValue = theArray[i];
      maxIndex = i;
    }
  }
  return maxIndex;
}

template <class T>
INLINE int arrayMinIndex(T* theArray, int numValues)
{
  int    minIndex = 0;
  real_t minValue = theArray[0];
  for(int i=0; i<numValues; i++)
  {
    if( theArray[i] < minValue )
    {
      minValue = theArray[i];
      minIndex = i;
    }
  }
  return minIndex;
}

INLINE real_t beatsToSeconds(real_t beat, real_t bpm)
{
  return (60.0/bpm)*beat;
}

INLINE real_t dB2amp(real_t dB)
{
  return exp(dB * 0.11512925464970228420089957273422);
  //return pow(10.0, (0.05*dB)); // naive, inefficient version
}

INLINE real_t degreeToRadiant(real_t degrees)
{
  return (PI/180.0)*degrees;
}

/*
INLINE void deleteAndNullifyPointer(void *pointer)
{
  delete pointer;
  pointer = NULL;
}
*/

INLINE real_t euclideanDistance(real_t x1, real_t y1, real_t x2, real_t y2)
{
  return sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
}

INLINE real_t exp10(real_t x)
{
  return exp(LN10*x);
}

INLINE real_t exp2(real_t x)
{
  return exp(LN2*x);
}

INLINE real_t freqToPitch(real_t freq)
{
  return 12.0 * log2(freq/440.0) + 69.0;
}

INLINE real_t freqToPitch(real_t freq, real_t masterTuneA4)
{
  return 12.0 * log2(freq/masterTuneA4) + 69.0;
}

/*
INLINE void ifNotNullDeleteAndSetNull(void* pointer)
{
  if( pointer != NULL )
  {
    delete pointer;
    pointer = NULL;
  }
}
*/

INLINE float indexToNormalizedValue(int index, int numIndices)
{
  return (float) (2*index+1) / (float) (2*numIndices);
}

INLINE bool isCloseTo(real_t x, real_t targetValue, real_t tolerance)
{
  if( fabs(x-targetValue) <= tolerance )
    return true;
  else
    return false;
}

INLINE bool isEven(int x)
{
  if( x%2 == 0 )
    return true;
  else
    return false;
}

INLINE bool isOdd(int x)
{
  if( x%2 != 0 )
    return true;
  else
    return false;
}

INLINE bool isPowerOfTwo(unsigned int x)
{
  unsigned int currentPower = 1;
  while( currentPower <= x )
  {
    if( currentPower == x )
      return true;
    currentPower *= 2;
  }
  return false;
}

INLINE real_t log2(real_t x)
{
  return ONE_OVER_LN2*log(x);
}

INLINE real_t logB(real_t x, real_t b)
{
  return log(x)/log(b);
}

INLINE real_t linToLin(real_t in, real_t inMin, real_t inMax, real_t outMin, real_t outMax)
{
  // map input to the range 0.0...1.0:
  real_t tmp = (in-inMin) / (inMax-inMin);

  // map the tmp-value to the range outMin...outMax:
  tmp *= (outMax-outMin);
  tmp += outMin;

  return tmp;
}

INLINE real_t linToExp(real_t in, real_t inMin, real_t inMax, real_t outMin, real_t outMax)
{
  // map input to the range 0.0...1.0:
  real_t tmp = (in-inMin) / (inMax-inMin);

  // map the tmp-value exponentially to the range outMin...outMax:
  //tmp = outMin * exp( tmp*(log(outMax)-log(outMin)) );
  return outMin * exp( tmp*(log(outMax/outMin)) );
}

INLINE real_t linToExpWithOffset(real_t in, real_t inMin, real_t inMax, real_t outMin,
                                 real_t outMax, real_t offset)
{
  real_t tmp = linToExp(in, inMin, inMax, outMin, outMax);
  tmp += offset;
  tmp *= outMax/(outMax+offset);
  return tmp;
}

INLINE real_t expToLin(real_t in, real_t inMin, real_t inMax, real_t outMin, real_t outMax)
{
  real_t tmp = log(in/inMin) / log(inMax/inMin);
  return outMin + tmp * (outMax-outMin);
}

INLINE real_t expToLinWithOffset(real_t in, real_t inMin, real_t inMax, real_t outMin,
                                 real_t outMax, real_t offset)
{
  real_t tmp = in*(inMax+offset)/inMax;
  tmp -= offset;
  return expToLin(tmp, inMin, inMax, outMin, outMax);
  /*
  real_t tmp = linToExp(in, inMin, inMax, outMin, outMax);
  tmp += offset;
  tmp *= outMax/(outMax+offset);
  return tmp;
  */
}

template <class T>
INLINE T nextPowerOfTwo(T x)
{
  T accu = 1;
  while(accu < x)
    accu *= 2;
  return accu;
}

INLINE int normalizedValueToIndex(float normalizedValue, int numIndices)
{
  return (int) floor(normalizedValue*numIndices);
}

INLINE real_t pitchOffsetToFreqFactor(real_t pitchOffset)
{
  return exp(0.057762265046662109118102676788181 * pitchOffset);
  //return pow(2.0, pitchOffset/12.0); // naive, slower but numerically more precise
}

INLINE real_t pitchToFreq(real_t pitch)
{
  return 8.1757989156437073336828122976033 * exp(0.057762265046662109118102676788181*pitch);
  //return 440.0*( pow(2.0, (pitch-69.0)/12.0) ); // naive, slower but numerically more precise
}

INLINE real_t pitchToFreq(real_t pitch, real_t masterTuneA4)
{
  return masterTuneA4 * 0.018581361171917516667460937040007
    * exp(0.057762265046662109118102676788181*pitch);
}

INLINE real_t radiantToDegree(real_t radiant)
{
  return (180.0/PI)*radiant;
}

INLINE real_t randomUniform(real_t min, real_t max, int seed)
{
  static unsigned long state = 0;
  if( seed >= 0 )
    state = seed;                                        // initialization, if desired
  state = 1664525*state + 1013904223;                    // mod implicitely by integer overflow
  return min + (max-min) * ((1.0/4294967296.0) * state); // transform to desired range
}

INLINE real_t round(real_t x)
{
  if( x-floor(x) >= 0.5 )
    return ceil(x);
  else
    return floor(x);
}

INLINE real_t secondsToBeats(real_t timeInSeconds, real_t bpm)
{
  return timeInSeconds*(bpm/60.0);
}

INLINE real_t sign(real_t x)
{
  if(x<0)
    return -1.0;
  else if(x>0)
    return 1.0;
  else
    return 0;
}

INLINE real_t wholeNotesToSeconds(real_t noteValue, real_t bpm)
{
  return (240.0/bpm)*noteValue;
}

} //[eh2k] end namespace

#endif // #ifndef GlobalFunctions_h
