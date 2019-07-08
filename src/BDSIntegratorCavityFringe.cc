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
#include "BDSIntegratorRMatrixThin.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Mag_EqRhs.hh"

BDSIntegratorCavityFringe::BDSIntegratorCavityFringe(BDSMagnetStrength const* strength,
                                                   G4Mag_EqRhs* eqOfMIn,
                                                   G4double maximumRadiusIn):
  BDSIntegratorRMatrixThin(strength, eqOfMIn, maximumRadiusIn)
{
  kick1   = (*strength)["kick1"];
  kick2   = (*strength)["kick2"];
  kick3   = (*strength)["kick3"];
  kick4   = (*strength)["kick4"];
  rmat11  = (*strength)["rmat11"];
  rmat12  = (*strength)["rmat12"];
  rmat13  = (*strength)["rmat13"];
  rmat14  = (*strength)["rmat14"];
  rmat21  = (*strength)["rmat21"];
  rmat22  = (*strength)["rmat22"];
  rmat23  = (*strength)["rmat23"];
  rmat24  = (*strength)["rmat24"];
  rmat31  = (*strength)["rmat31"];
  rmat32  = (*strength)["rmat32"];
  rmat33  = (*strength)["rmat33"];
  rmat34  = (*strength)["rmat34"];
  rmat41  = (*strength)["rmat41"];
  rmat42  = (*strength)["rmat42"];
  rmat43  = (*strength)["rmat43"];
  rmat44  = (*strength)["rmat44"];

#ifdef BDSDEBUG
  G4cout << "RMatrix " << rmat11 << " " << rmat12 << " " << rmat13 << " " << rmat14 << " " << kick1 << G4endl;
  G4cout << "RMatrix " << rmat21 << " " << rmat22 << " " << rmat23 << " " << rmat24 << " " << kick2 <<  G4endl;
  G4cout << "RMatrix " << rmat31 << " " << rmat32 << " " << rmat33 << " " << rmat34 << " " << kick3 <<  G4endl;
  G4cout << "RMatrix " << rmat41 << " " << rmat42 << " " << rmat43 << " " << rmat44 << " " << kick4 <<  G4endl;
#endif
}

void BDSIntegratorCavityFringe::Stepper(const G4double yIn[],
                                       const G4double /*dydx*/[],
                                       const G4double h,
                                       G4double       yOut[],
                                       G4double       yErr[])
{BDSIntegratorRMatrixThin::Stepper(yIn,0,h,yOut,yErr);}
