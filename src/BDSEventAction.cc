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
#include "BDSDebug.hh"
#include "BDSEventAction.hh"
#include "BDSCCDPixelSD.hh"
#include "BDSCCDPixelHit.hh"

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
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"
#include "G4ChordFinder.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4Version.hh"

#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSSamplerHit.hh"
#include "BDSEnergyCounterHit.hh"

// #include "BDSLWCalorimeter.hh"
// #include "BDSLWCalorimeterHit.hh"

#include "BDSSynchrotronRadiation.hh"

#include "BDSAcceleratorComponent.hh"

#include "BDSOutput.hh"

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::list<BDSEnergyCounterSD*>  ECList;
extern ECList* theECList;

G4int event_number; // event number, used for checking on printing verboseEventNumber
G4bool FireLaserCompton;  // bool to ensure that Laserwire can only occur once in an event

extern BDSOutput* bdsOutput;

//======================================================

BDSEventAction::BDSEventAction():
  SamplerCollID_plane(-1),SamplerCollID_cylin(-1),
  drawFlag("all"),Traj(NULL),trajEndPoint(NULL)
{ 
  verbose            = BDSExecOptions::Instance()->GetVerbose();
  verboseStep        = BDSExecOptions::Instance()->GetVerboseStep();
  verboseEvent       = BDSExecOptions::Instance()->GetVerboseEvent();
  verboseEventNumber = BDSExecOptions::Instance()->GetVerboseEventNumber();
  isBatch            = BDSExecOptions::Instance()->GetBatch();

  if(isBatch) printModulo=10;
  else printModulo=1;
  
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
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__ << " processing begin of event action" << G4endl;
#endif

  event_number = evt->GetEventID();
  
  if(BDSGlobalConstants::Instance()->DoTwiss())
    {
      if(event_number==0) {
	G4cout << "\n---> Calculating Twiss Parameters";
	if(BDSGlobalConstants::Instance()->GetSynchRescale())
	  G4cout<<" and Rescaling magnets" <<G4endl;
      }
    }
  else
    {
      if (BDSGlobalConstants::Instance()->isReference==false && (event_number+1)%printModulo ==0)
	{
	  G4cout << "\n---> Begin of event: " << event_number << G4endl;
	}
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
   
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << "begin of event action done"<<G4endl;
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
      if(event_number==BDSExecOptions::Instance()->GetNPTwiss()-1)
	{
	  G4cout << "\n---> Done" <<G4endl;
	  G4EventManager::GetEventManager()->GetStackManager()->clear();
	}
    }
  if(verboseEvent || verboseEventNumber == event_number){
    G4cout << __METHOD_NAME__ << " processing end of event"<<G4endl;
  }
  
#ifdef DEBUG 
  G4cout<<"BDSEventAction : storing hits"<<G4endl;
#endif


  //Record the primary events
  AddPrimaryHits(evt);
  
  // are there any planar samplers?
  // if so, record the hits for each sampler 
  
#ifdef DEBUG 
  G4cout<<"BDSEventAction : processing planar hits collection"<<G4endl;
#endif
  
  BDSSamplerHitsCollection*  SampHC=NULL;
  if(SamplerCollID_plane>=0)
    SampHC = (BDSSamplerHitsCollection*)(evt->GetHCofThisEvent()->GetHC(SamplerCollID_plane));
  
  if(SampHC){
#ifdef DEBUG
    G4cout << __METHOD_NAME__ << " - planar hits collection found. Writing hits." << G4endl;
#endif
    bdsOutput->WriteHits(SampHC);
  } else {
#ifdef DEBUG
    G4cout << __METHOD_NAME__ << " - no planar hits collection found. Not writing hits." << G4endl;
#endif
  }

  SampHC=NULL;

  // are there any cylindrical samplers?
  // if so, record the hits

#ifdef DEBUG
G4cout<<"BDSEventAction : processing cylinder hits collection"<<G4endl;
#endif

  if(SamplerCollID_cylin>=0)
    SampHC = (BDSSamplerHitsCollection*)(evt->GetHCofThisEvent()->GetHC(SamplerCollID_cylin));

  if (SampHC)  bdsOutput->WriteHits(SampHC);
  
  // are there any Laser wire calorimeters?
  // TODO : check it !!! at present not writing LW stuff
  // remember to uncomment LWCalHC above if using this

  // 
  //  G4cout<<"BDSEventAction : processing laserwire calorimeter hits collection"<<G4endl;
  //
  //BDSLWCalorimeterHitsCollection* LWCalHC=NULL;
  // if(LWCalorimeterCollID>=0) 
  //   LWCalHC=(BDSLWCalorimeterHitsCollection*)(evt->GetHCofThisEvent()->GetHC(LWCalorimeterCollID));
  // if (LWCalHC) bdsOutput->WriteHits(SampHC);


  // create energy loss histogram

#ifdef DEBUG 
  G4cout<<"BDSEventAction : storing energy loss histograms"<<G4endl;
#endif
  
  BDSEnergyCounterHitsCollection* BDSEnergyCounter_HC=NULL;
  for(iEC=theECList->begin();iEC!=theECList->end();++iEC)
    {
      G4String name=(*iEC)->GetCollectionName(0);
      
      G4int BDSEnergyCounter_ID= G4SDManager::GetSDMpointer()->GetCollectionID(name);
      
      if(BDSEnergyCounter_ID>=0)
	{
	  BDSEnergyCounter_HC=
	    (BDSEnergyCounterHitsCollection*)(evt->GetHCofThisEvent()->GetHC(BDSEnergyCounter_ID));
	
	  if(BDSEnergyCounter_HC) {
	    bdsOutput->WriteEnergyLoss(BDSEnergyCounter_HC);
	  }
	}
    }
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " finished writing energy loss." << G4endl;
#endif



#ifdef DEBUG 
  G4cout<<"BDSEventAction : CCD camera hits histograms"<<G4endl;
#endif
  
  BDSCCDPixelHitsCollection* BDSCCDPixel_HC=NULL;
  G4String name="CCDPixel";
  G4int BDSCCDPixel_ID= -1;
  //G4SDManager::GetSDMpointer()->GetCollectionID(name);
  if(BDSCCDPixel_ID>=0)
    {
      BDSCCDPixel_HC=
	(BDSCCDPixelHitsCollection*)(evt->GetHCofThisEvent()->GetHC(BDSCCDPixel_ID));
      
      if(BDSCCDPixel_HC) {
#ifdef DEBUG
	G4cout << __METHOD_NAME__ << " - writing CCD hits..." << G4endl;
#endif
	bdsOutput->WriteCCDHits(BDSCCDPixel_HC);
#ifdef DEBUG
	G4cout << __METHOD_NAME__ << " - finished writing CCD hits " << G4endl;
#endif
      }
    }
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " finished writing energy loss." << G4endl;
#endif
  
  
  // if events per ntuples not set (default 0) - only write out at end 
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " getting number of events per ntuple..." << G4endl;
#endif
  int evntsPerNtuple = BDSGlobalConstants::Instance()->GetNumberOfEventsPerNtuple();
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " finished getting number of events per ntuple." << G4endl;
#endif

  // if doTwiss write out at end
  if( BDSGlobalConstants::Instance()->DoTwiss() && (event_number==BDSExecOptions::Instance()->GetNPTwiss()-1)) {
    bdsOutput->Commit(); // write and open new file
  }
  
  if( !BDSGlobalConstants::Instance()->DoTwiss() && 
      (
       (evntsPerNtuple>0 && (event_number+1)%evntsPerNtuple == 0) || 
       (event_number+1) == BDSGlobalConstants::Instance()->GetNumberToGenerate()
       )
      )
    {
#ifdef DEBUG
      G4cout << __METHOD_NAME__ << " writing out events." << G4endl;
#endif

#ifdef DEBUG 
      G4cout<<"writing to file "<<G4endl;
#endif
      // notify the output about the event end
      // this can be used for splitting output files etc.
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

void BDSEventAction::AddPrimaryHits(const G4Event* /*evt*/){
#ifdef DEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //Save the primary particle as a hit 
  G4PrimaryVertex* primaryVertex= G4RunManager::GetRunManager()->GetCurrentEvent()->GetPrimaryVertex();
  G4PrimaryParticle* primaryParticle=primaryVertex->GetPrimary();
#if G4VERSION_NUMBER > 950
  G4ThreeVector momDir = primaryParticle->GetMomentumDirection();
  G4double E = primaryParticle->GetTotalEnergy();
  G4double xp = momDir.x();
  G4double yp = momDir.y();
  G4double zp = momDir.z();
#else
  // implementation for older G4 versions to be fixed!
  G4double E=0.0, xp=0.0, yp=0.0, zp=0.0;
#endif
  G4double x0 = primaryVertex->GetX0();
  G4double y0 = primaryVertex->GetY0();
  G4double z0 = primaryVertex->GetZ0();
  G4double t = primaryVertex->GetT0();
  G4double weight = primaryParticle->GetWeight();
  G4int PDGType=primaryParticle->GetPDGcode();
  G4int nEvent = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  G4String samplerName="primaries";
  bdsOutput->WritePrimary(samplerName, E, x0, y0, z0, xp, yp, zp, t, weight, PDGType, nEvent);

#ifdef DEBUG
  G4cout << __METHOD_NAME__ << " finished" << G4endl;
#endif
}


//======================================================
