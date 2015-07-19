#include "BDSTunnelFactoryBase.hh"
#include "BDSTunnelFactorySquare.hh"
#include "BDSTunnelFactoryRectangular.hh"

#include "BDSDebug.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSTunnelInfo.hh"

#include "globals.hh"                 // geant4 globals / types

BDSTunnelFactorySquare* BDSTunnelFactorySquare::_instance = 0;

BDSTunnelFactorySquare* BDSTunnelFactorySquare::Instance()
{
  if (_instance == 0)
    {_instance = new BDSTunnelFactorySquare();}
  return _instance;
}

BDSTunnelFactorySquare::BDSTunnelFactorySquare():BDSTunnelFactoryBase()
{
}

BDSTunnelFactorySquare::~BDSTunnelFactorySquare()
{
  _instance = 0;
}

BDSGeometryComponent* BDSTunnelFactorySquare::CreateTunnelSection(G4String      name,
								  G4double      length,
								  G4double      tunnelThickness,
								  G4double      tunnelSoilThickness,
								  G4Material*   tunnelMaterial,
								  G4Material*   tunnelSoilMaterial,
								  G4bool        tunnelFloor,
								  G4double      tunnelFloorOffset,
								  G4double      tunnel1,
								  G4double      /*tunnel2*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnelFloorOffset, tunnel1);
  return BDSTunnelFactoryRectangular::Instance()->CreateTunnelSection(name, length, tunnelThickness,
							       tunnelSoilThickness, tunnelMaterial,
							       tunnelSoilMaterial, tunnelFloor,
							       tunnelFloorOffset, tunnel1, tunnel1);
}


BDSGeometryComponent* BDSTunnelFactorySquare::CreateTunnelSectionAngledInOut(G4String      name,
									     G4double      length,
									     G4double      angleIn,
									     G4double      angleOut,
									     G4double      tunnelThickness,
									     G4double      tunnelSoilThickness,
									     G4Material*   tunnelMaterial,
									     G4Material*   tunnelSoilMaterial,
									     G4bool        tunnelFloor,
									     G4double      tunnelFloorOffset,
									     G4double      tunnel1,
									     G4double      /*tunnel2*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // test input parameters - set global options as default if not specified
  TestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial,
		      tunnelSoilMaterial, tunnelFloorOffset, tunnel1);
  return BDSTunnelFactoryRectangular::Instance()->CreateTunnelSectionAngledInOut(name, length, angleIn, angleOut,
										 tunnelThickness,
										 tunnelSoilThickness, tunnelMaterial,
										 tunnelSoilMaterial, tunnelFloor,
										 tunnelFloorOffset, tunnel1, tunnel1);
}


/// functions below here are private to this particular factory
void BDSTunnelFactorySquare::TestInputParameters(G4double&    length,
						 G4double&    tunnelThickness,
						 G4double&    tunnelSoilThickness,
						 G4Material*& tunnelMaterial,
						 G4Material*& tunnelSoilMaterial,
						 G4double&    tunnelFloorOffset,
						 G4double&    tunnel1)
{
  CommontTestInputParameters(length, tunnelThickness, tunnelSoilThickness, tunnelMaterial, tunnelSoilMaterial);
  
  if (tunnelFloorOffset < 1e-10)
    {tunnelFloorOffset = defaultModel->floorOffset;}
  
  if (tunnel1 < 1e-10)
    {tunnel1 = defaultModel->aper1;}
}
