#include "BDSFieldEInterpolated2D.hh"
#include "BDSInterpolator2D.hh"

#include "G4ThreeVector.hh"

BDSFieldEInterpolated2D::BDSFieldEInterpolated2D(BDSInterpolator2D* interpolatorIn,
						 G4Transform3D      offset,
						 G4double           eScalingIn):
  BDSFieldEInterpolated(offset, eScalingIn),
  interpolator(interpolatorIn)
{;}

BDSFieldEInterpolated2D::~BDSFieldEInterpolated2D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldEInterpolated2D::GetField(const G4ThreeVector& position,
						const G4double       /*t*/) const
{return interpolator->GetInterpolatedValue(position[0], position[1]) * EScaling();}
