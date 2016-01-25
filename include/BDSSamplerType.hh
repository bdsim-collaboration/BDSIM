#ifndef BDSSAMPLERTYPE_H
#define BDSSAMPLERTYPE_H

#include "globals.hh" // geant4 globals / types

#include "BDSTypeSafeEnum.hh"

#include <map>

/**
 * @brief Type definition for samplers.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

/// these must be manually duplicated in the
/// corresponding source file for this header in a string map!
struct samplertypes_def {
  enum type {none, plane, cylinder};
};

typedef BDSTypeSafeEnum<samplertypes_def,int> BDSSamplerType;

namespace BDS
{
  BDSSamplerType DetermineSamplerType(G4String samplerType);
}

#endif
