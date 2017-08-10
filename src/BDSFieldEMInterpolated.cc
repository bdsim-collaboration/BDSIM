#include "BDSFieldEMInterpolated.hh"

#include "globals.hh"

BDSFieldEMInterpolated::BDSFieldEMInterpolated(G4Transform3D offset,
					       G4double      eScalingIn,
					       G4double      bScalingIn):
  BDSFieldEM(offset),
  eScaling(eScalingIn),
  bScaling(bScalingIn)
{;}
