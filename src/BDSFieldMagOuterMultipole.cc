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
#include "BDSFieldMagOuterMultipole.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>

BDSFieldMagOuterMultipole::BDSFieldMagOuterMultipole(const G4int              orderIn,
						     const BDSMagnetStrength* /*stIn*/,
						     const G4double&          /*poleTipRadius*/):
  order(orderIn),
  normalisation(1)
{
  G4double angle = CLHEP::pi/(2.*order);
  rotation = new G4RotationMatrix();
  antiRotation = new G4RotationMatrix();
  if (order > 1)
    {// don't do for dipole
      rotation->rotateZ(angle);
      antiRotation->rotateZ(-angle);
    }
  factor = (order + 1)/2.;
  m = G4TwoVector(0,1);
}

BDSFieldMagOuterMultipole::~BDSFieldMagOuterMultipole()
{
  delete rotation;
  delete antiRotation;
}

G4ThreeVector BDSFieldMagOuterMultipole::GetField(const G4ThreeVector &position,
						  const G4double       /*t*/) const
{
  // rotate from dipole frame to npole frame
  G4ThreeVector rotatedPosition(position);
  rotatedPosition = rotatedPosition.transform(*rotation);

  // construct 2D vector of position
  G4TwoVector r(rotatedPosition.x(), rotatedPosition.y());
  G4double rmag = r.mag(); // calculate magnitude

  // calculate angle in 2D polar coordinates from y axis vertical clockwise
  G4double angle = std::atan2(r.x(),r.y());
  if (angle < 0)
    {angle = CLHEP::twopi + angle;}

  // the point to query in the nominal dipole equation
  G4TwoVector query(0,rmag);
  query.rotate(-factor*angle);

  // calculate the field according to a magnetic dipole m at position r.
  G4TwoVector b = 3*query*(m.dot(query))/std::pow(rmag,5) - m/std::pow(rmag,3);

  // package in 3-vector
  G4ThreeVector result = G4ThreeVector(b.x(), b.y(), 0);

  // rotate back to n-pole frame
  G4ThreeVector rotatedResult = (*antiRotation)*result;
  rotatedResult *= normalisation;
  return rotatedResult;
}