/* BDSIM
   Author: L. Nevay,
   Last modified 15/04/2014
   Copyright (c) 2014.  ALL RIGHTS RESERVED.
*/

#include "BDSExecOptions.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"

#include "BDSTerminatorSD.hh"

#include "G4ios.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"
#include "G4Step.hh"


BDSTerminatorSD::BDSTerminatorSD(G4String name)
  :G4VSensitiveDetector(name)
{
  verbose  = BDSExecOptions::Instance()->GetVerbose();
}

BDSTerminatorSD::~BDSTerminatorSD()
{;}

void BDSTerminatorSD::Initialize(G4HCofThisEvent* /*HCE*/)
{
  BDSGlobalConstants::Instance()->ResetTurnNumber();
  //we don't actually use HCE here as we don't need to log any of the particle info
}

G4bool BDSTerminatorSD::ProcessHits(G4Step*aStep, G4TouchableHistory*)
{
  G4int turnstaken = BDSGlobalConstants::Instance()->GetTurnsTaken();
  // feedback info but only every 10 turns to avoid slow down and output bloat
  if (turnstaken % 10 == 0)
    {
      G4cout << "Turn: " << std::right << std::setw(4) << std::fixed
	     << turnstaken << " / " << std::left 
	     << BDSGlobalConstants::Instance()->GetTurnsToTake() << G4endl;
    }
#ifdef BDSDEBUG
  G4cout << "Incrementing turn number " << G4endl;
#endif
  G4Track* theTrack = aStep->GetTrack();
  if (theTrack->GetParentID() == 0){
    //this is a primary track
    //should only increment turn number for primaries
    #ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << " primary particle - incrementing turn number" << G4endl;
    #endif
    BDSGlobalConstants::Instance()->IncrementTurnNumber();
  }
  #ifdef BDSDEBUG
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
