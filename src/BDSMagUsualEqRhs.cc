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
#include "BDSGlobalConstants.hh"
#include "BDSMagUsualEqRhs.hh"
#include "BDSStep.hh"
#include "G4Mag_EqRhs.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ClassicalRK4.hh"
#include "G4ChargeState.hh"
#include "G4MagneticField.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "BDSGlobalConstants.hh"

BDSMagUsualEqRhs::BDSMagUsualEqRhs(G4MagneticField* MagField) : G4Mag_UsualEqRhs(MagField),
                                                                fMassCof(0.),
                                                                fCof_val(0.)
{}

BDSMagUsualEqRhs::~BDSMagUsualEqRhs() {}

void BDSMagUsualEqRhs::SetChargeMomentumMass( G4ChargeState particleCharge,
                       G4double MomentumXc,
                       G4double particleMass)
{
  G4Mag_EqRhs::SetChargeMomentumMass(particleCharge, MomentumXc, particleMass);
  G4double pcharge = particleCharge.GetCharge();

  fCof_val = pcharge*eplus*c_light ; //  B must be in Tesla
  fMassCof = particleMass*particleMass;
}

G4double BDSMagUsualEqRhs::Beta(const G4double y[])
{
  G4ThreeVector mom = G4ThreeVector(y[3], y[4], y[5]);
  G4double beta = Beta(mom);
  return beta;
}

G4double BDSMagUsualEqRhs::Beta(const G4ThreeVector mom)
{
  G4double momentum_mag_square = mom.mag2();
  G4double Energy = std::sqrt(momentum_mag_square + fMassCof);
  G4double beta   = std::sqrt(momentum_mag_square) / Energy;
  return beta;
}

G4double BDSMagUsualEqRhs::TotalEnergy(const G4double y[])
{
  G4ThreeVector mom = G4ThreeVector(y[3], y[4], y[5]);
  G4double Energy = TotalEnergy(mom);
  return Energy;
}

G4double BDSMagUsualEqRhs::TotalEnergy(const G4ThreeVector mom)
{
  G4double momentum_mag_square = mom.mag2();
  G4double Energy = std::sqrt(momentum_mag_square + fMassCof);
  return Energy;
}

void BDSMagUsualEqRhs::EvaluateRhsGivenB(const G4double y[],
                                         const G4double B[3],
                                         G4double dydx[]) const
{
  G4double momentum_mag_square = y[3]*y[3] + y[4]*y[4] + y[5]*y[5];
  G4double inv_momentum_magnitude = 1.0 / std::sqrt( momentum_mag_square );

  G4double cof = FCof()*inv_momentum_magnitude;

  dydx[0] = y[3]*inv_momentum_magnitude;       //  (d/ds)x = Vx/V
  dydx[1] = y[4]*inv_momentum_magnitude;       //  (d/ds)y = Vy/V
  dydx[2] = y[5]*inv_momentum_magnitude;       //  (d/ds)z = Vz/V

  dydx[3] = cof*(y[4]*B[2] - y[5]*B[1]) ;   // Ax = a*(Vy*Bz - Vz*By)
  dydx[4] = cof*(y[5]*B[0] - y[3]*B[2]) ;   // Ay = a*(Vz*Bx - Vx*Bz)
  dydx[5] = cof*(y[3]*B[1] - y[4]*B[0]) ;   // Az = a*(Vx*By - Vy*Bx)

  return ;
}
