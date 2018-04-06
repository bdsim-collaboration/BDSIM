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

#include "BDSDebug.hh"
#include "BDSIntegratorRMatrix.hh"

BDSIntegratorRMatrix::BDSIntegratorRMatrix(BDSMagnetStrength const* strength,
                                           G4Mag_EqRhs* eqOfMIn) :
  BDSIntegratorMag(eqOfMIn, 6)
{}

void BDSIntegratorRMatrix::Stepper(const G4double yIn[],
                                   const G4double unitMomentum[],
                                   const G4double h,
                                   G4double       yOut[],
                                   G4double       yErr[]) {
    for (G4int i = 0; i < 3; i++)
    {
        yOut[i]   = yIn[i] + h * unitMomentum[i]; // update position
        yOut[i+3] = yIn[i+3];                     // momentum doesn't change
        yErr[i]   = 0;
        yErr[i+3] = 0;
    }

    return;
}