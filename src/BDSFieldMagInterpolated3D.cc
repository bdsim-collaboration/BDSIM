#include "BDSFieldMagInterpolated3D.hh"
#include "BDSInterpolator3D.hh"

#include "G4ThreeVector.hh"

BDSFieldMagInterpolated3D::BDSFieldMagInterpolated3D(BDSInterpolator3D* interpolatorIn,
						     G4Transform3D      offset,
						     G4double           scalingIn):
  BDSFieldMag(offset),
  interpolator(interpolatorIn),
  scaling(scalingIn)
{;}

BDSFieldMagInterpolated3D::~BDSFieldMagInterpolated3D()
{
  delete interpolator;
}

G4ThreeVector BDSFieldMagInterpolated3D::GetField(const G4ThreeVector& position,
						  const G4double       /*t*/) const
{
	G4ThreeVector interpolatedValue = interpolator->GetInterpolatedValue(position[0], position[1], position[2]) * scaling;

    return interpolatedValue;

}
