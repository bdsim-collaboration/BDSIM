#ifndef BDSINTERPOLATOR2DNEAREST_H
#define BDSINTERPOLATOR2DNEAREST_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator2D.hh"

#include "G4Types.hh"

class BDSArray2DCoords;

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
  BDSInterpolator2DNearest(BDSArray2DCoords* arrayIn);
  virtual ~BDSInterpolator2DNearest();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x, G4double y) const;
  
private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator2DNearest() = delete;
};

#endif
