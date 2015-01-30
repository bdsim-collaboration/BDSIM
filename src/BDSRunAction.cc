/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

//==========================================================
//==========================================================

#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSOutputBase.hh" 
#include "BDSRunAction.hh"
#include "BDSRunManager.hh"
#include "BDSDebug.hh"

#include "G4Run.hh"
//#include "G4UImanager.hh"
//#include "G4VVisManager.hh"
#include "G4ios.hh"

extern BDSOutputBase* bdsOutput;         // output interface

//==========================================================

BDSRunAction::BDSRunAction()
{
}

//==========================================================

BDSRunAction::~BDSRunAction()
{}

//==========================================================

void BDSRunAction::BeginOfRunAction(const G4Run* aRun)
{
  //Get the current time
  starttime = time(NULL);

  //Output feedback
  G4cout << __METHOD_NAME__ << " Run " << aRun->GetRunID() << " start. Time is " << asctime(localtime(&starttime)) << G4endl;

  //  if (G4VVisManager::GetConcreteInstance())
  //    {
      //      G4UImanager* UI = G4UImanager::GetUIpointer(); 
      //  UI->ApplyCommand("/vis/scene/notifyHandlers");
  //    } 


}

//==========================================================

void BDSRunAction::EndOfRunAction(const G4Run* aRun)
{
  //Get the current time
  stoptime = time(NULL);

  //Output feedback
  G4cout << __METHOD_NAME__ << "Run " << aRun->GetRunID() << " end. Time is " << asctime(localtime(&stoptime)) << G4endl;
  
  // Write output
  if(BDSExecOptions::Instance()->GetBatch()) {  // Non-interactive mode
    bdsOutput->Write(); // write last file
  } else {
    bdsOutput->Commit(); // write and open new file
  }

  // note difftime only calculates to the integer second
  G4cout << "Run Duration >> " << (int)difftime(stoptime,starttime) << " s" << G4endl;
}
//==========================================================
