#ifndef BDSEVENTACTION_H
#define BDSEVENTACTION_H

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "BDSAnalysisManager.hh"
#include <vector>

class BDSEventAction : public G4UserEventAction
{
public:
  BDSEventAction();
  virtual ~BDSEventAction();
  
public:
  virtual void   BeginOfEventAction(const G4Event*);
  virtual void   EndOfEventAction(const G4Event*);
  void           WritePrimaryVertex();
    
private:
  BDSAnalysisManager* analMan;
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
};

#endif

