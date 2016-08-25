#include "BDSBeamPipe.hh"

#include "globals.hh"         // geant4 globals / types

BDSBeamPipe::BDSBeamPipe(G4VSolid*        containerSolidIn,
			 G4LogicalVolume* containerLVIn,
			 BDSExtent        extentIn,
			 G4VSolid*        containerSubtractionSolidIn,
			 G4LogicalVolume* vacuumLVIn,
			 G4bool           containerIsCircularIn,
			 G4double         containerRadiusIn,
			 G4ThreeVector    inputFaceNormalIn,
			 G4ThreeVector    outputFaceNormalIn):
  BDSGeometryComponent(containerSolidIn, containerLVIn, extentIn),
  containerSubtractionSolid(containerSubtractionSolidIn),
  vacuumLogicalVolume(vacuumLVIn),
  containerIsCircular(containerIsCircularIn),
  containerRadius(containerRadiusIn),
  inputFaceNormal(inputFaceNormalIn),
  outputFaceNormal(outputFaceNormalIn)
{;}

BDSBeamPipe::~BDSBeamPipe()
{
  delete containerSubtractionSolid;
}
