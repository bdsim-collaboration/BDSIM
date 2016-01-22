#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParallelWorldSampler.hh"

#include "globals.hh" // geant4 types / globals
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VUserParallelWorld.hh"


BDSParallelWorldSampler::BDSParallelWorldSampler():
  G4VUserParallelWorld("Sampler_Parallel_World")
{;}

void BDSParallelWorldSampler::Construct()
{
  G4VPhysicalVolume* samplerWorld   = GetWorld();
  G4LogicalVolume*   samplerWorldLV = samplerWorld->GetLogicalVolume();
  
  // build parallel world geometry here


  BDSBeamline* beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();
  const G4bool checkOvleraps = BDSGlobalConstants::Instance()->GetCheckOverlaps();
  for (auto element : beamline)
    {
      if (element->AttachSampler())
	{
	  //place sampler at element->GetPlacement();

	  G4Transform3D* pt            = element->GetPlacementTransform();
	  G4String       placementName = element->GetPlacementName() + "_pv";

	  G4PVPlacement* elementPV = new G4PVPlacement(*pt,            // placement transform
						       placementName,  // name of placement
						       element->GetContainerLogicalVolume(), // logical volume
						       samplerWorldLV, // mother volume
						       false,          // no boolean operation
						       element->GetCopyNo(), // copy number
						       checkOverlaps);
	}
    }

}
