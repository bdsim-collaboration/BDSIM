/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"

#include "BDSTerminatorSD.hh"

#include "G4ios.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"
#include "G4Step.hh"

#include <iomanip>


BDSTerminatorSD::BDSTerminatorSD(G4String name)
  :G4VSensitiveDetector(name)
{
}

BDSTerminatorSD::~BDSTerminatorSD()
{;}

void BDSTerminatorSD::Initialize(G4HCofThisEvent* /*HCE*/)
{
  BDSGlobalConstants::Instance()->ResetTurnNumber();
  //we don't actually use HCE here as we don't need to log any of the particle info
}

G4bool BDSTerminatorSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  G4Track* theTrack    = aStep->GetTrack();
  G4int parentID       = theTrack->GetParentID();
  G4double trackLength = theTrack->GetTrackLength();
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "parentID:         " << parentID    << G4endl;
  G4cout << __METHOD_NAME__ << "track lenth (mm): " << trackLength << G4endl;
#endif
  if ((parentID == 0) && (trackLength > 1000) && aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary)
    {
      // parentID == 0 -> primary particle - should only increment turn number for primaries
      // trackLength > 1000 (mm) -> not due to initial coordinate offset (at least 1 turn)
      // pre step point status is on a geometry boundary -> particle just entering volume and not starting in middle
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Incrementing turn number " << G4endl;
      G4cout << __METHOD_NAME__ << "Primary particle - incrementing turn number" << G4endl;
      G4cout << __METHOD_NAME__ << "Track length is : " << trackLength << " m" << G4endl;
      G4cout << __METHOD_NAME__ << "Turn number is  : " << BDSGlobalConstants::Instance()->TurnsTaken() << G4endl;
#endif
      G4int turnsToTake = BDSGlobalConstants::Instance()->TurnsToTake();
      G4double fraction = BDSGlobalConstants::Instance()->PrintModuloFraction();
      G4int printModulo = (G4int)ceil(turnsToTake * fraction);

      G4int turnstaken = BDSGlobalConstants::Instance()->TurnsTaken();
      if (turnstaken%printModulo == 0)
	{
	  // save flags since G4cout flags are changed
	  std::ios_base::fmtflags ff = G4cout.flags();
	  G4cout << "Turn: " << std::right << std::setw(4) << std::fixed
		 << turnstaken+1 << " / " << std::left
		 << BDSGlobalConstants::Instance()->TurnsToTake() << G4endl;
	  // reset flags
	  G4cout.flags(ff);
	}
      
      BDSGlobalConstants::Instance()->IncrementTurnNumber();
      
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "New turn number : " << BDSGlobalConstants::Instance()->TurnsTaken() << G4endl;
#endif
    }
#ifdef BDSDEBUG
  else
    {G4cout << __METHOD_NAME__ << "not incrementing turn number" << G4endl;}
#endif
  return true;
}


void BDSTerminatorSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
  BDSGlobalConstants::Instance()->ResetTurnNumber();
}
