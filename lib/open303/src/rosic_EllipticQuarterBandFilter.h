#ifndef rosic_EllipticQuarterBandFilter_h
#define rosic_EllipticQuarterBandFilter_h

#include <string.h> // for memmove

// rosic-indcludes:
#include "GlobalDefinitions.h"

namespace rosic
{

  /**

  This is an elliptic subband filter of 12th order using a Direct Form II implementation structure.

  */

  class EllipticQuarterBandFilter
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    EllipticQuarterBandFilter();   

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    /** Resets the filter state. */
    void reset();

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates a single filtered output-sample. */
    INLINE real_t getSample(real_t in);

    //=============================================================================================

  protected:

    // state buffer:
    real_t w[12];

  };

  //-----------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE real_t EllipticQuarterBandFilter::getSample(real_t in)
  {
    return in; //[eh2k] 
    
    const real_t a01 =   -9.1891604652189471;
    const real_t a02 =   40.177553696870497;
    const real_t a03 = -110.11636661771178;
    const real_t a04 =  210.18506612078195;
    const real_t a05 = -293.84744771903240;
    const real_t a06 =  308.16345558359234;
    const real_t a07 = -244.06786780384243;
    const real_t a08 =  144.81877911392738;
    const real_t a09 =  -62.770692151724198;
    const real_t a10 =   18.867762095902137;
    const real_t a11 =   -3.5327094230551848;
    const real_t a12 =    0.31183189275203149;

    const real_t b00 =    0.00013671732099945628;
    const real_t b01 =   -0.00055538501265606384;
    const real_t b02 =    0.0013681887636296387;
    const real_t b03 =   -0.0022158566490711852;
    const real_t b04 =    0.0028320091007278322;
    const real_t b05 =   -0.0029776933151090413;
    const real_t b06 =    0.0030283628243514991;    
    const real_t b07 =   -0.0029776933151090413;
    const real_t b08 =    0.0028320091007278331;
    const real_t b09 =   -0.0022158566490711861;
    const real_t b10 =    0.0013681887636296393;    
    const real_t b11 =   -0.00055538501265606384;
    const real_t b12 =    0.00013671732099945636;

    // calculate intermediate and output sample via direct form II - the parentheses facilitate 
    // out-of-order execution of the independent additions (for performance optimization):
    real_t tmp =   (in + TINY)
                 - ( (a01*w[0] + a02*w[1] ) + (a03*w[2]  + a04*w[3]   ) ) 
                 - ( (a05*w[4] + a06*w[5] ) + (a07*w[6]  + a08*w[7]   ) )
                 - ( (a09*w[8] + a10*w[9] ) + (a11*w[10] +  a12*w[11] ) );
   
    real_t y =     b00*tmp 
                 + ( (b01*w[0] + b02*w[1])  +  (b03*w[2]  + b04*w[3]  ) )  
                 + ( (b05*w[4] + b06*w[5])  +  (b07*w[6]  + b08*w[7]  ) )
                 + ( (b09*w[8] + b10*w[9])  +  (b11*w[10] + b12*w[11] ) );

    // update state variables:
    memmove(&w[1], &w[0], 11*sizeof(real_t));
    w[0] = tmp;

    return y;
  }

} // end namespace rosic

#endif // rosic_EllipticQuarterBandFilter_h
