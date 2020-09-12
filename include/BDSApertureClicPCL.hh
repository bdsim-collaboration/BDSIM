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
  BDSApertureClicPCL() = delete;
  BDSApertureClicPCL(G4double xIn,
			         G4double yTopIn,
			         G4double yBottomIn,
			         G4double ySepIn);
  virtual ~BDSApertureClicPCL(){;}

  G4double x;
  G4double yTop;
  G4double yBottom;
  G4double ySep;

  G4bool    Equals(const BDSAperture* other) const override;
  void      CheckInfoOK()                    const override;
  G4double  RadiusToEncompass()              const override {return std::max({yBottom, ySep+yTop, x}) + tiltOffset.Radius();}
  BDSExtent Extent()                         const override;
  unsigned int MinimumNumberOfPoints()       const override {return 10;}

  BDSApertureClicPCL        operator+ (G4double number) const;
  const BDSApertureClicPCL& operator+=(G4double number);
  BDSApertureClicPCL        operator* (G4double number) const;
  const BDSApertureClicPCL& operator*=(G4double number);
  
  BDSAperture* Plus(G4double number) const override;
  BDSAperture* Times(G4double number) const override;
  BDSAperture* Clone() const override;

protected:
  BDSPolygon PolygonNPoints(unsigned int nPointsIn) const override;
};

#endif
