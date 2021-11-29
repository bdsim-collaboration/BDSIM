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
#include "BDSArrayReflectionType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>
#include <vector>

// dictionary for BDSArrayReflectionType for reflexivity
template<>
std::map<BDSArrayReflectionType, std::string>* BDSArrayReflectionType::dictionary =
  new std::map<BDSArrayReflectionType, std::string> ({
      {BDSArrayReflectionType::flipx,               "flipx"},
      {BDSArrayReflectionType::flipy,               "flipy"},
      {BDSArrayReflectionType::flipz,               "flipz"},
      {BDSArrayReflectionType::flipt,               "flipt"},
      {BDSArrayReflectionType::reflectx,            "reflectx"},
      {BDSArrayReflectionType::reflecty,            "reflecty"},
      {BDSArrayReflectionType::reflectz,            "reflectz"},
      {BDSArrayReflectionType::reflectt,            "reflectt"},
      {BDSArrayReflectionType::reflectxydipole,     "reflectxydipole"},
      {BDSArrayReflectionType::reflectxzdipole,     "reflectxzdipole"},
      {BDSArrayReflectionType::reflectyzdipole,     "reflectyzdipole"},
      {BDSArrayReflectionType::reflectxyquadrupole, "reflectxyquadrupole"}
});

BDSArrayReflectionType BDS::DetermineArrayReflectionType(G4String arrayReflectionType)
{
  std::map<G4String, BDSArrayReflectionType> types;
  types["flipx"]               = BDSArrayReflectionType::flipx;
  types["flipy"]               = BDSArrayReflectionType::flipy;
  types["flipz"]               = BDSArrayReflectionType::flipz;
  types["flipt"]               = BDSArrayReflectionType::flipt;
  types["reflectx"]            = BDSArrayReflectionType::reflectx;
  types["reflecty"]            = BDSArrayReflectionType::reflecty;
  types["reflectz"]            = BDSArrayReflectionType::reflectz;
  types["reflectt"]            = BDSArrayReflectionType::reflectt;
  types["reflectxydipole"]     = BDSArrayReflectionType::reflectxydipole;
  types["reflectxzdipole"]     = BDSArrayReflectionType::reflectxzdipole;
  types["reflectyzdipole"]     = BDSArrayReflectionType::reflectyzdipole;
  types["reflectxyquadrupole"] = BDSArrayReflectionType::reflectxyquadrupole;
  
  arrayReflectionType.toLower();

  auto result = types.find(arrayReflectionType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + arrayReflectionType + "\" is not a valid array reflection type\n";
      msg += "Available array reflection types are:\n";
      for (const auto& it : types)
	{msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined array reflection type to be " << result->second << G4endl;
#endif
  return result->second;
}

BDSArrayReflectionTypeSet BDS::DetermineArrayReflectionTypeSet(const G4String& arrayReflectionType)
{
  BDSArrayReflectionTypeSet result;
  std::vector<G4String> words = BDS::SplitOnWhiteSpace(arrayReflectionType);
  for (const auto& word : words)
    {result.insert(BDS::DetermineArrayReflectionType(word));}
  return result;
}

std::ostream& operator<< (std::ostream &out, BDSArrayReflectionTypeSet const& t)
{
  for (const auto& v : t)
    {out << v;}
  return out;
}