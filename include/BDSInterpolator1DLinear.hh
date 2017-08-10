#ifndef BDSINTERPOLATE1DLINEAR_H
#define BDSINTERPOLATE1DLINEAR_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator1D.hh"

#include "G4Types.hh"

class BDSArray1DCoords;

/** 
 * @brief Linear interpolation over 1d array.
 * 
 * Does not own array - so multiple interpolators could be used on same data.
 * 
 * @author Laurie Nevay
 */

class BDSInterpolator1DLinear: public BDSInterpolator1D
{
public:
  explicit BDSInterpolator1DLinear(BDSArray1DCoords* arrayIn);
  virtual ~BDSInterpolator1DLinear();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x) const;

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator1DLinear() = delete;
};

#endif
