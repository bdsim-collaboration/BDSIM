#include "BDSGlobalConstants.hh" 
#include "BDSSamplerPlane.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDManager.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Box.hh"

class G4Transform3D;

static const G4double BDSSamplerPlane::chordLength = 1e-9; // 1pm (geant4 units are mm)

BDSSamplerPlane(G4String       name,
		G4Transform3D* transform,
		G4double       boxHalfWidth):
  BDSSampler(name, transform)
{
  containerSolid = containerSolidSampler = new G4Box(name + "_solid",
						     boxHalfWidth,
						     boxHalfWidth,
						     chordLength*0.5);

  SetExtentX(-boxHalfWidth, boxHalfWidth);
  SetExtentY(-boxHalfWidth, boxHalfWidth);
  SexExtentZ(-chordLength*0.5, chordLength*0.5);

  CommonConstruction();
}

BDSSamplerSD* BDSSampler::GetSensitiveDetector() const
{
  return BDSSDManager::Instance()->GetSamplerPlaneSD();
}
