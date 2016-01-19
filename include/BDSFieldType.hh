#ifndef BDSFIELDTYPE_H
#define BDSFIELDTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for field formats - 
 * used for comparison in factory methods.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct fieldtypes_def
{
  enum type {zero, threed, xy, mokka, solenoid, dipole, quadrupole,
	     sextupole, octupole, decapole, multipole, muonspoiler,
             skewquadrupole, skewsextupole, skewoctupole, skewdecapole};
};

typedef BDSTypeSafeEnum<fieldtypes_def,int> BDSFieldType;

namespace BDS
{
  /// function that gives corresponding enum value for string (case-insensitive)
  BDSFieldType DetermineFieldType(G4String fieldType);
}

#endif


