#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSEventAction.hh"
#include "BDSEventInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSHistogram.hh"
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
#include <map>
#include <string>
#include <vector>

using namespace std::chrono;

extern BDSOutputBase* bdsOutput;       // output interface

G4bool FireLaserCompton;  // bool to ensure that Laserwire can only occur once in an event

namespace {
  // Function to recursively connect t
  void connectTraj(const std::map<int, BDSTrajectory*> &trackIDMap, std::map<BDSTrajectory*, bool> &interestingTraj, BDSTrajectory* t) {
    G4int parentID = t->GetParentID();
    if (parentID > 0) {
      BDSTrajectory *t2 = trackIDMap.at(parentID);
      interestingTraj.insert(std::pair<BDSTrajectory *, bool>(t2, true));
      connectTraj(trackIDMap, interestingTraj, t2);
    }
    else {
      return;
    }
  }
}

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
  storeTrajectory    = BDSGlobalConstants::Instance()->StoreTrajectory();

  if(isBatch)
    {printModulo = BDSGlobalConstants::Instance()->PrintModulo();}
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

  // get the current time - last thing before we hand off to geant4
  startTime = time(nullptr);
  eventInfo->SetStartTime(startTime);
  eventInfo->SetStopTime(startTime); // initialise to duration of 0

  milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  starts = (G4double)ms.count()/1000.0;
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
  bdsOutput->WriteEventInfo(eventInfo->GetInfo());

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
    {
      if (SampHC->entries() > 0)
	{bdsOutput->WriteHits(SampHC);}
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "processing cylinder hits collection" << G4endl;
#endif
  SampHC = nullptr;
  if(samplerCollID_cylin >= 0)
    {SampHC = (BDSSamplerHitsCollection*)(HCE->GetHC(samplerCollID_cylin));}
  if(SampHC)
    {
      if (SampHC->entries() > 0)
	{bdsOutput->WriteHits(SampHC);}
    }

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
      bdsOutput->WriteEnergyLoss(energyCounterHits); // write hits
    }

  if (tunnelEnergyCounterHits)
   {bdsOutput->WriteTunnelHits(tunnelEnergyCounterHits);}

  // primary hits and losses from
  G4TrajectoryContainer* trajCont = evt->GetTrajectoryContainer();
  if (trajCont)
    {
      BDSTrajectory *primary = BDS::GetPrimaryTrajectory(trajCont);
      BDSTrajectoryPoint *primaryFirstInt = primary->FirstInteraction();
      BDSTrajectoryPoint *primaryLastInt = primary->LastInteraction();
      bdsOutput->WritePrimaryHit(primaryFirstInt);
      bdsOutput->WritePrimaryLoss(primaryLastInt);
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
    // can't access the timing information stored in BDSRunAction
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

  if (storeTrajectory && trajCont)
  {
    std::map<BDSTrajectory *, bool> interestingTraj;

    TrajectoryVector *trajVec = trajCont->GetVector();

#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "trajectories ntrajectory=" << trajCont->size() << " storeTrajectoryEnergyThreshold=" << BDSGlobalConstants::Instance()->StoreTrajectoryEnergyThreshold() << G4endl;
#endif

    // build trackID map
    std::map<int, BDSTrajectory *> trackIDMap;
    for (auto iT1 : *trajVec) {
      BDSTrajectory *traj = (BDSTrajectory *) iT1;
      trackIDMap.insert(std::pair<int, BDSTrajectory *>(traj->GetTrackID(), traj));
    }

    // build depth map
    std::map<BDSTrajectory *, int> depthMap;
    for (auto iT1 : *trajVec) {
      BDSTrajectory *traj = (BDSTrajectory *) (iT1);
      if (traj->GetParentID() == 0) {
        depthMap.insert(std::pair<BDSTrajectory *, int>(traj, 0));
      } else {
        depthMap.insert(std::pair<BDSTrajectory *, int>(traj, depthMap.at(trackIDMap.at(traj->GetParentID())) + 1));
      }
    }

    // loop over trajectories and determine if traj should be stored
    for (auto iT1 : *trajVec) {
      BDSTrajectory *traj = (BDSTrajectory *) (iT1);
      G4int parentID = traj->GetParentID();

      // always store primaries
      if (parentID == 0) {
        interestingTraj.insert(std::pair<BDSTrajectory *, bool>(traj, true));
        continue;
      }

      // check on energy (if energy threshold is not negative
      if (BDSGlobalConstants::Instance()->StoreTrajectoryEnergyThreshold() * CLHEP::GeV >= 0 &&
          traj->GetInitialKineticEnergy() >
          BDSGlobalConstants::Instance()->StoreTrajectoryEnergyThreshold() * CLHEP::GeV) {
        interestingTraj.insert(std::pair<BDSTrajectory *, bool>(traj, true));
        continue;
      }

      // check on particle if not empty string
      if (!BDSGlobalConstants::Instance()->StoreTrajectoryParticle().empty()) {
        G4String particleName = traj->GetParticleName();
        //G4cout << particleName << G4endl;
        std::size_t found = BDSGlobalConstants::Instance()->StoreTrajectoryParticle().find(particleName);
        if (found != std::string::npos) {
          interestingTraj.insert(std::pair<BDSTrajectory *, bool>(traj, true));
          continue;
        }
      }

      // check on trajectory tree dept (depth = 0 means only primaries)
      const G4int depth = BDSGlobalConstants::Instance()->StoreTrajectoryDepth();
      if (depthMap[traj] <= depth) {
        interestingTraj.insert(std::pair<BDSTrajectory *, bool>(traj, true));
        continue;
      }

      // check on coordinates (and TODO momentum)
      // clear out trajectories that don't reach point TrajCutGTZ or greater than TrajCutLTR
      BDSTrajectoryPoint *trajEndPoint = (BDSTrajectoryPoint *) traj->GetPoint(traj->GetPointEntries() - 1);
      G4ThreeVector trajEndPointThreeVector = trajEndPoint->GetPosition();
      G4bool greaterThanZInteresting =
              trajEndPointThreeVector.z() / CLHEP::m > BDSGlobalConstants::Instance()->TrajCutGTZ();
      G4double radius = std::hypot(trajEndPointThreeVector.x() / CLHEP::m,
				   trajEndPointThreeVector.y() / CLHEP::m);
      G4bool withinRInteresting = radius < BDSGlobalConstants::Instance()->TrajCutLTR();
      if (greaterThanZInteresting || withinRInteresting) {
        interestingTraj.insert(std::pair<BDSTrajectory *, bool>(traj, true));
        continue;
      }
      // if not interesting store false
      interestingTraj.insert(std::pair<BDSTrajectory *, bool>(traj, false));
    }

    // Connect trajectory graphs
    if (BDSGlobalConstants::Instance()->TrajConnect() && trackIDMap.size() > 1) {
      for (auto i : interestingTraj) {
        connectTraj(trackIDMap, interestingTraj, i.first);
      }
    }

    // Output interesting trajectories
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "storing trajectories nInterestingTrajectory=" << interestingTraj.size() << G4endl;
#endif
    std::vector<BDSTrajectory *> interestingTrajVec;
    // TODO sort accordings to trackID
    for (auto i : trackIDMap) {
      if (interestingTraj.at(i.second)) {
	interestingTrajVec.push_back(i.second);
      }
    }

    // Relabel with new track IDS

    // make map
    std::map<BDSTrajectory *, G4int> interestingTrajIndexMap;
    int idx = 0;
    for (auto i : interestingTrajVec) {
      interestingTrajIndexMap[i] = idx;
      idx++;
    }

    for (auto i : interestingTrajVec) {
      G4int parentIndex = -1;
      try {
        parentIndex = (G4int) interestingTrajIndexMap.at(trackIDMap.at(i->GetParentID()));
      }
      catch (const std::exception &ex) {
        parentIndex = -1;
      }
      i->SetParentIndex(parentIndex);
    }

    bdsOutput->WriteTrajectory(interestingTrajVec);
  }
  
  bdsOutput->FillEvent(); // this fills data and clears event level structures

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
