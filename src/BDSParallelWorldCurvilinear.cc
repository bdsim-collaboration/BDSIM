#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParallelWorldCurvilinear.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"


BDSParallelWorldCurvilinear::BDSParallelWorldCurvilinear():
  G4VUserParallelWorld("CurvilinearWorld"),
  clWorldVis(nullptr)
{;}

BDSParallelWorldCurvilinear::~BDSParallelWorldCurvilinear()
{
  delete clWorldVis;
}

void BDSParallelWorldCurvilinear::Construct()
{// general abbreviation: 'cl' == curvilinear
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  G4VPhysicalVolume* clWorld   = GetWorld();
  G4LogicalVolume*   clWorldLV = clWorld->GetLogicalVolume();

  // Register read out world PV with our auxiliary navigator. This gives curvilinear
  // coordinates for multiple applications - CL = curvilinear.
  BDSAuxiliaryNavigator::AttachWorldVolumeToNavigatorCL(clWorld);

  // Visualisation
  clWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr()));
  clWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  clWorldLV->SetVisAttributes(clWorldVis);

  // place things in curvilinear world
  BDSBeamline* beamline = BDSAcceleratorModel::Instance()->GetCurvilinearBeamline();
  G4bool  checkOverlaps = BDSGlobalConstants::Instance()->CheckOverlaps();
  for (const auto element : *beamline)
    {
      G4String name        = element->GetName(); 
      G4String clPVName    = element->GetPlacementName() + "_pv";
      G4int    nCopy       = element->GetCopyNo();
      G4Transform3D*  clpt = element->GetReadOutPlacementTransform();
      BDSAcceleratorComponent* accComp = element->GetAcceleratorComponent();
      G4LogicalVolume*             vol = accComp->GetContainerLogicalVolume();
      
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "placing curvilinear geometry" << G4endl;
      G4cout << "placement transform position: " << clpt->getTranslation()  << G4endl;
      G4cout << "placement transform rotation: " << clpt->getRotation()     << G4endl; 
#endif
      G4PVPlacement* readOutPV = new G4PVPlacement(*clpt,          // placement transform
						   clPVName,       // name
						   vol,            // logical volume
						   clWorld,        // mother  volume
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
