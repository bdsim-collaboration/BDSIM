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

  SetAcceleratorVacuumLogicalVolume(pipe->GetVacuumLogicalVolume());

  // update extents
  InheritExtents(pipe);
}

std::vector<G4LogicalVolume*> BDSDrift::GetAllSensitiveVolumes() const
{
  std::vector<G4LogicalVolume*> result;
  for (auto it : allSensitiveVolumes)
    {result.push_back(it);}

  if (pipe)
    {
      for (auto it : pipe->GetAllSensitiveVolumes())
	{result.push_back(it);}
    }
  return result;
}
