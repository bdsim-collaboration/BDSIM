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
#include "BDSUserSpecialCutsParallel.hh"

#include "G4ForceCondition.hh"
#include "G4LogicalVolume.hh"
#include "G4TouchableHandle.hh"
#include "G4TransportationProcessType.hh"
#include "G4UserLimits.hh"
#include "G4VParticleChange.hh"
#include "G4VPhysicalVolume.hh"

#include <limits>

BDSUserSpecialCutsParallel::BDSUserSpecialCutsParallel(const G4String& nameIn):
  G4ParallelWorldProcess(nameIn)
{
  SetProcessSubType(static_cast<int>(G4TransportationProcessType::USER_SPECIAL_CUTS));
}
 
G4double BDSUserSpecialCutsParallel::PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
                                                                          G4double /*previousStepSize*/,
                                                                          G4ForceCondition* condition)
{
  *condition = G4ForceCondition::StronglyForced;
  
  G4double proposedStep = std::numeric_limits<double>::max();
  G4UserLimits* userLimits = fOldGhostTouchable->GetVolume()->GetLogicalVolume()->GetUserLimits();
  if (userLimits)
  {
    auto bdsUserLimitsPDG = dynamic_cast<BDSUserLimitsPDGSet*>(userLimits);
    if (bdsUserLimitsPDG)
    {
      if (aTrack.GetKineticEnergy() <= bdsUserLimitsPDG->GetUserMinEkine(aTrack))
        {proposedStep = 0;}
    }
  }
   return proposedStep;
}

G4VParticleChange* BDSUserSpecialCutsParallel::PostStepDoIt(const G4Track& aTrack,
							    const G4Step& aStep)
{
  auto pChange = G4ParallelWorldProcess::PostStepDoIt(aTrack, aStep);
  pChange->ProposeLocalEnergyDeposit(aTrack.GetKineticEnergy());
  pChange->ProposeTrackStatus(fStopAndKill);
  return pChange;
}












