#include "enums.h"

const char *typestr(int type) {
  switch(type){
  case _MARKER : 
    return "marker";
  case _DRIFT :
    return "drift";
  case _PCLDRIFT :
    return "pcldrift";
  case _RF :
   return "rf";
  case _SBEND : 
    return "sbend";
  case _RBEND :
    return "rbend";
  case _QUAD :
    return "quadrupole";
  case _SEXTUPOLE :
    return "sextupole";
  case _OCTUPOLE :
    return "octupole";
  case _MULT :
    return "multipole";
  case _SOLENOID : 
    return "solenoid";
  case _ECOL : 
    return "ecol";
  case _MUSPOILER : 
    return "muspoiler";
  case _VKICK :
    return "vkick";
  case _HKICK :
    return "hkick";
  case _RCOL : 
    return "rcol";
  case _LINE :
    return "line";
  case _REV_LINE :
    return "rev_line";
  case _SAMPLER :
    return "sampler";
  case _CSAMPLER:
    return "csampler";
  case _GAS:
    return "gas";
  case _TUNNEL:
    return "tunnel";
  case _MATERIAL:
    return "material";
  case _ATOM:
    return "atom";
  case _LASER:
    return "laser";
  case _ELEMENT :
    return "element";
  case _TRANSFORM3D :
    return "transform3d";
  case _SCREEN :
    return "screen";
 case _AWAKESCREEN :
    return "awakescreen";
  case _TELEPORTER : 
    return "teleporter";
  case _TERMINATOR :
    return "terminator";
  default:
    return "none";
  }
}
