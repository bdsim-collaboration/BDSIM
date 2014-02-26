/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Removed StringFromInt function
   Added/Changed Sampler code for Plane Sampler or Cylinder Sampler (GABs Code)
*/


#ifndef BDSEventAction_h
#define BDSEventAction_h 


#include "BDSGlobalConstants.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSEnergyCounterSD.hh"
#include "G4Trajectory.hh"
#include "G4TrajectoryContainer.hh"
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
  void AddPrimaryHits(const G4Event* event);
  void SetDrawFlag   (G4String val)  {drawFlag = val;};
    
private:
  G4bool verbose;
  G4bool verboseStep;
  G4bool verboseEvent;
  G4int  verboseEventNumber;
  G4bool isBatch;
  G4int  nptwiss;

  G4int                       SamplerCollID_plane;                
  G4int                       SamplerCollID_cylin;                
  //    G4int                       SamplerCollID_primaries;                
  //    G4int                       LWCalorimeterCollID;
  //    G4int                       EnergyCounterCollID;                
  G4String                    drawFlag;
  
  G4int printModulo;
  G4int itsOutputFileNumber;
  G4int itsRecordSize;

  G4VTrajectory* Traj;
  G4VTrajectoryPoint* trajEndPoint;
  G4ThreeVector trajEndPointThreeVector;
  std::vector<G4VTrajectory*> interestingTrajectories;
  

  std::list<BDSAcceleratorComponent*>::const_iterator iBeam;
  std::list<BDSEnergyCounterSD*>::const_iterator iEC;

  BDSAcceleratorComponent* LastComp;
  };

#endif

    
