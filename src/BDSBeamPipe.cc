#include "BDSBeamPipe.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include <utility>            // for std::pair

BDSBeamPipe::BDSBeamPipe(G4VSolid*                 containerSolidIn,
			 G4LogicalVolume*          containerLVIn,
			 std::pair<double, double> extentXIn,
			 std::pair<double, double> extentYIn,
			 std::pair<double, double> extentZIn,
			 G4VSolid*                 containerSubtractionSolidIn,
			 G4LogicalVolume*          vacuumLVIn,
			 G4bool                    containerIsCircularIn,
			 G4double                  containerRadiusIn
			 ):
  BDSGeometryComponent(containerSolidIn, containerLVIn, extentXIn, extentYIn, extentZIn),
  containerSubtractionSolid(containerSubtractionSolidIn),
  vacuumLogicalVolume(vacuumLVIn),
  containerIsCircular(containerIsCircularIn),
  containerRadius(containerRadiusIn)
{;}

BDSBeamPipe::~BDSBeamPipe()
{
  BDSGeometryComponent::~BDSGeometryComponent();

  delete containerSubtractionSolid; containerSubtractionSolid = nullptr;
  delete vacuumLogicalVolume;       vacuumLogicalVolume = nullptr;
}
