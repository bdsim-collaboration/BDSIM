/* BDSIM code.    Version 1.0

*/

#include "BDSTransform3D.hh"

//============================================================

BDSTransform3D::BDSTransform3D(G4String aName, G4double x,G4double y,G4double z,
			       G4double phi,G4double theta, G4double psi):
  BDSAcceleratorComponent(
			  aName, 
			  0,
			  0,
			  0,
			  0,
			  "",
                          "",
			  0, // angle
			  x, // x offset
			  y, // y offset
			  z) // z offset
{
  SetTheta(theta);
  SetPsi(psi);
  SetPhi(phi);
}

void BDSTransform3D::BuildMarkerLogicalVolume()
{
  // nothing to do here
}


BDSTransform3D::~BDSTransform3D()
{
}
