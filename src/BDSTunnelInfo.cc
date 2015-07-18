#include "BDSMaterials.hh"
#include "BDSTunnelInfo.hh"
#include "BDSTunnelType.hh"

class G4Material;

BDSTunnelInfo::BDSTunnelInfo()
{
  type          = BDSTunnelType::circular;
  thickness     = 0.0;
  soilThickness = 0.0;
  material      = NULL;
  soilMaterial  = NULL;
  buildFloor    = false;
  floorOffset   = 0.0;
  aper1         = 0.0;
  aper2         = 0.0;
  sensitive     = false;
}

BDSTunnelInfo::BDSTunnelInfo(BDSTunnelType typeIn,
			     G4double      thicknessIn,
			     G4double      soilThicknessIn,
			     G4Material*   materialIn,
			     G4Material*   soilMaterialIn,
			     G4bool        buildFloorIn,
			     G4double      floorOffsetIn,
			     G4double      aper1In,
			     G4double      aper2In,
			     G4bool        sensitiveIn):
  type(typeIn), thickness(thicknessIn),
  soilThickness(soilThicknessIn), material(materialIn),
  soilMaterial(soilMaterialIn), buildFloor(buildFloorIn),
  floorOffset(floorOffsetIn), aper1(aper1In), aper2(aper2In),
  sensitive(sensitiveIn)
{;}

BDSTunnelInfo::BDSTunnelInfo(G4String typeIn,
			     G4double thicknessIn,
			     G4double soilThicknessIn,
			     G4String materialIn,
			     G4String soilMaterialIn,
			     G4bool   buildFloorIn,
			     G4double floorOffsetIn,
			     G4double aper1In,
			     G4double aper2In,
			     G4bool   sensitiveIn):
  thickness(thicknessIn), soilThickness(soilThicknessIn),
  buildFloor(buildFloorIn), floorOffset(floorOffsetIn),
  aper1(aper1In), aper2(aper2In),
  sensitive(sensitiveIn)
{
  type         = BDS::DetermineTunnelType(typeIn);
  material     = BDSMaterials::Instance()->GetMaterial(materialIn);
  soilMaterial = BDSMaterials::Instance()->GetMaterial(soilMaterialIn);
}
