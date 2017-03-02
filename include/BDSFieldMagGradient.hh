#ifndef BDSFIELDMAGGRADIENT_H
#define BDSFIELDMAGGRADIENT_H


#include "globals.hh"

class BDSFieldMagInterpolated2D;
class BDSMagnetStrength;

/**
 *Find 
 *
 *
 *
 *
 * @author Josh Albrecht
 */
class BDSFieldMagGradient
{
public:

  BDSFieldMagGradient();

  //Find the Magnet strengths of a multipole field to nth order.
  BDSMagnetStrength* CalculateMultipoles(BDSFieldMagInterpolated2D* BField, G4int order);

};

#endif
