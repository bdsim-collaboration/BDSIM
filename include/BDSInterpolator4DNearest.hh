#ifndef BDSINTERPOLATOR4DNEAREST_H
#define BDSINTERPOLATOR4DNEAREST_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator4D.hh"

#include "G4Types.hh"

class BDSArray4DCoords;

/**
 * @brief Interpolated field array that gives the nearest neighbour value.
 *
 * Does not own array - so multiple interpolators could be used on same data.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator4DNearest: public BDSInterpolator4D
{
public:
  BDSInterpolator4DNearest(BDSArray4DCoords* arrayIn);
  virtual ~BDSInterpolator4DNearest();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x,
					      G4double y,
					      G4double z,
					      G4double t) const;

private:
  /// Private default constructor to force the use of the provided one.
  BDSInterpolator4DNearest() = delete;
};

#endif
