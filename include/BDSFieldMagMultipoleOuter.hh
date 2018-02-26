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
#ifndef BDSFIELDMAGMULTIPOLEOUTER_H
#define BDSFIELDMAGMULTIPOLEOUTER_H

#include "BDSFieldMag.hh"

#include "globals.hh" // geant4 types / globals
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

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

class BDSFieldMagMultipoleOuter: public BDSFieldMag
{
public:
  BDSFieldMagMultipoleOuter(const G4int              orderIn,
			    const BDSMagnetStrength* stIn,
			    const G4double&          poleTipRadius,
			    BDSFieldMag*             innerFieldIn,
			    const G4double&          beamPipeRadius = 0);

  virtual ~BDSFieldMagMultipoleOuter();

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const double         t = 0) const;

private:
  const G4int       order;         ///< N-poles / 2.
  G4double          normalisation; ///< Storage of the overal normalisation factor.
  G4RotationMatrix* rotation;      ///< Rotation into frame of magnetic dipole.
  G4RotationMatrix* antiRotation;  ///< Corresponding anti-rotation.
  G4double          factor;        ///< Scale factor for angle to query in dipole to get n-pole.
  G4bool            negativeField; ///< Sign of magnetic field.
  const G4TwoVector m;             ///< Magnetic dipole vector.
  BDSFieldMag*      innerField;    ///< Field for inside pole tips.
  G4bool            useInnerField; ///< Whether to use it or not.
  G4double          poleTipRadius; ///< Radius of transition between inner and outer fields.
  G4double          transitionLengthScale; ///< Length scale over which to fade between fields.
};

#endif
