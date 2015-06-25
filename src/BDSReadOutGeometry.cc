#include "BDSAcceleratorModel.hh"
#include "BDSDebug.hh"
#include "BDSReadOutGeometry.hh"


BDSReadOutGeometry::BDSReadOutGeometry(G4String name):
  G4VReadOutGeometry(name)
{;}

BDSReadOutGeometry::~BDSReadOutGeometry()
{;}

G4VPhysicalVolume* BDSReadOutGeometry::Build()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSAcceleratorModel::Instance()->GetReadOutWorldPV();
}
