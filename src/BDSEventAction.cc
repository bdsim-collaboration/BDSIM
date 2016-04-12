#include "BDSAnalysisManager.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSEventAction.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSOutputBase.hh" 
#include "BDSRunManager.hh"
#include "BDSSamplerHit.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSTunnelHit.hh"

#include "globals.hh"                  // geant4 types / globals
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Run.hh"
#include "G4HCofThisEvent.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4SDManager.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "Randomize.hh" // for G4UniformRand

#include <list>
#include <map>
#include <vector>
#include <algorithm>

extern BDSOutputBase* bdsOutput;       // output interface

G4bool FireLaserCompton;  // bool to ensure that Laserwire can only occur once in an event

BDSEventAction::BDSEventAction():
  analMan(nullptr),
  samplerCollID_plane(-1),
  samplerCollID_cylin(-1),
  energyCounterCollID(-1),
  primaryCounterCollID(-1),
  tunnelCollID(-1)
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
  if(primaryCounterCollID < 0)
    {primaryCounterCollID = g4SDMan->GetCollectionID("ec_on_axis_read_out/primary_counter");}
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

  // energy deposition collections - eloss, primary hits, primary losses, tunnel hits
  BDSEnergyCounterHitsCollection* energyCounterHits  = (BDSEnergyCounterHitsCollection*)(HCE->GetHC(energyCounterCollID));
  BDSEnergyCounterHitsCollection* primaryCounterHits = (BDSEnergyCounterHitsCollection*)(HCE->GetHC(primaryCounterCollID));
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

  //if we have primary hits, find the first one and write that
  if(primaryCounterHits)
    {
      if (primaryCounterHits->entries()>0)
	{
	  BDSEnergyCounterHit* thePrimaryHit  = BDS::LowestSPosPrimaryHit(primaryCounterHits);
	  BDSEnergyCounterHit* thePrimaryLoss = BDS::HighestSPosPrimaryHit(primaryCounterHits);
	  //write
	  if (thePrimaryHit && thePrimaryLoss)
	    {
	      bdsOutput->WritePrimaryLoss(thePrimaryLoss);
	      bdsOutput->WritePrimaryHit(thePrimaryHit);
        // bdsOutput->GetAnalysis()->Fill1D();
	      // general histos
	      analMan->Fill1DHistogram(0, thePrimaryHit->GetSBefore()/CLHEP::m);
	      analMan->Fill1DHistogram(1, thePrimaryLoss->GetSAfter()/CLHEP::m);
	      // per element histos
	      analMan->Fill1DHistogram(3, thePrimaryHit->GetSBefore()/CLHEP::m);
	      analMan->Fill1DHistogram(4, thePrimaryLoss->GetSAfter()/CLHEP::m);
	    }
	}
    }

  // we should only try and access the tunnel hits collection if it was actually
  // instantiated which won't happen if the tunnel isn't build and placed. During
  // placement the SD is attached, which is done on demand as it's a read out one,
  // so without placement, accessing this will cause a segfault.
  if (BDSGlobalConstants::Instance()->BuildTunnel())
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
      bdsOutput->Commit(); // write and open new file
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
  if(BDSGlobalConstants::Instance()->StoreTrajectory() ||
          BDSGlobalConstants::Instance()->StoreMuonTrajectories() ||
          BDSGlobalConstants::Instance()->StoreNeutronTrajectories())
    {
      std::vector<BDSTrajectory*> interestingTrajectories;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "storing trajectories" << G4endl;
#endif
      G4TrajectoryContainer* trajCont = evt->GetTrajectoryContainer();
      if(!trajCont) return;
      TrajectoryVector* trajVec = trajCont->GetVector();
      // clear out trajectories that don't reach point x
      for(auto iT1 = trajVec->begin(); iT1 < trajVec->end(); iT1++)
	{
	  BDSTrajectory* traj=(BDSTrajectory*)(*iT1);
	  BDSTrajectoryPoint* trajEndPoint = (BDSTrajectoryPoint*)traj->GetPoint((int)traj->GetPointEntries()-1);
	  G4ThreeVector trajEndPointThreeVector = trajEndPoint->GetPosition();
	  G4bool greaterThanZInteresting = trajEndPointThreeVector.z()/CLHEP::m >
              BDSGlobalConstants::Instance()->TrajCutGTZ();
	  G4double radius   = sqrt(pow(trajEndPointThreeVector.x()/CLHEP::m, 2) + pow(trajEndPointThreeVector.y()/CLHEP::m, 2));
	  G4bool withinRInteresting = radius < BDSGlobalConstants::Instance()->TrajCutLTR();
	  if (greaterThanZInteresting && withinRInteresting)
	    {interestingTrajectories.push_back(traj);}
	}
    
      //Output interesting trajectories
      if(interestingTrajectories.size() > 0)
	{
	  bdsOutput->WriteTrajectory(interestingTrajectories);
	  interestingTrajectories.clear();
	}
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
