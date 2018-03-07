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
#include "BDSGlobalConstants.hh"
#include "BDSRunManager.hh"
#include "BDSStackingAction.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

BDSStackingAction::BDSStackingAction(const BDSGlobalConstants* globals)
{
  killNeutrinos     = globals->KillNeutrinos();
  stopSecondaries   = globals->StopSecondaries();
  maxTracksPerEvent = globals->MaximumTracksPerEvent();
  if (maxTracksPerEvent == 0) // 0 is default -> no action - set maximum possible number
    {maxTracksPerEvent = LONG_MAX;}
}

BDSStackingAction::~BDSStackingAction()
{;}

G4ClassificationOfNewTrack BDSStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
  G4ClassificationOfNewTrack classification = fUrgent;

#ifdef BDSDEBUG 
  G4cout<<"StackingAction: ClassifyNewtrack "<<aTrack->GetTrackID()<<
    " "<<aTrack->GetDefinition()->GetParticleName()<<G4endl;
  G4StackManager* SM = G4EventManager::GetEventManager()->GetStackManager();
  G4cout<<"N total tracks :     " << SM->GetNTotalTrack()     << G4endl;
  G4cout<<"N waiting tracks :   " << SM->GetNWaitingTrack()   << G4endl;
  G4cout<<"N urgent tracks :    " << SM->GetNUrgentTrack()    << G4endl;
  G4cout<<"N postponed tracks : " << SM->GetNPostponedTrack() << G4endl;
  G4cout<<"Events to process : "<<
    BDSRunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed()<<G4endl;
  G4cout<<"Number of event : "<<
    BDSRunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEvent()<<G4endl;
#endif

  // default -> no action
  if (aTrack->GetTrackID() > maxTracksPerEvent)
    {return fKill;}

  // Kill all neutrinos
  if(killNeutrinos)
    {
      G4int pdgNr = std::abs(aTrack->GetParticleDefinition()->GetPDGEncoding());
      if( pdgNr == 12 || pdgNr == 14 || pdgNr == 16)
	{return fKill;}
    }

  // kill secondaries
  if(stopSecondaries && (aTrack->GetParentID() > 0))
    {return fKill;}
    
  return classification;
}

void BDSStackingAction::NewStage()
{
  // urgent stack empty, looking into the waiting stack
 
#ifdef BDSDEBUG
  G4cout<<"StackingAction: New stage"<<G4endl;
#endif

  return;
}
    
void BDSStackingAction::PrepareNewEvent()
{;}


