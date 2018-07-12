/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#include "BDSTrackingAction.hh"
#include "BDSTrajectory.hh"

#include "globals.hh" // geant4 types / globals
#include "G4TrackingManager.hh"
#include "G4Track.hh"

BDSTrackingAction::BDSTrackingAction(const G4bool& batchMode,
				     const G4bool& storeTrajectoryIn):
  interactive(!batchMode),
  storeTrajectory(storeTrajectoryIn)
{;}

void BDSTrackingAction::PreUserTrackingAction(const G4Track* track)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " TrackID=" << track->GetTrackID()
	 << " ParentID=" << track->GetParentID() << G4endl;
#endif

  // we only create a trajectory if we're in interactive mode
  // (for visualisation), if storeTrajectory is on, or it's the primary
  // particle.
  if (interactive || storeTrajectory || track->GetParentID() == 0)
    {
      fpTrackingManager->SetStoreTrajectory(1);
      BDSTrajectory* bdsTraj = new BDSTrajectory(track,interactive);
      fpTrackingManager->SetTrajectory(bdsTraj);
    }
  else
    {fpTrackingManager->SetStoreTrajectory(0);}
}
