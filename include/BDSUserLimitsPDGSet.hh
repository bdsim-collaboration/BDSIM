/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#ifndef BDSUSERLIMITSPDGSET_H
#define BDSUSERLIMITSPDGSET_H
#include "G4Types.hh"
#include "G4UserLimits.hh"

#include <map>

class G4Track;

/**
 * @brief User limits where the Ek Minimum depends on the PDG ID.
 * 
 * @author Laurie Nevay
 */

class BDSUserLimitsPDGSet: public G4UserLimits
{
public:
  BDSUserLimitsPDGSet(G4bool killAllIn,
		      const std::map<G4int, G4double> pdgToEkMinimumIn);
  
  virtual ~BDSUserLimitsPDGSet() = default;

  /// Replace this function alone where we return the minimum Ek based on the PDG ID.
  virtual G4double GetUserMinEkine(const G4Track& aTrack);

protected:
  G4bool killAll;
  std::map<G4int, G4double> pdgToEkMinimum;
};

#endif
