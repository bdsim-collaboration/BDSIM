#include "BDSFieldEInterpolated4D.hh"
#include "BDSInterpolator4D.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

BDSFieldEInterpolated4D::BDSFieldEInterpolated4D(BDSInterpolator4D* interpolatorIn,
						 G4Transform3D      offset):
  BDSFieldE(offset),
  interpolator(interpolatorIn)
{;}

BDSFieldEInterpolated4D::~BDSFieldEInterpolated4D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldEInterpolated4D::GetField(const G4ThreeVector& position,
						const G4double       t) const
{
  return interpolator->GetInterpolatedValue(position[0], position[1], position[2], t);
}
