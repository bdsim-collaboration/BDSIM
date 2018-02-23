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
#ifndef BDSFIELDMAGOUTERMULTIPOLE_H
#define BDSFIELDMAGOUTERMULTIPOLE_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

class BDSMagnetStrength;

/**
 * @brief A simple parameterisation of N-Pole outer yoke magnetic field.
 *
 */

class BDSFieldMagOuterMultipole: public BDSFieldMag
{
public:
  BDSFieldMagOuterMultipole(const G4int              orderIn,
			    const BDSMagnetStrength* stIn,
			    const G4double&          poleTipRadius);

  virtual ~BDSFieldMagOuterMultipole();

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const double         t = 0) const;

private:
  const G4int    order;
  const G4double normalisation;
  G4RotationMatrix* rotation;
  G4RotationMatrix* antiRotation;
  G4double          factor;
  G4TwoVector       m;            ///< Magnetic dipole vector.
};

#endif
