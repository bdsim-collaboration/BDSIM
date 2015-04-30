#ifndef BDSTUNNELFACTORYELLIPTICAL_H
#define BDSTUNNELFACTORYELLIPTICAL_H

#include "BDSTunnelFactoryBase.hh"
#include "BDSGeometryComponent.hh"

#include "globals.hh" // geant4 globals / types

/**
 * @brief Factory for elliptical tunnel segments
 * 
 * singleton pattern
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSTunnelFactoryElliptical: public BDSTunnelFactoryBase
{
public:
  static BDSTunnelFactoryElliptical* Instance(); /// singleton accessor
  
  virtual ~BDSTunnelFactoryElliptical();

  virtual BDSGeometryComponent* BuildTunnelSection(G4double      length,
						   G4double      tunnelThickness,
						   G4double      tunnelSoilThickness,
						   G4Material*   tunnelMaterial,
						   G4Material*   tunnelSoilMaterial,
						   G4bool        tunnelFloor,
						   G4double      tunnelFloorOffset,
						   G4double      tunnel1,
						   G4double      tunnel2);

private:
  /// Private constructor - singelton pattern
  BDSTunnelFactoryElliptical();

  /// Singleton patter instance
  static BDSTunnelFactoryElliptical* _instance;  

  /// Test input parameters - this is only for basic viability not in comparison to
  /// the accelerator model or other components
  void TestInputParameters(G4double&    length,
			   G4double&    tunnelThickness,
			   G4double&    tunnelSoilThickness,
			   G4Material*& tunnelMaterial,
			   G4Material*& tunnelSoildMaterial,
			   G4double&    tunnelFloorOffset,
			   G4double&    tunnel1,
			   G4double&    tunnel2);

};
  
#endif
