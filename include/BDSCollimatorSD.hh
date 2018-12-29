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
#ifndef BDSCOLLIMATORSD_H
#define BDSCOLLIMATORSD_H

#include "BDSCollimatorHit.hh"
#include "BDSSensitiveDetector.hh"

#include "globals.hh" // geant4 types / globals

class BDSAuxiliaryNavigator;
class BDSEnergyCounterHit;
class BDSGlobalConstants;

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/**
 * @brief The sensitive detector class that provides sensitivity to collimators instances.
 *
 * This class creates BDSCollimatorHits for each collimator this SD is
 * attached to.
 * 
 * @author Laurie Nevay
 */

class BDSCollimatorSD: public BDSSensitiveDetector
{ 
public:
  /// Construct a sampler with name and type (plane/cylinder).
  explicit BDSCollimatorSD(G4String name,
			   G4bool storeHitsForIonsIn,
			   G4bool storeHitsForAllIn);
  virtual ~BDSCollimatorSD();

  /// Overriden from G4VSensitiveDetector. Creates hits collection and registers it with
  /// the hits collection of this event (HCE).
  virtual void Initialize(G4HCofThisEvent* HCE);

  /// Overriden from G4VSensitiveDetector. Creates hit instances and appends them to the
  /// hits collection.
  virtual G4bool ProcessHits(G4Step* aStep,
			     G4TouchableHistory* readOutTH);

  /// Separate interface where the already generated energy deposition hit for this step
  /// is provided to be recorded with the collimator hit.
  G4bool ProcessHits(G4Step* step,
		     G4TouchableHistory* readOutTH,
		     BDSEnergyCounterHit* energyDepositionHit);

  virtual G4VHit* last() const;

private:
  inline bool IsIon(const int& pdgID) const {return pdgID > 1000000000;}
  
  /// The hits collection for this sensitive detector class that's owned by each instance.
  BDSCollimatorHitsCollection* collimatorCollection;

  /// The name of the hits collection that's created and registered.
  G4String itsCollectionName;

  /// Hits collection ID - an integer look up for the hits collection
  /// provided by G4SDManager (a registry) that is given to the
  /// G4HCofThisEvent (Hits collection of the event).
  int itsHCID;

  /// Cached pointer to global constants as accessed many times
  BDSGlobalConstants* globals;

  G4bool storeHitsForIons;
  G4bool storeHitsForAll;
  BDSAuxiliaryNavigator* auxNavigator;
};

#endif

