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
  verbose  = BDSExecOptions::Instance()->GetVerbose();
  collectionName.insert("Terminator_"+name);
}

BDSTerminatorSD::~BDSTerminatorSD()
{;}

void BDSTerminatorSD::Initialize(G4HCofThisEvent* /*HCE*/)
{
  if (itsHCID < 0)
    {itsHCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);}
  BDSGlobalConstants::Instance()->ResetTurnNumber();
  //we don't actually use HCE here as we don't need to log any of the particle info
}

G4bool BDSTerminatorSD::ProcessHits(G4Step*aStep,G4TouchableHistory*)
{
  
  if (verbose){
    G4cout << "Number of turns to take: " << BDSGlobalConstants::Instance()->GetTurnsToTake() << G4endl;
    G4cout << "Number of turns taken:   " << BDSGlobalConstants::Instance()->GetTurnsTaken() << G4endl;
    G4cout << "Incrementing turn number " << G4endl;
  }
  theTrack = aStep->GetTrack();
  if (theTrack->GetParentID() == 0){
    //this is a primary track
    //should only increment turn number for primaries
    #ifdef DEBUG
    G4cout << __METHOD_NAME__ << " primary particle - incrementing turn number" << G4endl;
    #endif
    BDSGlobalConstants::Instance()->IncrementTurnNumber();
  }
  #ifdef DEBUG
  else
    {G4cout << __METHOD_NAME__ << " secondary particle - not incrementing turn number" << G4endl;}
  #endif
  return true;
}


void BDSTerminatorSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
  BDSGlobalConstants::Instance()->ResetTurnNumber();
}

void BDSTerminatorSD::clear()
{} 

void BDSTerminatorSD::DrawAll()
{} 

void BDSTerminatorSD::PrintAll()
{} 
