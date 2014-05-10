/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

//==========================================================
//==========================================================

#include "BDSRunAction.hh"

#include "G4Run.hh"
//#include "G4UImanager.hh"
//#include "G4VVisManager.hh"
#include "G4ios.hh"

#include "BDSTrackingFIFO.hh"

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
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

  //  if (G4VVisManager::GetConcreteInstance())
  //    {
      //      G4UImanager* UI = G4UImanager::GetUIpointer(); 
      //  UI->ApplyCommand("/vis/scene/notifyHandlers");
  //    } 


}

//==========================================================

void BDSRunAction::EndOfRunAction(const G4Run* aRun)
{
  //Do the fifo at the end of the run.
  BDSTrackingFIFO* fifo = new BDSTrackingFIFO();
  fifo->doFifo();
  G4cout << "### Run " << aRun->GetRunID() << " end." << G4endl;
}
//==========================================================
