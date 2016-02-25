#include "BDSSamplerInfo.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

class BDSBeamlineElement;
class BDSSampler;

BDSSamplerInfo::BDSSamplerInfo(G4String            nameIn,
			       BDSSampler*         samplerIn,
			       G4Transform3D       transformIn,
			       G4double            sPositionIn,
			       BDSBeamlineElement* elementIn):
  name(nameIn),
  sampler(samplerIn),
  transform(transformIn),
  sPosition(sPositionIn),
  element(elementIn)
{
  transformInverse = G4Transform3D(transform.inverse());
}
