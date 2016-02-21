#ifndef BDSGEOMETRYTYPE_H
#define BDSGEOMETRYTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for external geometry formats - 
 * used for comparison in factory methods.
 * 
 * @author Laurie Nevay
 */

struct geometrytypes_def
{
  enum type {threed, xy, mokka, lcdd, gmad, gdml};
};

typedef BDSTypeSafeEnum<geometrytypes_def,int> BDSGeometryType;

namespace BDS
{
  /// function that gives corresponding enum value for string (case-insensitive)
  BDSGeometryType DetermineGeometryType(G4String geometryType);
}

#endif
