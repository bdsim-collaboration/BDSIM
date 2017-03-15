#include "BDSFieldEInterpolated.hh"

#include "globals.hh"

BDSFieldEInterpolated::BDSFieldEInterpolated(G4Transform3D offset):
  BDSFieldE(offset),
  eScaling(1.0)
{;}

BDSFieldEInterpolated::BDSFieldEInterpolated(G4Transform3D offset,
					     G4double      eScalingIn):
  BDSFieldE(offset),
  eScaling(eScalingIn)
{;}
