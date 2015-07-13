#include "BDSSDManager.hh"

#include "globals.hh"              // geant4 types / globals
#include "G4SDManager.hh"

#include "BDSDebug.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSLWCalorimeterSD.hh"
#include "BDSSamplerSD.hh"
#include "BDSTerminatorSD.hh"
#include "BDSReadOutGeometry.hh"

#include "G4VReadOutGeometry.hh"

BDSSDManager* BDSSDManager::_instance = 0;

BDSSDManager* BDSSDManager::Instance()
{
  if (_instance == 0)
    {_instance = new BDSSDManager();}
  return _instance;
}

BDSSDManager::~BDSSDManager()
{
  delete samplerPlane;
  delete samplerCylinder;
  delete eCounterOnAxis;
  delete terminator;
  delete eCounterOnAxisRO;
}

BDSSDManager::BDSSDManager()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Constructor - creating all necessary Sensitive Detectors" << G4endl;
#endif
  //instantiate all necessary SD classes
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();

  //read out geometry SD - construct on demand
  eCounterOnAxisRO = NULL;

  //sampler plane
  samplerPlane = new BDSSamplerSD("sampler_plane","plane");
  SDMan->AddNewDetector(samplerPlane);

  //sampler cylindrical
  samplerCylinder = new BDSSamplerSD("sampler_cylinder","cylinder");
  SDMan->AddNewDetector(samplerCylinder);

  //on axis energy counter - uses read out geometry
  eCounterOnAxis = new BDSEnergyCounterSD("ec_on_axis");
  SDMan->AddNewDetector(eCounterOnAxis);
  
  terminator  = new BDSTerminatorSD("terminator");
  SDMan->AddNewDetector(terminator);

  lwCalorimeter = new BDSLWCalorimeterSD("lw_calorimeter");
  SDMan->AddNewDetector(lwCalorimeter);
}

BDSEnergyCounterSD* BDSSDManager::GetEnergyCounterOnAxisSDRO()
{
  if (!eCounterOnAxisRO)
    {ConstructECounterSDOnAxisOnDemand();}
  return eCounterOnAxisRO;
}

void BDSSDManager::ConstructECounterSDOnAxisOnDemand()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // this CANNOT be used until the beamline has been
  // placed in detector construction and therefore the readout
  // geometry also exists - hence make this on demand
  // the SDManager will be called early on to make samplers during
  // object construction
  // on axis energy counter - uses read out geometry
  eCounterOnAxisRO = new BDSEnergyCounterSD("ec_on_axis_read_out");
  BDSReadOutGeometry* roGeom = new BDSReadOutGeometry("readOutGeometry");
  // although unnecessary for bdsim this MUST be called for geant4 to
  // register things properly
  // this method actually invokes roGeom->Build() which we have to implement
  // but geant4 must do this - so messy!  
  roGeom->BuildROGeometry();
  eCounterOnAxisRO->SetROgeometry(roGeom); // attach the read out geometry to this SD
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  SDMan->AddNewDetector(eCounterOnAxisRO);
}
