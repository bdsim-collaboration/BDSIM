#include "BDSTunnelInfo.hh"
#include "BDSTunnelType.hh"

#include "globals.hh"   // geant4 globals / types

class G4Material;

BDSTunnelInfo::BDSTunnelInfo()
{
  tunnelInfo = BDSTunnelType::circular;
  tunnelThickness = 0.0;
  tunnelSoilThickness = 0.0;
  tunnelMaterial      = NULL;
  tunnelSoilMaterial  = NULL;
  tunnelFloor         = false;
  tunnelFloorOffset   = 0.0;
  tunnel1             = 0.0;
  tunnel2             = 0.0;
}

BDSTunnelInfo::BDSTunnelInfo(BDSTunnelType tunnelTypeIn,
			     G4double      lengthIn,
			     G4double      tunnelThicknessIn,
			     G4double      tunnelSoilThicknessIn,
			     G4Material*   tunnelMaterialIn,
			     G4Material*   tunnelSoilMaterialIn,
			     G4bool        tunnelFloorIn,
			     G4double      tunnelFloorOffsetIn,
			     G4double      tunnel1In,
			     G4double      tunnel2In):
  tunnelType(tunnelTypeIn), length(lengthIn), tunnelThickness(tunnelThicknessIn),
  tunnelSoilThickness(tunnelSoilThicknessIn), tunnelMaterial(tunnelMaterialIn),
  tunnelSoilMaterial(tunnelSoilMaterialIn), tunnelFloor(tunnelFloorIn),
  tunnelFloorOffset(tunnelFloorOffsetIn), tunnel1(tunnel1In), tunnel2(tunnel2In)
{;}
