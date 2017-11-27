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
#ifndef BDSEVENTACTION_H
#define BDSEVENTACTION_H

#include "globals.hh" // geant4 types / globals
#include "G4UserEventAction.hh"

#include <ctime>
#include <string>

class BDSEventInfo;
class BDSOutput;
class G4Event;
class G4PrimaryVertex;

/**
 * @brief Process information at the event level.
 */

class BDSEventAction: public G4UserEventAction
{
public:
  explicit BDSEventAction(BDSOutput* outputIn);
  virtual ~BDSEventAction();
  
public:
  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

  void StoreSeedState(G4String seedState) {seedStateAtStart = seedState;}
    
private:
  BDSOutput* output;         ///< Cache of output instance. Not owned by this class.
  G4bool verboseEvent;       ///< Copy of BDSGlobalConstants::VerboseEvent()
  G4int  verboseEventNumber; ///< Copy of BDSGlobalConstants::VerboseEventNumber()
  G4bool isBatch;
  G4bool storeTrajectory;    ///< Cache of whether to store trajectories or not.
  G4int  printModulo;

  G4int samplerCollID_plane;
  G4int samplerCollID_cylin;
  G4int energyCounterCollID; ///< Collection ID for energy deposition hits for read out geometry.
  G4int tunnelEnergyCounterCollID; ///< Collection ID for the tunnel energy deposition.
  
  time_t startTime; ///< Time at the start of the event.
  time_t stopTime;  ///< Time at the end of the event.

  G4double starts; ///< Precise start time in seconds.
  G4double stops;  ///< Precise stop time in seconds.

  std::string seedStateAtStart; ///< Seed state at start of the event.

  /// A copy of the pointer to event info instance that is registered to the event. Geant4
  /// deletes this as necessary.
  BDSEventInfo* eventInfo;
};

#endif

