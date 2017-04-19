#ifndef BDSFIELDEINTERPOLATED_H
#define BDSFIELDEINTERPOLATED_H

#include "BDSFieldE.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

/**
 * @brief Class to provide scaling and a base class pointer for interpolator fields.
 *
 * @author Laurie Nevay
 */

class BDSFieldEInterpolated: public BDSFieldE
{
public:
  explicit BDSFieldEInterpolated(G4Transform3D offset,
				 G4double      eScalingIn = 1.0);

  virtual ~BDSFieldEInterpolated(){;}

  /// Accessor.
  inline G4double EScaling() const {return eScaling;}

  /// Setter.
  inline void     SetEScaling(G4double eScalingIn) {eScaling = eScalingIn;}
  
private:
  G4double eScaling; ///< E field scaling value.
};

#endif
