#include "BDSMagnetOuterInfo.hh"

#include "BDSMagnetGeometryType.hh"
#include "globals.hh"             // geant4 globals / types
#include "G4Material.hh"

BDSMagnetOuterInfo::BDSMagnetOuterInfo()
{
  geometryType  = BDSMagnetGeometryType::cylindrical;
  outerDiameter = 0;
  outerMaterial = NULL;
}

BDSMagnetOuterInfo::BDSMagnetOuterInfo(BDSMagnetGeometryType geometryTypeIn,
				       G4double              outerDiameterIn,
				       G4Material*           outerMaterialIn)
{
  geometryType  = geometryTypeIn;
  outerDiameter = outerDiameterIn;
  outerMaterial = outerMaterialIn;
}
  
