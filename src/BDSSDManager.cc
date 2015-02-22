#include "BDSSDManager.hh"

#include "G4SDManager.hh"

#include "BDSSamplerSD.hh"
#include "BDSEnergyCounterSD.hh"


BDSSDManager* BDSSDManager::_instance = 0;

BDSSDManager* BDSSDManager::Instance()
{
  if (_instance == 0)
    {_instance = new BDSSDManager();}
  return _instance;
}

BDSSDManager::~BDSSDManager()
{;}

BDSSDManager::BDSSDManager()
{
  //instantiate all necessary SD classes
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();

  //sampler plane
  samplerPlane = new BDSSamplerSD("sampler","plane");
  SDMan->AddNewDetector(samplerPlane);

  //sampler cylindrical
  samplerCylinder = new BDSSamplerSD("sampler","cylinder");
  SDMan->AddNewDetector(samplerCylinder);

  //on axis energy counter
  eCounterOnAxis = new BDSEnergyCounterSD("ec_on_axis");
  SDMan->AddNewDetector(eCounterOnAxis);

}
