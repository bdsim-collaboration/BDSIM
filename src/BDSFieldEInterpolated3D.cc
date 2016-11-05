#include "BDSFieldEInterpolated3D.hh"
#include "BDSInterpolator3D.hh"

#include "G4ThreeVector.hh"

BDSFieldEInterpolated3D::BDSFieldEInterpolated3D(BDSInterpolator3D* interpolatorIn,
						 G4Transform3D      offset,
						 G4double           scalingIn):
  BDSFieldE(offset),
  interpolator(interpolatorIn),
  scaling(scalingIn)
{;}

BDSFieldEInterpolated3D::~BDSFieldEInterpolated3D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldEInterpolated3D::GetField(const G4ThreeVector& position,
						const G4double       /*t*/) const
{return interpolator->GetInterpolatedValue(position[0], position[1], position[2]) * scaling;}
