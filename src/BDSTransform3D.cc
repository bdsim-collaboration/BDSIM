/* BDSIM code.    Version 1.0

*/
#include "BDSGlobalConstants.hh" 

#include "BDSTransform3D.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"

//============================================================

BDSTransform3D::BDSTransform3D(G4String aName, G4double x,G4double y,G4double z,
			       G4double phi,G4double theta, G4double psi):
  BDSAcceleratorComponent(
			  aName, 
			  0,
			  0,
			  0,
			  0,
			  NULL,
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
  SetType("transform3d");
}

void BDSTransform3D::BuildMarkerLogicalVolume()
{
  // nothing to do here
}


G4VisAttributes* BDSTransform3D::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0,1,0));
  return itsVisAttributes;
}


BDSTransform3D::~BDSTransform3D()
{
}
