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
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSLinkStackingAction.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSRunManager.hh"


#include "G4Run.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4Types.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Version.hh"


G4double BDSLinkStackingAction::energyKilled = 0;

BDSLinkStackingAction::BDSLinkStackingAction(const BDSGlobalConstants* globals,
const std::set<G4int> pdgIDsToAllowIn,
G4bool protonsAndIonsOnlyIn):
pdgIDsToAllow(pdgIDsToAllowIn),
protonsAndIonsOnly(protonsAndIonsOnlyIn)
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
  G4ClassificationOfNewTrack classification = fUrgent;

  // If beyond max number of tracks, kill it
  if (aTrack->GetTrackID() > maxTracksPerEvent)
    {classification = fKill;}

  // kill secondaries
  if (stopSecondaries && (aTrack->GetParentID() > 0))
    {classification = fKill;}

  auto definition = aTrack->GetDefinition();
  G4int pdgID = definition->GetPDGEncoding();

  // Kill all neutrinos
  if (killNeutrinos)
    {
      if (pdgID == 12 || pdgID == 14 || pdgID == 16)
        {classification = fKill;}
    }

  if (protonsAndIonsOnly)
    {
      if (!BDS::IsIon(definition))
        {classification = fKill;}
      if (pdgID == 2212)
        {classification = fUrgent;}
    }
  else if (!pdgIDsToAllow.empty())
     {
        if (pdgIDsToAllow.find(pdgID) != pdgIDsToAllow.end())
          {classification = fKill;}
     }
  return classification;
}


