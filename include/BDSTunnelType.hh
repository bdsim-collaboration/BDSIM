#ifndef BDSTUNNELTYPE
#define BDSTUNNELTYPE

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Geometry type definition for tunnel segments - 
 * used for comparison in factory methods. 
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct tunneltypes_def {
  enum type { circular, square};

};

typedef BDSTypeSafeEnum<tunneltypes_def, G4int> BDSTunnelType;

#endif



