#include "BDSAcceleratorModel.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParallelWorldCurvilinear.hh"

#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

class BDSBeamline;

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

  // Register read out world PV with our auxiliary navigator.
  BDSAuxiliaryNavigator::AttachWorldVolumeToNavigatorCL(clWorld);

  // Visualisation
  G4LogicalVolume*   clWorldLV = clWorld->GetLogicalVolume();
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  clWorldVis = new G4VisAttributes(*(globals->GetVisibleDebugVisAttr()));
  clWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  clWorldLV->SetVisAttributes(clWorldVis);

  BDSBeamlineSet blSetMain = BDSAcceleratorModel::Instance()->BeamlineSetMain();

  BDSDetectorConstruction::PlaceBeamlineInWorld(blSetMain.curvilinearWorld, clWorld,
						globals->CheckOverlaps(),
						nullptr, false, true);
}
