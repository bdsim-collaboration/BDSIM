#ifndef BDSMAGNETTYPE_H
#define BDSMAGNETTYPE_H

#include "BDSTypeSafeEnum.hh"
#include <map>

/**
 * @brief Type definition for everything that inherits
 * BDSMagnet - ie is a magnet
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */


/// Magnet types enum - note, these must be manually duplicated in the
/// corresponding source file for this header in a string map!
struct magnettypes_def {
  enum type { decapole, hkicker, vkicker, muspoiler, octupole, quadrupole,
	      sectorbend, rectangularbend, rfcavity, sextupole,
	      solenoid, multipole};
};

typedef BDSTypeSafeEnum<magnettypes_def,int> BDSMagnetType;

#endif
