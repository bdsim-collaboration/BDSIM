#include "BDSAcceleratorComponent.hh"
#include "BDSDrift.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 globals / types

BDSDrift::BDSDrift(G4String         name, 
		   G4double         length,
		   G4double         e1in,
		   G4double         e2in,
		   BDSBeamPipeInfo* beamPipeInfo,
		   G4int            precisionRegion):
  BDSAcceleratorComponent(name, length, 0, "drift", precisionRegion, beamPipeInfo),e1(e1in),e2(e2in)
{;}

BDSDrift::~BDSDrift()
{;}

void BDSDrift::Build()
{
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe;
  if (BDS::IsFinite(e1) && BDS::IsFinite(e2))
    {
      pipe = factory->CreateBeamPipeAngledInOut(beamPipeInfo->beamPipeType,
						name,
						chordLength,
						e1,
						e2,
						beamPipeInfo->aper1,
						beamPipeInfo->aper2,
						beamPipeInfo->aper3,
						beamPipeInfo->aper4,
						beamPipeInfo->vacuumMaterial,
						beamPipeInfo->beamPipeThickness,
						beamPipeInfo->beamPipeMaterial);
    }
  else if (BDS::IsFinite(e1))
    {
      pipe = factory->CreateBeamPipeAngledIn(beamPipeInfo->beamPipeType,
                                        name,
                                        chordLength,
                                        e1,
                                        beamPipeInfo->aper1,
                                        beamPipeInfo->aper2,
                                        beamPipeInfo->aper3,
                                        beamPipeInfo->aper4,
                                        beamPipeInfo->vacuumMaterial,
                                        beamPipeInfo->beamPipeThickness,
                                        beamPipeInfo->beamPipeMaterial);
    }
  else if (BDS::IsFinite(e2))
    {
      pipe = factory->CreateBeamPipeAngledOut(beamPipeInfo->beamPipeType,
                                        name,
                                        chordLength,
                                        e2,
                                        beamPipeInfo->aper1,
                                        beamPipeInfo->aper2,
                                        beamPipeInfo->aper3,
                                        beamPipeInfo->aper4,
                                        beamPipeInfo->vacuumMaterial,
                                        beamPipeInfo->beamPipeThickness,
                                        beamPipeInfo->beamPipeMaterial);
    }
  else
    {
      pipe = factory->CreateBeamPipe(name,
                                        chordLength,
                                        beamPipeInfo);
    }

  RegisterDaughter(pipe);
  
  // make the beam pipe container, this object's container
  containerLogicalVolume = pipe->GetContainerLogicalVolume();
  containerSolid         = pipe->GetContainerSolid();

  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());

  // update extents
  InheritExtents(pipe);
}
