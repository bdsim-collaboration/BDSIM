#ifndef __ELEMENTTYPE_H
#define __ELEMENTTYPE_H

#include <ostream>
#include <string>

// types of elements
namespace GMAD {
enum class ElementType {
  _NONE        = -1,
  _MARKER      = 1,
  _DRIFT       = 2,
  _RF          = 3,
  _SBEND       = 4, 
  _QUAD        = 5,
  _SEXTUPOLE   = 6,
  _OCTUPOLE    = 7,
  _DECAPOLE    = 8,
  _MULT        = 9,
  _SOLENOID    = 10,
  _LINE        = 11,
  _REV_LINE    = -11, //for line inversion in sublines
    // = 12,
  _ECOL        = 13,
  _RCOL        = 14,
  _LASER       = 15,
  _MATERIAL    = 16,
  _RBEND       = 17,
  _ATOM        = 18,
    // = 19,
  _ELEMENT     = 20,
  _SCREEN      = 21,
  _AWAKESCREEN = 22,
  _VKICK       = 31,
  _HKICK       = 32,
  _SAMPLER     = 41,
  _CSAMPLER    = 42,
  _DUMP        = 43,
  _TUNNEL      = 52,
  _TRANSFORM3D = 61,
  _MUSPOILER   = 62,
  _DEGRADER    = 63,
  _TELEPORTER  = 98,
  _TERMINATOR  = 99
};

/// conversion from enum to string
std::string typestr(ElementType type);

/// output operator
std::ostream& operator << (std::ostream& out, ElementType type);
}

#endif
