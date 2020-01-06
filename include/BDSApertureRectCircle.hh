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
#ifndef BDSAPERTURERECTCIRCLE_H
#define BDSAPERTURERECTCIRCLE_H

#include "BDSAperture.hh"
#include "BDSTiltOffset.hh"

#include "G4Types.hh"

class BDSExtent;
class BDSPolygon;

/**
 * @brief Rectcircle aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureRectCircle: public BDSAperture
{
public:
  BDSApertureRectCircle(G4double aIn,
			G4double bIn,
			G4double radiusIn);
  virtual ~BDSApertureRectCircle(){;}

  G4double a;
  G4double b;
  G4double radius;

  virtual G4bool    Equals(const BDSAperture* other)   const;
  virtual void      CheckInfoOK()                      const;
  virtual G4double  RadiusToEncompass()                const {return radius + tiltOffset.Radius();}
  virtual BDSExtent Extent()                           const;
  virtual G4int     MinimumNumberOfPoints()            const {return 4;}

  BDSApertureRectCircle        operator+ (const G4double number) const;
  const BDSApertureRectCircle& operator+=(const G4double number);
  BDSApertureRectCircle        operator* (const G4double number) const;
  const BDSApertureRectCircle& operator*=(const G4double number);

  virtual BDSPolygon Polygon(G4int nPointsIn = 0) const;

private:
  BDSApertureRectCircle() = delete;
};

#endif
