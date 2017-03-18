#ifndef BDSFIELDEINTERPOLATED_H
#define BDSFIELDEINTERPOLATED_H

#include "BDSFieldE.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

class BDSFieldEInterpolated: public BDSFieldE
{
public:
  BDSFieldEInterpolated(G4Transform3D offset);
  BDSFieldEInterpolated(G4Transform3D offset,
			G4double      eScalingIn);

  virtual ~BDSFieldEInterpolated(){;}

  /// Accessor.
  inline G4double EScaling() const {return eScaling;}

  /// Setter.
  inline void     SetEScaling(G4double eScalingIn) {eScaling = eScalingIn;}
  
private:
  G4double eScaling; ///< E field scaling value.
};

#endif
