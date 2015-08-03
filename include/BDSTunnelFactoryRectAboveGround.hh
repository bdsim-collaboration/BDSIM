#ifndef BDSTUNNELFACTORYRECTABOVEGROUND_H
#define BDSTUNNELFACTORYRECTABOVEGROUND_H

#include "BDSTunnelFactoryBase.hh"

#include "globals.hh" // geant4 globals / types

class BDSTunnelSection;

/**
 * @brief Factory for rectangular tunnel segments above ground - ie there
 * is no soil. Builds tunnel, floor (optional) and concrete slab that it
 * all sits on.
 * 
 * singleton pattern
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSTunnelFactoryRectAboveGround: public BDSTunnelFactoryBase
{
public:
  static BDSTunnelFactoryRectAboveGround* Instance(); /// singleton accessor
  
  virtual ~BDSTunnelFactoryRectAboveGround();
  
  /// Create a tunnel section with flat input and output faces.
  virtual BDSTunnelSection* CreateTunnelSection(G4String    name,
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
  static BDSTunnelFactoryRectAboveGround* _instance;

  /// Private constructor as singleton pattern
  BDSTunnelFactoryRectAboveGround();

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

  G4VSolid* slabSolid;
  //G4double  slabXHalfWidth;
  G4double  slabYHalfWidth;

};
  
#endif
