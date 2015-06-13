#include "BDSReadOutGeometry.hh"

#include "BDSAcceleratorModel.hh"

BDSReadOutGeometry::BDSReadOutGeometry(G4String name):
  G4VReadOutGeometry(name)
{;}

BDSReadOutGeometry::~BDSReadOutGeometry()
{;}

G4VPhysicalVolume* BDSReadOutGeometry::Build()
{
  return BDSAcceleratorModel::Instance()->GetReadOutWorldPV();
}
