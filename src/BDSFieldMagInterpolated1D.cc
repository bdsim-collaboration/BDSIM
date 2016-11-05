#include "BDSFieldMagInterpolated1D.hh"
#include "BDSInterpolator1D.hh"

#include "G4ThreeVector.hh"

BDSFieldMagInterpolated1D::BDSFieldMagInterpolated1D(BDSInterpolator1D* interpolatorIn,
						     G4Transform3D      offset,
						     G4double           scalingIn):
  BDSFieldMag(offset),
  interpolator(interpolatorIn),
  scaling(scalingIn)
{;}

BDSFieldMagInterpolated1D::~BDSFieldMagInterpolated1D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldMagInterpolated1D::GetField(const G4ThreeVector& position,
						  const G4double       /*t*/) const
{return interpolator->GetInterpolatedValue(position[0]) * scaling;}
