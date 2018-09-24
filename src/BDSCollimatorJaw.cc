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
#include "BDSCollimatorJaw.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Box.hh"
#include "G4VSolid.hh"

BDSCollimatorJaw::BDSCollimatorJaw(G4String  nameIn,
                   G4double  lengthIn,
                   G4double  horizontalWidthIn,
                   G4double  xApertureIn,
                   G4double  yApertureIn,
                   G4double  xOutApertureIn,
                   G4double  yOutApertureIn,
                   G4String  collimatorMaterialIn,
                   G4String  vacuumMaterialIn,
                   G4Colour* colourIn):
  BDSCollimator(nameIn, lengthIn, horizontalWidthIn, "jcol",
        xApertureIn, yApertureIn,xOutApertureIn, yOutApertureIn,
        collimatorMaterialIn,
        vacuumMaterialIn, colourIn)
{;}

void BDSCollimatorJaw::BuildInnerCollimator()
{
  // no inner solid being subtracted so nothing to do

  // vacuum solid still needed - should be total height of the element but with width of aperture
  if (BDS::IsFinite(xAperture))
    {
      vacuumSolid = new G4Box(name + "_vacuum_solid",               // name
                              xAperture - lengthSafety,             // x half width
                              0.5 * horizontalWidth - lengthSafety, // y half width
                              chordLength * 0.5);                   // z half length
    }
  else
    {
      vacuumSolid = new G4Box(name + "_vacuum_solid",               // name
                              0.5 * horizontalWidth - lengthSafety, // x half width
                              yAperture - lengthSafety,             // y half width
                              chordLength * 0.5);                   // z half length
    }


  RegisterSolid(vacuumSolid);
}
