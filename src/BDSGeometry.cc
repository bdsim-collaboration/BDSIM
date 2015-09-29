#include "BDSGeometry.hh"
#include "BDSGeometryType.hh"

#include "globals.hh" // geant4 globals / types

BDSGeometry::BDSGeometry()
{
  format = BDSGeometryType::gdml;
  file   = "";
  Init();
}

BDSGeometry::BDSGeometry(BDSGeometryType formatIn, G4String fileIn):
  format(formatIn),
  file(fileIn)
{
  Init();
}

BDSGeometry::BDSGeometry(G4String formatIn, G4String fileIn):
  file(fileIn)
{
  format = BDS::DetermineGeometryType(formatIn);
  Init();
}

BDSGeometry::~BDSGeometry()
{;} // doesn't seem to own anything

void BDSGeometry::Init()
{
  alignInVolume  = nullptr;
  alignOutVolume = nullptr;
  length         = 0;
  field          = nullptr;
}
