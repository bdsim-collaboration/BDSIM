#include "BDSGlobalConstants.hh"
#include "BDSSampler.hh"

#include "globals.hh" // geant4 types / globals
#include "G4LogicalVolume.hh"

BDSSampler::BDSSampler(G4String nameIn):
  BDSGeometryComponent(nullptr, nullptr),
  name(nameIn)
{;}

void BDSSampler::CommonConstruction()
{
  // Construct logical volume from solid. Note it's ok to have nullptr for material
  // as the sampler is only placed in a parallel world where the material is ignored.
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       nullptr,
					       GetName() + "_lv");
  
  containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
}
