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

#ifndef BDSINTEGRATORRMATRIXTHIN_HH
#define BDSINTEGRATORRMATRIXTHIN_HH

#include "BDSIntegratorMag.hh"

#include "globals.hh"

class G4Mag_EqRhs;
class BDSMagnetStrength;

/**
 * @brief Integrator that just moves the particle parallel to the s axis
 *
 * Teleporter for moving particles without changing angle but only advances the particle
 * by h
 *
 */

class BDSIntegratorRMatrixThin: public BDSIntegratorMag
{
public:
  BDSIntegratorRMatrixThin(BDSMagnetStrength const* strength, G4Mag_EqRhs* eqOfMIn);

  virtual ~BDSIntegratorRMatrixThin(){;}

  virtual void Stepper(const G4double y[],
                       const G4double unitMomentum[],
                       const G4double h,
                       G4double       yOut[],
                       G4double       yErr[]);

private:
  BDSIntegratorRMatrixThin();

  double kick1;
  double kick2;
  double kick3;
  double kick4;
  double rmat11;
  double rmat12;
  double rmat13;
  double rmat14;
  double rmat21;
  double rmat22;
  double rmat23;
  double rmat24;
  double rmat31;
  double rmat32;
  double rmat33;
  double rmat34;
  double rmat41;
  double rmat42;
  double rmat43;
  double rmat44;
};

#endif // BDSINTEGRATORRMATRIXTHIN_HH
