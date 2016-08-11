#include "BDSAnalysisManager.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSEventAction.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutputBase.hh"
#include "BDSRunManager.hh"
#include "BDSSamplerHit.hh"
#include "BDSTrajectory.hh"
#include "BDSTunnelHit.hh"
#include "BDSRandom.hh"

#include "globals.hh"                  // geant4 types / globals
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Run.hh"
#include "G4HCofThisEvent.hh"
#include "G4TrajectoryContainer.hh"
#include "G4RichTrajectoryPoint.hh"
#include "G4SDManager.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "Randomize.hh" // for G4UniformRand

#include "CLHEP/Random/Random.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std::chrono;

extern BDSOutputBase* bdsOutput;       // output interface

G4bool FireLaserCompton;  // bool to ensure that Laserwire can only occur once in an event

BDSEventAction::BDSEventAction():
  analMan(nullptr),
  samplerCollID_plane(-1),
  samplerCollID_cylin(-1),
  energyCounterCollID(-1),
  tunnelCollID(-1),
  startTime(0),
  stopTime(0),
  starts(0),
  stops(0),
  seedStateAtStart("")
{
  verboseEvent       = BDSGlobalConstants::Instance()->VerboseEvent();
  verboseEventNumber = BDSGlobalConstants::Instance()->VerboseEventNumber();
  isBatch            = BDSGlobalConstants::Instance()->Batch();
  useTunnel          = BDSGlobalConstants::Instance()->BuildTunnel();

  if(isBatch)
    {
      G4int nGenerate = BDSGlobalConstants::Instance()->NGenerate();
      G4double fraction = BDSGlobalConstants::Instance()->PrintModuloFraction();
      printModulo = (G4int)ceil(nGenerate * fraction);
      if (printModulo < 0)
	{printModulo = 1;}
    }
  else printModulo=1;
}

BDSEventAction::~BDSEventAction()
{}

void BDSEventAction::BeginOfEventAction(const G4Event* evt)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "processing begin of event action" << G4endl;
#endif
  // save the random engine state
  std::stringstream ss;
  CLHEP::HepRandom::saveFullState(ss);
  seedStateAtStart = ss.str();

  // get the current time
  startTime = time(nullptr);

  milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  starts = (G4double)ms.count()/1000.0;

  // get pointer to analysis manager
  analMan = BDSAnalysisManager::Instance();

  // number feedback
  G4int event_number = evt->GetEventID();
  if (event_number%printModulo == 0)
    {G4cout << "\n---> Begin of event: " << event_number << G4endl;}
  if(verboseEvent)
    {G4cout << __METHOD_NAME__ << "event #" << event_number << G4endl;}

  // get hit collection IDs for easy access
  G4SDManager* g4SDMan = G4SDManager::GetSDMpointer();
  if(samplerCollID_plane < 0)
    {samplerCollID_plane  = g4SDMan->GetCollectionID("Sampler_plane");}
  if(samplerCollID_cylin < 0)
    {samplerCollID_cylin  = g4SDMan->GetCollectionID("Sampler_cylinder");}
  if(energyCounterCollID < 0)
    {energyCounterCollID  = g4SDMan->GetCollectionID("ec_on_axis_read_out/energy_counter");}
  if (useTunnel)
    {
      if(tunnelCollID < 0)
	{tunnelCollID = g4SDMan->GetCollectionID("tunnel_hits");} // defined in BDSSDManager.cc
    }
  //if (lWCalorimeterCollID<1)
  //{lWCalorimeterCollID = G4SDManager::GetSDMpointer()->GetCollectionID("LWCalorimeterCollection");}
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

  milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  stops = (G4double)ms.count()/1000.0;
  G4float duration = stops - starts;

  // Record timing in output
  bdsOutput->WriteEventInfo(startTime, stopTime, (G4float) duration, seedStateAtStart);

  // Get the hits collection of this event - all hits from different SDs.
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();

  // Get event number information
  G4int event_number = evt->GetEventID();
  if(verboseEvent || verboseEventNumber == event_number)
    {G4cout << __METHOD_NAME__ << "processing end of event"<<G4endl;}

  // Record the primary vertex in output
  WritePrimaryVertex();

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

  // LASERWIRE - TO FIX / REIMPLEMENT
  // remember to uncomment LWCalHC above if using this
  // BDSLWCalorimeterHitsCollection* LWCalHC=nullptr;
  // if(LWCalorimeterCollID >= 0)
  //   LWCalHC=(BDSLWCalorimeterHitsCollection*)(evt->GetHCofThisEvent()->GetHC(LWCalorimeterCollID));
  // if (LWCalHC)
  //    {bdsOutput->WriteHits(SampHC);}

  // energy deposition collections - eloss, tunnel hits
  BDSEnergyCounterHitsCollection* energyCounterHits  = (BDSEnergyCounterHitsCollection*)(HCE->GetHC(energyCounterCollID));
  BDSTunnelHitsCollection*        tunnelHits         = nullptr;
  if (useTunnel)
    {tunnelHits = (BDSTunnelHitsCollection*)(HCE->GetHC(tunnelCollID));}

  // fill histograms
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "filling histograms & writing energy loss hits" << G4endl;
#endif
  BDSAnalysisManager* analMan = BDSAnalysisManager::Instance();
  //if we have energy deposition hits, write them
  if(energyCounterHits)
    {
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

  // primary hits and losses from
  G4TrajectoryContainer* trajCont = evt->GetTrajectoryContainer();
  BDSTrajectory*         primary  = BDS::GetPrimaryTrajectory(trajCont);
  BDSTrajectoryPoint* primaryFirstInt = BDSTrajectory::FirstInteraction(primary);
  BDSTrajectoryPoint* primaryLastInt  = BDSTrajectory::LastInteraction(primary);
  bdsOutput->WritePrimaryHit(primaryFirstInt);
  bdsOutput->WritePrimaryLoss(primaryLastInt);


  // we should only try and access the tunnel hits collection if it was actually
  // instantiated which won't happen if the tunnel isn't build and placed. During
  // placement the SD is attached, which is done on demand as it's a read out one,
  // so without placement, accessing this will cause a segfault.
  if (useTunnel)
  {
    if (tunnelHits)
      {bdsOutput->WriteTunnelHits(tunnelHits);} // write hits
  }

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

    G4TrajectoryContainer* trajCont = evt->GetTrajectoryContainer();
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
	    if (!BDSGlobalConstants::Instance()->StoreTrajectoryParticle().empty()) {
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

void BDSEventAction::WritePrimaryVertex()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  //Save the primary particle as a hit
  G4PrimaryVertex*   primaryVertex   = BDSRunManager::GetRunManager()->GetCurrentEvent()->GetPrimaryVertex();
  G4PrimaryParticle* primaryParticle = primaryVertex->GetPrimary();
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
  G4int              nEvent          = BDSRunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  G4int              turnstaken      = BDSGlobalConstants::Instance()->TurnsTaken();
  bdsOutput->WritePrimary(E, x0, y0, z0, xp, yp, zp, t, weight, PDGType, nEvent, turnstaken);
}
