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
#ifndef BDSTRACKINGACTION_H
#define BDSTRACKINGACTION_H

#include "globals.hh" // geant4 types / globals
#include "G4UserTrackingAction.hh"

class G4Track;

/**
 * @brief Action to decide whether or not to store trajectory information.
 */

class BDSTrackingAction: public G4UserTrackingAction
{
public:
  BDSTrackingAction();
  explicit BDSTrackingAction(G4bool batchMode);
  
  virtual ~BDSTrackingAction(){;}

  /// Used to decide whether or not to store trajectories.
  virtual void PreUserTrackingAction(const G4Track* track);

private:
  /// Whether we're using the visualiser - in which case always store
  /// trajectories for visualisation.
  G4bool interactive;

  /// Flag from global constants to control storing all trajectories. Cache
  /// this from global constants at the beginning as it doesn't change.
  G4bool storeTrajectory;
};

#endif
