#include "BDSGeometry.hh"
#include "BDSGeometryType.hh"
#include "BDSUtilities.hh"

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
  containingDir = BDS::GetFullPath(fileIn,true); // strip of the file name effictively
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
