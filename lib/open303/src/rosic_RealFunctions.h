#ifndef rosic_RealFunctions_h
#define rosic_RealFunctions_h

// standard library includes:
#include <math.h>
#include <stdlib.h>

// rosic includes:
#include "GlobalFunctions.h"
#include "rosic_NumberManipulations.h"

namespace rosic
{

  /** Inverse hyperbolic sine. */
  INLINE real_t asinh(real_t x);

  /** Returns -1.0 if x is below low, 0.0 if x is between low and high and 1.0 if x is above high. */
  INLINE real_t belowOrAbove(real_t x, real_t low, real_t high);

  /** Clips x into the range min...max. */
  template <class T>
  INLINE T clip(T x, T min, T max);

  /** Evaluates the quartic polynomial y = a4*x^4 + a3*x^3 + a2*x^2 + a1*x + a0 at x. */
  INLINE real_t evaluateQuartic(real_t x, real_t a0, real_t a1, real_t a2, real_t a3, real_t a4);

  /** foldover at the specified value */
  INLINE real_t foldOver(real_t x, real_t min, real_t max);

  /** Computes an integer power of x by successively multiplying x with itself. */
  INLINE real_t integerPower(real_t x, int exponent);

  /** Generates a pseudo-random number between min and max. */
  INLINE real_t random(real_t min=0.0, real_t max=1.0);

  /** Generates a 2*pi periodic saw wave. */
  INLINE real_t sawWave(real_t x);

  /** Calculates sine and cosine of x - this is more efficient than calling sin(x) and
  cos(x) seperately. */
  INLINE void sinCos(real_t x, real_t* sinResult, real_t* cosResult);

  /** Calculates a parabolic approximation of the sine and cosine of x. */
  INLINE void sinCosApprox(real_t x, real_t* sinResult, real_t* cosResult);

  /** Generates a 2*pi periodic square wave. */
  INLINE real_t sqrWave(real_t x);

  /** Rational approximation of the hyperbolic tangent. */
  INLINE real_t tanhApprox(real_t x);

  /** Generates a 2*pi periodic triangle wave. */
  INLINE real_t triWave(real_t x);

  //===============================================================================================
  // implementation:

  INLINE real_t asinh(real_t x)
  {
    return log(x + sqrt(x*x+1) );
  }

  INLINE real_t belowOrAbove(real_t x, real_t low, real_t high)
  {
    if( x < low )
      return -1.0;
    else if ( x > high )
      return 1.0;
    else
      return 0.0;
  }

  template <class T>
  INLINE T clip(T x, T min, T max)
  {
    if( x > max )
      return max;
    else if ( x < min )
      return min;
    else return x;
  }

  INLINE real_t evaluateQuartic(real_t x, real_t a0, real_t a1, real_t a2, real_t a3, real_t a4)
  {
    real_t x2 = x*x;
    return x*(a3*x2+a1) + x2*(a4*x2+a2) + a0;
  }

  INLINE real_t foldOver(real_t x, real_t min, real_t max)
  {
    if( x > max )
      return max - (x-max);
    else if( x < min )
      return min - (x-min);
    else return x;
  }

  INLINE real_t integerPower(real_t x, int exponent)
  {
    real_t accu = 1.0;
    for(int i=0; i<exponent; i++)
      accu *= x;
    return accu;
  }

  INLINE real_t random(real_t min, real_t max)
  {
    real_t tmp = (1.0/RAND_MAX) * rand() ;  // between 0...1
    return linToLin(tmp, 0.0, 1.0, min, max);
  }

  INLINE real_t sawWave(real_t x)
  {
    real_t tmp = fmod(x, 2*PI);
    if( tmp < PI )
      return tmp/PI;
    else
      return (tmp/PI)-2.0;
  }

  INLINE void sinCos(real_t x, real_t* sinResult, real_t* cosResult)
  {
    #ifdef __GNUC__  // \todo assembly-version causes compiler errors on gcc
      *sinResult = sin(x);
      *cosResult = cos(x);
    #else
      real_t s, c;     // do we need these intermediate variables?
      __asm fld x
      __asm fsincos
      __asm fstp c
      __asm fstp s
      *sinResult = s;
      *cosResult = c;
    #endif
  }

  INLINE void sinCosApprox(real_t x, real_t* sinResult, real_t* cosResult)
  {
    static const real_t c = 0.70710678118654752440;

    // restrict input x to the range 0.0...2*PI:
    while( x > 2.0*PI )
      x -= 2*PI;
    while( x < 0.0 )
      x += 2*PI;

    if( x < PI/2 )
    {
      real_t tmp1 = x;
      real_t tmp2 = (2/PI) * tmp1 - 0.5;
      real_t tmp3 = (2-4*c)*tmp2*tmp2 + c;
      *sinResult  = tmp3 + tmp2;
      *cosResult  = tmp3 - tmp2;
    }
    else if( x < PI )
    {
      real_t tmp1 = (x-PI/2);
      real_t tmp2 = 0.5 - (2/PI) * tmp1;
      real_t tmp3 = (2-4*c)*tmp2*tmp2 + c;
      *sinResult  = tmp2 + tmp3;
      *cosResult  = tmp2 - tmp3;
    }
    else if( x < 1.5*PI )
    {
      real_t tmp1 = (x-PI);
      real_t tmp2 = (2/PI) * tmp1 - 0.5;
      real_t tmp3 = (4*c-2)*tmp2*tmp2 - c;
      *sinResult  = tmp3 - tmp2;
      *cosResult  = tmp3 + tmp2;
    }
    else
    {
      real_t tmp1 = (x-1.5*PI);
      real_t tmp2 = (2/PI) * tmp1 - 0.5;
      real_t tmp3 = (2-4*c)*tmp2*tmp2 + c;
      *sinResult  = tmp2 - tmp3;
      *cosResult  = tmp2 + tmp3;
    }
  }

  INLINE real_t sqrWave(real_t x)
  {
    real_t tmp = fmod(x, 2*PI);
    if( tmp < PI )
      return 1.0;
    else
      return -1.0;
  }

  INLINE real_t tanhApprox(real_t x)
  {
    real_t a = fabs(2*x);
    real_t b = 24+a*(12+a*(6+a));
    return 2*(x*b)/(a*b+48);
  }

  INLINE real_t triWave(real_t x)
  {
    real_t tmp = fmod(x, 2*PI);
    if( tmp < 0.5*PI )
      return tmp/(0.5*PI);
    else if( tmp < 1.5*PI )
      return 1.0 - ((tmp-0.5*PI)/(0.5*PI));
    else
      return -1.0 + ((tmp-1.5*PI)/(0.5*PI));
  }

} // end namespace rosic

#endif // #ifndef rosic_RealFunctions_h
