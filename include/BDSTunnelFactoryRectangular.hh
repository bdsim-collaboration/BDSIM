#ifndef BDSTUNNELFACTORYRECTANGULAR_H
#define BDSTUNNELFACTORYRECTANGULAR_H

#include "BDSTunnelFactoryBase.hh"
#include "BDSGeometryComponent.hh"

#include "globals.hh" // geant4 globals / types

/**
 * @brief Factory for rectangular tunnel segments. Builds floor, tunnel and soil.
 * 
 * singleton pattern
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSTunnelFactoryRectangular: public BDSTunnelFactoryBase
{
public:
  static BDSTunnelFactoryRectangular* Instance(); /// singleton accessor
  
  virtual ~BDSTunnelFactoryRectangular();
  
  /// Create a tunnel section with flat input and output faces.
  virtual BDSGeometryComponent* CreateTunnelSection(G4String    name,
						    G4double    length,
						    G4double    tunnelThickness,
						    G4double    tunnelSoilThickness,
						    G4Material* tunnelMaterial,
						    G4Material* tunnelSoilMaterial,
						    G4bool      tunnelFloor,
						    G4double    tunnelFloorOffset,
						    G4double    tunnel1,
						    G4double    tunnel2,
						    G4bool      visible);

  /// Create a tunnel section with an angled input and output face.
  virtual BDSGeometryComponent* CreateTunnelSectionAngledInOut(G4String    name,
							       G4double    length,
							       G4double    angleIn,
							       G4double    angleOut,
							       G4double    tunnelThickness,
							       G4double    tunnelSoilThickness,
							       G4Material* tunnelMaterial,
							       G4Material* tunnelSoilMaterial,
							       G4bool      tunnelFloor,
							       G4double    tunnelFloorOffset,
							       G4double    tunnel1,
							       G4double    tunnel2,
							       G4bool      visible);
							       
private:
  /// Singleton pattern instance
  static BDSTunnelFactoryRectangular* _instance;

  /// Private constructor as singleton pattern
  BDSTunnelFactoryRectangular();

  /// Test input parameters - this is only for basic viability not in comparison to
  /// the accelerator model or other components
  void TestInputParameters(G4double&    length,
			   G4double&    tunnelThickness,
			   G4double&    tunnelSoilThickness,
			   G4Material*& tunnelMaterial,
			   G4Material*& tunnelSoildMaterial,
			   G4double&    tunnelFloorOffset,
			   G4double&    tunnel1,
			   G4double&    tunnel2);

  BDSGeometryComponent* CommonFinalConstruction(G4String    name,
						G4double    length,
						G4Material* tunnelMaterial,
						G4Material* soilMaterial,
						G4double    containerXRadius,
						G4double    containerYRadius,
						G4bool      visible);

};
  
#endif
