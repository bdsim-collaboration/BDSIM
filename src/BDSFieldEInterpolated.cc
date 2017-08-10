#include "BDSFieldEInterpolated.hh"

#include "globals.hh"

BDSFieldEInterpolated::BDSFieldEInterpolated(G4Transform3D offset,
					     G4double      eScalingIn):
  BDSFieldE(offset),
  eScaling(eScalingIn)
{;}
