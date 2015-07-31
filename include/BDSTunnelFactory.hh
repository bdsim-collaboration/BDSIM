#ifndef BDSTUNNELFACTORY_H
#define BDSTUNNELFACTORY_H

#include "globals.hh"           // geant4 globals / types

#include "BDSTunnelType.hh"

class BDSTunnelFactoryBase;
class BDSTunnelSection;

class G4Material;

class BDSTunnelFactory
{
public:
  static BDSTunnelFactory* Instance(); ///< singleton pattern
  
  ~BDSTunnelFactory();

  /// Create a tunnel section with straight input and output face.
  BDSTunnelSection* CreateTunnelSection(BDSTunnelType tunnelType,
					G4String      name,
					G4double      length,
					G4double      tunnelThickness,
					G4double      tunnelSoilThickness,
					G4Material*   tunnelMaterial,
					G4Material*   tunnelSoilMaterial,
					G4bool        tunnelFloor,
					G4double      tunnelFloorOffset,
					G4double      tunnel1,
					G4double      tunnel2,
					G4bool        visible = true);

  /// Create a tunnel section with an angled input face and flat output face. Note,
  /// this is implemented in this base class as a dispatch to the AngledInOut function.
  BDSTunnelSection* CreateTunnelSectionAngledIn(BDSTunnelType tunnelType,
						G4String      name,
						G4double      length,
						G4double      angleIn,
						G4double      tunnelThickness,
						G4double      tunnelSoilThickness,
						G4Material*   tunnelMaterial,
						G4Material*   tunnelSoilMaterial,
						G4bool        tunnelFloor,
						G4double      tunnelFloorOffset,
						G4double      tunnel1,
						G4double      tunnel2,
						G4bool        visible = true);

  /// Create a tunnel section with an angled output face and flat input face. Note,
  /// this is implemented in this base class as a dispatch to the AngledInOut function.
  BDSTunnelSection* CreateTunnelSectionAngledOut(BDSTunnelType tunnelType,
						 G4String      name,
						 G4double      length,
						 G4double      angleOut,
						 G4double      tunnelThickness,
						 G4double      tunnelSoilThickness,
						 G4Material*   tunnelMaterial,
						 G4Material*   tunnelSoilMaterial,
						 G4bool        tunnelFloor,
						 G4double      tunnelFloorOffset,
						 G4double      tunnel1,
						 G4double      tunnel2,
						 G4bool        visible = true);

  /// Create a tunnel section with an angled input and output face.
  BDSTunnelSection* CreateTunnelSectionAngledInOut(BDSTunnelType tunnelType,
						   G4String      name,
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
						   G4double      tunnel2,
						   G4bool        visible = true);

  /// Create a tunnel section with two angled faces as defined by normal vectors for
  /// each face.
  BDSTunnelSection* CreateTunnelSectionAngled(BDSTunnelType  tunnelType,
					      G4String       name,
					      G4double       length,
					      G4ThreeVector  inputFaceIn,
					      G4ThreeVector  outputFaceIn,
					      G4double       tunnelThickness,
					      G4double       tunnelSoilThickness,
					      G4Material*    tunnelMaterial,
					      G4Material*    tunnelSoilMaterial,
					      G4bool         tunnelFloor,
					      G4double       tunnelFloorOffset,
					      G4double       tunnel1,
					      G4double       tunnel2,
					      G4bool         visible = true);
  
private:
  /// Private constructor as a singleton
  BDSTunnelFactory();

  /// Singleton instance
  static BDSTunnelFactory* _instance;

  /// Accessor method to dispatch to appropriate derived factory
  BDSTunnelFactoryBase* GetAppropriateFactory(BDSTunnelType tunnelType);

};



#endif
