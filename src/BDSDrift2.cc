#include "BDSDrift2.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Material.hh"

#include <list>

#include "BDSAcceleratorComponent.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeInfo.hh"

BDSDrift2::BDSDrift2 (G4String     nameIn, 
		      G4double     lengthIn,
		      beamPipeInfo beamPipeInfoIn):
  BDSAcceleratorComponent(nameIn,
			  lengthIn,
			  beamPipeInfoIn.aper1,
			  beamPipeInfoIn.aper1,
			  beamPipeInfoIn.aper2,
			  std::list<G4double>(),
			  std::list<G4double>()),
  itsBeamPipeInfo(beamPipeInfoIn)
{;}

void BDSDrift2::Build() {
  
  BDSBeamPipe* pipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(itsBeamPipeInfo.beamPipeType,
								     itsName,
								     itsLength,
								     itsBeamPipeInfo.aper1,
								     itsBeamPipeInfo.aper2,
								     itsBeamPipeInfo.aper3,
								     itsBeamPipeInfo.aper4,
								     itsBeamPipeInfo.vacuumMaterial,
								     itsBeamPipeInfo.beamPipeThickness,
								     itsBeamPipeInfo.beamPipeMaterial
								     );
  
  itsMarkerLogicalVolume = pipe->GetContainerLogicalVolume();
}
