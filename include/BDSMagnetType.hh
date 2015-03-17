#ifndef BDSMAGNETTYPE_H
#define BDSMAGNETTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for everything that inherits
 * BDSMultipole - ie is a magnet
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct magnettypes_def {
  enum type { decapole, hkicker, vkicker, muspoiler, octupole, quadrupole,
	      sectorbend,rectangularbend, rfcavity, sbend, sextupole,
	      solenoid, multipole};
};

typedef BDSTypeSafeEnum<magnettypes_def,int> BDSMagnetType;

#endif



