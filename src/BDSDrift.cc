#include "BDSAcceleratorComponent.hh"
#include "BDSDrift.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"

#include "globals.hh" // geant4 globals / types

BDSDrift::BDSDrift(G4String         name, 
		   G4double         length,
		   BDSBeamPipeInfo* beamPipeInfo):
  BDSAcceleratorComponent(name, length, 0, "drift", beamPipeInfo)
{;}

BDSDrift::~BDSDrift()
{;}

void BDSDrift::Build()
{
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe = factory->CreateBeamPipe(name,
					      chordLength,
					      beamPipeInfo);;

  RegisterDaughter(pipe);
  
  // make the beam pipe container, this object's container
  containerLogicalVolume = pipe->GetContainerLogicalVolume();
  containerSolid         = pipe->GetContainerSolid();

  // register vacuum volume (for biasing)
  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());

  // update extents
  InheritExtents(pipe);

  // update faces
  SetInputFaceNormal(pipe->InputFaceNormal());
  SetOutputFaceNormal(pipe->OutputFaceNormal());
}
