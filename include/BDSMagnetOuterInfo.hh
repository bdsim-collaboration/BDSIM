#ifndef BDSMAGNETOUTERINFO_H
#define BDSMAGNETOUTERINFO_H

#include "BDSMagnetGeometryType.hh"

#include "globals.hh"         // geant4 types / globals
#include "G4Material.hh"

#include <ostream>

/**
 * @brief Holder struct of all information required to
 * create the outer geometry of a magnet. 
 * 
 * This reduces the number of argument to all magnet constructors 
 * plus aggregates common tasks in the component factory.
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuterInfo
{
public:
  /// default constructor
  BDSMagnetOuterInfo();

  /// extra constructor to assign all members at once
  BDSMagnetOuterInfo(G4String              nameIn,
		     BDSMagnetGeometryType geometryTypeIn,
		     G4double              outerDiameterIn,
		     G4Material*           outerMaterialIn,
		     G4double              angle = 0.0);

  G4String              name;
  BDSMagnetGeometryType geometryType;
  G4double              outerDiameter;
  G4Material*           outerMaterial;
  G4double              angle;

  /// Output stream operator implementation.
  friend std::ostream& operator<< (std::ostream& out, BDSMagnetOuterInfo const& info);
};

#endif
