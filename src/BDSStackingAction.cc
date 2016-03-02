#include "BDSGlobalConstants.hh"
#include "BDSPhotonCounter.hh"
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

BDSStackingAction::BDSStackingAction()
{;}

BDSStackingAction::~BDSStackingAction()
{;}

G4ClassificationOfNewTrack BDSStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
  G4ClassificationOfNewTrack classification = fUrgent;

#ifdef BDSDEBUG 
  G4cout<<"StackingAction: ClassifyNewtrack "<<aTrack->GetTrackID()<<
    " "<<aTrack->GetDefinition()->GetParticleName()<<G4endl;
  G4StackManager* SM = G4EventManager::GetEventManager()->GetStackManager();
  G4cout<<"N total tracks : "<<SM->GetNTotalTrack() << G4endl;
  G4cout<<"N waiting tracks : "<<SM->GetNWaitingTrack() << G4endl;
  G4cout<<"N urgent tracks : "<<SM->GetNUrgentTrack() << G4endl;
  G4cout<<"N postponed tracks : "<<SM->GetNPostponedTrack() << G4endl;
  G4cout<<"Events to process : "<<
    BDSRunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed()<<G4endl;
  G4cout<<"Number of event : "<<
    BDSRunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEvent()<<G4endl;
#endif

  //Kill all neutrinos
  G4bool killNeutrinos = true;
  if( killNeutrinos ){
    G4int pdgNr = aTrack->GetParticleDefinition()->GetPDGEncoding();
    if( abs(pdgNr) == 12 || abs(pdgNr) == 14 || abs(pdgNr) == 16) {
      return fKill;
    }
  }

  // kill secondaries
  if(BDSGlobalConstants::Instance()->GetStopSecondaries() && (aTrack->GetParentID() > 0) )
    {return fKill;}
  
  if(BDSGlobalConstants::Instance()->GetStopTracks()) // if tracks killed after interaction
    {
      // kill secondary electrons      
      if( (aTrack->GetParentID() > 0) && (aTrack->GetDefinition() == G4Electron::ElectronDefinition() ) ) {
	return fKill;
      }
      
      // kill secondary photons      
      if( (aTrack->GetParentID() > 0) && (aTrack->GetDefinition() == G4Gamma::GammaDefinition()) && !BDSGlobalConstants::Instance()->GetSynchRadOn())
	{
	  return fKill;
	}
      
      // kill secondary positrons
      if((aTrack->GetParentID() > 0) && (aTrack->GetDefinition() == G4Positron::PositronDefinition()))
	{
	  return fKill;
	}

      // kill secondary protons/antiprotons
      if( (aTrack->GetParentID() > 0) && 
	  ( (aTrack->GetDefinition() == G4Proton::ProtonDefinition() ) ||
	    (aTrack->GetDefinition() == G4AntiProton::AntiProtonDefinition()) ) )
	{
	  return fKill;
	}
    }

  return classification;
}

void BDSStackingAction::countPhoton(const G4Track* aTrack)
{
  BDSPhotonCounter::Instance()->countPhoton(aTrack);
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


