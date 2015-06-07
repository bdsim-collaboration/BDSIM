#include "BDSTransform3D.hh"

BDSTransform3D::BDSTransform3D(G4String aName,
			       G4double x,
			       G4double y,
			       G4double z,
			       G4double phi,
			       G4double theta,
			       G4double psi):
  BDSAcceleratorComponent(name,0,0,"transform3d")
{
  /*
  SetTheta(theta);
  SetPsi(psi);
  SetPhi(phi);
  */

  // well this is broken for now :(
}

BDSTransform3D::~BDSTransform3D()
{
}
