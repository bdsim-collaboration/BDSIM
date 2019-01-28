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
 * by h.
 *
 * If the new particle x,y coordinates are greater than maximumRadius, they are clipped
 * to this value.
 *
 * @author Stewart Boogert
 */

class BDSIntegratorRMatrixThin: public BDSIntegratorMag
{
public:
  BDSIntegratorRMatrixThin(BDSMagnetStrength const* strength,
			   G4Mag_EqRhs* eqOfMIn,
			   G4double maximumRadiusIn);

  virtual ~BDSIntegratorRMatrixThin(){;}

  virtual void Stepper(const G4double y[],
                       const G4double dydx[],
                       const G4double h,
                       G4double       yOut[],
                       G4double       yErr[]);

private:
  BDSIntegratorRMatrixThin();

  G4double kick1;
  G4double kick2;
  G4double kick3;
  G4double kick4;
  G4double rmat11;
  G4double rmat12;
  G4double rmat13;
  G4double rmat14;
  G4double rmat15;
  G4double rmat16;
  G4double rmat21;
  G4double rmat22;
  G4double rmat23;
  G4double rmat24;
  G4double rmat25;
  G4double rmat26;
  G4double rmat31;
  G4double rmat32;
  G4double rmat33;
  G4double rmat34;
  G4double rmat35;
  G4double rmat36;
  G4double rmat41;
  G4double rmat42;
  G4double rmat43;
  G4double rmat44;
  G4double rmat45;
  G4double rmat46;
  G4double rmat51;
  G4double rmat52;
  G4double rmat53;
  G4double rmat54;
  G4double rmat55;
  G4double rmat56;
  G4double rmat61;
  G4double rmat62;
  G4double rmat63;
  G4double rmat64;
  G4double rmat65;
  G4double rmat66;

  G4double maximumRadius;
};

#endif
