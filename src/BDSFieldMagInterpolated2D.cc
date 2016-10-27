#include "BDSFieldMagInterpolated2D.hh"
#include "BDSInterpolator2D.hh"

#include "G4ThreeVector.hh"

BDSFieldMagInterpolated2D::BDSFieldMagInterpolated2D(BDSInterpolator2D* interpolatorIn,
						     G4Transform3D      offset):
  BDSFieldMag(offset),
  interpolator(interpolatorIn)
{;}

BDSFieldMagInterpolated2D::~BDSFieldMagInterpolated2D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldMagInterpolated2D::GetField(const G4ThreeVector& position,
						  const G4double       /*t*/) const
{return interpolator->GetInterpolatedValue(position[0], position[1]);}
