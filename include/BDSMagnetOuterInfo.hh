#ifndef BDSMAGNETOUTERINFO
#define BDSMAGNETOUTERINFO

#include "BDSMagnetGeometryType.hh"
#include "globals.hh"         // geant4 types / globals
#include "G4Material.hh"

/**
 * @brief Holder struct of all information required to
 * create the outer geometry of a magnet. 
 * 
 * This reduces the number of argument to all magnet constructors 
 * plus aggregates common tasks in the component factory.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct BDSMagnetOuterInfo {
  /// default constructor
  BDSMagnetOuterInfo();

  /// extra constructor to assign all members at once
  BDSMagnetOuterInfo(BDSMagnetGeometryType geometryTypeIn,
		     G4double              outerDiameterIn,
		     G4Material*           outerMaterialIn);
		  
  BDSMagnetGeometryType geometryType;
  G4double        outerDiameter;
  G4Material*     outerMaterial;
};

#endif
