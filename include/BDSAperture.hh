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
#ifndef BDSAPERTURE_H
#define BDSAPERTURE_H

#include "BDSApertureType.hh"

#include "globals.hh"

/**
 * @brief A definition of an aperture shape.
 *
 * @author Laurie Nevay
 */

class BDSAperture
{
public:
  explicit BDSAperture(BDSApertureType apertureTypeIn,
		       G4double        offsetXIn = 0,
		       G4double        offsetYIn = 0,
		       G4double        tiltIn    = 0);
  virtual ~BDSAperture(){;}

  void SetTiltOffset(G4double offsetXIn,
		     G4double offsetYIn,
		     G4double tiltIn);
  
  BDSApertureType apertureType;
  G4double        offsetX;
  G4double        offsetY;
  G4double        tilt;

  friend bool operator == (const BDSAperture& lhs, const BDSAperture& rhs) {return (lhs.apertureType == rhs.apertureType) && lhs.Equals(&rhs);}
  friend bool operator != (const BDSAperture& lhs, const BDSAperture& rhs) {return !(lhs == rhs);}
  friend bool operator <  (const BDSAperture& lhs, const BDSAperture& rhs) {return lhs.LessThan(&rhs);}
  friend bool operator >  (const BDSAperture& lhs, const BDSAperture& rhs) {return rhs < lhs;}
  friend bool operator <= (const BDSAperture& lhs, const BDSAperture& rhs) {return !(lhs > rhs);}
  friend bool operator >= (const BDSAperture& lhs, const BDSAperture& rhs) {return !(lhs < rhs);}

  virtual bool Equals(const BDSAperture* other) const = 0;
  virtual bool LessThan(const BDSAperture* other) const = 0;
  virtual void CheckInfoOK() const = 0;

  void CheckRequiredParametersSet(G4double aper1,     G4bool aper1Set,
				  G4double aper2 = 0, G4bool aper2Set = false,
				  G4double aper3 = 0, G4bool aper3Set = false,
				  G4double aper4 = 0, G4bool aper4Set = false);
};

#endif
