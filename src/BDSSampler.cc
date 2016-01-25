#include "BDSGlobalConstants.hh"
#include "BDSSampler.hh"

#include "globals.hh" // geant4 types / globals
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"

#include <vector>

/// Initialise static members.
G4int BDSSampler::totalNumberOfSamplers = 0;
std::vector<G4Transform3D> BDSSampler::transforms;
std::vector<G4Transform3D> BDSSampler::inverseTransforms;
std::vector<G4String>      BDSSampler::names;

BDSSampler::BDSSampler(G4String      name,
		       G4Transform3D transform):
  BDSGeometryComponent(nullptr, nullptr)
{
  names.push_back(name);
  transforms.push_back(transform);
  inverseTransforms.push_back(transform.inverse());
  samplerID = totalNumberOfSamplers;
  totalNumberOfSamplers++;
}

void BDSSampler::CommonConstruction()
{
  // Construct logical volume from solid. Note it's ok to have nullptr for material
  // as the sampler is only placed in a parallel world where the material is ignored.
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       nullptr,
					       GetName() + "_lv");
  
  containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());
}

G4int BDSSampler::AddExternalSampler(G4String      name,
				     G4Transform3D transform)
{
  names.push_back(name);
  transforms.push_back(transform);
  if (transform != G4Transform3D::Identity)
    {inverseTransforms.push_back(transform.inverse());}
  else
    {inverseTransforms.push_back(G4Transform3D());}
  G4int ID = totalNumberOfSamplers;
  totalNumberOfSamplers++;

  return ID;
}
