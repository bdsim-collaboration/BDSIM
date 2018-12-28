/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSCollimatorRectangular.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4VSolid.hh"

#include <cmath>

BDSCollimatorRectangular::BDSCollimatorRectangular(G4String    name,
						   G4double    length,
						   G4double    horizontalWidth,
						   G4Material* collimatorMaterial,
						   G4Material* vacuumMaterial,
						   G4double    xAperture,
						   G4double    yAperture,
						   G4double    xOutAperture,
						   G4double    yOutAperture,
						   G4Colour*   colour):
  BDSCollimator(name, length, horizontalWidth, "rcol",
		collimatorMaterial, vacuumMaterial, xAperture,
		yAperture, xOutAperture, yOutAperture, colour)
{;}

void BDSCollimatorRectangular::BuildInnerCollimator()
{
  if(tapered)
    {
      // Make subtracted volume longer than the solid volume
      G4double xGradient = std::abs((xAperture - xOutAperture)) / chordLength;
      G4double yGradient = std::abs((yAperture - yOutAperture)) / chordLength;
      
      G4double deltam = 0.1 * chordLength;
      G4double deltax = xGradient * deltam;
      G4double deltay = yGradient * deltam;
      
      innerSolid  = new G4Trd(name + "_inner_solid",             // name
                              xAperture + deltax,                // X entrance half length
                              xOutAperture - deltax,             // X exit half length
                              yAperture + deltay,                // Y entrance half length
                              yOutAperture - deltay,             // Y exit half length
                              (chordLength + 2*deltam) * 0.5);   // Z half length
    
      vacuumSolid = new G4Trd(name + "_vacuum_solid",               // name
                              xAperture - lengthSafetyLarge,        // X entrance half length
                              xOutAperture - lengthSafetyLarge,     // X exit half length
                              yAperture - lengthSafetyLarge,        // Y entrance half length
                              yOutAperture - lengthSafetyLarge,     // Y exit half length
                              chordLength*0.5 - lengthSafetyLarge); // Z half length
    }
  else
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
    }
    
  RegisterSolid(innerSolid);
  RegisterSolid(vacuumSolid);
}
