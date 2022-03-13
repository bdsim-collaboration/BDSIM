/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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

namespace GMAD
{
  class Aperture;
  struct Element;
  class SamplerPlacement;
}

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

  BDSAperture* CreateAperture(const GMAD::Element& el) const;
  BDSAperture* CreateAperture(const GMAD::Aperture& ap) const;
  BDSAperture* CreateAperture(const GMAD::SamplerPlacement& sp) const;
  BDSAperture* CreateAperture(BDSApertureType at,
			      G4double        a1,
			      G4double        a2,
			      G4double        a3,
			      G4double        a4,
			      G4double        tilt,
			      G4double        offsetX,
			      G4double        offsetY,
			      unsigned int    nPoints) const;
  
  /// Create any aperture shape with flat ends. The apertureOut is optionally
  /// for defining the possibly different aperture shape at the output end.
  G4VSolid* CreateSolid(const G4String&    name,
			G4double           length,
			const BDSAperture* apertureIn,
			const BDSAperture* apertureOut = nullptr,
			const G4ThreeVector* normalIn  = nullptr,
			const G4ThreeVector* normalOut = nullptr,
			G4double           lengthExtraForBoolean = 0);
  
  G4VSolid* CreateSolidWithInner(const G4String&      name,
                                 G4double             length,
                                 const BDSAperture*   apertureInInside,
                                 const BDSAperture*   apertureOutInside,
                                 G4double             thickness,
                                 const G4ThreeVector* normalIn,
                                 const G4ThreeVector* normalOut,
                                 G4double             lengthExtraForBoolean = 0);
  G4VSolid* CreateSolidWithInnerVariableThickness(const G4String& name,
                                                  G4double length,
                                                  const BDSAperture* apertureInOutside,
                                                  const BDSAperture* apertureInInside,
                                                  const BDSAperture* apertureOutOutside = nullptr,
                                                  const BDSAperture* apertureOutInside  = nullptr,
                                                  const G4ThreeVector* normalIn  = nullptr,
                                                  const G4ThreeVector* normalOut = nullptr,
                                                  G4double           lengthExtraForBoolean = 0);

private:
  /// @{ Flat faced construction function.
  G4VSolid* CreateCircle()      const;
  G4VSolid* CreateRectangle()   const;
  G4VSolid* CreateEllipse()     const;
  G4VSolid* CreateRectCircle()  const;
  G4VSolid* CreateRectEllipse() const;
  G4VSolid* CreateRaceTrack()   const;
  G4VSolid* CreateOctagonal()   const;
  G4VSolid* CreateClicPCL()     const;
  /// @}

  /// Utility to make angled solid for intersection when cutting angled faces.
  /// Makes use of members assumed to be set for current factory product.
  G4VSolid* CutSolid(const G4String& name,
                     G4double radiusToEncompass) const;

  /// Create a solid as an extruded solid
  G4VSolid* CreateExtrudedSolid() const;

  /// Create a shape with different ends.
  G4VSolid* CreateDifferentEnds() const;
  
  G4VSolid* CreateTubeByPoints()  const;

  /// @{ Specialisation for particular solids.
  G4VSolid* CircleToCircle() const;
  /// @}
  /// @{ Specialisation for particular solids for hollow solid.
  G4VSolid* HollowCircleToCircle() const;
  /// @}

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
  
  std::map<std::pair<BDSApertureType, BDSApertureType>, Constructor> hollowSpecialisations;
};



#endif
