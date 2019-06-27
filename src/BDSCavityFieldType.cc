/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include "BDSCavityFieldType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"

#include <map>
#include <string>

// dictionary for BDSCavityFieldType
template<>
std::map<BDSCavityFieldType, std::string>* BDSCavityFieldType::dictionary =
  new std::map<BDSCavityFieldType, std::string> ({
  {BDSCavityFieldType::bdsim, "bdsim"},
  {BDSCavityFieldType::mad8, "mad8"}
});

BDSCavityFieldType BDS::DetermineCavityFieldType(G4String cavityFieldType)
{
  std::map<G4String, BDSCavityFieldType> types;
  types["bdsim"] = BDSCavityFieldType::bdsim;
  types["mad8"] = BDSCavityFieldType::mad8;

  cavityFieldType.toLower();

  auto result = types.find(cavityFieldType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cout << __METHOD_NAME__ << "\"" << cavityFieldType << "\" is not a valid cavity field type" << G4endl;
      G4cout << "Available cavity field types are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      throw BDSException(__METHOD_NAME__, "");
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined cavity field type to be " << result->second << G4endl;
#endif
  return result->second;
}
