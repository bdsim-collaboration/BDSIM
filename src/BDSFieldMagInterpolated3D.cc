#include "BDSFieldMagInterpolated3D.hh"
#include "BDSInterpolator3D.hh"

#include "G4ThreeVector.hh"

BDSFieldMagInterpolated3D::BDSFieldMagInterpolated3D(BDSInterpolator3D* interpolatorIn,
						     G4Transform3D      offset):
  BDSFieldMag(offset),
  interpolator(interpolatorIn)
{;}

BDSFieldMagInterpolated3D::~BDSFieldMagInterpolated3D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldMagInterpolated3D::GetField(const G4ThreeVector& position,
						  const G4double       /*t*/) const
{return interpolator->GetInterpolatedValue(position[0], position[1], position[2]);}
