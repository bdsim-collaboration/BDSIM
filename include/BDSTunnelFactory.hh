#ifndef BDSTUNNELFACTORY_H
#define BDSTUNNELFACTORY_H

#include "BDSGeometryComponent.hh"
#include "BDSTunnelFactoryBase.hh"
#include "BDSTunnelType.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4Material.hh"

class BDSTunnelFactory
{
public:
  static BDSTunnelFactory* Instance(); ///< singleton pattern
  
  ~BDSTunnelFactory();

  BDSGeometryComponent* BuildTunnelSection(BDSTunnelType tunnelType,
					   G4double      length,
					   G4double      tunnelThickness,
					   G4double      tunnelSoilThickness,
					   G4Material*   tunnelMaterial,
					   G4Material*   tunnelSoilMaterial,
					   G4bool        tunnelFloor,
					   G4double      tunnelFloorOffset,
					   G4double      tunnel1,
					   G4double      tunnel2);

private:
  /// Private constructor as a singleton
  BDSTunnelFactory();

  /// Singleton instance
  static BDSTunnelFactory* _instance;

  /// Accessor method to dispatch to appropriate derived factory
  BDSTunnelFactoryBase* GetAppropriateFactory(BDSTunnelType tunnelType);

};



#endif
