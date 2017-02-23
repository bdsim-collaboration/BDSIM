#ifndef BDSFIELDCLASSTYPE_H
#define BDSFIELDCLASSTYPE_H

#include "BDSFieldType.hh"
#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for general field class. Maps to a 
 * BDSFieldType enum.
 *
 * 'irregular' fields are typically dummy field objects necessary for
 * certain program mechanics or special cases, such as the teleporter.
 * 
 * @author Laurie Nevay
 */

struct fieldclasstypes_def
{
  enum type {magnetic, electric, electromagnetic, irregular};
};

typedef BDSTypeSafeEnum<fieldclasstypes_def,int> BDSFieldClassType;

namespace BDS
{
  /// Function that gives the corresponding enum value for a field type enum.
  BDSFieldClassType DetermineFieldClassType(BDSFieldType fieldType);
}

#endif


