#ifndef BDSSDMANAGER_H
#define BDSSDMANAGER_H

#include "BDSSamplerSD.hh"
#include "BDSEnergyCounterSD.hh"

/**
 * @brief A singleton class that holds all required sensitive
 * detector class instances.  Each sensitive detector class
 * need only be instantiated once and attached to the relevant
 * volume. More factorised geometry construction will mean
 * SDs will be assigned in construction and not in detector
 * construciton afterwards as a post processing step. This 
 * instantiates all necessary SDs and holds them.
 */

class BDSSDManager
{
public:
  static BDSSDManager* Instance(); /// singleton accessort

  ~BDSSDManager();

  /// SD for samplers (plane type)
  BDSSamplerSD*       GetSamplerPlaneSD() {return samplerPlane;}
  /// SD for samplers (cylinder type)
  BDSSamplerSD*       GetSamplerCylinderSD() {return samplerCylinder;} 
  /// SD for any component that's symmetric about the beam axis
  BDSEnergyCounterSD* GetEnergyCounterOnAxisSD(){return eCounterOnAxis;}
  /// SD for any component that's symmetric about the beam axis and
  /// uses the read out geometry
  BDSEnergyCounterSD* GetEnergyCounterOnAxisSDRO();
  
private:
  BDSSDManager(); /// private default constructor for singleton
  static BDSSDManager* _instance;

  //SD instances
  BDSSamplerSD*       samplerPlane;
  BDSSamplerSD*       samplerCylinder;
  BDSEnergyCounterSD* eCounterOnAxis;

  // duplicate ecounter here that's made on demand
  // and registered to read out geometry - this requires
  // the readout geometry to be already created so can't
  // be used during object construction
  BDSEnergyCounterSD* eCounterOnAxisRO;

  /// function to create the energy counter SD on demand
  void ConstructECounterSDOnAxisOnDemand();
};




#endif
