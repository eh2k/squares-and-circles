#ifndef rosic_LeakyIntegrator_h
#define rosic_LeakyIntegrator_h

// rosic-indcludes:
#include "rosic_RealFunctions.h"

namespace rosic
{

  /**

  This is a leaky integrator type lowpass filter with user adjustable time constant which is set 
  up in milliseconds.

  */

  class LeakyIntegrator  
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    LeakyIntegrator();  

    /** Destructor. */
    ~LeakyIntegrator();  

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets the sample-rate. */
    void setSampleRate(real_t newSampleRate);    

    /** Sets the time constant (tau), value is expected in milliseconds. */
    void setTimeConstant(real_t newTimeConstant); 

    /** Sets the internal state of the filter to the passed value. */
    void setState(real_t newState) { y1 = newState; }

    //---------------------------------------------------------------------------------------------
    // inquiry:

    /** Returns the time constant (tau) in milliseconds. */
    real_t getTimeConstant() const { return tau; }

    /** Returns the normalizer, required to normalize the impulse response of a series connection 
    of two digital RC-type filters with time constants tau1 and tau2 (in milliseconds) to unity at 
    the given samplerate. */
    static real_t getNormalizer(real_t tau1, real_t tau2, real_t sampleRate);

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates one sample at a time. */
    INLINE real_t getSample(real_t in);

    //---------------------------------------------------------------------------------------------
    // others:

    /** Resets the internal state of the filter. */
    void reset();

    //=============================================================================================

  protected:

    /** Calculates the filter coefficient. */
    void calculateCoefficient();

    real_t coeff;        // filter coefficient
    real_t y1;           // previous output sample
    real_t sampleRate;   // the samplerate
    real_t tau;          // time constant in milliseconds

  };

  //-----------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE real_t LeakyIntegrator::getSample(real_t in)
  {
    return y1 = in + coeff*(y1-in);
  }

} // end namespace rosic

#endif 
