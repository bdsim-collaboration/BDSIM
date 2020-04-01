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
#ifndef BDSAPERTURERECTELLIPSE_H
#define BDSAPERTURERECTELLIPSE_H

#include "BDSAperture.hh"
#include "BDSTiltOffset.hh"

#include "G4Types.hh"

#include <algorithm>

class BDSExtent;
class BDSPolygon;

/**
 * @brief RectEllipse aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureRectEllipse: public BDSAperture
{
public:
  explicit BDSApertureRectEllipse(G4double rectangleAIn,
				  G4double rectangleBIn,
				  G4double ellipseAIn,
				  G4double ellipseBIn);
  virtual ~BDSApertureRectEllipse(){;}

  G4double rectangleA;
  G4double rectangleB;
  G4double ellipseA;
  G4double ellipseB;

  virtual G4bool    Equals(const BDSAperture* other) const;
  virtual void      CheckInfoOK()                    const;
  virtual G4double  RadiusToEncompass()              const {return std::max(ellipseA, ellipseB) + tiltOffset.Radius();}
  virtual BDSExtent Extent()                         const;
  virtual G4int     MinimumNumberOfPoints()          const {return 4;}

  BDSApertureRectEllipse        operator+ (G4double number) const;
  const BDSApertureRectEllipse& operator+=(G4double number);
  BDSApertureRectEllipse        operator* (G4double number) const;
  const BDSApertureRectEllipse& operator*=(G4double number);

  virtual BDSPolygon Polygon(G4int nPointsIn = 0) const;
};

#endif
