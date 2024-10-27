#include "rosic_MipMappedWaveTable.h"
#include <algorithm>
using namespace rosic;

MipMappedWaveTable::MipMappedWaveTable()
{
  // init member variables:
  sampleRate = SAMPLE_RATE;
  waveform   = 0;
  symmetry   = 0.5;

  // initialize internal 'back-panel' parameters
  tanhShaperFactor = dB2amp(36.9);
  tanhShaperOffset = 4.37;
  squarePhaseShift = 180.0;

  // set up the fourier-transformer:
  fourierTransformer.setBlockSize(tableLength);

  // initialize the buffers:
  initPrototypeTable();
}

MipMappedWaveTable::~MipMappedWaveTable()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void MipMappedWaveTable::setWaveform(real_t* newWaveForm, int lengthInSamples)
{
  int i;
  if( lengthInSamples == tableLength )
  {
    // just copy the values into the internal buffer, when the length of the passed table and the
    // internal table match:
    for( i=0; i<tableLength; i++ )
      prototypeTable[i] = newWaveForm[i];
  }
  else
  {
    // implement periodic sinc-interpolation here...
  }
}

void MipMappedWaveTable::setWaveform(int newWaveform)
{
  if( (newWaveform >= 0) && (newWaveform != waveform) )
  {
    waveform = newWaveform;
    renderWaveform();
  }
}

void MipMappedWaveTable::setSymmetry(real_t newSymmetry)
{
  symmetry = newSymmetry;
  renderWaveform();
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void MipMappedWaveTable::initPrototypeTable()
{
  for(int i=0; i<(tableLength/* [eh2k] +4 */); i++)
    prototypeTable[i] = 0.0;
}
void MipMappedWaveTable::removeDC()
{
  // calculate DC-offset (= average value of the table):
  real_t dcOffset = 0.0;
  int i;
  for(i=0; i<tableLength; i++)
    dcOffset += prototypeTable[i];
  dcOffset = dcOffset / tableLength;

  // remove DC-Offset:
  for(i=0; i<tableLength; i++)
    prototypeTable[i] -= dcOffset;
}

void MipMappedWaveTable::normalize()
{
  // find maximum:
  real_t max = 0.0;
  int    i;
  for(i=0; i<tableLength; i++)
    if( fabs(prototypeTable[i]) > max)
      max = fabs(prototypeTable[i]);

  // normalize to amplitude 1.0:
  real_t scale = 1.0/max;
  for(i=0; i<tableLength; i++)
    prototypeTable[i] *= scale;
}

void MipMappedWaveTable::reverseTime()
{
  int    i;
  real_t tmpTable[tableLength+4];

  for(i=0; i<tableLength; i++)
    tmpTable[i] = prototypeTable[tableLength-i-1];

  for(i=0; i<tableLength; i++)
    prototypeTable[i] = tmpTable[i];
}

void MipMappedWaveTable::renderWaveform()
{
  switch( waveform )
  {
  case   SINE:      fillWithSine();        break;
  case   TRIANGLE:  fillWithTriangle();    break;
  case   SQUARE:    fillWithSquare();      break;
  case   SAW:       fillWithSaw();         break;
  case   SQUARE303: fillWithSquare303();   break;
  case   SAW303:    fillWithSaw303();      break;

  default :  fillWithSine();
  }
}

void MipMappedWaveTable::generateMipMap(WaveTable& tableSet)
{
  real_t spectrum[tableLength];
  //static int    position, offset;
  int t, i; // indices for the table and position

  //position = 0;             // begin of the 1st table (index 0)
  //offset   = tableLength+4; // offset between tow tables, the 4 is the number
  // of additional samples used for interpolation

  // copy the prototypeTable into the 1st table of the mipmap (this actually makes the
  // prototypeTable redundant - room for optimization here):
  t = 0;
  for(i=0; i<tableLength; i++)
    tableSet[0][i] = prototypeTable[i];

  // additional sample(s) for the interpolator:
  tableSet[t][tableLength]   = tableSet[t][0];
  tableSet[t][tableLength+1] = tableSet[t][1];
  tableSet[t][tableLength+2] = tableSet[t][2];
  tableSet[t][tableLength+3] = tableSet[t][3];

  // get the spectrum from the prototype-table:
  fourierTransformer.transformRealSignal(prototypeTable, spectrum);

  // ensure that DC and Nyquist are zero:
  spectrum[0] = 0.0;
  spectrum[1] = 0.0;

  // now, render the bandlimited versions by successively shrinking the
  // spectrum by one octave and iFFT'ing this spectrum:
  int lowBin, highBin;
  for(t=1; t<numTables; t++)
  {
    lowBin  = (int) (tableLength / pow(2.0, t));   // the cutoff-bin
    highBin = (int) (tableLength / pow(2.0, t-1)); // the bin up to which the
    // spectrum is currently still nonzero

    // zero out the bins above the cutoff-bin:
    for(i=lowBin; i<highBin; i++)
      spectrum[i] = 0.0;

    // transform the truncated spectrum back to the time-domain and store it in
    // the tableSet
    fourierTransformer.transformSymmetricSpectrum(spectrum, tableSet[t]);

    // additional sample(s) for the interpolator:
    tableSet[t][tableLength]   = tableSet[t][0];
    tableSet[t][tableLength+1] = tableSet[t][1];
    tableSet[t][tableLength+2] = tableSet[t][2];
    tableSet[t][tableLength+3] = tableSet[t][3];
  }
}

//-------------------------------------------------------------------------------------------------
// fill the prototype-table with various standard waveforms:

void MipMappedWaveTable::fillWithSine()
{
  for (long i=0; i<tableLength; i++)
    prototypeTable[i] = sin( (2.0*PI*i) / (real_t) (tableLength) );
}

void MipMappedWaveTable::fillWithTriangle()
{
  int i;
  for (i=0; i<(tableLength/4); i++)
    prototypeTable[i] = (real_t)(4*i) / (real_t)(tableLength);

  for (i=(tableLength/4); i<(3*tableLength/4); i++)
    prototypeTable[i] = 2.0 - ((real_t)(4*i) / (real_t)(tableLength));

  for (i=(3*tableLength/4); i<(tableLength); i++)
    prototypeTable[i] = -4.0+ ((real_t)(4*i) / (real_t)(tableLength));
}

void MipMappedWaveTable::fillWithSquare()
{
  int    N  = tableLength;
  real_t k  = symmetry;
  int    N1 = clip(roundToInt(k*(N-1)), 1, N-1);
  for(int n=0; n<N1; n++)
    prototypeTable[n] = +1.0;
  for(int n=N1; n<N; n++)
    prototypeTable[n] = -1.0;
}

void MipMappedWaveTable::fillWithSaw()
{
  int    N  = tableLength;
  real_t k  = symmetry;
  int    N1 = clip(roundToInt(k*(N-1)), 1, N-1);
  int    N2 = N-N1;
  real_t s1 = 1.0 / (N1-1);
  real_t s2 = 1.0 / N2;
  for(int n=0; n<N1; n++)
    prototypeTable[n] = s1*n;
  for(int n=N1; n<N; n++)
    prototypeTable[n] = -1.0 + s2*(n-N1);
}

void MipMappedWaveTable::fillWithSquare303()
{
  // generate the saw-wave:
  int    N  = tableLength;
  real_t k  = 0.5;
  int    N1 = clip(roundToInt(k*(N-1)), 1, N-1);
  int    N2 = N-N1;
  real_t s1 = 1.0 / (N1-1);
  real_t s2 = 1.0 / N2;
  for(int n=0; n<N1; n++)
    prototypeTable[n] = s1*n;
  for(int n=N1; n<N; n++)
    prototypeTable[n] = -1.0 + s2*(n-N1);

  // switch polarity and apply tanh-shaping with dc-offset:
  for(int n=0; n<N; n++)
    prototypeTable[n] = -tanh(tanhShaperFactor*prototypeTable[n] + tanhShaperOffset);

  // do a circular shift to phase-align with the saw-wave, when both waveforms are mixed:
  int nShift = roundToInt(N*squarePhaseShift/360.0);
  
  std::rotate(&prototypeTable[0], &prototypeTable[0] + nShift, &prototypeTable[N]); //[eh2k] circularShift(prototypeTable, N, nShift);
}

void MipMappedWaveTable::fillWithSaw303()
{
  // generate the saw-wave:
  int    N  = tableLength;
  real_t k  = 0.5;
  int    N1 = clip(roundToInt(k*(N-1)), 1, N-1);
  int    N2 = N-N1;
  real_t s1 = 1.0 / (N1-1);
  real_t s2 = 1.0 / N2;
  for(int n=0; n<N1; n++)
    prototypeTable[n] = s1*n;
  for(int n=N1; n<N; n++)
    prototypeTable[n] = -1.0 + s2*(n-N1);

  // switch polarity:
  //for(int n=0; n<N; n++)
  //  prototypeTable[n] = -prototypeTable[n];
}

void MipMappedWaveTable::fillWithPeak()
{
  int i;
  for (i=0; i<(tableLength/2); i++)
    prototypeTable[i] = 1 - (real_t)(2*i) / (real_t)(tableLength);

  for (i=(tableLength/2); i<(tableLength); i++)
    prototypeTable[i] = 0.0;

  removeDC();
  normalize();
}

void MipMappedWaveTable::fillWithMoogSaw()
{
  // the sawUp part:
  int i;
  for (i=0; i<(tableLength/2); i++)
    prototypeTable[i] = (real_t)(2*i) / (real_t)(tableLength);

  for (i=(tableLength/2); i<(tableLength); i++)
    prototypeTable[i] = (real_t)(2*i) / (real_t)(tableLength) - 2.0;

  // the triangle part:
  for (i=0; i<(tableLength/2); i++)
    prototypeTable[i] += 1 - (real_t)(4*i) / (real_t)(tableLength);

  for (i=(tableLength/2); i<tableLength; i++)
    prototypeTable[i] += -1 + (real_t)(4*i) / (real_t)(tableLength);

  removeDC();
  normalize();
}













