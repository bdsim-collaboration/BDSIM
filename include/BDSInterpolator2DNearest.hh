#ifndef BDSINTERPOLATOR2DNEAREST_H
#define BDSINTERPOLATOR2DNEAREST_H

#include "BDSFieldData.hh"
#include "BDSInterpolator2D.hh"

#include "globals.hh" // geant4 types / globals

/**
 * @brief Interpolated field array that gives the nearest neighbour value.
 *
 * Does not own array - so multiple interpolators could be used on same data.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator2DNearest: public BDSInterpolator2D
{
public:
  BDSInterpolator2DNearest(BDSArray2DCoords3VF* arrayIn);
  virtual ~BDSInterpolator2DNearest();

protected:
  virtual BDSThreeVectorF GetInterpolatedValueT(G4double x, G4double y) const;
  
private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator2DNearest() = delete;

  /// The field data.
  BDSArray2DCoords3VF* array;
};

#endif
