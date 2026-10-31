#include "rosic_BlendOscillator.h"
#include "rosic_RealFunctions.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

BlendOscillator::BlendOscillator()
{
  // init member variables:
  tableLengthDbl       = (real_t) WaveTableLength;  // typecasted version
  sampleRate           = SAMPLE_RATE;
  freq                 = 440.0;
  increment            = (tableLengthDbl*freq)/sampleRate;
  phaseIndex           = 0.0;
  startIndex           = 0.0;
  waveTable1           = NULL;
  waveTable2           = NULL;
  pulseFreq            = -1.0;
  pulseWidthPoly       = NULL;
  pulseSagPoly         = NULL;
  pulseGainSet         = 0.0;
  pulseSmoothing       = 0.0;
  pulseLowpass         = 1.0;
  pulseState           = 0.0;
  tableOffset          = 2;

  // somewhat redundant:
  setSampleRate(sampleRate);       // sampleRate = 44100 Hz by default
  setFrequency (freq);            // frequency = 440 Hz by default
  setStartPhase(0.0);              // sartPhase = 0 by default

  //setWaveForm1(MipMappedWaveTable::SAW);
  //setWaveForm2(MipMappedWaveTable::SQUARE);

  resetPhase();
}

BlendOscillator::~BlendOscillator()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void BlendOscillator::setSampleRate(real_t newSampleRate)
{
  if( newSampleRate > 0.0 )
    sampleRate = newSampleRate;
  sampleRateRec = 1.0 / sampleRate;
  increment = tableLengthDbl*freq*sampleRateRec;
}

void BlendOscillator::setWaveTables(const WaveTable* newWaveTable1, const WaveTable* newWaveTable2)
{
  waveTable1 = newWaveTable1;
  waveTable2 = newWaveTable2;
}

void BlendOscillator::setStartPhase(real_t StartPhase)
{
  if( (StartPhase>=0) && (StartPhase<=360) )
    startIndex = (StartPhase/360.0)*tableLengthDbl;
}

//-------------------------------------------------------------------------------------------------
// event processing:

void BlendOscillator::calculatePulse()
{
  // [eh2k] the character's pulse (setPulse()): its width and sag from the pitch, x octaves above C2,
  // held to C1 - 6 semitones ... C4
  pulseFreq = freq;
  const real_t x = clip((real_t)log2f(freq * (1.0 / 65.40639132514966)), (real_t)-1.5, (real_t)2.0);
  if( pulseWidthPoly == NULL )
    return;
  real_t w = pulseWidthPoly[0] + x*(pulseWidthPoly[1] + x*pulseWidthPoly[2]);
  w = clip(w, (real_t)0.05, (real_t)0.95);
  pulseDelay = w * tableLengthDbl;
  pulseSag   = pulseSagPoly[0] + x*pulseSagPoly[1];
  pulseGain  = pulseGainSet;
  if( pulseSmoothing > 0.0 )
    pulseLowpass = 1.0 - expf(-2.0 * PI * pulseSmoothing * freq * sampleRateRec);
}

void BlendOscillator::setPulse(const real_t *width, const real_t *sag, real_t gain, real_t smoothing,
                               int newTableOffset)
{
  pulseWidthPoly = width;
  pulseSagPoly   = sag;
  pulseGainSet   = gain;
  pulseSmoothing = smoothing;
  tableOffset    = newTableOffset;
  pulseFreq      = -1.0; // calculatePulse() at the next increment
}

void BlendOscillator::resetPhase()
{
  phaseIndex = startIndex;
}

void BlendOscillator::setPhase(real_t PhaseIndex)
{
  phaseIndex = startIndex+PhaseIndex;
}
