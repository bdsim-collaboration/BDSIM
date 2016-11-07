#include "BDSFieldEInterpolated1D.hh"
#include "BDSInterpolator1D.hh"

#include "G4ThreeVector.hh"

BDSFieldEInterpolated1D::BDSFieldEInterpolated1D(BDSInterpolator1D* interpolatorIn,
						 G4Transform3D      offset,
						 G4double           scalingIn):
  BDSFieldE(offset),
  interpolator(interpolatorIn),
  scaling(scalingIn)
{;}

BDSFieldEInterpolated1D::~BDSFieldEInterpolated1D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldEInterpolated1D::GetField(const G4ThreeVector& position,
						const G4double       /*t*/) const
{return interpolator->GetInterpolatedValue(position[0]) * scaling;}
