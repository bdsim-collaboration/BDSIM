//  
//   BDSIM, (C) 2001-2006 
//    
//   version 0.2 
//   last modified : 28 Mar 2006 by agapov@pp.rhul.ac.uk
//  



//
//    Stacking action - taken when secondaries created
//


#include "BDSGlobalConstants.hh"
#include "BDSStackingAction.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ios.hh"

BDSStackingAction::BDSStackingAction()
{ 
}

BDSStackingAction::~BDSStackingAction()
{
}

G4ClassificationOfNewTrack BDSStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
  G4ClassificationOfNewTrack classification = fUrgent;


#ifdef DEBUG 
  G4cout<<"StackingAction: ClassifyNewtrack "<<aTrack->GetTrackID()<<
    " "<<aTrack->GetDefinition()->GetParticleName()<<G4endl;
  G4StackManager* SM = G4EventManager::GetEventManager()->GetStackManager();
  G4cout<<"N total tracks : "<<SM->GetNTotalTrack() << G4endl;
  G4cout<<"N waiting tracks : "<<SM->GetNWaitingTrack() << G4endl;
  G4cout<<"N urgent tracks : "<<SM->GetNUrgentTrack() << G4endl;
  G4cout<<"N postponed tracks : "<<SM->GetNPostponedTrack() << G4endl;
  G4cout<<"Events to process : "<<
    G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed()<<G4endl;
  G4cout<<"Number of event : "<<
    G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEvent()<<G4endl;
#endif
  
  if(BDSGlobalConstants::Instance()->DoTwiss())
    {
      if((aTrack->GetParentID() <= 0) &&
         (aTrack->GetTrackStatus()==fPostponeToNextEvent) )
	classification = fPostpone;
    }
  
  if(BDSGlobalConstants::Instance()->GetStopTracks()) // if tracks killed after interaction
    {
      
      // kill secondary electrons
      
      if( (aTrack->GetParentID() > 0) && 
	  (aTrack->GetDefinition() == G4Electron::ElectronDefinition() ) )
	{
          
	  
	  classification = fKill;
	  //classification = fUrgent;

	  // if we are in the twiss module - aperture hit is suspicious
	  if( BDSGlobalConstants::Instance()->DoTwiss() ) 
	    G4cout<<"WARNING : Electron "<<aTrack->GetParentID()<<" outside of aperture, twiss results will be incorrect"<<
	      G4endl;

	}
      
      // kill secondary photons
      
      if( (aTrack->GetParentID() > 0) && 
	  (aTrack->GetDefinition() == G4Gamma::GammaDefinition() ) )
	{
	  classification = fKill;
	}
      
      // kill secondary positrons
      
      if( (aTrack->GetParentID() > 0) && 
	  (aTrack->GetDefinition() == G4Positron::PositronDefinition() ) )
	{
	  classification = fKill;

	  // if we are in the twiss module - aperture hit is suspicious
	  if( BDSGlobalConstants::Instance()->DoTwiss() ) 
	    G4cout<<"WARNING : Positron outside of aperture, twiss results will be incorrect"<<
	      G4endl;
	}

      // kill secondary protons/antiprotons
      
      if( (aTrack->GetParentID() > 0) && 
	  ( (aTrack->GetDefinition() == G4Proton::ProtonDefinition() ) ||
	    (aTrack->GetDefinition() == G4AntiProton::AntiProtonDefinition()) ) )
	{
	  classification = fKill;
	  
	  // if we are in the twiss module - aperture hit is suspicious
	  if( BDSGlobalConstants::Instance()->DoTwiss() ) 
	    G4cout<<"WARNING : Proton outside of aperture, twiss results will be incorrect"<<
	      G4endl;
	}
      
    }

  if(BDSGlobalConstants::Instance()->getWaitingForDump()) // if waiting for placet synchronization
    {
       // when waiting to synchronize with placet - put on postponed stack
      if( aTrack->GetTrackStatus()==fPostponeToNextEvent )
	classification = fPostpone;
     }
  
  if(BDSGlobalConstants::Instance()->getDumping()) // in the process of dumping
    {
#ifdef DEBUG
      G4cout<<"reclassifying track "<<aTrack->GetTrackID()<<G4endl;
      G4cout<<"r= "<<aTrack->GetPosition()<<G4endl;
#endif
      
      G4AffineTransform tf = BDSGlobalConstants::Instance()->GetDumpTransform().Inverse();
      G4ThreeVector initialPos=aTrack->GetPosition();
      G4ThreeVector momDir=aTrack->GetMomentumDirection().unit();
      G4ThreeVector transformedPos = initialPos;
      G4ThreeVector LocalPosition=tf.TransformPoint(transformedPos);
      G4ThreeVector LocalDirection=tf.TransformAxis(momDir);
      
#ifdef DEBUG
      G4cout << "Stacking: Pos = " << transformedPos << G4endl;
      G4cout << "LocalPos: Pos = " << LocalPosition << G4endl;
      G4cout << "Stacking: mom = " << momDir << G4endl;
      G4cout << "LocalDir: mom = " << LocalDirection << G4endl;
#endif
      
      G4double x=LocalPosition.x()/CLHEP::micrometer;
      G4double y=LocalPosition.y()/CLHEP::micrometer;
      G4double z=LocalPosition.z()/CLHEP::micrometer;
      G4double xPrime=LocalDirection.x()/(1e-6*CLHEP::radian);
      G4double yPrime=LocalDirection.y()/(1e-6*CLHEP::radian);
      G4double t=aTrack->GetGlobalTime();
      
      BDSGlobalConstants::Instance()->fileDump.precision(15);
      // TODO : dump the file
      //        BDSGlobalConstants::Instance()->fileDump << aTrack->GetTotalEnergy()/CLHEP::GeV << "\t"
      //<< x << "\t" << y << "\t" << z << "\t"
      //<< xPrime << "\t" << yPrime << "\t" << t <<"\n"; // SPM
#ifdef DEBUG
      printf("Out: %.15f %.15f %.15f %.15f %.15f %.15f %f\n",
	     aTrack->GetTotalEnergy()/CLHEP::GeV,x,y,z,xPrime,yPrime,t);
#endif
      tmpParticle outputParticle, transformedParticle;
      if(aTrack->GetDefinition()->GetPDGEncoding()==-11)
	outputParticle.E=-(aTrack->GetTotalEnergy());
      else outputParticle.E=aTrack->GetTotalEnergy();
      outputParticle.xp=momDir.x();
      outputParticle.yp=momDir.y();
      outputParticle.x=initialPos.x();
      outputParticle.y=initialPos.y();
      outputParticle.z=initialPos.z();
      outputParticle.t=t;
      outputParticle.trackID=aTrack->GetTrackID();
      outputParticle.parentID=aTrack->GetParentID();
      
      transformedParticle.x=x;
      transformedParticle.y=y;
      transformedParticle.z=z;
      transformedParticle.xp=xPrime;
      transformedParticle.yp=yPrime;
      transformedParticle.t=t;
      transformedParticle.E=aTrack->GetTotalEnergy()/CLHEP::GeV;
      transformedParticle.parentID=aTrack->GetParentID();
      
      BDSGlobalConstants::Instance()->outputQueue.push_back(outputParticle);
      BDSGlobalConstants::Instance()->transformedQueue.push_back(transformedParticle);
      classification = fPostpone;
    }
  
  if(BDSGlobalConstants::Instance()->getReading()){
    classification = fWaiting_1;
  }

  //For improvement in efficiency 
  //  if(aTrack->GetNextVolume() != aTrack->GetVolume()) classification = fWaiting; //Track all particles in same volume first
  //  if(aTrack->GetTrackID()!=1) classification = fWaiting_1;  //Not a secondary
  //  if(aTrack->GetTotalEnergy()<0.1*BDSGlobalConstants::Instance()->GetBeamTotalEnergy()) classification = fWaiting_2;  //Below certain thresholds
  //  if(aTrack->GetTotalEnergy()<0.01*BDSGlobalConstants::Instance()->GetBeamTotalEnergy()) classification = fWaiting_3; 
  //  if(aTrack->GetTotalEnergy()<0.001*BDSGlobalConstants::Instance()->GetBeamTotalEnergy()) classification = fWaiting_4; 
  
  return classification;
}


void BDSStackingAction::NewStage()
{
  // urgent stack empty, looking into the waiting stack
 
#ifdef DEBUG
  G4cout<<"StackingAction: New stage"<<G4endl;
#endif

  return;
 
}
    
void BDSStackingAction::PrepareNewEvent()
{ 
}


