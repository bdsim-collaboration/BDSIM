#include "BDSGeometryComponent.hh"
#include "BDSGeometryComponentHollow.hh"

#include "globals.hh" // geant4 types / globals
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4VSolid.hh"

#include <utility>


BDSGeometryComponentHollow::BDSGeometryComponentHollow(G4VSolid*        containerSolidIn,
						       G4LogicalVolume* containerLVIn):
  BDSGeometryComponent(containerSolidIn, containerLVIn)
{;}

BDSGeometryComponentHollow::BDSGeometryComponentHollow(G4VSolid*        containerSolidIn,
						       G4LogicalVolume* containerLVIn,
						       std::pair<G4double,G4double> extentXIn,
						       std::pair<G4double,G4double> extentYIn,
						       std::pair<G4double,G4double> extentZIn,
						       std::pair<G4double,G4double> innerExtentXIn,
						       std::pair<G4double,G4double> innerExtentYIn,
						       std::pair<G4double,G4double> innerExtentZIn,
						       G4ThreeVector                placementOffset):
  BDSGeometryComponent(containerSolidIn,
		       containerLVIn,
		       extentXIn,
		       extentYIn,
		       extentZIn,
		       placementOffset),
  innerExtentX(innerExtentXIn),
  innerExtentY(innerExtentYIn),
  innerExtentZ(innerExtentZIn)
{;}
  
