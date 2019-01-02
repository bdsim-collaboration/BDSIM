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
#include "BDSCollimatorSD.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMultiSensitiveDetectorOrdered.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDFilterIon.hh"
#include "BDSSDFilterOr.hh"
#include "BDSSDFilterPrimary.hh"
#include "BDSSDManager.hh"
#include "BDSSDType.hh"
#include "BDSTerminatorSD.hh"
#include "BDSVolumeExitSD.hh"

#include "G4SDManager.hh"
#include "G4Version.hh"

#if G4VERSION_NUMBER > 1029
#include "G4MultiSensitiveDetector.hh"
#endif

BDSSDManager* BDSSDManager::instance = nullptr;

BDSSDManager* BDSSDManager::Instance()
{
  if (!instance)
    {instance = new BDSSDManager();}
  return instance;
}

BDSSDManager::~BDSSDManager()
{
  // no need to delete SD's as they are all registered in G4SDManager
  instance = nullptr;

  for (auto kv : filters)
    {delete kv.second;}
}

BDSSDManager::BDSSDManager()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Constructor - creating all necessary Sensitive Detectors" << G4endl;
#endif
  filters["primary"] = new BDSSDFilterPrimary("primary");
  filters["ion"]     = new BDSSDFilterIon("ion");
  BDSSDFilterOr* primaryOrIon = new BDSSDFilterOr("primary_or_ion");
  primaryOrIon->RegisterFilter(filters["primary"]);
  primaryOrIon->RegisterFilter(filters["ion"]);
  filters["primaryorion"] = primaryOrIon;
  
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  
  // sampler plane
  samplerPlane = new BDSSamplerSD("plane");
  SDMan->AddNewDetector(samplerPlane);

  // Sampler cylindrical
  samplerCylinder = new BDSSamplerSD("cylinder");
  SDMan->AddNewDetector(samplerCylinder);

  // Terminator sd to measure how many times that primary has passed through the terminator
  terminator  = new BDSTerminatorSD("terminator");
  SDMan->AddNewDetector(terminator);

  G4bool stopSecondaries = BDSGlobalConstants::Instance()->StopSecondaries();
  G4bool verbose         = BDSGlobalConstants::Instance()->Verbose();

  eCounter = new BDSEnergyCounterSD("general", stopSecondaries, verbose);
  SDMan->AddNewDetector(eCounter);

  eCounterVacuum = new BDSEnergyCounterSD("vacuum", stopSecondaries, verbose);
  SDMan->AddNewDetector(eCounterVacuum);

  eCounterTunnel = new BDSEnergyCounterSD("tunnel", stopSecondaries, verbose);
  SDMan->AddNewDetector(eCounterTunnel);

  eCounterWorld = new BDSEnergyCounterSD("worldLoss", stopSecondaries, verbose);
  SDMan->AddNewDetector(eCounterWorld);

  worldExit= new BDSVolumeExitSD("worldExit", true);
  SDMan->AddNewDetector(worldExit);

#if G4VERSION_NUMBER > 1029
  // only multiple SDs since 10.3
  G4MultiSensitiveDetector* wcsd = new G4MultiSensitiveDetector("world_complete");
  SDMan->AddNewDetector(wcsd);
  wcsd->AddSD(eCounterWorld);
  wcsd->AddSD(worldExit);
  worldCompleteSD = wcsd;
#endif

  collimatorSD = new BDSCollimatorSD("collimator");
  collimatorCompleteSD = new BDSMultiSensitiveDetectorOrdered("collimator_complete");
  collimatorCompleteSD->AddSD(eCounter);
  collimatorCompleteSD->AddSD(collimatorSD);
  // set up a filter for the collimator sensitive detector - always store primary hits
  G4VSDFilter* filter = nullptr;
  G4bool storeAll  = BDSGlobalConstants::Instance()->StoreCollimatorHitsAll();
  G4bool storeIons = BDSGlobalConstants::Instance()->StoreCollimatorHitsIons();
  if (storeAll)
    {filter = nullptr;} // no filter -> store all
  else if (storeIons) // primaries plus ion fragments
    {filter = filters["primaryorion"];}
  else
    {filter = filters["primary"];} // just primaries

  // we only want to attach the filter to the collimator SD and not the energy counter SD
  // via the 'complete' SD. i.e. we always want energy deposition but collimator hits by
  // the filter.
  collimatorSD->SetFilter(filter);
  SDMan->AddNewDetector(collimatorSD);
  SDMan->AddNewDetector(collimatorCompleteSD);
}

G4VSensitiveDetector* BDSSDManager::SensitiveDetector(const BDSSDType sdType) const
{
  switch (sdType.underlying())
    {
    case BDSSDType::samplerplane:
      {return samplerPlane; break;}
    case BDSSDType::samplercylinder:
      {return samplerCylinder; break;}
    case BDSSDType::terminator:
      {return terminator; break;}
    case BDSSDType::energydep:
      {return eCounter; break;}
    case BDSSDType::energydepvacuum:
      {return eCounterVacuum; break;}
    case BDSSDType::energydeptunnel:
      {return eCounterTunnel; break;}
    case BDSSDType::energydepworld:
      {return eCounterWorld; break;}
    case BDSSDType::worldexit:
      {return worldExit; break;}
    case BDSSDType::worldcomplete:
#if G4VERSION_NUMBER > 1029
      {return worldCompleteSD; break;}
#else
      {return nullptr; break;}
#endif
    case BDSSDType::collimator:
      {return collimatorSD; break;}
    case BDSSDType::collimatorcomplete:
      {return collimatorCompleteSD; break;}
    default:
      {return nullptr; break;}
    }
}
