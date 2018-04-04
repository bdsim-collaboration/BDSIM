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
#ifndef BDSENERGYCOUNTERSD_H
#define BDSENERGYCOUNTERSD_H

#include "BDSEnergyCounterHit.hh"

#include "G4VSensitiveDetector.hh"

class BDSAuxiliaryNavigator;

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

/**
 * @brief Generates BDSEnergyCounterHits from step information - uses curvilinear coords.
 *
 * This class interrogates a G4Step and generates an energy deposition hit if there was
 * a change in energy. This assigns the energy deposition to a point randomly (uniformly)
 * along the step.  It also uses a BDSAuxiliaryNavigator instance to use transforms from
 * the curvilinear parallel world for curvilinear coordinates.
 */

class BDSEnergyCounterSD: public G4VSensitiveDetector
{
public:
  BDSEnergyCounterSD(G4String name,
		     G4bool   stopSecondariesIn,
		     G4bool   verboseIn = false);
  virtual ~BDSEnergyCounterSD();

  virtual void Initialize(G4HCofThisEvent*HCE);
  virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
  
private:
  /// assignment and copy constructor not implemented nor used
  BDSEnergyCounterSD& operator=(const BDSEnergyCounterSD&);
  BDSEnergyCounterSD(BDSEnergyCounterSD&);
  BDSEnergyCounterSD() = delete;

  G4bool   stopSecondaries; ///< Cache of whether secondaries are stopped.
  G4bool   verbose;         ///< Cache of whether verbose output.
  G4String colName;         ///< Collection name.
  BDSEnergyCounterHitsCollection* energyCounterCollection;
  G4int    HCIDe;

  ///@{ per hit variable
  G4double enrg;
  G4double weight;
  G4double X,Y,Z,sBefore,sAfter; // global coordinates
  G4double x,y,z;   // local coordinates
  G4double globalTime; // time since start of event
  G4double stepLength;
  G4int    ptype;
  G4int    trackID;
  G4int    parentID;
  G4String volName;
  G4int    turnstaken;
  G4int    eventnumber;
  ///@}

  /// Navigator for checking points in read out geometry
  BDSAuxiliaryNavigator* auxNavigator;
};

#endif

