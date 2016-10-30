#include "BDSFieldEMInterpolated3D.hh"
#include "BDSInterpolator3D.hh"

#include "G4ThreeVector.hh"

#include <utility>

BDSFieldEMInterpolated3D::BDSFieldEMInterpolated3D(BDSInterpolator3D* eInterpolatorIn,
						   BDSInterpolator3D* bInterpolatorIn,
						   G4Transform3D      offset):
  BDSFieldEM(offset),
  eInterpolator(eInterpolatorIn),
  bInterpolator(bInterpolatorIn)
{;}

BDSFieldEMInterpolated3D::~BDSFieldEMInterpolated3D()
{
  delete eInterpolator;
  delete bInterpolator;
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMInterpolated3D::GetField(const G4ThreeVector& position,
									  const G4double       /*t*/) const
{
  G4ThreeVector e = eInterpolator->GetInterpolatedValue(position[0], position[1], position[2]);
  G4ThreeVector b = bInterpolator->GetInterpolatedValue(position[0], position[1], position[2]);
  return std::make_pair(b,e);
}
