/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
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
  case ElementType::_THINMULT :
    return "thinmultipole";
  case ElementType::_SOLENOID :
    return "solenoid";
  case ElementType::_ECOL : 
    return "ecol";
  case ElementType::_RCOL : 
    return "rcol";
  case ElementType::_SHIELD:
    return "shield";
  case ElementType::_VKICKER :
    return "vkicker";
  case ElementType::_HKICKER :
    return "hkicker";
  case ElementType::_KICKER :
    return "kicker";
  case ElementType::_TKICKER :
    return "tkicker";
  case ElementType::_LINE :
    return "line";
  case ElementType::_REV_LINE :
    return "rev_line";
  case ElementType::_MATERIAL:
    return "material";
  case ElementType::_LASER:
    return "laser";
  case ElementType::_ELEMENT :
    return "element";
  case ElementType::_TRANSFORM3D :
    return "transform3d";
  case ElementType::_MUONSPOILER : 
    return "muonspoiler";
  case ElementType::_SCREEN :
    return "screen";
  case ElementType::_AWAKESCREEN :
    return "awakescreen";
  case ElementType::_AWAKESPECTROMETER :
    return "awakespectrometer";
  case ElementType::_DEGRADER :
    return "degrader";
  case ElementType::_GAP :
    return "gap";
  case ElementType::_THINRMATRIX :
    return "thinrmatrix";
  case ElementType::_PARALLELTRANSPORTER :
    return "paralleltransporter";
  case ElementType::_RMATRIX :
    return "rmatrix";
  case ElementType::_UNDULATOR :
    return "undulator";
  default:
    return "none";
  }
}
