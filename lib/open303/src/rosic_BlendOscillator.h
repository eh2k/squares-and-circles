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

    /** [eh2k] Sets the pulse's width (the delay between the two reads of the table) and sag from
    the frequency. */
    void calculatePulse();

    /** [eh2k] The pulse: gain*(saw(t) - sag*saw(t + width)), width = width[0] + width[1]x +
    width[2]x^2, sag = sag[0] + sag[1]x (x the octaves above C2, held to -1.5...2), a one-pole lowpass
    at smoothing times the frequency (0: none), and the table offset (1: frequencies up to nyquist/2,
    2: up to nyquist/4). The arrays are kept, not copied. */
    void setPulse(const real_t *width, const real_t *sag, real_t gain, real_t smoothing, int newTableOffset);

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
    real_t pulseFreq;         // [eh2k] the frequency the pulse's terms are for
    real_t pulseDelay;        // the pulse's width as a delay in the table (in samples)
    real_t pulseSag;          // the scale of the delayed copy
    real_t pulseGain;         // the pulse's level
    const real_t *pulseWidthPoly, *pulseSagPoly; // setPulse()'s
    real_t pulseGainSet;
    real_t pulseSmoothing;    // the lowpass at this many times the frequency (0: none)
    real_t pulseLowpass;      // its coefficient
    real_t pulseState;
    int    tableOffset;

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
    if( freq != pulseFreq ) // [eh2k]
      calculatePulse();
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
    tableNumber += tableOffset;   // [eh2k] 1: frequencies up to nyquist/2, 2: up to nyquist/4
    if( tableNumber > 11 )
      tableNumber = 11;
    if( tableNumber < 0 ) // [eh2k] below 48000 / 2048 / 2^tableOffset Hz (11.7 Hz with 1): the first table, not the
      tableNumber = 0;    // memory before the mip map

    // wraparound if necessary:
    while( phaseIndex>=tableLengthDbl )
      phaseIndex -= tableLengthDbl;

    int    intIndex = floorInt(phaseIndex);
    real_t frac     = phaseIndex  - (real_t) intIndex;
    const real_t *table = &(*waveTable1)[tableNumber][0];

    out1 = interpolateLinear(table, intIndex, frac);
    out2 = 0.0;
    if( blend > 0.0 )
    {
      // [eh2k] the pulse: the saw minus a delayed and scaled copy of itself, its width and
      // the scale (the top's sag) by the pitch
      real_t phase2 = phaseIndex + pulseDelay;
      if( phase2 >= tableLengthDbl )
        phase2 -= tableLengthDbl;
      int    intIndex2 = floorInt(phase2);
      real_t frac2     = phase2 - (real_t) intIndex2;
      out2 = pulseGain * (out1 - pulseSag * interpolateLinear(table, intIndex2, frac2));
      if( pulseSmoothing > 0.0 )
        out2 = pulseState += pulseLowpass * (out2 - pulseState);
    }

    phaseIndex += increment;
    return (1.0-blend) * out1 + blend * out2;
  }

} // end namespace rosic

#endif // rosic_BlendOscillator_h
