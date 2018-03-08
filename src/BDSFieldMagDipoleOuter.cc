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
#include "BDSFieldMagDipole.hh"
#include "BDSFieldMagDipoleOuter.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <vector>

BDSFieldMagDipoleOuter::BDSFieldMagDipoleOuter(const BDSMagnetStrength* strength,
					       const G4double&          poleTipRadius):
  normalisation(1)
{
  BDSFieldMagDipole* innerField = new BDSFieldMagDipole(strength); // encapsulates logic about field parameters
  // store copy of nominal field strength in vector form
  localField = innerField->GetField(G4ThreeVector(0,0,0)); // doesn't vary with position
  m = localField.unit(); // ensure unit vector

  G4ThreeVector normalisationPoint = m*poleTipRadius;
  G4ThreeVector innerFieldValue = innerField->GetField(normalisationPoint);
  G4ThreeVector outerFieldValue = GetField(normalisationPoint);
  
  normalisation = innerFieldValue.mag() / outerFieldValue.mag();

  delete innerField;
}

G4ThreeVector BDSFieldMagDipoleOuter::GetField(const G4ThreeVector& position,
					       const G4double       /*t*/) const
{
  G4double rmag = position.mag();

  if (rmag < 1) // closer than 1mm from dipole
    {return localField;}

  // calculate the field according to a magnetic dipole m at position r.
  G4ThreeVector b = 3*position*(m.dot(position))/std::pow(rmag,5) - m/std::pow(rmag,3);

  b *= normalisation;

  return b;
}
