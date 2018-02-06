#ifndef ELEMENTTYPE_H
#define ELEMENTTYPE_H

#include <ostream>
#include <string>

namespace GMAD
{
  /// types of elements
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
    // = 18,
    // = 19,
    _ELEMENT     = 20,
    _SCREEN      = 21,
    _AWAKESCREEN = 22,
    _AWAKESPECTROMETER = 23,
    _SHIELD      = 24,
    _VKICKER     = 31,
    _HKICKER     = 32,
    _KICKER      = 33,
    _TKICKER     = 34,
    _TRANSFORM3D = 61,
    _MUSPOILER   = 62,
    _DEGRADER    = 63,
    _THINMULT    = 64,
    _GAP         = 65
  };

  /// conversion from enum to string
  std::string typestr(ElementType type);

  /// output operator
  std::ostream& operator << (std::ostream& out, ElementType type);
}

#endif
