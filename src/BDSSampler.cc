#include "BDSSampler.hh"

#include "BDSGlobalConstants.hh" 
#include "BDSMaterials.hh"
#include "BDSDebug.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDManager.hh"

G4Box* BDSSampler::containerSolidSampler = nullptr;
G4LogicalVolume* BDSSampler::containerLogicalVolumeSampler = nullptr;

BDSSampler::BDSSampler(G4String name,
		       G4double length):
  BDSSamplerBase(name, length, "sampler")
{}

void BDSSampler::BuildContainerLogicalVolume()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  G4double samplerDiameter = BDSGlobalConstants::Instance()->GetSamplerDiameter() * 0.5;

  if(!containerSolidSampler)
    {
      containerSolid = containerSolidSampler = new G4Box("Sampler_solid",
							 samplerDiameter,
							 samplerDiameter,
							 chordLength*0.5);
      containerLogicalVolume = containerLogicalVolumeSampler = new G4LogicalVolume(containerSolidSampler,
										   emptyMaterial,
										   "Sampler");

      // set user limits, vis attributes and sensitive detector
      BDSSamplerBase::BuildContainerLogicalVolume();
    }
  else
    {
      containerSolid = containerSolidSampler;
      containerLogicalVolume = containerLogicalVolumeSampler;
    }
}

BDSSamplerSD* BDSSampler::GetSensitiveDetector()const
{
  return BDSSDManager::Instance()->GetSamplerPlaneSD();
}

