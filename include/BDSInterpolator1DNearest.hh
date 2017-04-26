#ifndef BDSINTERPOLATOR1DNEAREST_H
#define BDSINTERPOLATOR1DNEAREST_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator1D.hh"

#include "G4Types.hh"

class BDSArray1DCoords;

/**
 * @brief Interpolated field array that gives the nearest neighbour value.
 *
 * Does not own array - so multiple interpolators could be used on same data.
 *
 * @author Laurie Nevay
 */

class BDSInterpolator1DNearest: public BDSInterpolator1D
{
public:
  explicit BDSInterpolator1DNearest(BDSArray1DCoords* arrayIn);
  virtual ~BDSInterpolator1DNearest();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x) const;
  
private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator1DNearest() = delete;
};

#endif
