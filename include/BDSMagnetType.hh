#ifndef BDSMAGNETTYPE_H
#define BDSMAGNETTYPE_H

#include "BDSTypeSafeEnum.hh"
#include <map>

/**
 * @brief Type definition for everything that inherits
 * BDSMagnet - ie is a magnet
 * 
 * @author Laurie Nevay
 */

struct magnettypes_def
{
  enum type { decapole, hkicker, vkicker, muonspoiler, octupole, quadrupole,
	      sectorbend, rectangularbend, rfcavity, sextupole,
	      solenoid, multipole, thinmultipole};
};

typedef BDSTypeSafeEnum<magnettypes_def,int> BDSMagnetType;

#endif
