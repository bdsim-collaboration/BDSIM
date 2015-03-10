#ifndef BDSMAGNETGEOMETRYTYPE_H
#define BDSMAGNETGEOMETRYTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types
#include "parser/element.h"

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

#endif



