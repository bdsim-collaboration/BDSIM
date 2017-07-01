#include "BDSCavity.hh"
#include "BDSExtent.hh"



BDSCavity::BDSCavity(G4VSolid*         containerSolidIn,
		     G4LogicalVolume*  containerLVIn,
		     BDSExtent         extentIn,
		     G4LogicalVolume*  vacuumLVIn,
		     G4ThreeVector     placementOffsetIn,
		     G4RotationMatrix* placementRotationIn):
  BDSGeometryComponent(containerSolidIn,
		       containerLVIn,
		       extentIn,
		       BDSExtent(),
		       placementOffsetIn,
		       placementRotationIn),
  vacuumLogicalVolume(vacuumLVIn)
{;}
