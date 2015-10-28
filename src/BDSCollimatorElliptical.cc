#include "BDSCollimatorElliptical.hh"
#include "BDSCollimatorBase.hh"

#include "BDSUtilities.hh"
#include "globals.hh" // geant4 globals / types
#include "G4EllipticalTube.hh"
#include "G4EllipticalCone.hh"
#include "G4VSolid.hh"

BDSCollimatorElliptical::BDSCollimatorElliptical(G4String name,
						 G4double length,
						 G4double outerDiameter,
						 G4double xAperture,
						 G4double yAperture,
                         G4double xOutAperture,
                         G4double yOutAperture,
						 G4String collimatorMaterial,
						 G4String vacuumMaterial):
  BDSCollimatorBase(name, length, outerDiameter, "ecol",
		    xAperture, yAperture,xOutAperture, yOutAperture,
		    collimatorMaterial,
		    vacuumMaterial)
{;}


void BDSCollimatorElliptical::BuildInnerCollimator()
{
  G4bool tapered = (BDS::IsFinite(xOutAperture) && BDS::IsFinite(yOutAperture));
  G4bool isOutLarger = ((xOutAperture > xAperture) && (yOutAperture > yAperture));
  
  // Swap variables around if exit size is large than entrance size
  if(tapered && isOutLarger)
    {
        G4double temp = xAperture;
        xAperture = xOutAperture;
        xOutAperture = temp;
        
        temp = yAperture;
        yAperture = yOutAperture;
        yOutAperture = temp;
    }
  
  if(tapered)
    {
      G4double zmax = chordLength * (xOutAperture + xAperture) / xAperture;

      innerSolid  = new G4EllipticalCone(name + "_inner_solid",    // name
                                         xAperture / zmax,         // Major axis of largest ellipse
                                         yAperture / zmax,         // Minor axis of largest ellipse
                                         chordLength,              // Height of cone
                                         zmax);                    // cut. Not sure what this actually does.
    
      vacuumSolid = new G4EllipticalCone(name + "_inner_solid",         // name
                                         xAperture/zmax - lengthSafety, // Major axis of largest ellipse
                                         yAperture/zmax - lengthSafety, // Minor axis of largest ellipse
                                         chordLength - lengthSafety,    // Height of cone
                                         zmax - lengthSafety);          // cut. Not sure what this actually does.
    }
  else
    {
      innerSolid  = new G4EllipticalTube(name + "_inner_solid",    // name
                                         xAperture,                // x half width
                                         yAperture,                // y half width
                                         chordLength);             // z half length
    // z half length long for unambiguous subtraction

      vacuumSolid = new G4EllipticalTube(name + "_inner_solid",    // name
                                         xAperture - lengthSafety, // x half width
                                         yAperture - lengthSafety, // y half width
                                         chordLength * 0.5);       // z half length
    }
        
  RegisterSolid(innerSolid);
  RegisterSolid(vacuumSolid);
}
