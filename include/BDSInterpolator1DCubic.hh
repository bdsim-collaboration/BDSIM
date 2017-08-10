#ifndef BDSINTERPOLATE1DCUBIC_H
#define BDSINTERPOLATE1DCUBIC_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator1D.hh"

#include "G4Types.hh"

class BDSArray1DCoords;

/** 
 * @brief Cubic interpolation over 1d array.
 * 
 * Does not own array - so multiple interpolators could be used on same data.
 * 
 * @author Laurie Nevay
 */

class BDSInterpolator1DCubic: public BDSInterpolator1D
{
public:
  explicit BDSInterpolator1DCubic(BDSArray1DCoords* arrayIn);
  virtual ~BDSInterpolator1DCubic();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x) const;

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator1DCubic() = delete;
};

#endif
