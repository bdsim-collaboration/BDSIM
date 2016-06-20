#ifndef BDSEVENTACTION_H
#define BDSEVENTACTION_H

#include "BDSAnalysisManager.hh"

#include "globals.hh" // geant4 types / globals
#include "G4UserEventAction.hh"

#include <ctime>
#include <string>
#include <vector>

class BDSEventAction: public G4UserEventAction
{
public:
  BDSEventAction();
  virtual ~BDSEventAction();
  
public:
  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);
  void         WritePrimaryVertex();
    
private:
  BDSAnalysisManager* analMan;
  G4bool verboseEvent;
  G4int  verboseEventNumber;
  G4bool isBatch;
  G4bool useTunnel;

  G4int samplerCollID_plane;
  G4int samplerCollID_cylin;
  //G4int lWCalorimeterCollID;
  G4int energyCounterCollID; ///< Collection ID for energy deposition hits for read out geometry.
  G4int primaryCounterCollID;///< Collection ID for primary hits for read out geometry.
  G4int tunnelCollID;        ///< Collection ID for tunnel hits from tunnel read out geometry.
  
  G4int printModulo;

  time_t startTime; ///< Time at the start of the event.
  time_t stopTime;  ///< Time at the end of the event.

  G4double starts; ///< Precise start time in seconds.
  G4double stops;  ///< Precise stop time in seconds.

  std::string seedStateAtStart; ///< Seed state at start of the event.
};

#endif

