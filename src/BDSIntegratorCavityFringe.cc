/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include "BDSIntegratorCavityFringe.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Mag_EqRhs.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

BDSIntegratorCavityFringe::BDSIntegratorCavityFringe(BDSMagnetStrength const* strength,
                                                   G4Mag_EqRhs* eqOfMIn,
                                                   G4double maximumRadiusIn):
  BDSIntegratorRMatrixThin(strength, eqOfMIn, maximumRadiusIn),
  phase((*strength)["phase"]),
  efield((*strength)["efield"]),
  isentrance((*strength)["isentrance"]),
  eq(static_cast<BDSMagUsualEqRhs*>(eqOfM))
{
  rmat11  = (*strength)["rmat11"];
  rmat21  = (*strength)["rmat21"];
  rmat22  = (*strength)["rmat22"];
  rmat33  = (*strength)["rmat33"];
  rmat43  = (*strength)["rmat43"];
  rmat44  = (*strength)["rmat44"];
}

void BDSIntegratorCavityFringe::Stepper(const G4double yIn[],
                                       const G4double /*dydx*/[],
                                       const G4double h,
                                       G4double       yOut[],
                                       G4double       yErr[])
{BDSIntegratorRMatrixThin::Stepper(yIn,0,h,yOut,yErr);}
