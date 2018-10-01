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
#include "BDSVolumeExitHit.hh"
#include "BDSVolumeExitSD.hh"

#include "globals.hh"
#include "G4SDManager.hh"
#include "G4StepPoint.hh"
#include "G4StepStatus.hh"

BDSVolumeExitSD::BDSVolumeExitSD(G4String name,
				 G4bool   worldExitIn,
				 G4bool   verboseIn):
  G4VSensitiveDetector("volume_exit/" + name),
  colName(name),
  HCIDve(-1),
  verbose(verboseIn),
  collection(nullptr)
{
  collectionName.insert(name);

  statusToMatch = worldExitIn ? G4StepStatus::fWorldBoundary : fGeomBoundary;
}

void BDSVolumeExitSD::Initialize(G4HCofThisEvent* HCE)
{
  collection = new BDSVolumeExitHitsCollection(GetName(), colName);
  if (HCIDve < 0)
    {HCIDve = G4SDManager::GetSDMpointer()->GetCollectionID(collection);}
  HCE->AddHitsCollection(HCIDve, collection);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Hits Collection ID: " << HCIDve << G4endl;
#endif
}

G4bool BDSVolumeExitSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*th*/)
{
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

  if (postStepPoint->GetStepStatus() == statusToMatch)
    {
      //BDSEnergyCounterSD::ProcessHits(step, th);
      return true;
    }
  else
    {return false;}
}
