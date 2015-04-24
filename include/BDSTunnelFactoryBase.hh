#ifndef BDSTUNNELFACTORYBASE_H
#define BDSTUNNELFACTORYBASE_H

#include "globals.hh"  // geant4 globals / types
#include "G4Material.hh"
#include "G4VSolid.hh"

#include "BDSGeometryComponent.hh"
#include "BDSTunnelType.hh"

class BDSTunnelFactoryBase
{
public:
  virtual BDSGeometryComponent* BuildTunnelSection(BDSTunnelType tunnleType,
						   G4double      length,
						   G4double      tunnelThickness,
						   G4double      tunnelSoilThickness,
						   G4Material*   tunnelMaterial,
						   G4Material*   tunnelSoilMaterial,
						   G4bool        tunnelFloor,
						   G4double      tunnelFloorOffset,
						   G4double      tunnel1,
						   G4double      tunnel2);


protected:
  virtual void CommonConstruction(G4Material* tunnelMaterial,
				  G4Material* tunnelSoilMaterial);

  G4VSolid* tunnelSolid;
  G4VSolid* soilSolid;
  G4VSolid* floorSolid;

}
       










#endif
