#include "BDSDebug.hh"
#include "BDSEventInfo.hh"
#include "BDSOutput.hh"
#include "BDSParser.hh"
#include "BDSRunAction.hh"

#include "parser/options.h"
#include "parser/optionsBase.h"

#include "globals.hh"               // geant4 globals / types
#include "G4Run.hh"

#include "CLHEP/Random/Random.h"

#include <sstream>
#include <string>

BDSRunAction::BDSRunAction(BDSOutput* outputIn):
  output(outputIn),
  starttime(time(nullptr)),
  seedStateAtStart(""),
  info(nullptr)
{;}

BDSRunAction::~BDSRunAction()
{
  delete info;
}

void BDSRunAction::BeginOfRunAction(const G4Run* aRun)
{
  info = new BDSEventInfo();
  
  // save the random engine state
  std::stringstream ss;
  CLHEP::HepRandom::saveFullState(ss);
  seedStateAtStart = ss.str();
  info->SetSeedStateAtStart(seedStateAtStart);
  
  // get the current time
  starttime = time(nullptr);
  info->SetStartTime(starttime);
  
  // Output feedback
  G4cout << __METHOD_NAME__ << "Run " << aRun->GetRunID()
	 << " start. Time is " << asctime(localtime(&starttime)) << G4endl;

  output->InitialiseGeometryDependent();
  output->NewFile();

  // Write options now file open.
  const GMAD::Options o = BDSParser::Instance()->GetOptions();
  const GMAD::OptionsBase* ob = dynamic_cast<const GMAD::OptionsBase*>(&o);
  output->FillOptions(ob);

  // Write model now file open.
  output->FillModel();
}

void BDSRunAction::EndOfRunAction(const G4Run* aRun)
{
  // Get the current time
  time_t stoptime = time(nullptr);
  info->SetStopTime(stoptime);
  // run duration
  G4float duration = difftime(stoptime, starttime);
  info->SetDuration(G4double(duration));


  // Output feedback
  G4cout << G4endl << __METHOD_NAME__ << "Run " << aRun->GetRunID()
	 << " end. Time is " << asctime(localtime(&stoptime));
  
  // Write output
  output->FillRun(info);
  output->CloseFile();
  info->Flush();

  // note difftime only calculates to the integer second
  G4cout << "Run Duration >> " << (int)duration << " s" << G4endl;
}
