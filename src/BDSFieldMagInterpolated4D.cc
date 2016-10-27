#include "BDSFieldMagInterpolated4D.hh"
#include "BDSInterpolator4D.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

BDSFieldMagInterpolated4D::BDSFieldMagInterpolated4D(BDSInterpolator4D* interpolatorIn,
						     G4Transform3D      offset):
  BDSFieldMag(offset),
  interpolator(interpolatorIn)
{;}

BDSFieldMagInterpolated4D::~BDSFieldMagInterpolated4D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldMagInterpolated4D::GetField(const G4ThreeVector& position,
						  const G4double       t) const
{
  return interpolator->GetInterpolatedValue(position[0], position[1], position[2], t);
}
