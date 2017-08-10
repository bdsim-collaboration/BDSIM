#include "BDSFieldMagInterpolated.hh"

#include "globals.hh"

BDSFieldMagInterpolated::BDSFieldMagInterpolated(G4Transform3D offset,
						 G4double      scalingIn):
  BDSFieldMag(offset),
  scaling(scalingIn)
{;}
