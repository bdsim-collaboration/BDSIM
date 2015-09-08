#include "BDSCollimatorRectangular.hh"
#include "BDSCollimatorBase.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Box.hh"
#include "G4VSolid.hh"

BDSCollimatorRectangular::BDSCollimatorRectangular(G4String name,
						   G4double length,
						   G4double outerDiameter,
						   G4double xAperture,
						   G4double yAperture,
						   G4String collimatorMaterial,
						   G4String vacuumMaterial):
  BDSCollimatorBase(name, length, outerDiameter, "rcol",
		    xAperture, yAperture,
		    collimatorMaterial,
		    vacuumMaterial)
{;}

void BDSCollimatorRectangular::BuildInnerCollimator()
{
  innerSolid  = new G4Box(name + "_inner_solid",    // name
			  xAperture,                // x half width
			  yAperture,                // y half width
			  chordLength);             // z half length
  // z half length long for unambiguous subtraction

  vacuumSolid = new G4Box(name + "_vacuum_solid",   // name
			  xAperture - lengthSafety, // x half width
			  yAperture - lengthSafety, // y half width
			  chordLength*0.5);         // z half length
  RegisterSolid(innerSolid);
  RegisterSolid(vacuumSolid);
}
