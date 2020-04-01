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
#ifndef BDSAPERTURECLICPCL_H
#define BDSAPERTURECLICPCL_H

#include "BDSAperture.hh"
#include "BDSTiltOffset.hh"

#include "G4Types.hh"

#include <algorithm>

class BDSExtent;
class BDSPolygon;

/**
 * @brief Rectcircle aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureClicPCL: public BDSAperture
{
public:
  BDSApertureClicPCL(G4double xIn,
			         G4double yTopIn,
			         G4double yBottomIn,
			         G4double ySepIn);
  virtual ~BDSApertureClicPCL(){;}

  G4double x;
  G4double yTop;
  G4double yBottom;
  G4double ySep;

  virtual G4bool    Equals(const BDSAperture* other) const;
  virtual void      CheckInfoOK()                    const;
  virtual G4double  RadiusToEncompass()              const {return std::max({yBottom, ySep+yTop, x}) + tiltOffset.Radius();}
  virtual BDSExtent Extent()                         const;
  virtual G4int     MinimumNumberOfPoints()          const {return 10;}

  BDSApertureClicPCL        operator+ (G4double number) const;
  const BDSApertureClicPCL& operator+=(G4double number);
  BDSApertureClicPCL        operator* (G4double number) const;
  const BDSApertureClicPCL& operator*=(G4double number);

  virtual BDSPolygon Polygon(G4int nPointsIn = 0) const;

private:
  BDSApertureClicPCL() = delete;
};

#endif
