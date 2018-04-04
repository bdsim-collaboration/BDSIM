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
#include "BDSIntegratorParallelTransport.hh"

BDSIntegratorParallelTransport::BDSIntegratorParallelTransport(double       aper,
                                                               G4Mag_EqRhs* eqOfMIn) :
        BDSIntegratorMag(eqOfMIn, 6)
{}

void BDSIntegratorParallelTransport::Stepper(const G4double yIn[],
                                             const G4double dydx[],
                                             const G4double h,
                                             G4double       yOut[],
                                             G4double       yErr[]) {
    
    return;
}