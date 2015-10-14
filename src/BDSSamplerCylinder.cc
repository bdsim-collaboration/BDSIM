#include "BDSSamplerCylinder.hh"

#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDManager.hh"

BDSSamplerCylinder::BDSSamplerCylinder(G4String name,
				       G4double length,
				       G4double radiusIn):
  BDSSamplerBase(name,
		 length,
		 "samplercylinder"),
  radius(radiusIn)
{}

void BDSSamplerCylinder::BuildContainerLogicalVolume()
{
  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial());
  containerSolid = new G4Tubs(name+"_body",
			      radius-1.e-6*CLHEP::m,
			      radius,
			      chordLength*0.5,
			      0,
			      CLHEP::twopi*CLHEP::radian);
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name);

  // set user limits, vis attributes and sensitive detector
  BDSSamplerBase::BuildContainerLogicalVolume();
}

BDSSamplerSD* BDSSamplerCylinder::GetSensitiveDetector()const
{
  return BDSSDManager::Instance()->GetSamplerCylinderSD();
}
