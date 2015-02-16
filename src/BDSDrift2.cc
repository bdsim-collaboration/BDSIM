#include "BDSDrift2.hh"


#include "globals.hh" // geant4 globals / types
//#include "G4LogicalVolume.hh"
//#include "G4Tubs.hh"
//#include "G4UserLimits.hh"
//#include "G4VisAttributes.hh"
//#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"

#include <list>

#include "BDSAcceleratorComponent.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeType.hh"

//#include <map>

// TEST new drift to test out new beampipe factory


BDSDrift2::BDSDrift2 (G4String        nameIn, 
		      G4double        lengthIn,
		      BDSBeamPipeType beamPipeTypeIn,
		      G4double        beamPipeThicknessIn,
		      G4double        aper1In,
		      G4double        aper2In,
		      G4double        aper3In,
		      G4double        aper4In,
		      G4Material*     beamPipeMaterialIn,
		      G4Material*     vacuumMaterialIn
		      ):
  BDSAcceleratorComponent(nameIn,
			  lengthIn,
			  aper1In,
			  aper1In,
			  aper2In,
			  std::list<G4double>(),
			  std::list<G4double>()),
  beamPipeType(beamPipeTypeIn),
  aper1(aper1In),
  aper2(aper2In),
  aper3(aper3In),
  aper4(aper4In),
  beamPipeThickness(beamPipeThicknessIn),
  beamPipeMaterial(beamPipeMaterialIn),
  vacuumMaterial(vacuumMaterialIn)
{;}

void BDSDrift2::Build() {
  BDSBeamPipe* pipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(beamPipeType,
								     itsName,
								     itsLength,
								     aper1,
								     aper2,
								     aper3,
								     aper4,
								     vacuumMaterial,
								     beamPipeThickness,
								     beamPipeMaterial
								     );

  itsMarkerLogicalVolume = pipe->GetContainerLogicalVolume();
}
