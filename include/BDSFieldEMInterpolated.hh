#ifndef BDSFIELDEMINTERPOLATED_H
#define BDSFIELDEMINTERPOLATED_H

#include "BDSFieldEM.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

/**
 * @brief Class to provide scaling and a base class pointer for interpolator fields.
 *
 * @author Laurie Nevay
 */

class BDSFieldEMInterpolated: public BDSFieldEM
{
public:
  BDSFieldEMInterpolated(G4Transform3D offset);
  BDSFieldEMInterpolated(G4Transform3D offset,
			 G4double      eScalingIn,
			 G4double      bScalingIn);

  virtual ~BDSFieldEMInterpolated(){;}

  /// @{ Accessor.
  inline G4double EScaling() const {return eScaling;}
  inline G4double BScaling() const {return bScaling;}
  /// @}

  /// @{ Setter.
  inline void     SetEScaling(G4double eScalingIn) {eScaling = eScalingIn;}
  inline void     SetBScaling(G4double bScalingIn) {bScaling = bScalingIn;}
  /// @}
  
private:
  G4double eScaling; ///< E field scaling value.
  G4double bScaling; ///< B field scaling value.
};

#endif
