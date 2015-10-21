#include "BDSRunManager.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"

#include "CLHEP/Random/Random.h"

void BDSRunManager::BeamOn(G4int n_event,const char* macroFile,G4int n_select)
{
  G4RunManager::BeamOn(n_event,macroFile,n_select);
}

void BDSRunManager::DoEventLoop(G4int n_event,const char* macroFile,G4int n_select)
{
  // save event loop state
  if(verboseLevel>0){
    // Print seed to try and recreate an event in a run 
    G4cout << __METHOD_NAME__ << "Random number generator's seed=" 
	   << CLHEP::HepRandom::getTheSeed() << G4endl;
    // Print generator full state to output 
    G4cout << __METHOD_NAME__ << "Random number generator's state: " << G4endl;
    CLHEP::HepRandom::saveFullState(G4cout);
  }

  G4RunManager::DoEventLoop(n_event,macroFile,n_select);
}

void BDSRunManager::ProcessOneEvent(G4int i_event)
{
  // additional output
  if(verboseLevel>3){
    G4cout << __METHOD_NAME__ << "Event="<<i_event<<G4endl;
    // Print seed to try and recreate an event in a run
    G4cout << __METHOD_NAME__ << "Random number generator's seed=" 
	   << CLHEP::HepRandom::getTheSeed() << G4endl;
    // Print generator full state to output 
    G4cout << __METHOD_NAME__ << "Random number generator's state: " << G4endl;
    CLHEP::HepRandom::saveFullState(G4cout);
  }

  G4RunManager::ProcessOneEvent(i_event);
}


