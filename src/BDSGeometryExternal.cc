#include "BDSGeometryExternal.hh"


BDSGeometryExternal::BDSGeometryExternal(G4VSolid*        containerSolid,
					 G4LogicalVolume* containerLV):
  BDSGeometryComponent(containerSolid, containerLV)
{;}

BDSGeometryExternal::BDSGeometryExternal(G4VSolid*         containerSolid,
					 G4LogicalVolume*  containerLV,
					 BDSExtent         extent,
					 BDSExtent         innerExtent,
					 G4ThreeVector     placementOffset,
					 G4RotationMatrix* placementRotation):
  BDSGeometryComponent(containerSolid, containerLV, extent, innerExtent,
		       placementOffset, placementRotation)
{;}

BDSGeometryExternal::~BDSGeometryExternal()
{;}
