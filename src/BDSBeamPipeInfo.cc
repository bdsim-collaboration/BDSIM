#include "BDSBeamPipeInfo.hh"





BDSBeamPipeInfo::BDSBeamPipeInfo(BDSBeamPipeType beamPipeTypeIn,
				 G4double        aper1In,
				 G4double        aper2In,
				 G4double        aper3In,
				 G4double        aper4In,
				 G4Material*     vacuumMaterialIn;
				 G4double        beamPipeThicknessIn;
				 G4Material*     beamPipeMaterialIn)
{
  beamPipeType      = beamPipeTypeIn;
  aper1             = aper1In;
  aper2             = aper2In;
  aper3             = aper3In;
  aper4             = aper4In;
  vacuumMaterial    = vacuumMaterialIn;
  beamPipeThickness = beamPipeThicknessIn;
  beamPipeMaterial  = beamPipeMaterialIn;
}
