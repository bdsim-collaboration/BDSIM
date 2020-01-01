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
#ifndef BDSAPERTUREFACTORY_H
#define BDSAPERTUREFACTORY_H

#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

class BDSAperture;
class G4VSolid;

/**
 * @brief Main interface for building basic shapes described by apertures.
 * 
 * @author Laurie Nevay
 */

class BDSApertureFactory
{
public:
  BDSApertureFactory();
  ~BDSApertureFactory();

  G4VSolid* CreateAperture(const G4String&    name,
			   G4double           length,
			   const BDSAperture* apertureIn,
			   const BDSAperture* apertureOut = nullptr,
			   G4double           lengthExtraForBoolean = 0);

  G4VSolid* CreateAperture(const G4String&      name,
			   G4double             length,
			   const BDSAperture*   apertureIn,
			   const G4ThreeVector& normalIn,
			   const G4ThreeVector& normalOut,
			   const BDSAperture*   apertureOut = nullptr,
			   G4double             lengthExtraForBoolean = 0);

private:
  G4VSolid* CommonConstruction(const G4String&    name,
			       G4double           length,
			       const BDSAperture* apertureIn,
			       const BDSAperture* apertureOut,
			       G4double           lengthExtraForBoolean);
  
  G4VSolid* CreateCircular()    const;
  G4VSolid* CreateRectangular() const;
  G4VSolid* CreateElliptical()  const;
  G4VSolid* CreateLHC()         const;
  G4VSolid* CreateRectEllipse() const;
  G4VSolid* CreateRaceTrack()   const;
  G4VSolid* CreateOctagonal()   const;
  G4VSolid* CreateClicPCL()     const;

  G4VSolid* CreateDifferentEnds() const;

  const G4double intersectionRadiusRatio;
  
  G4String           productName;
  G4double           productLength;
  const BDSAperture* productApertureIn;
  const BDSAperture* productApertureOut;
  G4double           productLengthExtra;
  G4ThreeVector      productNormalIn;
  G4ThreeVector      productNormalOut;
  G4bool             angledFaces;
};



#endif
