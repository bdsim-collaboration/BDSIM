#include "BDSFieldEMInterpolated2D.hh"
#include "BDSInterpolator2D.hh"

#include "G4ThreeVector.hh"

#include <utility>

BDSFieldEMInterpolated2D::BDSFieldEMInterpolated2D(BDSInterpolator2D* eInterpolatorIn,
						   BDSInterpolator2D* bInterpolatorIn,
						   G4Transform3D      offset,
						   G4double           scalingIn):
  BDSFieldEM(offset),
  eInterpolator(eInterpolatorIn),
  bInterpolator(bInterpolatorIn),
  scaling(scalingIn)
{;}

BDSFieldEMInterpolated2D::~BDSFieldEMInterpolated2D()
{
  delete eInterpolator;
  delete bInterpolator;
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMInterpolated2D::GetField(const G4ThreeVector& position,
									  const G4double       /*t*/) const
{
  G4ThreeVector e = eInterpolator->GetInterpolatedValue(position[0], position[1]) * scaling;
  G4ThreeVector b = bInterpolator->GetInterpolatedValue(position[0], position[1]) * scaling;
  return std::make_pair(b,e);
}
