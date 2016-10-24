#include "BDSAnalysisManager.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSEventAction.hh"
#include "BDSEventInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutputBase.hh"
#include "BDSSamplerHit.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDManager.hh"
#include "BDSTrajectory.hh"

#include "globals.hh"                  // geant4 types / globals
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4Run.hh"
#include "G4SDManager.hh"
#include "G4TrajectoryContainer.hh"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <string>
#include <vector>

using namespace std::chrono;

extern BDSOutputBase* bdsOutput;       // output interface

G4bool FireLaserCompton;  // bool to ensure that Laserwire can only occur once in an event

BDSEventAction::BDSEventAction():
  samplerCollID_plane(-1),
  samplerCollID_cylin(-1),
  energyCounterCollID(-1),
  tunnelEnergyCounterCollID(-1),
  startTime(0),
  stopTime(0),
  starts(0),
  stops(0),
  seedStateAtStart(""),
  eventInfo(nullptr)
{
  verboseEvent       = BDSGlobalConstants::Instance()->VerboseEvent();
  verboseEventNumber = BDSGlobalConstants::Instance()->VerboseEventNumber();
  isBatch            = BDSGlobalConstants::Instance()->Batch();

  if(isBatch)
    {
      G4int nGenerate = BDSGlobalConstants::Instance()->NGenerate();
      G4double fraction = BDSGlobalConstants::Instance()->PrintModuloFraction();
      printModulo = (G4int)ceil(nGenerate * fraction);
      if (printModulo < 0)
	{printModulo = 1;}
    }
  else
    {printModulo=1;}
}

BDSEventAction::~BDSEventAction()
{}

void BDSEventAction::BeginOfEventAction(const G4Event* evt)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "processing begin of event action" << G4endl;
#endif
  // update reference to event info
  eventInfo = static_cast<BDSEventInfo*>(evt->GetUserInformation());

  // get the current time
  startTime = time(nullptr);
  eventInfo->SetStartTime(startTime);
  eventInfo->SetStopTime(startTime); // initialise to duration of 0

  milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  starts = (G4double)ms.count()/1000.0;

  // number feedback
  G4int event_number = evt->GetEventID();
  if (event_number%printModulo == 0)
    {G4cout << "---> Begin of event: " << event_number << G4endl;}
  if(verboseEvent)
    {G4cout << __METHOD_NAME__ << "event #" << event_number << G4endl;}

  // cache hit collection IDs for quicker access
  if(samplerCollID_plane < 0)
    { // if one is -1 then all need initialised.
      G4SDManager*  g4SDMan  = G4SDManager::GetSDMpointer();
      BDSSDManager* bdsSDMan = BDSSDManager::Instance();
      samplerCollID_plane       = g4SDMan->GetCollectionID(bdsSDMan->GetSamplerPlaneSD()->GetName());
      samplerCollID_cylin       = g4SDMan->GetCollectionID(bdsSDMan->GetSamplerCylinderSD()->GetName());
      energyCounterCollID       = g4SDMan->GetCollectionID(bdsSDMan->GetEnergyCounterSD()->GetName());
      tunnelEnergyCounterCollID = g4SDMan->GetCollectionID(bdsSDMan->GetEnergyCounterTunnelSD()->GetName());
    }
  FireLaserCompton=true;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "begin of event action done" << G4endl;
#endif
}

void BDSEventAction::EndOfEventAction(const G4Event* evt)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "processing end of event action" << G4endl;
#endif
  // Get the current time
  stopTime = time(nullptr);
  eventInfo->SetStopTime(stopTime);
  
  milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  stops = (G4double)ms.count()/1000.0;
  eventInfo->SetDuration(stops - starts);

  // Record timing in output
  bdsOutput->WriteEventInfo(eventInfo);

  // Get the hits collection of this event - all hits from different SDs.
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();

  // Get event number information
  G4int event_number = evt->GetEventID();
  if(verboseEvent || verboseEventNumber == event_number)
    {G4cout << __METHOD_NAME__ << "processing end of event"<<G4endl;}

  // Record the primary vertex in output
  WritePrimaryVertex(event_number, evt->GetPrimaryVertex());

  // Now process each of the hits collections in turn, writing them to output.
  // After this, fill the appropriate histograms with information from this event.

  // samplers
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "processing sampler hits collection" << G4endl;
#endif
  BDSSamplerHitsCollection* SampHC = nullptr;
  if(samplerCollID_plane >= 0)
    {SampHC = (BDSSamplerHitsCollection*)(evt->GetHCofThisEvent()->GetHC(samplerCollID_plane));}
  if(SampHC)
    {bdsOutput->WriteHits(SampHC);}

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "processing cylinder hits collection" << G4endl;
#endif
  SampHC = nullptr;
  if(samplerCollID_cylin >= 0)
    {SampHC = (BDSSamplerHitsCollection*)(HCE->GetHC(samplerCollID_cylin));}
  if(SampHC)
    {bdsOutput->WriteHits(SampHC);}

  // energy deposition collections - eloss, tunnel hits
  BDSEnergyCounterHitsCollection* energyCounterHits       = (BDSEnergyCounterHitsCollection*)(HCE->GetHC(energyCounterCollID));
  BDSEnergyCounterHitsCollection* tunnelEnergyCounterHits = (BDSEnergyCounterHitsCollection*)(HCE->GetHC(tunnelEnergyCounterCollID));
  
  // fill histograms
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "filling histograms & writing energy loss hits" << G4endl;
#endif
  //if we have energy deposition hits, write them
  if(energyCounterHits)
    {
      BDSAnalysisManager* analMan     = BDSAnalysisManager::Instance();
      BDSHistogram1D* generalELoss    = analMan->GetHistogram(2);
      BDSHistogram1D* perElementELoss = analMan->GetHistogram(5);

      bdsOutput->WriteEnergyLoss(energyCounterHits); // write hits
      //bin hits in histograms
      for (G4int i = 0; i < energyCounterHits->entries(); i++)
	{
	  BDSEnergyCounterHit hit = *((*energyCounterHits)[i]);
	  G4double sHit = hit.GetSHit()/CLHEP::m;
	  G4double eW   = hit.GetEnergyWeighted()/CLHEP::GeV;
	  generalELoss->Fill(sHit, eW);
	  perElementELoss->Fill(sHit, eW);
	}
    }

  if (tunnelEnergyCounterHits)
   {bdsOutput->WriteTunnelHits(tunnelEnergyCounterHits);}

  // primary hits and losses from
  G4TrajectoryContainer* trajCont = evt->GetTrajectoryContainer();
  BDSTrajectory*         primary  = BDS::GetPrimaryTrajectory(trajCont);
  BDSTrajectoryPoint* primaryFirstInt = BDSTrajectory::FirstInteraction(primary);
  BDSTrajectoryPoint* primaryLastInt  = BDSTrajectory::LastInteraction(primary);
  bdsOutput->WritePrimaryHit(primaryFirstInt);
  bdsOutput->WritePrimaryLoss(primaryLastInt);

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "finished writing energy loss" << G4endl;
#endif

  // if events per ntuples not set (default 0) - only write out at end
  int evntsPerNtuple = BDSGlobalConstants::Instance()->NumberOfEventsPerNtuple();

  if (evntsPerNtuple>0 && (event_number+1)%evntsPerNtuple == 0)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "writing events" << G4endl;
#endif
    // note the timing information will be wrong here as the run hasn't finished but
    // the file is bridged. There's no good way around this just now as this class
    // can access the timing information stored in BDSRunAction
    bdsOutput->Commit(0, 0, 0, seedStateAtStart); // write and open new file
#ifdef BDSDEBUG
      G4cout << "done" << G4endl;
#endif
    }

  // needed to draw trajectories and hits:
  if(!isBatch)
  {
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "drawing the event" << G4endl;
#endif
    evt->Draw();
  }

  // Save interesting trajectories

  if(BDSGlobalConstants::Instance()->StoreTrajectory())
    {
      std::vector<BDSTrajectory*> interestingTrajectories;
      
      TrajectoryVector* trajVec = trajCont->GetVector();
      
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "trajectories ntrajectory=" << trajCont->size() << " storeTrajectoryEnergyThreshold=" << BDSGlobalConstants::Instance()->StoreTrajectoryEnergyThreshold() << G4endl;
#endif
      for (auto iT1 : *trajVec)
	{
	  BDSTrajectory* traj = (BDSTrajectory*) (iT1);
	  
	  G4int parentID=traj->GetParentID();
	  // always store primaries
	  if(parentID==0)
	    {
	      interestingTrajectories.push_back(traj);
	      continue;
	    }
	  
	  // check on energy (if energy threshold is not negative
	  if (BDSGlobalConstants::Instance()->StoreTrajectoryEnergyThreshold()*CLHEP::GeV >= 0 &&
	      traj->GetInitialKineticEnergy() > BDSGlobalConstants::Instance()->StoreTrajectoryEnergyThreshold()*CLHEP::GeV)
	    {
	      interestingTrajectories.push_back(traj);
	      continue;
	    }
	  
	  // check on particle if not empty string
	  if (!BDSGlobalConstants::Instance()->StoreTrajectoryParticle().empty())
	    {
	      G4String particleName = traj->GetParticleName();
	      //G4cout << particleName << G4endl;
	      std::size_t found = BDSGlobalConstants::Instance()->StoreTrajectoryParticle().find(particleName);
	      if (found != std::string::npos)
		{
		  interestingTrajectories.push_back(traj);
		  continue;
		}
	    }
	  
	  // check on depth
	  // depth = 0 means only primaries
#if 0
	  const G4int depth = BDSGlobalConstants::Instance()->StoreTrajectoryDepth();
	  // check directly for primaries and secondaries
	  if (parentID == 0 ||(depth > 0 && parentID == 1))
	    {
	      interestingTrajectories.push_back(traj);
	      continue;
	    }
	  
	  G4bool depthCheck = false;
	  // starting loop with tertiaries
	  for (G4int i=1; i<depth; i++)
	    {
	      // find track of parentID
	      // looping over vector seems only way?
	      for(auto iT2 : *trajVec)
		{
		  BDSTrajectory* tr2 = (BDSTrajectory*) (iT2);
		  if(tr2->GetTrackID()==parentID)
		    {
		      parentID = tr2->GetParentID();
		      break;
		    }
		}
	      // best to stop at parentID == 1
	      if (parentID == 1)
		{
		  depthCheck = true;
		  break;
		}
	    }
	  if(depthCheck == true)
	    {
	      interestingTrajectories.push_back(traj);
	      continue;
	    }
#endif
	  
#if 0
	  // clear out trajectories that don't reach point x
	  BDSTrajectoryPoint* trajEndPoint = (BDSTrajectoryPoint*)traj->GetPoint(traj->GetPointEntries() - 1);
	  G4ThreeVector trajEndPointThreeVector = trajEndPoint->GetPosition();
	  G4bool greaterThanZInteresting = trajEndPointThreeVector.z() / CLHEP::m > BDSGlobalConstants::Instance()->TrajCutGTZ();
	  G4double radius = std::sqrt(std::pow(trajEndPointThreeVector.x() / CLHEP::m, 2) + std::pow(trajEndPointThreeVector.y() / CLHEP::m, 2));
	  G4bool withinRInteresting = radius < BDSGlobalConstants::Instance()->TrajCutLTR();
	  if (greaterThanZInteresting && withinRInteresting)
	    {
	      interestingTrajectories.push_back(traj);
	      continue;
	    }
#endif
	}
      
      //Output interesting trajectories
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "storing trajectories nInterestingTrajectory=" << interestingTrajectories.size() << G4endl;
#endif
      
      bdsOutput->WriteTrajectory(interestingTrajectories);
      interestingTrajectories.clear();
    }
  
  bdsOutput->FillEvent();

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "end of event action done"<<G4endl;
#endif
}

void BDSEventAction::WritePrimaryVertex(G4int eventID,
					const G4PrimaryVertex* primaryVertexIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //Save the primary particle as a hit
  const G4PrimaryVertex*   primaryVertex   = primaryVertexIn;
  const G4PrimaryParticle* primaryParticle = primaryVertex->GetPrimary();
  G4ThreeVector      momDir          = primaryParticle->GetMomentumDirection();
  G4double           E               = primaryParticle->GetTotalEnergy();
  G4double           xp              = momDir.x();
  G4double           yp              = momDir.y();
  G4double           zp              = momDir.z();
  G4double           x0              = primaryVertex->GetX0();
  G4double           y0              = primaryVertex->GetY0();
  G4double           z0              = primaryVertex->GetZ0();
  G4double           t               = primaryVertex->GetT0();
  G4double           weight          = primaryParticle->GetWeight();
  G4int              PDGType         = primaryParticle->GetPDGcode();
  G4int              nEvent          = eventID;
  G4int              turnstaken      = BDSGlobalConstants::Instance()->TurnsTaken();
  bdsOutput->WritePrimary(E, x0, y0, z0, xp, yp, zp, t, weight, PDGType, nEvent, turnstaken);
}
