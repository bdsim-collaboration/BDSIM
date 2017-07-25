#include "BDSAcceleratorModel.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParallelWorldCurvilinear.hh"

#include "G4String.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

class BDSBeamline;

BDSParallelWorldCurvilinear::BDSParallelWorldCurvilinear(G4String name):
  G4VUserParallelWorld("CurvilinearWorld_" + name),
  suffix(name),
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

  G4VPhysicalVolume* clWorld = GetWorld();

  // TBC - only register main one for now
  // register read out world PV with our auxiliary navigator.
  if (suffix == "main")
    {BDSAuxiliaryNavigator::AttachWorldVolumeToNavigatorCL(clWorld);}

  G4LogicalVolume* clWorldLV = clWorld->GetLogicalVolume();

  // visualisation
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  clWorldVis = new G4VisAttributes(*(globals->GetVisibleDebugVisAttr()));
  clWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  clWorldLV->SetVisAttributes(clWorldVis);

  BDSBeamlineSet blSet = BDSAcceleratorModel::Instance()->BeamlineSet(suffix);

  BDSDetectorConstruction::PlaceBeamlineInWorld(blSet.curvilinearWorld, clWorld,
						globals->CheckOverlaps(),
						nullptr, false, true, true);
}
