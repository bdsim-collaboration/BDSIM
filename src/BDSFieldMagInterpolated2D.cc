#include "BDSFieldMagInterpolated2D.hh"


BDSFieldMagInterpolated2D::BDSFieldMagInterpolated2D(BDSInterpolator2D* interpolatorIn,
						     G4Transform3D      offset):
  BDSFieldMag(offset),
  interpolator(interpolatorIn)
{;}

BDSFieldMagInterpolated2D::~BDSFieldMagInterpolated2D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldMagInterpolated2D::GetField(const G4ThreeVector& position) const
{return interpolator->GetInterpolatedValue(position[0], position[1]);}
