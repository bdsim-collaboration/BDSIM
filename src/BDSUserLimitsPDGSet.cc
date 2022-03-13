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
#include "BDSUserLimitsPDGSet.hh"

#include "G4Track.hh"
#include "G4Types.hh"

#include <limits>
#include <map>

BDSUserLimitsPDGSet::BDSUserLimitsPDGSet(G4bool killAllIn,
                                         const std::map<G4int, G4double> pdgToEkMinimumIn):
  G4UserLimits(),
  killAll(killAllIn),
  pdgToEkMinimum(pdgToEkMinimumIn)
{;}

G4double BDSUserLimitsPDGSet::GetUserMinEkine(const G4Track& aTrack)
{
  if (killAll)
    {return std::numeric_limits<double>::max();}
  else
    {
      G4int pdgID = aTrack.GetDynamicParticle()->GetPDGcode();
      auto search = pdgToEkMinimum.find(pdgID);
      if (search != pdgToEkMinimum.end())
        {return search->second;}
      else
        {return 0;}
    }
}


