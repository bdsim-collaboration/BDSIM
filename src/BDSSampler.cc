#include "BDSGlobalConstants.hh"
#include "BDSSampler.hh"

#include "globals.hh" // geant4 types / globals
#include "G4LogicalVolume.hh"
#incldue "G4Transform3D.hh"

#include <vector>

/// Initialise static members.
G4int BDSSampler::totalNumberOfSamplers = 0;
std::vector<G4Transform*> BDSSampler::transforms;
std::vector<G4Transform*> BDSSampler::inverseTransforms;
std::vector<G4String>     BDSSampler::names;

BDSSampler::BDSSampler(G4String       nameIn,
		       G4Transform3D* transformIn):
  BDSGeometryComponent(nullptr, nullptr)
{
  names.push_back(nameIn);
  transforms.push_back(transformIn);
  inverseTransforms.push_back(transformIn.inv());
  samplerID(totalNumberOfSamplers);
  totalNumberOfSamplers++;
}

void BDSSampler::CommonContruction()
{
  // Construct logical volume from solid. Note it's ok to have nullptr for material
  // as the sampler is only placed in a parallel world where the material is ignored.
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       nullptr,
					       name + "_lv");
  
  containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
}
