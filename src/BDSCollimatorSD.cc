/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include "BDSBeamline.hh"
#include "BDSCollimatorSD.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSStep.hh"

#include "globals.hh" // geant4 types / globals
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"

#include <map>
#include <vector>

BDSCollimatorSD::BDSCollimatorSD(G4String name):
  BDSSensitiveDetector("collimator/" + name),
  collimatorCollection(nullptr),
  itsCollectionName(name),
  itsHCID(-1),
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
}

G4bool BDSCollimatorSD::ProcessHits(G4Step* step, G4TouchableHistory* rOHist)
{
  std::vector<G4VHit*> hits;
  return ProcessHitsOrdered(step, rOHist, hits);
}

G4bool BDSCollimatorSD::ProcessHitsOrdered(G4Step* step,
					   G4TouchableHistory* /*rOHist*/,
					   const std::vector<G4VHit*>& hits)
{
  G4VHit* lastHit = nullptr;
  BDSEnergyCounterHit* lastHitEDep = nullptr;
  if (!hits.empty())
    {
      lastHit = hits.back();
      lastHitEDep = dynamic_cast<BDSEnergyCounterHit*>(lastHit);
    }

  // get pre step point in local coordinates.
  BDSStep stepLocal = auxNavigator->ConvertToLocal(step);
  G4StepPoint* preStepPoint = step->GetPreStepPoint();
  G4ThreeVector preStepPosLocal = stepLocal.PreStepPoint();
  G4ThreeVector momGlobal       = preStepPoint->GetMomentumDirection();
  // uses existing transform cached in aux navigator here
  G4ThreeVector preStepMomLocal = auxNavigator->ConvertAxisToLocal(momGlobal);

  G4double totalEnergy = preStepPoint->GetTotalEnergy();

  // get which beam line it's in and the index
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(stepLocal.VolumeForTransform());

  // map the index to a collimator index
  BDSBeamline* beamline = nullptr;
  G4int beamlineIndex   = -1;
  G4int collimatorIndex = -1;

  if (theInfo)
    {
      beamlineIndex = theInfo->GetBeamlineIndex();
      beamline      = theInfo->GetBeamline();
  
      auto beamlineMap = mapping.find(beamline);
      if (beamlineMap != mapping.end())
	{
	  const auto& indexMap = beamlineMap->second;
	  collimatorIndex = indexMap.at(beamlineIndex);
	}
      else
	{// create map for this beam line
	  std::map<G4int, G4int> indexMap;
	  std::vector<G4int> collimatorIndices = beamline->GetIndicesOfCollimators();
	  for (G4int i = 0; i < (G4int)collimatorIndices.size(); i++)
	    {indexMap[i] = collimatorIndices[i];}
	  mapping[beamline] = indexMap;
	  collimatorIndex = indexMap.at(beamlineIndex);
	}
    }
  
  BDSCollimatorHit* hit = new BDSCollimatorHit(beamline,
					       collimatorIndex,
					       preStepPosLocal,
					       preStepMomLocal,
					       totalEnergy,
					       lastHitEDep);

  collimatorCollection->insert(hit);
  return true;
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
