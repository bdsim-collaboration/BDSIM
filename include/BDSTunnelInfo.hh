#ifndef BDSTUNNELINFO
#define BDSTUNNELINFO

#include "globals.hh"         // geant4 types / globals
#include "G4Material.hh"
#include "BDSTunnelType.hh"

/**
 * @brief Holder struct of all information required to
 * create the a section of tunnel. This reduces 
 * the number of argument to all constructors 
 * plus aggregates common tasks in the component factory.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct BDSTunnelInfo {
  /// default constructor
  BDSTunnelInfo();

  /// extra constructor to assign all members at once
  BDSTunnelInfo(BDSTunnelType tunnelTypeIn,
		G4double      aper1In = 2.0*CLHEP::m,
		G4double      aper2In = 0.0,
		G4double      aper3In = 0.0,
		G4double      aper4In = 0.0,
		G4double      tunnelThicknessIn = 20*CLHEP::cm,
		G4Material*   tunnelMaterialIn = NULL,
		G4double      soilThicknessIn = 1*CLHEP::m,
		G4double      tunnelOffsetXIn = 0,
		G4double      tunnelOffsetYIn = 0);
		  
  BDSTunnelType tunnelType;
  G4double      aper1;
  G4double      aper2;
  G4double      aper3;
  G4double      aper4;
  G4double      tunnelThickness;
  G4Material*   tunnelMaterial;
  G4double      soilThickness;
  G4double      tunnelOffsetX;
  G4double      tunnelOffsetY;
  
};

#endif
