#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParallelWorldSampler.hh"
#include "BDSSampler.hh"
#include "BDSSamplerCylinder.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerType.hh"

#include "globals.hh" // geant4 types / globals
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VUserParallelWorld.hh"


BDSParallelWorldSampler::BDSParallelWorldSampler():
  G4VUserParallelWorld("SamplerWorld"),
  samplerWorldVis(nullptr)
{;}

BDSParallelWorldSampler::~BDSParallelWorldSampler()
{
  for (auto placement : placements)
    {delete placement;}
  delete samplerWorldVis;
}

void BDSParallelWorldSampler::Construct()
{
  G4VPhysicalVolume* samplerWorld   = GetWorld();
  G4LogicalVolume*   samplerWorldLV = samplerWorld->GetLogicalVolume();

  samplerWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr()));
  samplerWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  samplerWorldLV->SetVisAttributes(samplerWorldVis);
  
  const G4double samplerR    = 0.5*BDSGlobalConstants::Instance()->GetSamplerDiameter();
  BDSBeamline* beamline      = BDSAcceleratorModel::Instance()->GetFlatBeamline();
  const G4bool checkOverlaps = BDSGlobalConstants::Instance()->GetCheckOverlaps();

  // For each element in the beamline construct and place the appropriate type
  // of sampler if required. Info encoded in BDSBeamlineElement instance
  for (auto element : (*beamline))
    {
      if (element->GetSamplerType() != BDSSamplerType::none)
	{
	  G4Transform3D* pt = element->GetSamplerPlacementTransform();
	  G4String name     = element->GetName();
	  BDSSampler* sampler = nullptr;
	  switch (element->GetSamplerType().underlying())
	    {
	    case BDSSamplerType::plane:
	      {
		sampler = new BDSSamplerPlane(name,
					      *pt,
					      samplerR);
		break;
	      }
	    case BDSSamplerType::cylinder:
	      {
		G4double length = element->GetAcceleratorComponent()->GetChordLength();
		sampler = new BDSSamplerCylinder(name,
						 *pt,
						 length,
						 samplerR);
		break;
	      }
	    default:
	      continue; // leave as nullptr - shouldn't occur due to if at top
	    }
	  
	  G4String placementName = name + "_pv";

	  if (sampler)
	    {
	      // record placements for cleaning up at destruction.
	      placements.push_back(new G4PVPlacement(*pt,              // placement transform
						     sampler->GetContainerLogicalVolume(), // logical volume
						     placementName,    // name of placement
						     samplerWorldLV,   // mother volume
						     false,            // no boolean operation
						     sampler->GetID(), // copy number
						     checkOverlaps));
	    }
	}
    }
}
