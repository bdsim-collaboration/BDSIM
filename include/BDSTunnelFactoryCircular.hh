#ifndef BDSTUNNELFACTORYCIRCULAR_H
#define BDSTUNNELFACTORYCIRCULAR_H

#include "BDSTunnelFactoryBase.hh"
#include "BDSGeometryComponent.hh"

#include "globals.hh" // geant4 globals / types

/**
 * @brief Factory for circular tunnel segments. Builds floor, tunnel and soil.
 * 
 * singleton pattern
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSTunnelFactoryCircular: public BDSTunnelFactoryBase
{
public:
  static BDSTunnelFactoryCircular* Instance(); /// singleton accessor
  
  virtual ~BDSTunnelFactoryCircular();
  
  /// Create a tunnel section with flat input and output faces.
  virtual BDSGeometryComponent* CreateTunnelSection(G4String      name,
						    G4double      length,
						    G4double      tunnelThickness,
						    G4double      tunnelSoilThickness,
						    G4Material*   tunnelMaterial,
						    G4Material*   tunnelSoilMaterial,
						    G4bool        tunnelFloor,
						    G4double      tunnelFloorOffset,
						    G4double      tunnel1,
						    G4double      tunnel2);

  /// Create a tunnel section with an angled input and output face.
  virtual BDSGeometryComponent* CreateTunnelSectionAngledInOut(G4String      name,
							       G4double      length,
							       G4double      angleIn,
							       G4double      angleOut,
							       G4double      tunnelThickness,
							       G4double      tunnelSoilThickness,
							       G4Material*   tunnelMaterial,
							       G4Material*   tunnelSoilMaterial,
							       G4bool        tunnelFloor,
							       G4double      tunnelFloorOffset,
							       G4double      tunnel1,
							       G4double      tunnel2);
							       
private:
  /// Singleton pattern instance
  static BDSTunnelFactoryCircular* _instance;

  /// Private constructor as singleton pattern
  BDSTunnelFactoryCircular();

  /// Test input parameters - this is only for basic viability not in comparison to
  /// the accelerator model or other components
  void TestInputParameters(G4double&    length,
			   G4double&    tunnelThickness,
			   G4double&    tunnelSoilThickness,
			   G4Material*& tunnelMaterial,
			   G4Material*& tunnelSoildMaterial,
			   G4double&    tunnelFloorOffset,
			   G4double&    tunnel1);

  BDSGeometryComponent* CommonFinalConstruction(G4String    name,
						G4double    length,
						G4Material* tunnelMaterial,
						G4Material* soilMaterial,
						G4double    containerRadius);

};
  
#endif
