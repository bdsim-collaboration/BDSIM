/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldGaborLens.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "TMath.h"

#include <cmath>
#include <utility>

BDSFieldGaborLens::BDSFieldGaborLens(BDSMagnetStrength const* strength):
bfieldMag((*strength)["field"])
{
  //TODO: some checks
}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldGaborLens::GetField(const G4ThreeVector& position,
                                                                    const G4double       /*t*/) const
{
  // Final local B and E fields:
  G4ThreeVector LocalB = G4ThreeVector(0, 0, 0);
  G4ThreeVector LocalE = G4ThreeVector(0, 0, 0);

  return std::make_pair(LocalB, LocalE);;
}
