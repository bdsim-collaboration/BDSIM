#include "BDSBeamPipe.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include <utility>            // for std::pair

BDSBeamPipe::BDSBeamPipe(G4VSolid*                 containerSolidIn,
			 G4LogicalVolume*          containerLVIn,
			 BDSExtent                 extentIn,
			 G4VSolid*                 containerSubtractionSolidIn,
			 G4LogicalVolume*          vacuumLVIn,
			 G4bool                    containerIsCircularIn,
			 G4double                  containerRadiusIn):
  BDSGeometryComponent(containerSolidIn, containerLVIn, extentIn),
  containerSubtractionSolid(containerSubtractionSolidIn),
  vacuumLogicalVolume(vacuumLVIn),
  containerIsCircular(containerIsCircularIn),
  containerRadius(containerRadiusIn)
{;}

BDSBeamPipe::~BDSBeamPipe()
{
  delete containerSubtractionSolid;
}
