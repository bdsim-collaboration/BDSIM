/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSFieldReflectionType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

// dictionary for BDSFieldType for reflexivity
template<>
std::map<BDSFieldReflectionType, std::string>* BDSFieldReflectionType::dictionary =
  new std::map<BDSFieldReflectionType, std::string> ({
      {BDSFieldReflectionType::none,               "none"},
      {BDSFieldReflectionType::vertical,           "vertical"},
      {BDSFieldReflectionType::horizontal,         "horizontal"},
      {BDSFieldReflectionType::mirrorvertical,     "mirrorvertical"},
      {BDSFieldReflectionType::mirrorhorizontal,   "mirrorhorizontal"},
      {BDSFieldReflectionType::quadrantdipole,     "quadrantdipole"},
      {BDSFieldReflectionType::quadrantquadrupole, "quadrantquadrupole"}
});

BDSFieldReflectionType BDS::DetermineFieldReflectionType(G4String fieldReflectionType)
{
  std::map<G4String, BDSFieldReflectionType> types;
  types["none"]               = BDSFieldReflectionType::none;
  types["vertical"]           = BDSFieldReflectionType::vertical;
  types["horizontal"]         = BDSFieldReflectionType::horizontal;
  types["mirrorvertical"]     = BDSFieldReflectionType::mirrorvertical;
  types["mirrorhorizontal"]   = BDSFieldReflectionType::mirrorhorizontal;
  types["quadrantdipole"]     = BDSFieldReflectionType::quadrantdipole;
  types["quadrantquadrupole"] = BDSFieldReflectionType::quadrantquadrupole;

  fieldReflectionType.toLower();

  auto result = types.find(fieldReflectionType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + fieldReflectionType + "\" is not a valid field reflection type\n";
      msg += "Available field reflection types are:\n";
      for (const auto& it : types)
	{msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined field reflection type to be " << result->second << G4endl;
#endif
  return result->second;
}
