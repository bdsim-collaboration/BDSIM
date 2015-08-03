#include "BDSBeamPipeInfo.hh"


BDSBeamPipeInfo::BDSBeamPipeInfo()
{
  beamPipeType      = BDSBeamPipeType::circular;
  aper1             = 0.0;
  aper2             = 0.0;
  aper3             = 0.0;
  aper4             = 0.0;
  vacuumMaterial    = nullptr;
  beamPipeThickness = 0.0;
  beamPipeMaterial  = nullptr;
}

BDSBeamPipeInfo::BDSBeamPipeInfo(BDSBeamPipeType beamPipeTypeIn,
				 G4double        aper1In,
				 G4double        aper2In,
				 G4double        aper3In,
				 G4double        aper4In,
				 G4Material*     vacuumMaterialIn,
				 G4double        beamPipeThicknessIn,
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
