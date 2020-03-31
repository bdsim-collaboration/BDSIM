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
#include "BDSGlobalConstants.hh"
#include "BDSLinkStackingAction.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSRunManager.hh"

#include "G4Event.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4Types.hh"
#include "G4ParticleDefinition.hh"

#include <set>

G4double BDSLinkStackingAction::kineticEnergyKilled = 0;

BDSLinkStackingAction::BDSLinkStackingAction(const BDSGlobalConstants* globals,
                                             const std::set<G4int>&    pdgIDsToAllowIn,
                                             G4bool                    protonsAndIonsOnlyIn,
                                             G4double                  minimumEKIn):
  pdgIDsToAllow(pdgIDsToAllowIn),
  protonsAndIonsOnly(protonsAndIonsOnlyIn),
  minimumEK(minimumEKIn)
{
  killNeutrinos     = globals->KillNeutrinos();
  stopSecondaries   = globals->StopSecondaries();
  maxTracksPerEvent = globals->MaximumTracksPerEvent();
  if (maxTracksPerEvent == 0) // 0 is default -> no action - set maximum possible number
    {maxTracksPerEvent = LONG_MAX;}
}

BDSLinkStackingAction::~BDSLinkStackingAction()
{;}

G4ClassificationOfNewTrack BDSLinkStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
  G4ClassificationOfNewTrack result = fUrgent;
  
  // If beyond max number of tracks, kill it
  if (aTrack->GetTrackID() > maxTracksPerEvent)
    {result = fKill;}

  if (aTrack->GetKineticEnergy() <= minimumEK)
    {result = fKill;}

  // kill secondaries
  if (stopSecondaries && (aTrack->GetParentID() > 0))
    {result = fKill;}

  auto definition = aTrack->GetDefinition();
  G4int pdgID = definition->GetPDGEncoding();

  // Kill all neutrinos
  if (killNeutrinos)
    {
      if (pdgID == 12 || pdgID == 14 || pdgID == 16)
        {result = fKill;}
    }

  if (protonsAndIonsOnly)
    {
      if (!BDS::IsIon(definition) && pdgID != 2212)
        {result = fKill;}
      else if (pdgID == 2212)
        {result = fUrgent;}
    }
  else if (!pdgIDsToAllow.empty())
     {
        if (pdgIDsToAllow.find(pdgID) != pdgIDsToAllow.end())
          {result = fKill;}
     }

  if (result == fKill)
    {kineticEnergyKilled += aTrack->GetKineticEnergy() * aTrack->GetWeight();}
  return result;
}


