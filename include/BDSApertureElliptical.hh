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
#ifndef BDSAPERTUREELLIPTICAL_H
#define BDSAPERTUREELLIPTICAL_H

#include "BDSAperture.hh"

#include "globals.hh"

/**
 * @brief Elliptical aperture.
 *
 * @author Laurie Nevay
 */

class BDSApertureElliptical: public BDSAperture
{
public:
  BDSApertureElliptical(G4double aIn,
			G4double bIn,
			G4int    nPointsIn = 0);
  virtual ~BDSApertureElliptical(){;}

  G4double a;
  G4double b;
  G4int    nPoints;

  virtual G4bool    Equals(const BDSAperture* other)   const;
  virtual G4bool    LessThan(const BDSAperture* other) const;
  virtual void      CheckInfoOK()                      const;
  virtual G4double  RadiusToEncompass()                const;
  virtual BDSExtent Extent()                           const;
  virtual G4int     MinimumNumberOfPoints()            const {return nPoints;}

  BDSApertureElliptical        operator+ (const G4double number) const;
  const BDSApertureElliptical& operator+=(const G4double number);
  BDSApertureElliptical        operator* (const G4double number) const;
  const BDSApertureElliptical& operator*=(const G4double number);
};

#endif
