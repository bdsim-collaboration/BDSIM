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
#ifndef BDSMAGUSUALEQRHS_H
#define BDSMAGUSUALEQRHS_H

#include "BDSAuxiliaryNavigator.hh"
#include "BDSIntegratorDrift.hh"

#include "globals.hh" // geant4 types / globals
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4ChargeState.hh"

/**
 * @brief
 *
 *
 * @author
 */

class BDSMagUsualEqRhs: public G4Mag_UsualEqRhs
{
public:
  BDSMagUsualEqRhs();
  virtual ~BDSMagUsualEqRhs();

  inline G4double FCof() const;

  inline G4double FMass() const;

  virtual void SetChargeMomentumMass(G4ChargeState particleCharge,
                                     G4double MomentumXc,
                                     G4double particleMass);

protected:

  G4double fMassCof;
  G4double fCof_val;

};

inline G4double BDSMagUsualEqRhs::FCof() const
  {
    return fCof_val;
  }

inline G4double BDSMagUsualEqRhs::FMass() const
{
  return fMassCof;
}

#endif
