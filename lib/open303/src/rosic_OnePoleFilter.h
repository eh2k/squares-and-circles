#ifndef rosic_OnePoleFilter_h
#define rosic_OnePoleFilter_h

// rosic-indcludes:
#include "rosic_RealFunctions.h"

namespace rosic
{

  /**

  This is an implementation of a simple one-pole filter unit.

  */

  class OnePoleFilter
  {

  public:

    /** This is an enumeration of the available filter modes. */
    enum modes
    {
      BYPASS = 0,
      LOWPASS,
      HIGHPASS,
      LOWSHELV,
      HIGHSHELV,
      ALLPASS
    };
    // \todo (maybe): let the user choose between LP/HP versions obtained via bilinear trafo and 
    // impulse invariant trafo

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    OnePoleFilter();   

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets the sample-rate. */
    void setSampleRate(real_t newSampleRate);

    /** Chooses the filter mode. See the enumeration for available modes. */
    void setMode(int newMode);

    /** Sets the cutoff-frequency for this filter. */
    void setCutoff(real_t newCutoff);

    /** This will set the time constant 'tau' for the case, when lowpass mode is chosen. This is 
    the time, it takes for the impulse response to die away to 1/e = 0.368... or equivalently, the
    time it takes for the step response to raise to 1-1/e = 0.632... */
    void setLowpassTimeConstant(real_t newTimeConstant) { setCutoff(1.0/(2*PI*newTimeConstant)); }

    /** Sets the gain factor for the shelving modes (this is not in decibels). */
    void setShelvingGain(real_t newGain);

    /** Sets the gain for the shelving modes in decibels. */
    void setShelvingGainInDecibels(real_t newGain);

    /** Sets the filter coefficients manually. */
    void setCoefficients(real_t newB0, real_t newB1, real_t newA1);

    /** Sets up the internal state variables for both channels. */
    void setInternalState(real_t newX1, real_t newY1);

    //---------------------------------------------------------------------------------------------
    // inquiry

    /** Returns the cutoff-frequency. */
    real_t getCutoff() const { return cutoff; }

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates a single filtered output-sample. */
    INLINE real_t getSample(real_t in);

    //---------------------------------------------------------------------------------------------
    // others:

    /** Resets the internal buffers (for the \f$ x[n-1], y[n-1] \f$-samples) to zero. */
    void reset();

    //=============================================================================================

  protected:

    // buffering:
    real_t x1, y1;

    // filter coefficients:
    real_t b0; // feedforward coeffs
    real_t b1;
    real_t a1; // feedback coeff

    // filter parameters:
    real_t cutoff;
    real_t shelvingGain;
    int    mode;  

    real_t sampleRate; 
    real_t sampleRateRec;  // reciprocal of the sampleRate

    // internal functions:
    void calcCoeffs();  // calculates filter coefficients from filter parameters

  };

  //-----------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE real_t OnePoleFilter::getSample(real_t in)
  {
    // calculate the output sample:
    y1 = b0*in + b1*x1 + a1*y1 + TINY;

    // update the buffer variables:
    x1 = in;

    return y1;
  }

} // end namespace rosic

#endif // rosic_OnePoleFilter_h
