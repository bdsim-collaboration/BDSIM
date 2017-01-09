#include "BDSGlobalConstants.hh" 
#include "BDSSamplerPlane.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDManager.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Box.hh"
#include "G4LogicalVolume.hh"

BDSSamplerPlane::BDSSamplerPlane(G4String name,
				 G4double boxHalfWidth):
  BDSSampler(name)
{
  containerSolid = new G4Box(name + "_solid",
			     boxHalfWidth,
			     boxHalfWidth,
			     chordLength*0.5);

  SetExtent(BDSExtent(boxHalfWidth, boxHalfWidth, chordLength*0.5));

  CommonConstruction();

  containerLogicalVolume->SetSensitiveDetector(BDSSDManager::Instance()->GetSamplerPlaneSD());
}
