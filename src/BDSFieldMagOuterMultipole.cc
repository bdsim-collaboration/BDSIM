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
#include "BDSFieldMagOuterMultipole.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>

BDSFieldMagOuterMultipole::BDSFieldMagOuterMultipole(const G4int    nPolesIn,
						     const G4double fieldStrengthIn,
						     const G4double phiOffsetIn):
  nPoles(nPolesIn),
  fieldStrength(fieldStrengthIn),
  phiOffset(phiOffsetIn)
{
  itsSectorPhi=CLHEP::twopi/G4double(nPoles);
}

G4ThreeVector BDSFieldMagOuterMultipole::GetField(const G4ThreeVector &position,
						  const G4double       /*t*/) const
{
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
}



