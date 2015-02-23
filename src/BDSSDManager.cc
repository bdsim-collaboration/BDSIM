#include "BDSSDManager.hh"

#include "globals.hh"              // geant4 types / globals
#include "G4SDManager.hh"

#include "BDSDebug.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSSamplerSD.hh"



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
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " Constructor - creating all necessary Sensitive Detectors" << G4endl;
#endif
  //instantiate all necessary SD classes
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();

  //sampler plane
  samplerPlane = new BDSSamplerSD("sampler_plane","plane");
  SDMan->AddNewDetector(samplerPlane);

  //sampler cylindrical
  samplerCylinder = new BDSSamplerSD("sampler_cylinder","cylinder");
  SDMan->AddNewDetector(samplerCylinder);

  //on axis energy counter
  eCounterOnAxis = new BDSEnergyCounterSD("ec_on_axis");
  SDMan->AddNewDetector(eCounterOnAxis);

}
