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
#ifndef BDSAPERTURECIRCLE_H
#define BDSAPERTURECIRCLE_H

#include "BDSAperture.hh"

#include "G4Types.hh"

class BDSExtent;
class BDSPolygon;

/**
 * @brief Circle aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureCircle: public BDSAperture
{
public:
  explicit BDSApertureCircle(G4double radiusIn,
                             G4int    nPointsIn = 0);
  virtual ~BDSApertureCircle(){;}

  G4double radius;
  G4int    nPoints;

  virtual G4bool    Equals(const BDSAperture* other)   const;
  virtual void      CheckInfoOK()                      const;
  virtual G4double  RadiusToEncompass()                const {return radius;}
  virtual BDSExtent Extent()                           const;
  virtual G4int     MinimumNumberOfPoints()            const {return nPoints;}

  BDSApertureCircle        operator+ (const G4double number) const;
  const BDSApertureCircle& operator+=(const G4double number);
  BDSApertureCircle        operator* (const G4double number) const;
  const BDSApertureCircle& operator*=(const G4double number);

  virtual BDSPolygon Polygon(G4int nPointsIn = 0) const;

private:
  BDSApertureCircle() = delete;
};

#endif
