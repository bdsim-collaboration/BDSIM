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
#ifndef BDSAPERTURERECTANGULAR_H
#define BDSAPERTURERECTANGULAR_H

#include "BDSAperture.hh"

#include "G4Types.hh"

#include <cmath>

/**
 * @brief Rectangular aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureRectangular: public BDSAperture
{
public:
  explicit BDSApertureRectangular(G4double aIn,
				  G4double bIn);
  virtual ~BDSApertureRectangular(){;}

  G4double a;
  G4double b;

  virtual G4bool    Equals(const BDSAperture* other)   const;
  virtual G4bool    LessThan(const BDSAperture* other) const;
  virtual void      CheckInfoOK()                      const;
  virtual G4double  RadiusToEncompass()                const {return std::hypot(a,b);}
  virtual BDSExtent Extent()                           const;
  virtual G4int     MinimumNumberOfPoints()            const {return 4;}

  BDSApertureRectangular        operator+ (const G4double number) const;
  const BDSApertureRectangular& operator+=(const G4double number);
  BDSApertureRectangular        operator* (const G4double number) const;
  const BDSApertureRectangular& operator*=(const G4double number);
};

#endif
