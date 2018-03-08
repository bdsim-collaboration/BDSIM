/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#ifndef BDSFIELDMAGDIPOLEOUTER_H
#define BDSFIELDMAGDIPOLEOUTER_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

#include <vector>

class BDSMagnetStrength;

/**
 * @brief A simple parameterisation of N-Pole outer yoke magnetic field.
 *
 * Field that roughly approximates the field in a yoke of an n-pole magnet.
 * The field for a magnetic dipole is used and mapped to higher orders.
 * This field only varies with x,y and not z - assumed x,y are transverse.
 * The order is the number of poles / 2 -> for a quadrupole this is 2. The
 * field is normalised to the regular magnetic field according to the strength
 * in the supplied BDSMagnetStrength instance along with a the pole tip radius.
 * The normal n-pole field is sampled at this radius.
 *
 * @author Laurie Nevay
 */

class BDSFieldMagDipoleOuter: public BDSFieldMag
{
public:
  BDSFieldMagDipoleOuter(const BDSMagnetStrength* strength,
			 const G4double&          poleTipRadius);

  virtual ~BDSFieldMagDipoleOuter(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const double         t = 0) const;

private:
  G4double      normalisation; ///< Storage of the overal normalisation factor.
  G4ThreeVector localField;    ///< Nominal dipole field.
  G4ThreeVector m;             ///< Dipole moment as unit vector of field direction.
};

#endif
