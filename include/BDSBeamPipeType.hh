#ifndef BDSBEAMPIPETYPE_H
#define BDSBEAMPIPETYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for beampipes - used for comparison
 * in factory methods
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct beampipetypes_def {
  enum type {circular, rectangular, elliptical, lhc, lhcdetailed, rectellipse, racetrack, octagonal};
};

typedef BDSTypeSafeEnum<beampipetypes_def,int> BDSBeamPipeType;

namespace BDS {
  /// function that gives corresponding enum value for string (case-insensitive)
  BDSBeamPipeType DetermineBeamPipeType(G4String apertureType);
}

#endif
