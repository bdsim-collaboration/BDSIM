/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/


#ifndef BDSEventAction_h
#define BDSEventAction_h 


#include "G4UserEventAction.hh"
#include "globals.hh"
#include "BDSAnalysisManager.hh"
#include "BDSTrajectory.hh"
#include <list>
#include <vector>

class BDSEventAction : public G4UserEventAction
{
public:
  BDSEventAction();
  virtual ~BDSEventAction();
  
public:
  virtual void   BeginOfEventAction(const G4Event*);
  virtual void   EndOfEventAction(const G4Event*);
  void AddPrimaryHits();
    
private:
  BDSAnalysisManager* analMan;
  G4bool verbose;
  G4bool verboseEvent;
  G4int  verboseEventNumber;
  G4bool isBatch;
  G4bool useTunnel;

  G4int samplerCollID_plane;
  G4int samplerCollID_cylin;
  //G4int lWCalorimeterCollID;
  G4int energyCounterCollID; ///< collection ID for energy deposition hits for read out geometry
  G4int primaryCounterCollID;///< collection ID for primary hits for read out geometry
  G4int tunnelCollID;        ///< collection ID for tunnel hits from tunnel read out geometry
  
  G4int printModulo;

  BDSTrajectory* traj;
  BDSTrajectoryPoint* trajEndPoint;
  G4ThreeVector trajEndPointThreeVector;
  std::vector<BDSTrajectory*> interestingTrajectories;
};

#endif

