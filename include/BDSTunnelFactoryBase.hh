#ifndef BDSTUNNELFACTORYBASE_H
#define BDSTUNNELFACTORYBASE_H

#include "globals.hh"  // geant4 globals / types
#include "G4Material.hh"
#include "G4VSolid.hh"

#include "BDSGeometryComponent.hh"
#include "BDSTunnelType.hh"

struct BDSTunnelInfo;

/**
 * @brief Abstract base class for tunnel factory classes
 * 
 * Abstract base class that defines the interface of each factory
 * to build any type of tunnel required. There are four general
 * possibilities apart from the shape (each derived class will give
 * a different cross-section) and these dictate the angled faces.
 * 
 * Uses Geant4 default units by default, ie mm, rad (not metres)
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSTunnelFactoryBase
{
public:
  /// Create a tunnel section with flat input and output faces. Pure virtual.
  virtual BDSGeometryComponent* CreateTunnelSection(G4String      name,
						    G4double      length,
						    G4double      tunnelThickness,
						    G4double      tunnelSoilThickness,
						    G4Material*   tunnelMaterial,
						    G4Material*   tunnelSoilMaterial,
						    G4bool        tunnelFloor,
						    G4double      tunnelFloorOffset,
						    G4double      tunnel1,
						    G4double      tunnel2) = 0;

  /// Create a tunnel section with an angled input face and flat output face. Note,
  /// this is implemented in this base class as a dispatch to the AngledInOut function.
  virtual BDSGeometryComponent* CreateTunnelSectionAngledIn(G4String      name,
							    G4double      length,
							    G4double      angleIn,
							    G4double      tunnelThickness,
							    G4double      tunnelSoilThickness,
							    G4Material*   tunnelMaterial,
							    G4Material*   tunnelSoilMaterial,
							    G4bool        tunnelFloor,
							    G4double      tunnelFloorOffset,
							    G4double      tunnel1,
							    G4double      tunnel2);

  /// Create a tunnel section with an angled output face and flat input face. Note,
  /// this is implemented in this base class as a dispatch to the AngledInOut function.
  virtual BDSGeometryComponent* CreateTunnelSectionAngledOut(G4String      name,
							     G4double      length,
							     G4double      angleOut,
							     G4double      tunnelThickness,
							     G4double      tunnelSoilThickness,
							     G4Material*   tunnelMaterial,
							     G4Material*   tunnelSoilMaterial,
							     G4bool        tunnelFloor,
							     G4double      tunnelFloorOffset,
							     G4double      tunnel1,
							     G4double      tunnel2);

  /// Create a tunnel section with an angled input and output face. Pure virtual.
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
							       G4double      tunnel2) = 0;

protected:
  /// protected default constructor so only derived classes can use it
  BDSTunnelFactoryBase();
  
  /// Calculate input and output normal vector
  std::pair<G4ThreeVector,G4ThreeVector> CalculateFaces(G4double angleIn,
							G4double angleOut);

  /// General basic viability tests for input parameters - these are only basic tests
  /// and not dependent on the accelerator model, other components or specific tunnel model
  /// in the derived factory class
  void CommontTestInputParameters(G4double&    length,
				  G4double&    tunnelThickness,
				  G4double&    tunnelSoilThickness,
				  G4Material*& tunnelMaterial,
				  G4Material*& tunnelSoilMaterial);
  
  virtual void CommonConstruction(G4String    name,
				  G4Material* tunnelMaterial,
				  G4Material* tunnelSoilMaterial,
				  G4double    length);

  /// Build logical volumes from solids + materials
  virtual void BuildLogicalVolumes(G4String    name,
				   G4Material* tunnelMaterial,
				   G4Material* tunnelSoilMaterial);

  /// Set the visual attributes for each logical volume
  virtual void SetVisAttributes();
  
  /// Set user limits for all logical volumes in the tunnel section
  virtual void SetUserLimits(G4double length);
  
  /// Prepare the output geometry component
  virtual void PrepareGeometryComponent();

  /// Set the sensitive volumes
  virtual void SetSensitiveVolumes();
  
  /// Place components in container volume
  virtual void PlaceComponents(G4String name);

  /// Reset factory members for next usage - avoids previously
  /// constructed parts being accidently used in new object
  virtual void TidyUp();

  BDSGeometryComponent* tunnelSection;
  
  G4double         lengthSafety;
  G4VSolid*        containerSolid;
  G4VSolid*        tunnelSolid;
  G4VSolid*        soilSolid;
  G4VSolid*        floorSolid;
  G4LogicalVolume* containerLV;
  G4LogicalVolume* tunnelLV;
  G4LogicalVolume* soilLV;
  G4LogicalVolume* floorLV;
  G4ThreeVector    floorDisplacement;
  G4bool           checkOverlaps;
  BDSTunnelInfo*   defaultModel;
};
       










#endif
