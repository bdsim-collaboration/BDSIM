#include "BDSSampler.hh"

#include "BDSGlobalConstants.hh" 
#include "BDSMaterials.hh"
#include "BDSDebug.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDManager.hh"

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
  containerSolid = new G4Box(name + "_solid",
			     samplerDiameter,
			     samplerDiameter,
			     chordLength*0.5);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name);

  // set user limits, vis attributes and sensitive detector
  BDSSamplerBase::BuildContainerLogicalVolume();
}

BDSSamplerSD* BDSSampler::GetSensitiveDetector()const
{
  return BDSSDManager::Instance()->GetSamplerPlaneSD();
}

