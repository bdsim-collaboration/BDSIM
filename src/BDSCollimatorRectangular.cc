/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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

#include "globals.hh"
#include "G4Box.hh"
#include "G4Trap.hh"

#include <cmath>

BDSCollimatorRectangular::BDSCollimatorRectangular(G4String    nameIn,
                                                   G4double    lengthIn,
                                                   G4double    horizontalWidthIn,
                                                   G4Material* collimatorMaterialIn,
                                                   G4Material* vacuumMaterialIn,
                                                   G4double    xApertureIn,
                                                   G4double    yApertureIn,
                                                   G4double    xApertureOutIn,
                                                   G4double    yApertureOutIn,
                                                   G4Colour*   colourIn,
                                                   G4bool      circularOuterIn):
  BDSCollimator(nameIn, lengthIn, horizontalWidthIn, "rcol",
                collimatorMaterialIn, vacuumMaterialIn, xApertureIn,
                yApertureIn, xApertureOutIn, yApertureOutIn, colourIn, circularOuterIn)
{;}

void BDSCollimatorRectangular::BuildInnerCollimator()
{
  if (tapered)
    {
      // Make subtracted volume longer than the solid volume
      G4double xGradient = (xApertureOut - xAperture) / chordLength;
      G4double yGradient = (yApertureOut - yAperture) / chordLength;
      
      G4double deltam = 0.01 * chordLength;
      G4double deltax = xGradient * deltam;
      G4double deltay = yGradient * deltam;

      innerSolid = new G4Trap(name + "_inner_solid",
                              (chordLength + 2*deltam) * 0.5, // pDz - z half length
                              0,                     // Theta
                              0,                     // phi
                              yAperture - deltay,    // pDy1 - half y at -pDz
                              xAperture - deltax,    // pDx1 - half x at -pDz, -pDy1
                              xAperture - deltax,    // pDx2 - half x at -pDz, +pDy1
                              0,                     // Alpha 1
                              yApertureOut + deltay, // pDy2 - half y at +pDz
                              xApertureOut + deltax, // pDx3 - half x at +pDz, -pDy2
                              xApertureOut + deltax, // pDx4 - half x at +pDz, +pDy2
                              0);                    // Alpha 2
      G4double lsl = lengthSafetyLarge;
      vacuumSolid = new G4Trap(name + "_vacuum_solid",
                              chordLength*0.5 - lsl, // pDz - z half length
                              0,                     // Theta
                              0,                     // phi
                              yAperture - lsl,       // pDy1 - half y at -pDz
                              xAperture - lsl,       // pDx1 - half x at -pDz, -pDy1
                              xAperture - lsl,       // pDx2 - half x at -pDz, +pDy1
                              0,                     // Alpha 1
                              yApertureOut - lsl,    // pDy2 - half y at +pDz
                              xApertureOut - lsl,    // pDx3 - half x at +pDz, -pDy2
                              xApertureOut - lsl,    // pDx4 - half x at +pDz, +pDy2
                              0);                    // Alpha 2
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
