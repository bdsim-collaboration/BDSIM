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

  G4TwoVector b = 3*query*(m.dot(query))/std::pow(rmag,5) - m/std::pow(rmag,3);

  G4ThreeVector result = G4ThreeVector(b.x(), b.y(), 0);
  
  G4ThreeVector rotatedResult = (*antiRotation)*result;
  return rotatedResult;
  /*
   *
   *   G4double quadrant = thet / sectorAngle;

  G4double firstQA = thet - (quadrant-1)*sectorAngle;

  if (thet > CLHEP::pi)
    {thet -= CLHEP::pi;}

  G4double thetp = thet*2;

  G4TwoVector rp = r;
  rp.rotate(-thet);

   *
   *

  G4TwoVector tu = ru;
  tu.rotate(CLHEP::halfpi);
  G4double theta = std::tan(r[0]/r[1]);

  //G4TwoVector m(0,1);
  G4double mmag = m.mag();

  //G4TwoVector b = (mmag / (std::pow(rmag,3))) * (2*std::cos(theta)*ru + std::sin(theta)*tu);

  //return G4ThreeVector(b.x(), b.y(), 0);

  
  G4double BFactor = fieldStrength/position.mag();
  G4double phi     = position.phi() - phiOffset;

  // extra term for dipoles, because of rotation required during positioning
  // of trapezoids
  if (nPoles==2)
    {phi += CLHEP::pi;}

  // define sectors relative to the y-axis
  phi = CLHEP::halfpi - phi;

  if(phi < 0)
    {phi += CLHEP::twopi;}
  if(phi > CLHEP::twopi)
    {phi -= CLHEP::twopi;}

  G4int nSector = G4int(phi/itsSectorPhi);

  BFactor *= std::pow(-1.0,nSector);

  G4ThreeVector localField;
  localField[0] = position.y()*BFactor;
  localField[1] = -position.x()*BFactor;
  localField[2] = 0;

  // extra term for dipoles, because of rotation required during positioning
  // of trapezoids
  if (nPoles==2)
    {localField[1] *= -1;}
  
  return localField;
   */
}



