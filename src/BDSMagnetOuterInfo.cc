#include "BDSMagnetOuterInfo.hh"

#include "BDSMagnetGeometryType.hh"
#include "globals.hh"             // geant4 globals / types
#include "G4Material.hh"

BDSMagnetOuterInfo::BDSMagnetOuterInfo()
{
  geometryType  = BDSMagnetGeometryType::cylindrical;
  outerDiameter = 0;
  outerMaterial = nullptr;
}

BDSMagnetOuterInfo::BDSMagnetOuterInfo(G4String              nameIn,
				       BDSMagnetGeometryType geometryTypeIn,
				       G4double              outerDiameterIn,
				       G4Material*           outerMaterialIn,
				       G4double              angleIn)
{
  name          = nameIn;
  geometryType  = geometryTypeIn;
  outerDiameter = outerDiameterIn;
  outerMaterial = outerMaterialIn;
  angle         = angleIn;
}
  
