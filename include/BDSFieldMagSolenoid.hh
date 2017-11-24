/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#ifndef BDSFIELDMAGSOLENOID_H
#define BDSFIELDMAGSOLENOID_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSMagnetStrength;

/**
 * @brief Class the provides the field in a solenoid.
 * 
 * This simply returns a uniform field that is independent of
 * position.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagSolenoid: public BDSFieldMag
{
public:
  BDSFieldMagSolenoid(BDSMagnetStrength const* strength,
		      G4double          const  brho);
  
  virtual ~BDSFieldMagSolenoid(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const G4double       t = 0) const;
  
private:
  /// Private default constructor to ensure use of supplied constructor
  BDSFieldMagSolenoid();

  /// The value of the uniform magnetic field in local coordinates.
  G4ThreeVector localField;
};

#endif
