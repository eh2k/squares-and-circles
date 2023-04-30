#ifndef rosic_BlendOscillator_h
#define rosic_BlendOscillator_h

// rosic-indcludes:
#include "rosic_NumberManipulations.h"

namespace rosic
{

  /**

  This is an oscillator that can continuously blend between two waveforms - this is more efficient
  than using two separate oscillators because the phase-accumulator has to be calculated only once
  for both waveforms.

  */

  static constexpr int WaveTableLength = 2048;
  typedef real_t WaveTable[12][2052];

  class BlendOscillator
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    BlendOscillator();

    /** Destructor. */
    ~BlendOscillator();

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets the sample-rateRate(). */
    void setSampleRate(real_t newSampleRate);

    /** Set start phase (range 0 - 360 degrees). */
    void setStartPhase(real_t StartPhase);

    /** An object of class WaveTable should be passed with this function which will be used in the 
    oscillator. Not to have "our own" WaveTable-object as member-variable avoids the need to have 
    the same waveform for different synth-voices multiple times in the memory. This function sets 
    the 1st wavetable. */
    void setWaveTables(const WaveTable* newWaveTable1, const WaveTable* newWaveTable2);

    /** Sets the blend/mix factor between the two waveforms. The value is expected between 0...1
    where 0 means waveform1 only, 1 means waveform2 only - in between there will be a linear blend
    between the two waveforms. */
    void setBlendFactor(real_t newBlendFactor) { blend = newBlendFactor; }

    /** Sets the frequency of the oscillator. */
    INLINE void setFrequency(real_t newFrequency);

    /** Sets the phase increment from outside. */
    INLINE void setIncrement(real_t newIncrement) { increment = newIncrement; }

    //---------------------------------------------------------------------------------------------
    // inquiry:

    /** Returns the blend/mix factor between the two waveforms as a value between 0...1 where 0 
    means waveform1 only, 1 means waveform2 only - in between there will be a linear blend between 
    the two waveforms. */
    real_t getBlendFactor() const { return blend; }

    /** Returns the phase increment. */
    INLINE real_t getIncrement() const { return increment; }

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates one output sample at a time. */
    INLINE real_t getSample();

    //---------------------------------------------------------------------------------------------
    // others:

    /** Calculates the phase-increments for first and second half-period according to freq and 
    pulseWidth. */
    INLINE void calculateIncrement();

    INLINE real_t interpolateLinear(const real_t* tableSet, int integerPart, real_t fractionalPart);

    /** Resets the phaseIndex to startIndex. */
    void resetPhase();

    /** Reset the phaseIndex to startIndex+PhaseIndex. */
    void setPhase(real_t PhaseIndex);

    //=============================================================================================

  protected:

    real_t tableLengthDbl;    // tableLength as real_t variable
    real_t phaseIndex;        // current phase index
    real_t freq;              // frequency of the oscillator
    real_t increment;         // phase increment per sample
    real_t blend;             // the blend factor between the two waveforms
    real_t startIndex;        // start-phase-index of the osc (range: 0 - tableLength)
    real_t sampleRate;        // the samplerate
    real_t sampleRateRec;     // 1/sampleRate

    const WaveTable *waveTable1, *waveTable2; // the 2 wavetables between which we blend

  };

  //-----------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE void BlendOscillator::setFrequency(real_t newFrequency)
  {
    if( (newFrequency > 0.0) && (newFrequency < 20000.0) )
      freq = newFrequency;
  }

  INLINE void BlendOscillator::calculateIncrement()
  {
    increment = tableLengthDbl*freq*sampleRateRec;
  }

  INLINE real_t BlendOscillator::interpolateLinear(const real_t* tableSet, int integerPart, real_t fractionalPart)
  {
    return   (1.0-fractionalPart) * tableSet[integerPart] 
           +      fractionalPart  * tableSet[integerPart+1];
  }

  INLINE real_t BlendOscillator::getSample()
  {
    real_t out1, out2;
    int    tableNumber;

    // from this increment, decide which table is to be used:
    tableNumber  = ((int)EXPOF(increment));
    //tableNumber += 1;           // generate frequencies up to nyquist/2 on the highest note
    tableNumber += 2;             // generate frequencies up to nyquist/4 on the highest note
                                  // \todo: make this number adjustable from outside

    // wraparound if necessary:
    while( phaseIndex>=tableLengthDbl )
      phaseIndex -= tableLengthDbl;

    int    intIndex = floorInt(phaseIndex);
    real_t frac     = phaseIndex  - (real_t) intIndex;

    out1 = (1.0-blend) * interpolateLinear(&(*waveTable1)[tableNumber][0], intIndex, frac);
    out2 =      blend  * interpolateLinear(&(*waveTable2)[tableNumber][0], intIndex, frac);
    
    out2 *= 0.5; // \todo: this is preliminary to scale the square in AciDevil we need to
                 // implement something more general here (like a kind of crest-compensation in 
                 // the wavetable-class)

    phaseIndex += increment;
    return out1 + out2;
  }

} // end namespace rosic

#endif // rosic_BlendOscillator_h
