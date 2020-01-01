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
#include "BDSApertureType.hh"

#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <map>
#include <utility>

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

  /// Create any aperture shape with flat ends. The apertureOut is optionally
  /// for definining the possibly different aperture shape at the output end.
  G4VSolid* CreateAperture(const G4String&    name,
			   G4double           length,
			   const BDSAperture* apertureIn,
			   const BDSAperture* apertureOut = nullptr,
			   G4double           lengthExtraForBoolean = 0);

  /// Create any aperture shape with angled ends specified by unit normals.
  G4VSolid* CreateAperture(const G4String&      name,
			   G4double             length,
			   const BDSAperture*   apertureIn,
			   const G4ThreeVector& normalIn,
			   const G4ThreeVector& normalOut,
			   const BDSAperture*   apertureOut = nullptr,
			   G4double             lengthExtraForBoolean = 0);

private:
  /// Common construction code for the main CreateAperture interfaces.
  G4VSolid* CommonConstruction(const G4String&    name,
			       G4double           length,
			       const BDSAperture* apertureIn,
			       const BDSAperture* apertureOut,
			       G4double           lengthExtraForBoolean);

  /// @{ Flat faced construction function.
  G4VSolid* CreateCircular()    const;
  G4VSolid* CreateRectangular() const;
  G4VSolid* CreateElliptical()  const;
  G4VSolid* CreateLHC()         const;
  G4VSolid* CreateRectEllipse() const;
  G4VSolid* CreateRaceTrack()   const;
  G4VSolid* CreateOctagonal()   const;
  G4VSolid* CreateClicPCL()     const;
  /// @}

  /// Create a shape with different ends.
  G4VSolid* CreateDifferentEnds() const;
  
  G4VSolid* CreateTubeByPoints()  const;

  G4VSolid* CircularToCircular() const;

  /// Ratio of radii used for an intersection solid - greater than 1 and
  /// defined arbitrarily.
  const G4double intersectionRadiusRatio;

  /// @{ Cache of input variable to simplify function arguments.
  G4String           productName;
  G4double           productLength;
  const BDSAperture* productApertureIn;
  const BDSAperture* productApertureOut;
  G4double           productLengthExtra;
  G4ThreeVector      productNormalIn;
  G4ThreeVector      productNormalOut;
  G4bool             angledFaces;
  /// @}

  /// Create a pair where the two values are in order. We do this so we can access
  /// a map of unique pairs.
  std::pair<BDSApertureType, BDSApertureType> MakePair(BDSApertureType a1,
						       BDSApertureType a2) const;

  /// Typedef for function pointers to simplify syntax.
  typedef G4VSolid*(BDSApertureFactory::*Constructor)(void) const;

  /// Map of unique aperture type pairs to member function pointers.
  std::map<std::pair<BDSApertureType, BDSApertureType>, Constructor> specialisations;
};



#endif
