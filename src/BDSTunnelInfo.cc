#include "BDSTunnelInfo.hh"

#include "BDSTunnelType.hh"
#include "G4Material.hh"

BDSTunnelInfo::BDSTunnelInfo()
{
  tunnelType      = BDSTunnelType::circular;
  aper1           = 0.0;
  aper2           = 0.0;
  aper3           = 0.0;
  aper4           = 0.0;
  tunnelThickness = 0.0;
  tunnelMaterial  = NULL;
  soilThickness   = 0.0;
  tunnelOffsetX   = 0.0;
  tunnelOffsetY   = 0.0;
}

BDSTunnelInfo::BDSTunnelInfo(BDSTunnelType tunnelTypeIn,
			     G4double      aper1In,
			     G4double      aper2In,
			     G4double      aper3In,
			     G4double      aper4In,
			     G4double      tunnelThicknessIn,
			     G4Material*   tunnelMaterialIn,
			     G4double      soilThicknessIn,
			     G4double      tunnelOffsetXIn,
			     G4double      tunnelOffsetYIn)
{
  tunnelType = tunnelTypeIn;
  aper1            = aper1In;
  aper2            = aper2In;
  aper3            = aper3In;
  aper4            = aper4In;
  tunnelThickness  = tunnelThicknessIn;
  tunnelMaterial   = tunnelMaterialIn;
  soilThickness    = soilThicknessIn;
  tunnelOffsetX    = tunnelOffsetXIn;
  tunnelOffsetY    = tunnelOffsetYIn;
}
  
