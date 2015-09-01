#include "elementtype.h"
#include <string>

using namespace GMAD;

std::ostream& GMAD::operator << (std::ostream& out, ElementType type) {
  return out << typestr(type);
}

std::string GMAD::typestr(ElementType type) {
  switch(type){
  case ElementType::_MARKER : 
    return "marker";
  case ElementType::_DRIFT :
    return "drift";
  case ElementType::_RF :
   return "rf";
  case ElementType::_SBEND : 
    return "sbend";
  case ElementType::_RBEND :
    return "rbend";
  case ElementType::_QUAD :
    return "quadrupole";
  case ElementType::_SEXTUPOLE :
    return "sextupole";
  case ElementType::_OCTUPOLE :
    return "octupole";
  case ElementType::_DECAPOLE :
    return "decapole";
  case ElementType::_MULT :
    return "multipole";
  case ElementType::_SOLENOID : 
    return "solenoid";
  case ElementType::_ECOL : 
    return "ecol";
  case ElementType::_MUSPOILER : 
    return "muspoiler";
  case ElementType::_VKICK :
    return "vkick";
  case ElementType::_HKICK :
    return "hkick";
  case ElementType::_RCOL : 
    return "rcol";
  case ElementType::_LINE :
    return "line";
  case ElementType::_REV_LINE :
    return "rev_line";
  case ElementType::_SAMPLER :
    return "sampler";
  case ElementType::_CSAMPLER:
    return "csampler";
  case ElementType::_GAS:
    return "gas";
  case ElementType::_TUNNEL:
    return "tunnel";
  case ElementType::_MATERIAL:
    return "material";
  case ElementType::_ATOM:
    return "atom";
  case ElementType::_LASER:
    return "laser";
  case ElementType::_ELEMENT :
    return "element";
  case ElementType::_TRANSFORM3D :
    return "transform3d";
  case ElementType::_SCREEN :
    return "screen";
 case ElementType::_AWAKESCREEN :
    return "awakescreen";
  case ElementType::_TELEPORTER : 
    return "teleporter";
  case ElementType::_TERMINATOR :
    return "terminator";
  default:
    return "none";
  }
}
