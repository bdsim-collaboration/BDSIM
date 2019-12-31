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
#ifndef BDSAPERTURECIRCULAR_H
#define BDSAPERTURECIRCULAR_H

#include "BDSAperture.hh"

#include "globals.hh"

/**
 * @brief Circular aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureCircular: public BDSAperture
{
public:
  BDSApertureCircular(G4double radiusIn);
  virtual ~BDSApertureCircular(){;}

  G4double radius;

  virtual G4bool    Equals(const BDSAperture* other)   const;
  virtual G4bool    LessThan(const BDSAperture* other) const;
  virtual void      CheckInfoOK()                      const;
  virtual G4double  RadiusToEncompass()                const {return radius;}
  virtual BDSExtent Extent()                           const;

  BDSApertureCircular        operator+ (const G4double number) const;
  const BDSApertureCircular& operator+=(const G4double number);
  BDSApertureCircular        operator* (const G4double number) const;
  const BDSApertureCircular& operator*=(const G4double number);
};

#endif
