#include "BDSDebug.hh"
#include "BDSOutput.hh"
#include "BDSRunAction.hh"

#include "globals.hh"               // geant4 globals / types
#include "G4Run.hh"

#include "CLHEP/Random/Random.h"

#include <sstream>
#include <string>

extern BDSOutput* bdsOutput;         // output interface

BDSRunAction::BDSRunAction()
{;}

BDSRunAction::~BDSRunAction()
{;}

void BDSRunAction::BeginOfRunAction(const G4Run* aRun)
{
  // save the random engine state
  std::stringstream ss;
  CLHEP::HepRandom::saveFullState(ss);
  seedStateAtStart = ss.str();
  
  // get the current time
  starttime = time(nullptr);

  // Output feedback
  G4cout << __METHOD_NAME__ << "Run " << aRun->GetRunID() << " start. Time is " << asctime(localtime(&starttime)) << G4endl;

  bdsOutput->Initialise(); // open file, create structures and histograms
}

void BDSRunAction::EndOfRunAction(const G4Run* aRun)
{
  // Get the current time
  stoptime = time(nullptr);
  // run duration
  G4float duration = difftime(stoptime,starttime);

  // Output feedback
  G4cout << G4endl << __METHOD_NAME__ << "Run " << aRun->GetRunID()
	 << " end. Time is " << asctime(localtime(&stoptime));
  
  // Write output
  bdsOutput->Write(starttime, stoptime, duration, seedStateAtStart); // write last file
  bdsOutput->Close();

  // note difftime only calculates to the integer second
  G4cout << "Run Duration >> " << (int)duration << " s" << G4endl;
}
