#include "BDSAcceleratorComponent.hh"
#include "BDSDrift.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"

#include "globals.hh" // geant4 globals / types

BDSDrift::BDSDrift (G4String         name, 
		    G4double         length,
		    BDSBeamPipeInfo* beamPipeInfo,
		    G4int            precisionRegion):
  BDSAcceleratorComponent(name, length, 0, "drift", precisionRegion, beamPipeInfo),
  pipe(nullptr)
{;}

BDSDrift::~BDSDrift()
{
  delete pipe;
}


void BDSDrift::Build()
{  
  pipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name,
							chordLength,
							beamPipeInfo);

  // make the beam pipe container, this object's container
  containerLogicalVolume = pipe->GetContainerLogicalVolume();
  containerSolid         = pipe->GetContainerSolid();

  // update extents
  InheritExtents(pipe);
}
