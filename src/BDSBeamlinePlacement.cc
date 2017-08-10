#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamlinePlacement.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

#include "globals.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"


BDSBeamlinePlacement::BDSBeamlinePlacement()
{;}

BDSBeamlinePlacement::~BDSBeamlinePlacement()
{;}

void BDSBeamlinePlacement::PlaceBeamlineInWorld(G4VPhysicalVolume*        world,
						BDSBeamline*              beamline,
						const BDSGlobalConstants* globals)
{
  // place things in curvilinear world
  G4bool checkOverlaps = globals->CheckOverlaps();
  for (const auto element : *beamline)
    {
      G4String name        = element->GetName(); 
      G4String clPVName    = element->GetPlacementName() + "_pv";
      G4int    nCopy       = element->GetCopyNo();
      G4Transform3D*  clpt = element->GetReadOutPlacementTransform();
      BDSAcceleratorComponent* accComp = element->GetAcceleratorComponent();
      G4LogicalVolume*             vol = accComp->GetContainerLogicalVolume();
      
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "placement transform position: " << clpt->getTranslation()  << G4endl;
      G4cout << __METHOD_NAME__ << "placement transform rotation: " << clpt->getRotation()     << G4endl; 
#endif
      G4PVPlacement* readOutPV = new G4PVPlacement(*clpt,          // placement transform
						   clPVName,       // name
						   vol,            // logical volume
						   world,          // mother  volume
						   false,	   // no boolean operation
						   nCopy,          // copy number
						   checkOverlaps); // overlap checking
      
      // Register the spos and other info of this element.
      // Used by energy counter sd to get spos of that logical volume at histogram time.
      // If it has a readout volume, that'll be used for sensitivity so only need to register
      // that. Should only register what we need to as used for every energy hit (many many many)
      
      // use the readOutLV name as this is what's accessed in BDSEnergyCounterSD
      BDSPhysicalVolumeInfo* theinfo = new BDSPhysicalVolumeInfo(name,
								 clPVName,
								 element->GetSPositionMiddle(),
								 element->GetIndex());
      
      BDSPhysicalVolumeInfoRegistry::Instance()->RegisterInfo(readOutPV, theinfo, true);
    }
}
