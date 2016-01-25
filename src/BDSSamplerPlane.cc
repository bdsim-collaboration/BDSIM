#include "BDSGlobalConstants.hh" 
#include "BDSSamplerPlane.hh"
#include "BDSSDManager.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"

const G4double BDSSamplerPlane::chordLength = 1e-9; // 1pm (geant4 units are mm)

BDSSamplerPlane::BDSSamplerPlane(G4String      name,
				 G4Transform3D transform,
				 G4double      boxHalfWidth):
  BDSSampler(name, transform)
{
  containerSolid = new G4Box(name + "_solid",
			     boxHalfWidth,
			     boxHalfWidth,
			     chordLength*0.5);

  SetExtentX(-boxHalfWidth, boxHalfWidth);
  SetExtentY(-boxHalfWidth, boxHalfWidth);
  SetExtentZ(-chordLength*0.5, chordLength*0.5);

  CommonConstruction();
}

void BDSSamplerPlane::SetSensitiveDetector()
{
  containerLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetSamplerPlaneSD());
}
