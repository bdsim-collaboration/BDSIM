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
#include "BDSGlobalConstants.hh"
#include "BDSMagUsualEqRhs.hh"
#include "BDSStep.hh"
#include "G4Mag_EqRhs.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ClassicalRK4.hh"
#include "G4ChargeState.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "BDSGlobalConstants.hh"

void BDSMagUsualEqRhs::SetChargeMomentumMass( G4ChargeState particleCharge,
                       G4double MomentumXc,
                       G4double particleMass)
  {
    G4Mag_EqRhs::SetChargeMomentumMass( particleCharge, MomentumXc, particleMass);
    G4double pcharge = particleCharge.GetCharge();

    fCof_val = pcharge*eplus*c_light ; //  B must be in Tesla
    fMassCof = particleMass*particleMass;

  }