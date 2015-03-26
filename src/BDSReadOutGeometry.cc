#include "BDSReadOutGeometry.hh"

#include "BDSAcceleratorModel.hh"
#include "globals.hh"
#include "G4VReadOutGeometry.hh"

BDSReadOutGeometry::BDSReadOutGeometry(G4String name):
  G4VReadOutGeometry(name)
{;}

BDSReadOutGeometry::~BDSReadOutGeometry()
{;}

G4VPhysicalVolume* BDSReadOutGeometry::Build()
{
  return BDSAcceleratorModel::Instance()->GetReadOutWorld();
}
