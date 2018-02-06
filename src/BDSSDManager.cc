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
#include "BDSEnergyCounterSD.hh"
#include "BDSSamplerSD.hh"
#include "BDSSDManager.hh"
#include "BDSTerminatorSD.hh"

#include "G4SDManager.hh"

BDSSDManager* BDSSDManager::_instance = nullptr;

BDSSDManager* BDSSDManager::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSSDManager();}
  return _instance;
}

BDSSDManager::~BDSSDManager()
{
  // no need to delete SD's as they are all registered in G4SDManager
  _instance = nullptr;
}

BDSSDManager::BDSSDManager()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Constructor - creating all necessary Sensitive Detectors" << G4endl;
#endif
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
  
  eCounter = new BDSEnergyCounterSD("general");
  SDMan->AddNewDetector(eCounter);

  tunnelECounter = new BDSEnergyCounterSD("tunnel");
  SDMan->AddNewDetector(tunnelECounter);
}
