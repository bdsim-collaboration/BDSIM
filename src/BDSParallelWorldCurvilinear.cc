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
#include "BDSSDManager.hh"

#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"


BDSParallelWorldCurvilinear::BDSParallelWorldCurvilinear():
  G4VUserParallelWorld("CurvilinearWorld"),
  clWorldVis(nullptr)
{;}

BDSParallelWorldCurvilinear::~BDSParallelWorldCurvilinear()
{;}

void BDSParallelWorldCurvilinear::Construct()
{// general abbreviation: 'cl' == curvilinear
  //#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  //#endif

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
  BDSBeamline* beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();
  
  G4VSensitiveDetector* eCSD = BDSSDManager::Instance()->GetEnergyCounterSD();
  G4bool checkOverlaps = BDSGlobalConstants::Instance()->CheckOverlaps();
  for (const auto element : *beamline)
    {
      // read out geometry logical volume - note may not exist for each item - must be tested
      auto accComp = element->GetAcceleratorComponent();
      G4LogicalVolume* readOutLV = accComp->GetReadOutLogicalVolume();

      if (!readOutLV)
	{continue;} // no read out geometry for this component

      readOutLV->SetSensitiveDetector(eCSD);

      G4String name          = element->GetName(); 
      G4String readOutPVName = element->GetPlacementName() + "_ro_pv";
      G4int    nCopy         = element->GetCopyNo();
      G4Transform3D* ropt    = element->GetReadOutPlacementTransform();

      //#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "placing read out geometry" << G4endl;
      G4cout << "placement transform position: " << ropt->getTranslation()  << G4endl;
      G4cout << "placement transform rotation: " << ropt->getRotation()  << G4endl; 
      //#endif
      G4PVPlacement* readOutPV = new G4PVPlacement(*ropt,          // placement transform
						   readOutPVName,  // name
						   readOutLV,      // logical volume
						   clWorld,        // mother  volume
						   false,	   // no boolean operation
						   nCopy,          // copy number
						   checkOverlaps); // overlap checking
      
      // Register the spos and other info of this elemnet.
      // Used by energy counter sd to get spos of that logical volume at histogram time.
      // If it has a readout volume, that'll be used for sensitivity so only need to register
      // that. Should only register what we need to as used for every energy hit (many many many)
      
      // use the readOutLV name as this is what's accessed in BDSEnergyCounterSD
      BDSPhysicalVolumeInfo* theinfo = new BDSPhysicalVolumeInfo(name,
								 readOutPVName,
								 element->GetSPositionMiddle(),
								 accComp->GetPrecisionRegion(),
								 element->GetIndex());
      
      BDSPhysicalVolumeInfoRegistry::Instance()->RegisterInfo(readOutPV, theinfo, true);
    }

}
