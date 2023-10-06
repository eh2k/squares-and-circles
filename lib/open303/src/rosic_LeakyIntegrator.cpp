#include "rosic_LeakyIntegrator.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

LeakyIntegrator::LeakyIntegrator()
{
  sampleRate  = SAMPLE_RATE; 
  tau         = 10.0f;    
  y1          = 0.0;

  calculateCoefficient();
}

LeakyIntegrator::~LeakyIntegrator()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void LeakyIntegrator::setSampleRate(real_t newSampleRate)
{
  if( newSampleRate > 0.0 )
  {
    sampleRate = newSampleRate;
    calculateCoefficient();
  }
}

void LeakyIntegrator::setTimeConstant(real_t newTimeConstant)
{
  if( newTimeConstant >= 0.0 && newTimeConstant != tau )
  {
    tau = newTimeConstant; 
    calculateCoefficient();
  }
}

//-------------------------------------------------------------------------------------------------
// inquiry:

real_t LeakyIntegrator::getNormalizer(real_t tau1, real_t tau2, real_t fs)
{
  real_t td = 0.001*tau1;
  real_t ta = 0.001*tau2;

  // catch some special cases:
  if( ta == 0.0 && td == 0.0 )
    return 1.0;
  else if( ta == 0.0 )
  {
    return 1.0 / (1.0-exp(-1.0/(fs*td)));
  }
  else if( td == 0.0 )
  {
    return 1.0 / (1.0-exp(-1.0/(fs*ta)));
  }

  // compute the filter coefficients:
  real_t x  = exp( -1.0 / (fs*td)  );
  real_t bd = 1-x;
  real_t ad = -x;
  x         = exp( -1.0 / (fs*ta)  );
  real_t ba = 1-x;
  real_t aa = -x;

  // compute the location and height of the peak:
  real_t xp;
  if( ta == td )
  {
    real_t tp  = ta;
    real_t np  = fs*tp;
    xp         = (np+1.0)*ba*ba*pow(aa, np);
  }
  else
  {
    real_t tp  = log(ta/td) / ( (1.0/td) - (1.0/ta) );
    real_t np  = fs*tp;
    real_t s   = 1.0 / (aa-ad);
    real_t b01 = s * aa*ba*bd;
    real_t b02 = s * ad*ba*bd;
    real_t a01 = s * (ad-aa)*aa;
    real_t a02 = s * (ad-aa)*ad;
    xp         = b01*pow(a01, np) - b02*pow(a02, np);
  }

  // return the normalizer as reciprocal of the peak height:
  return 1.0/xp;
}

//-------------------------------------------------------------------------------------------------
// others:

void LeakyIntegrator::reset()
{
  y1 = 0;
}

void LeakyIntegrator::calculateCoefficient()
{
  if( tau > 0.0 )
    coeff = exp( -1.0 / (sampleRate*0.001*tau)  );
  else
    coeff = 0.0;
}

