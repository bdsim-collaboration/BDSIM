#include "BDSAcceleratorModel.hh"
#include "BDSDebug.hh"
#include "BDSTunnelReadOutGeometry.hh"

BDSTunnelReadOutGeometry::BDSTunnelReadOutGeometry(G4String name):
  G4VReadOutGeometry(name)
{;}

BDSTunnelReadOutGeometry::~BDSTunnelReadOutGeometry()
{;}

G4VPhysicalVolume* BDSTunnelReadOutGeometry::Build()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  return BDSAcceleratorModel::Instance()->GetTunnelReadOutWorldPV();
}
