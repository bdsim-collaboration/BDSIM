#include "BDSDrift.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Material.hh"

#include <list>

#include "BDSAcceleratorComponent.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"

class BDSTiltOffset;

BDSDrift::BDSDrift (G4String         name, 
		    G4double         length,
		    BDSBeamPipeInfo* beamPipeInfoIn,
		    BDSTiltOffset    tiltOffset):
  BDSAcceleratorComponent(name, length, 0, "drift", tiltOffset)
{
  beamPipeInfo = beamPipeInfoIn;
}

void BDSDrift::Build()
{  
  BDSBeamPipe* pipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(name,
								     chordLength,
								     beamPipeInfo);
  if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe())
    {RegisterSensitiveVolumes(pipe->GetAllSensitiveVolumes());}
  
  containerLogicalVolume = pipe->GetContainerLogicalVolume();
  containerSolid         = pipe->GetContainerSolid();

  // Set extents
  SetExtentX(pipe->GetExtentX());
  SetExtentY(pipe->GetExtentY());
  SetExtentZ(pipe->GetExtentZ());
}
