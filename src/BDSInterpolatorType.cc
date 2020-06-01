/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSInterpolatorType.hh"
#include "BDSDebug.hh"

#include "globals.hh" // geant4 types / globals

#include <map>
#include <string>

// dictionary for BDSInterpolatorType for reflexivity
template<>
std::map<BDSInterpolatorType, std::string>* BDSInterpolatorType::dictionary =
  new std::map<BDSInterpolatorType, std::string> ({
      {BDSInterpolatorType::nearestauto,"nearestauto"},
      {BDSInterpolatorType::nearest1d,  "nearest1d"},
      {BDSInterpolatorType::nearest2d,  "nearest2d"},
      {BDSInterpolatorType::nearest3d,  "nearest3d"},
      {BDSInterpolatorType::nearest4d,  "nearest4d"},
      {BDSInterpolatorType::linearauto, "linearauto"},
      {BDSInterpolatorType::linear1d,   "linear1d"},
      {BDSInterpolatorType::linear2d,   "linear2d"},
      {BDSInterpolatorType::linear3d,   "linear3d"},
      {BDSInterpolatorType::linear4d,   "linear4d"},
      {BDSInterpolatorType::cubicauto,  "cubicauto"},
      {BDSInterpolatorType::cubic1d,    "cubic1d"},
      {BDSInterpolatorType::cubic2d,    "cubic2d"},
      {BDSInterpolatorType::cubic3d,    "cubic3d"},
      {BDSInterpolatorType::cubic4d,    "cubic4d"}
    });

BDSInterpolatorType BDS::DetermineInterpolatorType(G4String interpolatorType)
{
  std::map<G4String, BDSInterpolatorType> types;
  types["nearest"]   = BDSInterpolatorType::nearestauto;
  types["nearest1d"] = BDSInterpolatorType::nearest1d;
  types["nearest2d"] = BDSInterpolatorType::nearest2d;
  types["nearest3d"] = BDSInterpolatorType::nearest3d;
  types["nearest4d"] = BDSInterpolatorType::nearest4d;
  types["linear"]    = BDSInterpolatorType::linearauto;
  types["linear1d"]  = BDSInterpolatorType::linear1d;
  types["linear2d"]  = BDSInterpolatorType::linear2d;
  types["linear3d"]  = BDSInterpolatorType::linear3d;
  types["linear4d"]  = BDSInterpolatorType::linear4d;
  types["cubic"]     = BDSInterpolatorType::cubicauto;
  types["cubic1d"]   = BDSInterpolatorType::cubic1d;
  types["cubic2d"]   = BDSInterpolatorType::cubic2d;
  types["cubic3d"]   = BDSInterpolatorType::cubic3d;
  types["cubic4d"]   = BDSInterpolatorType::cubic4d;

  interpolatorType.toLower();

  auto result = types.find(interpolatorType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << "\"" << interpolatorType
	     << "\" is not a valid field type" << G4endl;

      G4cout << "Available interpolator types are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined interpolator type to be " << result->second << G4endl;
#endif
  return result->second;
}

G4int BDS::NDimensionsOfInterpolatorType(const BDSInterpolatorType& it)
{
  G4int result = 0;
  switch (it.underlying())
    {
      case BDSInterpolatorType::nearest1d:
      case BDSInterpolatorType::linear1d:
      case BDSInterpolatorType::cubic1d:
        {result = 1; break;}
      case BDSInterpolatorType::nearest2d:
      case BDSInterpolatorType::linear2d:
      case BDSInterpolatorType::cubic2d:
        {result = 2; break;}
      case BDSInterpolatorType::nearest3d:
      case BDSInterpolatorType::linear3d:
      case BDSInterpolatorType::cubic3d:
        {result = 3; break;}
      case BDSInterpolatorType::nearest4d:
      case BDSInterpolatorType::linear4d:
      case BDSInterpolatorType::cubic4d:
        {result = 4; break;}
      default:
        {result = 0; break;}
    }
  return result;
}
