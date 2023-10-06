#ifndef rosic_DecayEnvelope_h
#define rosic_DecayEnvelope_h

// rosic-indcludes:
#include "rosic_RealFunctions.h"

namespace rosic
{

  /**

  This is a class implements an envelope generator that realizes a pure exponential decay. The 
  output of the envelope is normalized to the range 0...1.

  */

  class DecayEnvelope
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    DecayEnvelope();  

    /** Destructor. */
    ~DecayEnvelope(); 

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets the sample-rate. */
    void setSampleRate(real_t newSampleRate);  

    /** Sets the time constant for the multiplicative accumulator (which we consider as primarily 
    responsible for the decaying part) in milliseconds. */
    void setDecayTimeConstant(real_t newTimeConstant);

    /** Switches into a mode where the normalization is not made with respect to the peak amplitude 
    but to the sum of the impulse response - if true, the output will be equivalent to a leaky 
    integrator's impulse response. */
    void setNormalizeSum(bool shouldNormalizeSum);

    //---------------------------------------------------------------------------------------------
    // inquiry:

    /** Returns the length of the decay phase (in milliseconds). */
    real_t getDecayTimeConstant() const { return tau; }

    /** True, if output is below some threshold. */
    bool endIsReached(real_t threshold);  

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates one output sample at a time. */
    INLINE real_t getSample();    

    //---------------------------------------------------------------------------------------------
    // others:

    /** Triggers the envelope - the next sample retrieved via getSample() will be 1. */
    void trigger();

  protected:

    /** Calculates the coefficient for multiplicative accumulation. */
    void calculateCoefficient();

    real_t c;             // coefficient for multiplicative accumulation
    real_t y;             // previous output
    real_t yInit;         // initial yalue for previous output (= y/c)
    real_t tau;           // time-constant (in milliseconds)
    real_t fs;            // sample-rate
    bool   normalizeSum;  // flag to indicate that the output should be normalized such that the 
                          // sum of the impulse response is unity (instead of the peak) - if true
                          // the output will be equivalent to a leaky integrator's impulse 
                          // response

  };

  //-----------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE real_t DecayEnvelope::getSample()
  {
    y *= c;
    return y;
  }

} // end namespace rosic

#endif // rosic_DecayEnvelope_h
