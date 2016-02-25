#include "BDSGlobalConstants.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSSDManager.hh"

#include "globals.hh" // geant types / globals
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"

BDSSamplerCylinder::BDSSamplerCylinder(G4String      name,
				       G4double      length,
				       G4double      radius):
  BDSSampler(name)
{
  containerSolid = new G4Tubs(name + "_solid",      // name
			      radius - 1*CLHEP::um, // inner radius
			      radius,               // outer radius
			      length*0.5,           // half length
			      0,                    // start angle
			      CLHEP::twopi);        // sweep angle

  SetExtentX(-radius, radius);
  SetExtentY(-radius, radius);
  SetExtentZ(-length*0.5, length*0.5);

  CommonConstruction();

  containerLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetSamplerCylinderSD());
}
