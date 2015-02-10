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
			 G4LogicalVolume*          vacuumLVIn,
			 G4bool                    containerIsCircularIn = false,
			 G4double                  containerRadiusIn = 0.0
			 ):
  BDSGeometryComponent(containerSolidIn, containerLVIn, extentXIn, extentYIn, extentZIn),
  vacuumLogicalVolume(vacuumLVIn), containerIsCircular(containIsCircularIn),
  containerRadius(containerRadiusIn)
{;}
