#ifndef BDSINTERPOLATE4DCUBIC_H
#define BDSINTERPOLATE4DCUBIC_H

#include "BDSFieldValue.hh"
#include "BDSInterpolator4D.hh"

#include "G4Types.hh"

class BDSArray4DCoords;

/** 
 * @brief Cubic interpolation over 4d array.
 * 
 * Does not own array - so multiple interpolators could be used on same data.
 * 
 * @author Laurie Nevay
 */

class BDSInterpolator4DCubic: public BDSInterpolator4D
{
public:
  explicit BDSInterpolator4DCubic(BDSArray4DCoords* arrayIn);
  virtual ~BDSInterpolator4DCubic();

protected:
  virtual BDSFieldValue GetInterpolatedValueT(G4double x,
					      G4double y,
					      G4double z,
					      G4double t) const;

private:
  /// Private default constructor to force use of provided one.
  BDSInterpolator4DCubic() = delete;
};

#endif
