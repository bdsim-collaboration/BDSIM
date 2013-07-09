/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Removed StringFromInt function - using BDSGlobalConstants::Instance() version
   Added/Changed Sampler code for Plane Sampler or Cylinder Sampler (GABs Code)
*/


//======================================================
//======================================================
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 

#include "BDSEventAction.hh"

#include <ctime> 
#include <list>
#include <map>
#include <vector>
#include <algorithm>

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4StackManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"

#include "G4ChordFinder.hh"

#include "BDSSampler.hh"
#include "BDSSamplerHit.hh"
#include "BDSEnergyCounterHit.hh"

#include "BDSLWCalorimeter.hh"
#include "BDSLWCalorimeterHit.hh"

#include "BDSSynchrotronRadiation.hh"

#include "BDSAcceleratorComponent.hh"

#include "BDSOutput.hh"

#include "BDSRunManager.hh"

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::list<BDSEnergyCounterSD*>  ECList;
extern ECList* theECList;

extern G4double BDSeBremFireDist;
extern G4double BDSeBremZMin,BDSeBremZMax;

// link to G4eBremsstrahlung.cc and G4GammaConversion.cc for muon studies
extern G4bool BDSeBremsFiredThisEvent;
extern G4double BDSeBremFireDist;

G4double htot;
G4int event_number;
G4bool FireLaserCompton;

extern BDSOutput* bdsOutput;

//======================================================

BDSEventAction::BDSEventAction():
  SamplerCollID_plane(-1),SamplerCollID_cylin(-1),
  LWCalorimeterCollID(-1),drawFlag("all"),
  Traj(NULL),trajEndPoint(NULL)
{ 
  verbose            = BDSExecOptions::Instance()->GetVerbose();
  verboseStep        = BDSExecOptions::Instance()->GetVerboseStep();
  verboseEvent       = BDSExecOptions::Instance()->GetVerboseEvent();
  verboseEventNumber = BDSExecOptions::Instance()->GetVerboseEventNumber();
  isBatch            = BDSExecOptions::Instance()->GetBatch();
  nptwiss            = BDSExecOptions::Instance()->GetNPTwiss();

  if(isBatch) printModulo=10;
  else printModulo=1;
  
  itsOutputFileNumber=1;
  
  itsRecordSize=1024;
    
  LastComp=NULL;
}

//======================================================

BDSEventAction::~BDSEventAction()
{
//   delete Traj;
//   delete trajEndPoint;
}

//======================================================

void BDSEventAction::BeginOfEventAction(const G4Event* evt)
{ 
  G4cout<<"BDSEventAction::BeginOfEventAction>"<<G4endl;
#ifdef DEBUG
  G4cout<<"BDSEventAction : processing begin of event action"<<G4endl;
#endif

  event_number = evt->GetEventID();
  htot=0.;

  
   if(BDSGlobalConstants::Instance()->DoTwiss())
     {
       if(event_number==0) {
         if(!BDSGlobalConstants::Instance()->GetSynchRescale()) G4cout << "\n---> Calculating Twiss Parameters"<<G4endl;
         if(BDSGlobalConstants::Instance()->GetSynchRescale()) G4cout<<"\n---> Calculating Twiss Parameters and Rescaling magnets" <<G4endl;
       }
     }
   else
     {
       if (BDSGlobalConstants::Instance()->isReference==false && (event_number+1)%printModulo ==0)
         {
           G4cout << "\n---> Begin of event: " << event_number ;
           G4cout << G4endl;
         }
     }
   
   if(verboseEvent) G4cout<<"Begin of event: "<<event_number<<G4endl ;
   
   
   G4SDManager * SDman = G4SDManager::GetSDMpointer();
   if( BDSSampler::GetNSamplers() > 0)
     {   
       SamplerCollID_plane = SDman->GetCollectionID("Sampler_plane");
     }
   
   if( BDSSamplerCylinder::GetNSamplers() > 0 )
     {   
       SamplerCollID_cylin = SDman->GetCollectionID("Sampler_cylinder"); 
     }
   
   //if( bdsOutput->GetLWCalorimeterNumber() > 0 )
   {
     //if (LWCalorimeterCollID==-1) 
     //LWCalorimeterCollID = SDman->GetCollectionID("LWCalorimeterCollection");
   }
   FireLaserCompton=true;
   
#ifdef DEBUG
   G4cout<<"BDSEventAction : begin of event action done"<<G4endl;
#endif
}

//======================================================

void BDSEventAction::EndOfEventAction(const G4Event* evt)
{
#ifdef DEBUG
  G4cout<<"BDSEventAction : processing end of event action"<<G4endl;
#endif
  
  if(BDSGlobalConstants::Instance()->DoTwiss())
    {
      if(event_number==nptwiss-1)
	{
	  G4cout << "\n---> Done" <<G4endl;
	  G4EventManager::GetEventManager()->GetStackManager()->clear();
	}
    }

 
  if(verboseEvent || verboseEventNumber == event_number)
    G4cout<<"processing end of event"<<G4endl;

  G4SDManager * SDman = G4SDManager::GetSDMpointer();

  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  
  BDSSamplerHitsCollection*  SampHC=NULL;
  //BDSLWCalorimeterHitsCollection* LWCalHC=NULL;
  BDSEnergyCounterHitsCollection* BDSEnergyCounter_HC=NULL;

#ifdef DEBUG 
  G4cout<<"BDSEventAction : storing hits"<<G4endl;
#endif

  // are there any planar samplers?
  // if so, record the hits for each sampler 

#ifdef DEBUG 
  G4cout<<"BDSEventAction : processing planar hits collection"<<G4endl;
#endif
  
  if(SamplerCollID_plane>=0)
    SampHC = (BDSSamplerHitsCollection*)(HCE->GetHC(SamplerCollID_plane));

  if(SampHC)bdsOutput->WriteHits(SampHC);

  SampHC=NULL;

  // are there any cylindrical samplers?
  // if so, record the hits

#ifdef DEBUG
G4cout<<"BDSEventAction : processing cylinder hits collection"<<G4endl;
#endif

  if(SamplerCollID_cylin>=0)
    SampHC = (BDSSamplerHitsCollection*)(HCE->GetHC(SamplerCollID_cylin));

  if (SampHC)  bdsOutput->WriteHits(SampHC);
  
  // are there any Laser wire calorimeters?
  // TODO : check it !!! at present not writing LW stuff
  // remember to uncomment LWCalHC above if using this

  // #ifdef DEBUG 
  //  G4cout<<"BDSEventAction : processing laserwire calorimeter hits collection"<<G4endl;
  //#endif
  // if(LWCalorimeterCollID>=0) 
  //   LWCalHC=(BDSLWCalorimeterHitsCollection*)(HCE->GetHC(LWCalorimeterCollID));

  // if (LWCalHC) bdsOutput->WriteHits(SampHC);


  // create energy loss histogram

#ifdef DEBUG 
  G4cout<<"BDSEventAction : storing energy loss histograms"<<G4endl;
#endif

#if 1
  for(iEC=theECList->begin();iEC!=theECList->end();iEC++)
    {
      G4String name=(*iEC)->GetCollectionName(0);
      
      G4int BDSEnergyCounter_ID= SDman->GetCollectionID(name);
      
      if(BDSEnergyCounter_ID>=0)
	{
	  BDSEnergyCounter_HC=
	    (BDSEnergyCounterHitsCollection*)(HCE->GetHC(BDSEnergyCounter_ID));
	
	  if(BDSEnergyCounter_HC) 
	    bdsOutput->WriteEnergyLoss(BDSEnergyCounter_HC);
	}
    }
#endif

  // if events per ntuples not set (default 0) - only write out at end 
  int evntsPerNtuple = BDSGlobalConstants::Instance()->GetNumberOfEventsPerNtuple();
  if( (evntsPerNtuple>0 && (event_number+1)%evntsPerNtuple == 0) || 
	(event_number+1) == BDSGlobalConstants::Instance()->GetNumberToGenerate())
    {
#ifdef DEBUG 
      G4cout<<"writing to file "<<G4endl;
#endif
      // notify the output about the event end
      // this can be used for splitting output files etc.
      //	bdsOutput->Commit(itsOutputFileNumber++);
      if((event_number+1) == BDSGlobalConstants::Instance()->GetNumberToGenerate()) {
	bdsOutput->Write(); // write last file
      } else {
	bdsOutput->Commit(); // write and open new file
      }
#ifdef DEBUG
      G4cout<<"done"<<G4endl;
#endif
    }
  
  // Save interesting trajectories
  
  G4TrajectoryContainer* TrajCont=evt->GetTrajectoryContainer();

  if(!TrajCont) return;
  
  TrajectoryVector* TrajVec=TrajCont->GetVector();
  TrajectoryVector::iterator iT1;

  
  if(BDSGlobalConstants::Instance()->GetStoreTrajectory() ||
     BDSGlobalConstants::Instance()->GetStoreMuonTrajectories() ||
     BDSGlobalConstants::Instance()->GetStoreNeutronTrajectories()){
#ifdef DEBUG
  G4cout<<"BDSEventAction : storing trajectories"<<G4endl;
#endif
    // clear out trajectories that don't reach point x
    for(iT1=TrajVec->begin();iT1<TrajVec->end();iT1++){
      this->Traj=(G4VTrajectory*)(*iT1);
      this->trajEndPoint = this->Traj->GetPoint((int)Traj->GetPointEntries()-1);
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

  // needed to draw trajectories and hits:
  if(!isBatch) {
#ifdef DEBUG 
    G4cout<<"BDSEventAction : drawing"<<G4endl;
#endif
    evt->Draw();
  }
  
  //clear out the remaining trajectories
#ifdef DEBUG 
  G4cout<<"BDSEventAction : deleting trajectories"<<G4endl;
#endif
  TrajCont->clearAndDestroy();
#ifdef DEBUG 
 G4cout<<"BDSEventAction : end of event action done"<<G4endl;
#endif
  }

//======================================================
