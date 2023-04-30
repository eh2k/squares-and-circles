#ifndef rosic_BiquadFilter_h
#define rosic_BiquadFilter_h

// rosic-indcludes:
#include "rosic_RealFunctions.h"

namespace rosic
{

  /**

  This is an implementation of a simple one-pole filter unit.

  */

  class BiquadFilter
  {

  public:

    /** Enumeration of the available filter modes. */
    enum modes
    {
      BYPASS = 0,
      LOWPASS6,
      LOWPASS12,
      HIGHPASS6,
      HIGHPASS12,
      BANDPASS,
      BANDREJECT,
      PEAK,
      LOW_SHELF,
      //HIGH_SHELF,
      //ALLPASS,

      NUM_FILTER_MODES
    };

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    BiquadFilter();   

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets the sample-rate (in Hz) at which the filter runs. */
    void setSampleRate(real_t newSampleRate);

    /** Sets the filter mode as one of the values in enum modes. */
    void setMode(int newMode);

    /** Sets the center frequency in Hz. */
    void setFrequency(real_t newFrequency);

    /** Sets the boost/cut gain in dB. */
    void setGain(real_t newGain);

    /** Sets the bandwidth in octaves. */
    void setBandwidth(real_t newBandwidth);

    //---------------------------------------------------------------------------------------------
    // inquiry

    /** Sets the filter mode as one of the values in enum modes. */
    int getMode() const { return mode; }

    /** Returns the center frequency in Hz. */
    real_t getFrequency() const { return frequency; }

    /** Returns the boost/cut gain in dB. */
    real_t getGain() const { return gain; }

    /** Returns the bandwidth in octaves. */
    real_t getBandwidth() const { return bandwidth; }

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

    // internal functions:
    void calcCoeffs();  // calculates filter coefficients from filter parameters

    real_t b0, b1, b2, a1, a2;
    real_t x1, x2, y1, y2;

    real_t frequency, gain, bandwidth;
    real_t sampleRate;
    int    mode;

  };

  //-----------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE real_t BiquadFilter::getSample(real_t in)
  {
    // calculate the output sample:
    real_t y = b0*in + b1*x1 + b2*x2 + a1*y1 + a2*y2 + TINY;

    // update the buffer variables:
    x2 = x1;
    x1 = in;
    y2 = y1;
    y1 = y;

    return y;
  }

} // end namespace rosic

#endif // rosic_BiquadFilter_h
