#include "BDSAcceleratorModel.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParallelWorldCurvilinearBridge.hh"

#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

class BDSBeamline;

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

  G4VPhysicalVolume* clbWorld = GetWorld();

  // Register read out world PV with our auxiliary navigator.
  BDSAuxiliaryNavigator::RegisterCurvilinearBridgeWorld(clbWorld);

  // Visualisation
  G4LogicalVolume* clbWorldLV = clbWorld->GetLogicalVolume();
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  clbWorldVis = new G4VisAttributes(*(globals->GetVisibleDebugVisAttr()));
  clbWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  clbWorldLV->SetVisAttributes(clbWorldVis);

  BDSBeamline* clbBeamline = BDSAcceleratorModel::Instance()->BeamlineSetMain().curvilinearBridgeWorld;

  BDSDetectorConstruction::PlaceBeamlineInWorld(clbBeamline, clbWorld, globals->CheckOverlaps(),
						nullptr, false, true);
}
