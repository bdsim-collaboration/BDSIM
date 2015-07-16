#ifndef BDSSDMANAGER_H
#define BDSSDMANAGER_H

#include "BDSEnergyCounterSD.hh"
#include "BDSLWCalorimeterSD.hh"
#include "BDSSamplerSD.hh"
#include "BDSTerminatorSD.hh"

/**
 * @brief A singleton class that holds all required sensitive
 * detector class instances.  Each sensitive detector class
 * need only be instantiated once and attached to the relevant
 * volume. More factorised geometry construction will mean
 * SDs will be assigned in construction and not in detector
 * construciton afterwards as a post processing step. This 
 * instantiates all necessary SDs and holds them.
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */

class BDSSDManager
{
public:
  static BDSSDManager* Instance(); /// singleton accessor

  ~BDSSDManager();

  /// SD for samplers (plane type)
  inline BDSSamplerSD*       GetSamplerPlaneSD() const;

  /// SD for samplers (cylinder type)
  inline BDSSamplerSD*       GetSamplerCylinderSD() const;

  /// SD for any component that's symmetric about the beam axis
  inline BDSEnergyCounterSD* GetEnergyCounterOnAxisSD() const;

  /// SD for measuring turns around circular machine and terminating
  /// particles appropriately.
  inline BDSTerminatorSD*    GetTerminatorSD() const;
  
  /// SD for any component that's symmetric about the beam axis and
  /// uses the read out geometry
  BDSEnergyCounterSD* GetEnergyCounterOnAxisSDRO();

  /// SD for a laserwire calorimeter
  BDSLWCalorimeterSD* GetLWCalorimeterSD() const;
  
private:
  BDSSDManager(); /// private default constructor for singleton
  static BDSSDManager* _instance;

  //SD instances
  BDSSamplerSD*       samplerPlane;
  BDSSamplerSD*       samplerCylinder;
  BDSEnergyCounterSD* eCounterOnAxis;
  BDSTerminatorSD*    terminator;
  BDSLWCalorimeterSD* lwCalorimeter;

  // duplicate ecounter here that's made on demand
  // and registered to read out geometry - this requires
  // the readout geometry to be already created so can't
  // be used during object construction
  BDSEnergyCounterSD* eCounterOnAxisRO;

  /// function to create the energy counter SD on demand
  void ConstructECounterSDOnAxisOnDemand();
};

inline BDSSamplerSD*       BDSSDManager::GetSamplerPlaneSD() const
{return samplerPlane;}

inline BDSSamplerSD*       BDSSDManager::GetSamplerCylinderSD() const
{return samplerCylinder;}

inline BDSEnergyCounterSD* BDSSDManager::GetEnergyCounterOnAxisSD() const
{return eCounterOnAxis;}

inline BDSTerminatorSD*    BDSSDManager::GetTerminatorSD() const
{return terminator;}

inline BDSLWCalorimeterSD* BDSSDManager::GetLWCalorimeterSD() const
{return lwCalorimeter;}

#endif
