#ifndef BDSFIELDTYPE_H
#define BDSFIELDTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for field formats - 
 * used for comparison in factory methods.
 * 
 * @author Laurie Nevay
 */

struct fieldtypes_def
{
  enum type {none, zero, threed, xy, mokka,
	     solenoid, dipole, quadrupole, dipolequadrupole, sextupole, octupole, decapole,
	     fringe, multipole, muonspoiler,
             skewquadrupole, skewsextupole, skewoctupole, skewdecapole,
	     rfcavity, rf};
};

typedef BDSTypeSafeEnum<fieldtypes_def,int> BDSFieldType;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive)
  BDSFieldType DetermineFieldType(G4String fieldType);
}

#endif


