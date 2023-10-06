#include "rosic_BlendOscillator.h"
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

void BlendOscillator::resetPhase()
{
  phaseIndex = startIndex;
}

void BlendOscillator::setPhase(real_t PhaseIndex)
{
  phaseIndex = startIndex+PhaseIndex;
}
