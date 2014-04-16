/* BDSIM
   Author: L. Nevay,
   Last modified 15/04/2014
   Copyright (c) 2014.  ALL RIGHTS RESERVED.
*/

#include "BDSExecOptions.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"

#include "BDSTerminatorSD.hh"

#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"
#include "G4Step.hh"


BDSTerminatorSD::BDSTerminatorSD(G4String name)
  :G4VSensitiveDetector(name), itsHCID(-1)
{
  itsverbose  = BDSExecOptions::Instance()->GetVerbose();
  collectionName.insert("Terminator_"+name);
}

BDSTerminatorSD::~BDSTerminatorSD()
{;}

void BDSTerminatorSD::Initialize(G4HCofThisEvent* HCE)
{
  if (itsHCID < 0)
    {itsHCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);}
  BDSGlobalConstants::Instance()->ResetTurnNumber();						
}

G4bool BDSTerminatorSD::ProcessHits(G4Step*aStep,G4TouchableHistory*)
{
  
#ifdef DEBUG
  G4cout << "Number of turns to take: " << BDSGlobalConstants::Instance()->GetNTurns() << G4endl;
  G4cout << "Number of turns taken:   " << BDSGlobalConstants::Instance()->GetTurnNumber() << G4endl;
  G4cout << "Incrementing turn number " << G4endl;
#endif
  BDSGlobalConstants::Instance()->IncrementTurnNumber();
  return true;
}


void BDSTerminatorSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  BDSGlobalConstants::Instance()->ResetTurnNumber();
}

void BDSTerminatorSD::clear()
{} 

void BDSTerminatorSD::DrawAll()
{} 

void BDSTerminatorSD::PrintAll()
{} 
