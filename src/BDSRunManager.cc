/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
//
// SPM: Altered BeamOn function to account for Placet synchronisation
//

#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSRunManager.hh"

#include "G4Version.hh"

#include "CLHEP/Random/Random.h"

// add keyword ThreadLocal for versions 10
#if G4VERSION_NUMBER > 999
G4ThreadLocal
#endif
BDSRunManager* BDSRunManager::fRunManager = 0;

BDSRunManager* BDSRunManager::GetRunManager() { 
  return fRunManager; 
}

BDSRunManager::BDSRunManager():G4RunManager() { 
  fRunManager = this;
}

BDSRunManager::~BDSRunManager(){
  fRunManager = NULL;
}

void BDSRunManager::BeamOn(G4int n_event,const char* macroFile,G4int n_select)
{
  G4RunManager::BeamOn(n_event,macroFile,n_select);

  // only do an additional BeamOn when holdingQueue is not empty
  if (BDSGlobalConstants::Instance()->holdingQueue.empty()) return;

  G4bool cond = ConfirmBeamOnCondition();
  G4StackManager* SM;
  SM = G4EventManager::GetEventManager()->GetStackManager();
  SM->SetNumberOfAdditionalWaitingStacks(4);
  if(cond)
  {
    while(!BDSGlobalConstants::Instance()->holdingQueue.empty()){
      BDSGlobalConstants::Instance()->setReadFromStack(true);
      SM->ClearPostponeStack();

      RunInitialization();
//      DoEventLoop(n_event,macroFile,n_select);
      DoEventLoop(BDSGlobalConstants::Instance()->holdingQueue.size(),macroFile,n_select);
      RunTermination();

      BDSGlobalConstants::Instance()->setReadFromStack(false);
    }
  }
}

void BDSRunManager::DoEventLoop(G4int n_event,const char* macroFile,G4int n_select)
{
  // save event loop state
  if(verboseLevel>0){
    // Print seed to try and recreate an event in a run 
    G4cout << __METHOD_NAME__ << "> Random number generator's seed=" 
	   << CLHEP::HepRandom::getTheSeed() << G4endl;
    // Print generator full state to output 
    G4cout << __METHOD_NAME__ << "> Random number generator's state: " << G4endl;
    CLHEP::HepRandom::saveFullState(G4cout);
  }

  G4RunManager::DoEventLoop(n_event,macroFile,n_select);
}

void BDSRunManager::ProcessOneEvent(G4int i_event)
{
  // additional output
  if(verboseLevel>3){
    G4cout << __METHOD_NAME__ << " Event="<<i_event<<G4endl;
    // Print seed to try and recreate an event in a run
    G4cout << __METHOD_NAME__ << "> Random number generator's seed=" 
	   << CLHEP::HepRandom::getTheSeed() << G4endl;
    // Print generator full state to output 
    G4cout << __METHOD_NAME__ << " Random number generator's state: " << G4endl;
    CLHEP::HepRandom::saveFullState(G4cout);
  }

  G4RunManager::ProcessOneEvent(i_event);
}


