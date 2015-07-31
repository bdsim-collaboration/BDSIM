#ifndef BDSTUNNELFACTORYELLIPTICAL_H
#define BDSTUNNELFACTORYELLIPTICAL_H

#include "BDSTunnelFactoryBase.hh"

#include "globals.hh" // geant4 globals / types

class BDSTunnelSection;

/**
 * @brief Factory for elliptical tunnel segments. Builds floor, tunnel and soil.
 * 
 * singleton pattern
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSTunnelFactoryElliptical: public BDSTunnelFactoryBase
{
public:
  static BDSTunnelFactoryElliptical* Instance(); /// singleton accessor
  
  virtual ~BDSTunnelFactoryElliptical();
  
  /// Create a tunnel section with flat input and output faces.
  virtual BDSTunnelSection* CreateTunnelSection(G4String      name,
						G4double      length,
						G4double      tunnelThickness,
						G4double      tunnelSoilThickness,
						G4Material*   tunnelMaterial,
						G4Material*   tunnelSoilMaterial,
						G4bool        tunnelFloor,
						G4double      tunnelFloorOffset,
						G4double      tunnel1,
						G4double      tunnel2,
						G4bool        visible);

  /// Create a tunnel section with an angled input and output face.
  virtual BDSTunnelSection* CreateTunnelSectionAngled(G4String      name,
						      G4double      length,
						      G4ThreeVector inputFace,
						      G4ThreeVector outputFace,
						      G4double      tunnelThickness,
						      G4double      tunnelSoilThickness,
						      G4Material*   tunnelMaterial,
						      G4Material*   tunnelSoilMaterial,
						      G4bool        tunnelFloor,
						      G4double      tunnelFloorOffset,
						      G4double      tunnel1,
						      G4double      tunnel2,
						      G4bool        visible);
							       
private:
  /// Singleton pattern instance
  static BDSTunnelFactoryElliptical* _instance;

  /// Private constructor as singleton pattern
  BDSTunnelFactoryElliptical();

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
};
  
#endif
