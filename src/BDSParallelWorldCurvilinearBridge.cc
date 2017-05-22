#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParallelWorldCurvilinearBridge.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"


BDSParallelWorldCurvilinearBridge::BDSParallelWorldCurvilinearBridge():
  G4VUserParallelWorld("CurvilinearBridgeWorld"),
  clbWorldVis(nullptr)
{;}

BDSParallelWorldCurvilinearBridge::~BDSParallelWorldCurvilinearBridge()
{
  delete clbWorldVis;
}

void BDSParallelWorldCurvilinearBridge::Construct()
{// general abbreviation: 'clb' == curvilinear bridge
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  G4VPhysicalVolume* clbWorld   = GetWorld();
  G4LogicalVolume*   clbWorldLV = clbWorld->GetLogicalVolume();

  // Register read out world PV with our auxiliary navigator. This gives curvilinear
  // coordinates for multiple applications - CL = curvilinear.
  //BDSAuxiliaryNavigator::AttachWorldVolumeToNavigatorCL(clWorld);
  //BDSAuxiliaryNavigator::RegisterCurvilinearBridgeWorld(clbworld);

  // Visualisation
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  clbWorldVis = new G4VisAttributes(*(globals->GetVisibleDebugVisAttr()));
  clbWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  clbWorldLV->SetVisAttributes(clbWorldVis);

  //PlaceBeamlineInWorld(clbWorld, beamline, globals);
}
