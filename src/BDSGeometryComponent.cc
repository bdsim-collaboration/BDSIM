#include "BDSGeometryComponent.hh"
#include "globals.hh"              // geant4 globals / types
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include <utility>                 // for std::pair

BDSGeometryComponet::BDSGeometryComponent(G4VSolid*        containerSolidIn,
					  G4LogicalVolume* containerLVIn):
  containerSolid(containerSolidIn),containerLogicalVolume(containerLVIn)
{
  SetExtentX(0,0); // initialise only - unphysical - should be set by child class
  SetExtentY(0,0);
  SetExtentZ(0,0);
}

BDSGeometryComponent::BDSGeometryComponent(G4VSolid*        containerSolidIn,
					   G4LogicalVolume* containerLVIn,
					   std::pair<G4double,G4double> extentXIn,
					   std::pair<G4double,G4double> extentYIn,
					   std::pair<G4double,G4double> extentZIn):
  containerSolid(containerSolidIn),
  containerLogicalVolume(containerLVIn),
  exentX(extentXIn),
  exentY(extentYIn),
  exentZ(extentZIn)
{;}

