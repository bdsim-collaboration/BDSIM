#ifndef BDSFIELDCLASSTYPE_H
#define BDSFIELDCLASSTYPE_H

#include "BDSFieldType.hh"
#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for general field class. Maps to a 
 * BDSFieldType enum.
 * 
 * @author Laurie Nevay
 */

struct fieldclasstypes_def
{
  enum type {magnetic, electric, electromagnetic};
};

typedef BDSTypeSafeEnum<fieldclasstypes_def,int> BDSFieldClassType;

namespace BDS
{
  /// Function that gives the corresponding enum value for string (case-insensitive).
  BDSFieldClassType DetermineFieldClassType(G4String fieldType);

  /// Function that gives the corresponding enum value for a field type enum.
  BDSFieldClassType DetermineFieldClassType(BDSFieldType fieldType);
}

#endif


