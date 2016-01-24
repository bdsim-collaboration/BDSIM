#include "BDSGlobalConstants.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDManager.hh"

#include "globals.hh" // geant types / globals
#include "G4Tubs.hh"

class G4Transform3D;

BDSSamplerCylinder::BDSSamplerCylinder(G4String       name,
				       G4Transform3D* transform,
				       G4double       length,
				       G4double       radius):
  BDSSampler(name, transform)
{
  containerSolid = new G4Tubs(name + "_solid",      // name
			      radius - 1*CLHEP::um, // inner radius
			      radius,               // outer radius
			      length*0.5,           // half length
			      0,                    // start angle
			      CLHEP::twopi);        // sweep angle

  SetExtentX(-radius, radius);
  SetExtentY(-radius, radius);
  SexExtentZ(-length*0.5, length*0.5);

  CommonConstruction();
}

BDSSamplerSD* BDSSamplerCylinder::GetSensitiveDetector()const
{
  return BDSSDManager::Instance()->GetSamplerCylinderSD();
}
