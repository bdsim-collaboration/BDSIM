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
  enum type {none,
	     teleporter,
	     bmap1d,  bmap2d,  bmap3d,  bmap4d,
	     emap1d,  emap2d,  emap3d,  emap4d,
	     ebmap1d, ebmap2d, ebmap3d, ebmap4d,
	     mokka,
	     solenoid, dipole, quadrupole, dipolequadrupole, sextupole,
	     octupole, decapole, multipole, muonspoiler,
             skewquadrupole, skewsextupole, skewoctupole, skewdecapole,
	     rfcavity, rf};
};

// NOTE - when adding a new field type, BDSFieldClassType should also be updated

typedef BDSTypeSafeEnum<fieldtypes_def,int> BDSFieldType;

namespace BDS
{
  /// Function that gives corresponding enum value for string (case-insensitive)
  BDSFieldType DetermineFieldType(G4String fieldType);
}

#endif


