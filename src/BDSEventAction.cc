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
#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSEventAction.hh"
#include "BDSEventInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutput.hh"
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

G4bool FireLaserCompton;  // bool to ensure that Laserwire can only occur once in an event

namespace {
  // Function to recursively connect t
  void connectTraj(const std::map<int, BDSTrajectory*> &trackIDMap,
		   std::map<BDSTrajectory*, bool> &interestingTraj, BDSTrajectory* t)
  {
    G4int parentID = t->GetParentID();
    if (parentID > 0)
      {
	BDSTrajectory *t2 = trackIDMap.at(parentID);
	interestingTraj.insert(std::pair<BDSTrajectory *, bool>(t2, true));
	connectTraj(trackIDMap, interestingTraj, t2);
      }
    else
      {return;}
  }
}

BDSEventAction::BDSEventAction(BDSOutput* outputIn):
  output(outputIn),
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
  eventInfo->SetIndex(event_number);
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
  // Get event number information
  G4int event_number = evt->GetEventID();
  if(verboseEvent || verboseEventNumber == event_number)
    {G4cout << __METHOD_NAME__ << "processing end of event"<<G4endl;}
  eventInfo->SetIndex(event_number);
  
  // Get the current time
  stopTime = time(nullptr);
  eventInfo->SetStopTime(stopTime);
  
  milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  stops = (G4double)ms.count()/1000.0;
  eventInfo->SetDuration(G4float(stops - starts));

  // Get the hits collection of this event - all hits from different SDs.
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();

  // samplers
  BDSSamplerHitsCollection* SampHC = nullptr;
  if(samplerCollID_plane >= 0)
    {SampHC = (BDSSamplerHitsCollection*)(evt->GetHCofThisEvent()->GetHC(samplerCollID_plane));}
  
  BDSSamplerHitsCollection* hitsCylinder = nullptr;
  if(samplerCollID_cylin >= 0)
    {hitsCylinder = (BDSSamplerHitsCollection*)(HCE->GetHC(samplerCollID_cylin));}

  // energy deposition collections - eloss, tunnel hits
  BDSEnergyCounterHitsCollection* energyCounterHits       = (BDSEnergyCounterHitsCollection*)(HCE->GetHC(energyCounterCollID));
  BDSEnergyCounterHitsCollection* tunnelEnergyCounterHits = (BDSEnergyCounterHitsCollection*)(HCE->GetHC(tunnelEnergyCounterCollID));

  // primary hits and losses from
  BDSTrajectoryPoint* primaryHit  = nullptr;
  BDSTrajectoryPoint* primaryLoss = nullptr;
  G4TrajectoryContainer* trajCont = evt->GetTrajectoryContainer();
  if (trajCont)
    {
      BDSTrajectory* primary = BDS::GetPrimaryTrajectory(trajCont);
      primaryHit  = primary->FirstInteraction();
      primaryLoss = primary->LastInteraction();
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
  std::vector<BDSTrajectory*> interestingTrajVec;
  if (storeTrajectory && trajCont)
  {
    G4double trajectoryEnergyThreshold = BDSGlobalConstants::Instance()->StoreTrajectoryEnergyThreshold(); // already in G4 GeV
    std::map<BDSTrajectory *, bool> interestingTraj;

    TrajectoryVector *trajVec = trajCont->GetVector();

#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "trajectories ntrajectory=" << trajCont->size()
	   << " storeTrajectoryEnergyThreshold=" << trajectoryEnergyThreshold << G4endl;
#endif

    // build trackID map
    std::map<int, BDSTrajectory *> trackIDMap;
    for (auto iT1 : *trajVec)
      {
	BDSTrajectory *traj = (BDSTrajectory *) iT1;
	trackIDMap.insert(std::pair<int, BDSTrajectory *>(traj->GetTrackID(), traj));
      }

    // build depth map
    std::map<BDSTrajectory *, int> depthMap;
    for (auto iT1 : *trajVec)
      {
	BDSTrajectory *traj = (BDSTrajectory *) (iT1);
	if (traj->GetParentID() == 0) 
	  {depthMap.insert(std::pair<BDSTrajectory *, int>(traj, 0));}
	else
	  {depthMap.insert(std::pair<BDSTrajectory *, int>(traj, depthMap.at(trackIDMap.at(traj->GetParentID())) + 1));}
      }

    // loop over trajectories and determine if it should be stored
    for (auto iT1 : *trajVec)
      {
	BDSTrajectory *traj = (BDSTrajectory *) (iT1);
	G4int parentID = traj->GetParentID();
	
	// always store primaries
	if (parentID == 0)
	  {
	    interestingTraj.insert(std::pair<BDSTrajectory *, bool>(traj, true));
	    continue;
	  }
	
	// check on energy (if energy threshold is not negative
	if (trajectoryEnergyThreshold >= 0 &&
	    traj->GetInitialKineticEnergy() > trajectoryEnergyThreshold)
	  {
	    interestingTraj.insert(std::pair<BDSTrajectory *, bool>(traj, true));
	    continue;
	  }
	
	// check on particle if not empty string
	G4String particleToStore = BDSGlobalConstants::Instance()->StoreTrajectoryParticle();
	if (!particleToStore.empty())
	  {
	    G4String particleName = traj->GetParticleName();
	    std::size_t found = particleToStore.find(particleName);
	    if (found != std::string::npos)
	      {
		interestingTraj.insert(std::pair<BDSTrajectory *, bool>(traj, true));
		continue;
	      }
	  }
	
	// check on trajectory tree dept (depth = 0 means only primaries)
	const G4int depth = BDSGlobalConstants::Instance()->StoreTrajectoryDepth();
	if (depthMap[traj] <= depth)
	  {
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
	if (greaterThanZInteresting || withinRInteresting)
	  {
	    interestingTraj.insert(std::pair<BDSTrajectory *, bool>(traj, true));
	    continue;
	  }

	// if not interesting store false
	interestingTraj.insert(std::pair<BDSTrajectory *, bool>(traj, false));
      }
    
    // Connect trajectory graphs
    if (BDSGlobalConstants::Instance()->TrajConnect() && trackIDMap.size() > 1)
      {
	for (auto i : interestingTraj)
	  {connectTraj(trackIDMap, interestingTraj, i.first);}
      }

    // Output interesting trajectories
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "storing trajectories nInterestingTrajectory=" << interestingTraj.size() << G4endl;
#endif
    
    // TODO sort accordings to trackID
    for (auto i : trackIDMap)
      {
	if (interestingTraj.at(i.second))
	  {interestingTrajVec.push_back(i.second);}
      }

    // Relabel with new track IDS

    // make map
    std::map<BDSTrajectory *, G4int> interestingTrajIndexMap;
    int idx = 0;
    for (auto i : interestingTrajVec)
      {
	interestingTrajIndexMap[i] = idx;
	idx++;
      }

    for (auto i : interestingTrajVec)
      {
	G4int parentIndex = -1;
	try
	  {parentIndex = (G4int) interestingTrajIndexMap.at(trackIDMap.at(i->GetParentID()));}
	catch (const std::exception &ex)
	  {parentIndex = -1;}
	i->SetParentIndex(parentIndex);
      }
  }
  
  output->FillEvent(eventInfo,
		    evt->GetPrimaryVertex(),
		    SampHC,
		    hitsCylinder,
		    energyCounterHits,
		    tunnelEnergyCounterHits,
		    primaryHit,
		    primaryLoss,
		    interestingTrajVec,
		    BDSGlobalConstants::Instance()->TurnsTaken());
  
  // if events per ntuples not set (default 0) - only write out at end
  G4int evntsPerNtuple = BDSGlobalConstants::Instance()->NumberOfEventsPerNtuple();
  if (evntsPerNtuple>0 && (event_number+1)%evntsPerNtuple == 0)
    {
      // note the timing information will be wrong here as the run hasn't finished but
      // the file is bridged. There's no good way around this just now as this class
      // can't access the timing information stored in BDSRunAction
      output->CloseAndOpenNewFile();
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "end of event action done"<<G4endl;
#endif
}
