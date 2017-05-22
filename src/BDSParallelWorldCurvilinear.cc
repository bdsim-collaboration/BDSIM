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
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  clWorldVis = new G4VisAttributes(*(globals->GetVisibleDebugVisAttr()));
  clWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  clWorldLV->SetVisAttributes(clWorldVis);

  BDSBeamline* beamline = BDSAcceleratorModel::Instance()->GetCurvilinearBeamline();
  
  PlaceBeamlineInWorld(clWorld, beamline, globals);
}
