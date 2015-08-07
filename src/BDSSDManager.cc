#include "BDSSDManager.hh"

#include "globals.hh"              // geant4 types / globals
#include "G4SDManager.hh"

#include "BDSDebug.hh"
#include "BDSEnergyCounterSD.hh"
#include "BDSLWCalorimeterSD.hh"
#include "BDSReadOutGeometry.hh"
#include "BDSSamplerSD.hh"
#include "BDSTerminatorSD.hh"
#include "BDSTunnelReadOutGeometry.hh"
#include "BDSTunnelSD.hh"

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
  // no need to delete SD's as they are all registered in G4SDManager
  _instance = 0;
}

BDSSDManager::BDSSDManager()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Constructor - creating all necessary Sensitive Detectors" << G4endl;
#endif
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();

  // read out geometry SD - construct on demand
  eCounterOnAxisRO = nullptr;

  // sampler plane
  samplerPlane = new BDSSamplerSD("sampler_plane","plane");
  SDMan->AddNewDetector(samplerPlane);

  // Sampler cylindrical
  samplerCylinder = new BDSSamplerSD("sampler_cylinder","cylinder");
  SDMan->AddNewDetector(samplerCylinder);

  // Terminator sd to measure how many times that primary has passed through the terminator
  terminator  = new BDSTerminatorSD("terminator");
  SDMan->AddNewDetector(terminator);

  // Lw calorimeter
  lwCalorimeter = new BDSLWCalorimeterSD("lw_calorimeter");
  SDMan->AddNewDetector(lwCalorimeter);

  // Tunnel read out SD - constructed on demand - uses tunnel read out geometry
  tunnelOnAxisRO = nullptr;
}

BDSEnergyCounterSD* BDSSDManager::GetEnergyCounterOnAxisSDRO()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if (!eCounterOnAxisRO)
    {ConstructECounterSDOnAxisOnDemand();}
  return eCounterOnAxisRO;
}

BDSTunnelSD* BDSSDManager::GetTunnelOnAxisSDRO()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  if (!tunnelOnAxisRO)
    {ConstructTunnelSDOnAxisOnDemand();}
  return tunnelOnAxisRO;
}

void BDSSDManager::ConstructECounterSDOnAxisOnDemand()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // this CANNOT be used until the beamline has been
  // placed in detector construction and therefore the readout
  // geometry also exists - hence make this on demand.
  // The SDManager will be called early on to make samplers during
  // object construction when the beamline won't exist which wouldn't work.
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

void BDSSDManager::ConstructTunnelSDOnAxisOnDemand()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // this CANNOT be used until the beamline has been
  // placed in detector construction and therefore the readout
  // geometry also exists - hence make this on demand.
  // The SDManager will be called early on to make samplers during
  // object construction when the beamline won't exist which wouldn't work.
  // on axis tunnel SD - uses read out geometry
  tunnelOnAxisRO = new BDSTunnelSD("tunnel_on_axis");
  BDSTunnelReadOutGeometry* roGeom = new BDSTunnelReadOutGeometry("tunnelReadOutGeometry");
  // although unnecessary for bdsim this MUST be called for geant4 to
  // register things properly
  // this method actually invokes roGeom->Build() which we have to implement
  // but geant4 must do this - so messy!  
  roGeom->BuildROGeometry();
  tunnelOnAxisRO->SetROgeometry(roGeom); // attach the read out geometry to this SD
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  SDMan->AddNewDetector(tunnelOnAxisRO);
}
