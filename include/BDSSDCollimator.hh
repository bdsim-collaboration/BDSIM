/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2023.

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
#ifndef BDSSDCOLLIMATOR_H
#define BDSSDCOLLIMATOR_H

#include "BDSHitCollimator.hh"
#include "BDSSensitiveDetector.hh"

#include "globals.hh" // geant4 types / globals
#include "G4String.hh"

#include <map>
#include <vector>

class BDSAuxiliaryNavigator;
class BDSHitEnergyDeposition;

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
class G4VHit;

/**
 * @brief The sensitive detector class that provides sensitivity to collimators instances.
 *
 * This class creates BDSHitCollimators for each collimator this SD is
 * attached to.
 * 
 * This class is designed to work with BDSSDEnergyDepositionB in an ordered multi SD. If not,
 * a nullptr will be stored for the energy deposition hit.
 * 
 * @author Laurie Nevay
 */

class BDSSDCollimator: public BDSSensitiveDetector
{ 
public:
  /// Include unique name for each instance.
  explicit BDSSDCollimator(G4String name);

  /// @{ Assignment and copy constructor not implemented nor used
  BDSSDCollimator& operator=(const BDSSDCollimator&) = delete;
  BDSSDCollimator(BDSSDCollimator&) = delete;
  /// @}

  virtual ~BDSSDCollimator();

  /// Overriden from G4VSensitiveDetector. Creates hits collection and registers it with
  /// the hits collection of this event (HCE).
  virtual void Initialize(G4HCofThisEvent* HCE) override;

  /// Overriden from G4VSensitiveDetector. Creates hit instances and appends them to the
  /// hits collection.
  virtual G4bool ProcessHits(G4Step* aStep,
			     G4TouchableHistory* rOHist) override;

  /// Override creation of hit w.r.t. hit already generated by other SD, namely the
  /// energy deposition SD for collimator hits.
  virtual G4bool ProcessHitsOrdered(G4Step* step,
				    G4TouchableHistory*  rOHist,
				    const std::vector<G4VHit*>& hits) override;

  /// Return the last collimator hit.
  virtual G4VHit* last() const override;

private:
  inline bool IsIon(const int& pdgID) const {return pdgID > 1000000000;}
  
  /// The hits collection for this sensitive detector class that's owned by each instance.
  BDSHitsCollectionCollimator* collimatorCollection;

  /// The name of the hits collection that's created and registered.
  G4String itsCollectionName;

  /// Hits collection ID - an integer look up for the hits collection
  /// provided by G4SDManager (a registry) that is given to the
  /// G4HCofThisEvent (Hits collection of the event).
  G4int itsHCID;

  /// An auxiliary navigator object for coordinate transforms.
  BDSAuxiliaryNavigator* auxNavigator;

  /// Map of beam line index to count of that object - ie beam line object 1203
  /// is the 3rd collimator. Store a map of this for each possible beam line.
  /// Populated dynamically as requested.
  std::map<BDSBeamline*, std::map<G4int, G4int> > mapping;
};

#endif

