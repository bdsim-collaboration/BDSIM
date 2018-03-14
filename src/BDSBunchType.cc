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
#include "BDSBeamPipeInfo.hh"
#include "BDSBunchType.hh"
#include "BDSDebug.hh"

#include "globals.hh"

#include <map>

// dictionary for BDSBunchType
template<>
std::map<BDSBunchType, std::string>* BDSBunchType::dictionary =
  new std::map<BDSBunchType, std::string> ({
      {BDSBunchType::reference,   "reference"},
      {BDSBunchType::gaussian,    "gaussian"},
      {BDSBunchType::square,      "square"},
      {BDSBunchType::circle,      "circle"},
      {BDSBunchType::ring,        "ring"},
      {BDSBunchType::eshell,      "eshell"},
      {BDSBunchType::sixtrack,    "sixtrack"},
      {BDSBunchType::twiss,       "twiss"},
      {BDSBunchType::halo,        "halo"},
      {BDSBunchType::userfile,    "userfile"},
      {BDSBunchType::composite,   "composite"},
      {BDSBunchType::ptc,         "ptc"}
});	

BDSBunchType BDS::DetermineBunchType(G4String distrType)
{
  std::map<G4String, BDSBunchType> types;

  types["reference"]      = BDSBunchType::reference;
  types["gaussian"]       = BDSBunchType::gaussian;
  types["gauss"]          = BDSBunchType::gaussian;
  types["gaussmatrix"]    = BDSBunchType::gaussian;
  types["square"]         = BDSBunchType::square;
  types["circle"]         = BDSBunchType::circle;
  types["ring"]           = BDSBunchType::ring;
  types["eshell"]         = BDSBunchType::eshell;
  types["sixtrack"]       = BDSBunchType::sixtrack;
  types["gausstwiss"]     = BDSBunchType::twiss;
  types["twiss"]          = BDSBunchType::twiss;
  types["halo"]           = BDSBunchType::halo;
  types["userfile"]       = BDSBunchType::userfile;
  types["composite"]      = BDSBunchType::composite;
  types["ptc"]            = BDSBunchType::ptc;

  distrType.toLower();

  auto result = types.find(distrType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << distrType << " is not a valid distribution" << G4endl;

      G4cout << "Available distributions are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined distribution to be " << result->second  << G4endl;
#endif
  return result->second;
}
