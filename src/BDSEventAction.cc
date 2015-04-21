#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"
#include "BDSEventAction.hh"
#include "BDSOutputBase.hh" 
#include "BDSRunManager.hh"
#include "BDSTrajectory.hh"

#include <list>
#include <map>
#include <vector>
#include <algorithm>

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Run.hh"
#include "G4HCofThisEvent.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"

#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSSamplerHit.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSAnalysisManager.hh"

// #include "BDSLWCalorimeter.hh"
// #include "BDSLWCalorimeterHit.hh"

extern BDSOutputBase* bdsOutput;         // output interface

G4bool FireLaserCompton;  // bool to ensure that Laserwire can only occur once in an event

BDSEventAction::BDSEventAction():
  SamplerCollID_plane(-1),SamplerCollID_cylin(-1),
  Traj(NULL),trajEndPoint(NULL)
{ 
  verbose            = BDSExecOptions::Instance()->GetVerbose();
  verboseEvent       = BDSExecOptions::Instance()->GetVerboseEvent();
  verboseEventNumber = BDSExecOptions::Instance()->GetVerboseEventNumber();
  isBatch            = BDSExecOptions::Instance()->GetBatch();

  if(isBatch) printModulo=10;
  else printModulo=1;
}

BDSEventAction::~BDSEventAction()
{}

void BDSEventAction::BeginOfEventAction(const G4Event* evt)
{ 
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__ << " Processing begin of event action" << G4endl;
#endif

  G4int event_number = evt->GetEventID();

  if ((event_number+1)%printModulo ==0)
    {
      G4cout << "\n---> Begin of event: " << event_number << G4endl;
    }
  
  if(verboseEvent) G4cout << __METHOD_NAME__ << "event #"<<event_number<<G4endl ;
   
  if( BDSSampler::GetNSamplers() > 0)
    {   
      SamplerCollID_plane = G4SDManager::GetSDMpointer()->GetCollectionID("Sampler_plane");
    }
   
  if( BDSSamplerCylinder::GetNSamplers() > 0 )
    {   
      SamplerCollID_cylin = G4SDManager::GetSDMpointer()->GetCollectionID("Sampler_cylinder"); 
    }
   
  //if( bdsOutput->GetLWCalorimeterNumber() > 0 )
  //   {
  //if (LWCalorimeterCollID==-1) 
  //LWCalorimeterCollID = G4SDManager::GetSDMpointer()->GetCollectionID("LWCalorimeterCollection");
  //   }
  FireLaserCompton=true;
   
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "begin of event action done"<<G4endl;
#endif
}

void BDSEventAction::EndOfEventAction(const G4Event* evt)
{
#ifdef BDSDEBUG
  G4cout<<"BDSEventAction : processing end of event action"<<G4endl;
#endif

  G4int event_number = evt->GetEventID();
  if(verboseEvent || verboseEventNumber == event_number){
    G4cout << __METHOD_NAME__ << " processing end of event"<<G4endl;
  }
 
#ifdef BDSDEBUG 
  G4cout<<"BDSEventAction : storing hits"<<G4endl;
#endif 
  //Record the primary events
  AddPrimaryHits();
  
#ifdef BDSDEBUG 
  G4cout<<"BDSEventAction : processing planar hits collection"<<G4endl;
#endif
  
  BDSSamplerHitsCollection*  SampHC=NULL;
  if(SamplerCollID_plane>=0)
    SampHC = (BDSSamplerHitsCollection*)(evt->GetHCofThisEvent()->GetHC(SamplerCollID_plane));
  
  if(SampHC){
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << " - planar hits collection found. Writing hits." << G4endl;
#endif
    bdsOutput->WriteHits(SampHC);
  } else {
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << " - no planar hits collection found. Not writing hits." << G4endl;
#endif
  }
  SampHC=NULL;
  
  // are there any cylindrical samplers?
  // if so, record the hits

#ifdef BDSDEBUG
G4cout<<"BDSEventAction : processing cylinder hits collection"<<G4endl;
#endif

  if(SamplerCollID_cylin>=0)
    SampHC = (BDSSamplerHitsCollection*)(evt->GetHCofThisEvent()->GetHC(SamplerCollID_cylin));

  if (SampHC)  bdsOutput->WriteHits(SampHC);
  
  // are there any Laser wire calorimeters?
  // TODO : check it !!! at present not writing LW stuff
  // remember to uncomment LWCalHC above if using this
  //BDSLWCalorimeterHitsCollection* LWCalHC=NULL;
  // if(LWCalorimeterCollID>=0) 
  //   LWCalHC=(BDSLWCalorimeterHitsCollection*)(evt->GetHCofThisEvent()->GetHC(LWCalorimeterCollID));
  // if (LWCalHC) bdsOutput->WriteHits(SampHC);


  // create energy loss histogram
#ifdef BDSDEBUG 
  G4cout<<"BDSEventAction : storing energy loss histograms"<<G4endl;
#endif

  G4SDManager* mySDMan = G4SDManager::GetSDMpointer();
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  BDSEnergyCounterHitsCollection* energyCounterHits = 
    (BDSEnergyCounterHitsCollection*)(HCE->GetHC(mySDMan->GetCollectionID("energy_counter")));
  BDSEnergyCounterHitsCollection* primaryCounterHits = 
    (BDSEnergyCounterHitsCollection*)(HCE->GetHC(mySDMan->GetCollectionID("primary_counter")));

  //if we have energy deposition hits, write them
  if(energyCounterHits)
    {
      bdsOutput->WriteEnergyLoss(energyCounterHits); // write hits

      //bin hits in histograms
      BDSAnalysisManager* analMan = BDSAnalysisManager::Instance();
      for (G4int i = 0; i < energyCounterHits->entries(); i++)
	{
	  //general eloss histo
	  analMan->Fill1DHistogram(2,(*energyCounterHits)[i]->GetS()/CLHEP::m,(*energyCounterHits)[i]->GetEnergy()/CLHEP::GeV);
	  //per element eloss histo
	  analMan->Fill1DHistogram(5,(*energyCounterHits)[i]->GetS()/CLHEP::m,(*energyCounterHits)[i]->GetEnergy()/CLHEP::GeV);
	}
    }

  //if we have primary hits, find the first one and write that
  if(primaryCounterHits) {
    if (primaryCounterHits->entries()>0){
      BDSEnergyCounterHit* thePrimaryHit  = BDS::LowestSPosPrimaryHit(primaryCounterHits);
      BDSEnergyCounterHit* thePrimaryLoss = BDS::HighestSPosPrimaryHit(primaryCounterHits);
      //write
      if (thePrimaryHit && thePrimaryLoss)
	{
	  bdsOutput->WritePrimaryLoss(thePrimaryLoss);
	  bdsOutput->WritePrimaryHit(thePrimaryHit);
	  // general histos
	  BDSAnalysisManager::Instance()->Fill1DHistogram(0,thePrimaryHit->GetS()/CLHEP::m);
	  BDSAnalysisManager::Instance()->Fill1DHistogram(1,thePrimaryLoss->GetS()/CLHEP::m);
	  // per element histos
      	  BDSAnalysisManager::Instance()->Fill1DHistogram(3,thePrimaryHit->GetS()/CLHEP::m);
	  BDSAnalysisManager::Instance()->Fill1DHistogram(4,thePrimaryLoss->GetS()/CLHEP::m);
	}
    }
  }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " finished writing energy loss." << G4endl;
#endif
  
  // if events per ntuples not set (default 0) - only write out at end 
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " getting number of events per ntuple..." << G4endl;
#endif
  int evntsPerNtuple = BDSGlobalConstants::Instance()->GetNumberOfEventsPerNtuple();
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " finished getting number of events per ntuple." << G4endl;
#endif

  if (evntsPerNtuple>0 && (event_number+1)%evntsPerNtuple == 0)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " writing events." << G4endl;
#endif      
      bdsOutput->Commit(); // write and open new file
#ifdef BDSDEBUG
      G4cout<<"done"<<G4endl;
#endif
    }

  // needed to draw trajectories and hits:
  if(!isBatch) {
#ifdef BDSDEBUG 
    G4cout<<"BDSEventAction : drawing"<<G4endl;
#endif
    evt->Draw();
  }
    
  // Save interesting trajectories
  G4TrajectoryContainer* TrajCont=evt->GetTrajectoryContainer();
  if(!TrajCont) return;
  TrajectoryVector* TrajVec=TrajCont->GetVector();
  TrajectoryVector::iterator iT1;
  
  if(BDSGlobalConstants::Instance()->GetStoreTrajectory() ||
     BDSGlobalConstants::Instance()->GetStoreMuonTrajectories() ||
     BDSGlobalConstants::Instance()->GetStoreNeutronTrajectories()){
#ifdef BDSDEBUG
  G4cout<<"BDSEventAction : storing trajectories"<<G4endl;
#endif
    // clear out trajectories that don't reach point x
    for(iT1=TrajVec->begin();iT1<TrajVec->end();iT1++){
      this->Traj=(BDSTrajectory*)(*iT1);
      this->trajEndPoint = (BDSTrajectoryPoint*)this->Traj->GetPoint((int)Traj->GetPointEntries()-1);
      this->trajEndPointThreeVector = this->trajEndPoint->GetPosition();
      if(trajEndPointThreeVector.z()/1000.0>BDSGlobalConstants::Instance()->GetTrajCutGTZ()  && 
         (sqrt(pow(trajEndPointThreeVector.x()/1000.0,2) + pow(trajEndPointThreeVector.y()/1000.0,2))<BDSGlobalConstants::Instance()->GetTrajCutLTR())
         ){ 
        this->interestingTrajectories.push_back(Traj);
      }
      
    }
    //Output interesting trajectories
    if(interestingTrajectories.size()>0){
      bdsOutput->WriteTrajectory(interestingTrajectories);
      interestingTrajectories.clear();
    }
  }

  //clear out the remaining trajectories
#ifdef BDSDEBUG 
  G4cout<<"BDSEventAction : deleting trajectories"<<G4endl;
#endif
  //  TrajCont->clearAndDestroy();
#ifdef BDSDEBUG 
 G4cout<<"BDSEventAction : end of event action done"<<G4endl;
#endif
}

void BDSEventAction::AddPrimaryHits(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //Save the primary particle as a hit 
  G4PrimaryVertex* primaryVertex= BDSRunManager::GetRunManager()->GetCurrentEvent()->GetPrimaryVertex();
  G4PrimaryParticle* primaryParticle=primaryVertex->GetPrimary();
  G4ThreeVector momDir = primaryParticle->GetMomentumDirection();
  G4double E = primaryParticle->GetTotalEnergy();
  G4double xp = momDir.x();
  G4double yp = momDir.y();
  G4double zp = momDir.z();
  G4double x0 = primaryVertex->GetX0();
  G4double y0 = primaryVertex->GetY0();
  G4double z0 = primaryVertex->GetZ0();
  G4double t = primaryVertex->GetT0();
  G4double weight = primaryParticle->GetWeight();
  G4int PDGType=primaryParticle->GetPDGcode();
  G4int nEvent = BDSRunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  G4String samplerName="primaries";
  G4int turnstaken = BDSGlobalConstants::Instance()->GetTurnsTaken();
  bdsOutput->WritePrimary(samplerName, E, x0, y0, z0, xp, yp, zp, t, weight, PDGType, nEvent, turnstaken);

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " finished" << G4endl;
#endif
  
}
