#ifndef BDSMAGNETGEOMETRYTYPE_H
#define BDSMAGNETGEOMETRYTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Geometry type definition for magnets - used for comparison
 * in factory methods. 
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct magnettypes_def {
  enum type { cylindrical, normalconducting, lhc };

};

typedef BDSTypeSafeEnum<magnettypes_def, G4int> BDSMagnetGeometryType;

namespace BDS {
  /// function to determine the type of the magnet geometry
  BDSMagnetGeometryType DetermineMagnetGeometryType(G4String geometryType,
						    G4bool   globalCheck = false);
}

#endif



