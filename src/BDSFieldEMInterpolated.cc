#include "BDSFieldEMInterpolated.hh"

#include "globals.hh"

BDSFieldEMInterpolated::BDSFieldEMInterpolated(G4Transform3D offset):
  BDSFieldEM(offset),
  eScaling(1.0),
  bScaling(1.0)
{;}

BDSFieldEMInterpolated::BDSFieldEMInterpolated(G4Transform3D offset,
					       G4double      eScalingIn,
					       G4double      bScalingIn):
  BDSFieldEM(offset),
  eScaling(eScalingIn),
  bScaling(bScalingIn)
{;}
