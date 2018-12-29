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
#include "BDSAuxiliaryNavigator.hh"
#include "BDSCollimatorSD.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AffineTransform.hh"
#include "G4DynamicParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"

#include <vector>

BDSCollimatorSD::BDSCollimatorSD(G4String name,
				 G4bool storeHitsForIonsIn,
				 G4bool storeHitsForAllIn):
  BDSSensitiveDetector("collimator/" + name),
  collimatorCollection(nullptr),
  itsCollectionName(name),
  itsHCID(-1),
  globals(nullptr),
  storeHitsForIons(storeHitsForIonsIn),
  storeHitsForAll(storeHitsForAllIn),
  auxNavigator(new BDSAuxiliaryNavigator())
{
  collectionName.insert(name);
}

BDSCollimatorSD::~BDSCollimatorSD()
{
  delete auxNavigator;
}

void BDSCollimatorSD::Initialize(G4HCofThisEvent* HCE)
{
  // Create Collimator hits collection
  collimatorCollection = new BDSCollimatorHitsCollection(GetName(), itsCollectionName);

  // Record id for use in EventAction to save time - slow string lookup by collection name
  if (itsHCID < 0)
    {itsHCID = G4SDManager::GetSDMpointer()->GetCollectionID(collimatorCollection);}
  HCE->AddHitsCollection(itsHCID, collimatorCollection);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Hits Collection ID: " << HCIDe << G4endl;
#endif
  
  globals  = BDSGlobalConstants::Instance(); // cache pointer to globals
}

G4bool BDSCollimatorSD::ProcessHits(G4Step* step, G4TouchableHistory* /*readOutTH*/)
{
  return false;
}

G4VHit* BDSCollimatorSD::last() const
{
  auto hitsVector = collimatorCollection->GetVector();
  if (hitsVector->empty())
    {return nullptr;}
  else
    {
      BDSCollimatorHit* lastHit = hitsVector->back();
      return dynamic_cast<G4VHit*>(lastHit);
    }
}
