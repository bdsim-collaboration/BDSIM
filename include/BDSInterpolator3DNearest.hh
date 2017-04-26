#ifndef BDSINTERPOLATOR3DNEAREST_H
#define BDSINTERPOLATOR3DNEAREST_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator3D.hh"

#include "G4Types.hh"

class BDSArray3DCoords;

/**
 * @brief Interpolated field array that gives the nearest neighbour value.
 *
 * Does not own array - so multiple interpolators could be used on same data.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator3DNearest: public BDSInterpolator3D
{
public:
  explicit BDSInterpolator3DNearest(BDSArray3DCoords* arrayIn);
  virtual ~BDSInterpolator3DNearest();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x, G4double y, G4double z) const;

private:
  /// Private default constructor to force the use of the provided one.
  BDSInterpolator3DNearest() = delete;
};

#endif
