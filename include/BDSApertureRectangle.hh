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
#ifndef BDSAPERTURERECTANGLE_H
#define BDSAPERTURERECTANGLE_H

#include "BDSAperture.hh"
#include "BDSTiltOffset.hh"

#include "G4Types.hh"

#include <cmath>

class BDSExtent;
class BDSPolygon;

/**
 * @brief Rectangular aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureRectangle: public BDSAperture
{
public:
  BDSApertureRectangle(G4double aIn,
                       G4double bIn);
  virtual ~BDSApertureRectangle(){;}

  G4double a;
  G4double b;

  virtual G4bool    Equals(const BDSAperture* other)   const;
  virtual void      CheckInfoOK()                      const;
  virtual G4double  RadiusToEncompass()                const {return std::hypot(a,b) + tiltOffset.Radius();}
  virtual BDSExtent Extent()                           const;
  virtual G4int     MinimumNumberOfPoints()            const {return 4;}

  BDSApertureRectangle        operator+ (const G4double number) const;
  const BDSApertureRectangle& operator+=(const G4double number);
  BDSApertureRectangle        operator* (const G4double number) const;
  const BDSApertureRectangle& operator*=(const G4double number);

  virtual BDSPolygon Polygon(G4int nPointsIn = 0) const;

private:
  BDSApertureRectangle() = delete;
};

#endif
