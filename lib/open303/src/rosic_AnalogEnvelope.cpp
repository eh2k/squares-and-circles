#include "rosic_AnalogEnvelope.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

AnalogEnvelope::AnalogEnvelope()
{
  sampleRate     = SAMPLE_RATE;
  startLevel     = 0.0;
  attackTime     = 0.0;
  peakLevel      = 1.0;
  holdTime       = 0.0;
  decayTime      = 0.1;
  sustainLevel   = 0.5;
  releaseTime    = 0.01;
  endLevel       = 0.0;
  time           = 0.0;
  timeScale      = 1.0;
  peakByVel      = 1.0;
  peakByKey      = 1.0;
  timeScaleByVel = 1.0;
  timeScaleByKey = 1.0;
  increment      = 1000.0*timeScale/sampleRate;
  tauScale       = 1.0;
  peakScale      = 1.0;
  noteIsOn       = false;
  outputIsZero   = true;

  previousOutput = 0.0;

  // call these functions to trigger the coefficient calculations:
  setAttack(attackTime);
  setDecay(decayTime);
  setRelease(releaseTime);
}

AnalogEnvelope::~AnalogEnvelope()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void AnalogEnvelope::setSampleRate(real_t newSampleRate)
{
  if( newSampleRate > 0.0 )
    sampleRate = newSampleRate;

  // adjust time increment:
  increment = 1000.0*timeScale/sampleRate;

  //re-calculate coefficients for the 3 filters:
  setAttack (attackTime);
  setDecay  (decayTime);
  setRelease(releaseTime);
}

void AnalogEnvelope::setAttack(real_t newAttackTime)
{
  if( newAttackTime > 0.0 )
  {
    attackTime  = newAttackTime;
    real_t tau  = (sampleRate*0.001*attackTime) * tauScale/timeScale;
    attackCoeff = 1.0 - exp( -1.0 / tau );
  }
  else // newAttackTime <= 0
  {
    attackTime  = 0.0;
    attackCoeff = 1.0;
  }
  calculateAccumulatedTimes();
}

void AnalogEnvelope::setHold(real_t newHoldTime)
{
  if( newHoldTime >= 0 )
    holdTime = newHoldTime;
  calculateAccumulatedTimes();
}

void AnalogEnvelope::setDecay(real_t newDecayTime)
{
  if( newDecayTime > 0.0 )
  {
    decayTime  = newDecayTime;
    real_t tau = (sampleRate*0.001*decayTime) * tauScale/timeScale;
    decayCoeff = 1.0 - exp( -1.0 / tau  );
  }
  else // newDecayTime <= 0
  {
    decayTime  = 0.0;
    decayCoeff = 1.0;
  }
  calculateAccumulatedTimes();
}

void AnalogEnvelope::setRelease(real_t newReleaseTime)
{
  if( newReleaseTime > 0.0 )
  {
    releaseTime  = newReleaseTime;
    real_t tau   = (sampleRate*0.001*releaseTime) * tauScale/timeScale;
    releaseCoeff = 1.0 - exp( -1.0 / tau  );
  }
  else // newReleaseTime <= 0
  {
    releaseTime  = 0.0;
    releaseCoeff = 1.0;
  }
  calculateAccumulatedTimes();
}

void AnalogEnvelope::setTimeScale(real_t newTimeScale)
{
  if( newTimeScale > 0 )
    timeScale = newTimeScale;

  increment  = 1000.0*timeScale/sampleRate;

  //re-calculate coefficients for the 3 filters:
  setAttack (attackTime);
  setDecay  (decayTime);
  setRelease(releaseTime);
}

void AnalogEnvelope::setTauScale(real_t newTauScale)
{
  if( newTauScale > 0 )
    tauScale = newTauScale;

  setAttack(attackTime);
  setDecay(decayTime);
  setRelease(releaseTime);
}

void AnalogEnvelope::setPeakScale(real_t newPeakScale)
{
  if( newPeakScale > 0 )
    peakScale = newPeakScale;
}

//-------------------------------------------------------------------------------------------------
// others:

void AnalogEnvelope::reset()
{
  time = 0.0;
}

void AnalogEnvelope::noteOn(bool startFromCurrentLevel, int newKey, int newVel)
{
  if( !startFromCurrentLevel )
    previousOutput = startLevel;  // may lead to clicks


  // \todo: calculate key and velocity scale factors for duration and peak-value...


  // reset time for the new note:
  time         = 0.0;
  noteIsOn     = true;
  outputIsZero = false;
}

void AnalogEnvelope::noteOff()
{
  noteIsOn = false;

  // advance time to the beginnig of the release phase:
  time = (attackTime + holdTime + decayTime + increment);
}

bool AnalogEnvelope::endIsReached()
{
  //return false; // test

  if( noteIsOn == false && previousOutput < 0.000001 )
    return true;
  else
    return false;
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void AnalogEnvelope::calculateAccumulatedTimes()
{
  attPlusHld               = attackTime + holdTime;
  attPlusHldPlusDec        = attPlusHld + decayTime;
  attPlusHldPlusDecPlusRel = attPlusHldPlusDec + releaseTime;
}
