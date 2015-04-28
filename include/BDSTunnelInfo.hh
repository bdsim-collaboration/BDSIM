#ifndef BDSTUNNELINFO
#define BDSTUNNELINFO

#include "BDSTunnelType.hh"
#include "globals.hh"         // geant4 types / globals
#include "G4Material.hh"

/**
 * @brief Holder struct of all information required to
 * create a section of tunnel. This reduces the number of 
 * arguments to all constructors plus aggregates common tasks
 * in the component factory.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct BDSTunnelInfo {
  /// default constructor
  BDSTunnelInfo();

  /// extra constructor to assign all members at once
  BDSTunnelInfo(BDSTunnelType tunnelTypeIn,
		G4double      lengthIn,
		G4double      tunnelThicknessIn,
		G4double      tunnelSoilThicknessIn,
		G4Material*   tunnelMaterialIn,
		G4Material*   tunnelSoilMaterialIn,
		G4bool        tunnelFloorIn,
		G4double      tunnelFloorOffsetIn,
		G4double      tunnel1In,
		G4double      tunnel2In);

  BDSTunnelType tunnelType;
  G4double      tunnelThickness;
  G4double      tunnelSoilThickness;
  G4Material*   tunnelMaterial;
  G4Material*   tunnelSoilMaterial;
  G4bool        tunnelFloor;
  G4double      tunnelFloorOffset;
  G4double      tunnel1;
  G4double      tunnel2;
};

#endif
