#include "BDSAcceleratorModel.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParallelWorldCurvilinearBridge.hh"

#include "G4String.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

class BDSBeamline;

BDSParallelWorldCurvilinearBridge::BDSParallelWorldCurvilinearBridge(G4String name):
  G4VUserParallelWorld("CurvilinearBridgeWorld_" + name),
  suffix(name),
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

  // TBC - only register main one for now
  // Register read out world PV with our auxiliary navigator.
  if (suffix == "main")
    {BDSAuxiliaryNavigator::RegisterCurvilinearBridgeWorld(clbWorld);}

  // Visualisation
  G4LogicalVolume* clbWorldLV = clbWorld->GetLogicalVolume();
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  clbWorldVis = new G4VisAttributes(*(globals->GetVisibleDebugVisAttr()));
  clbWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  clbWorldLV->SetVisAttributes(clbWorldVis);

  BDSBeamlineSet blSet = BDSAcceleratorModel::Instance()->BeamlineSet(suffix);

  BDSDetectorConstruction::PlaceBeamlineInWorld(blSet.curvilinearBridgeWorld, clbWorld,
						globals->CheckOverlaps(),
						nullptr, false, true);
}
