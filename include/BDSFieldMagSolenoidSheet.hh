/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#ifndef BDSFIELDMAGSOLENOIDSHEET_H
#define BDSFIELDMAGSOLENOIDSHEET_H

#include "BDSFieldMag.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

class BDSMagnetStrength;

/**
 * @brief Class that provides the magnetic field due to a cylinder of current.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMagSolenoidSheet: public BDSFieldMag
{
public:
  BDSFieldMagSolenoidSheet() = delete;
  /// This constructor uses the "field" and "length" parameters
  /// from the BDSMagnetStrength instance and forwards to the next constructor.
  BDSFieldMagSolenoidSheet(BDSMagnetStrength const* strength,
			   G4double radiusIn);
  BDSFieldMagSolenoidSheet(G4double fullLength,
                           G4double sheetRadius,
                           G4double B0);

  virtual ~BDSFieldMagSolenoidSheet(){;}

  /// Access the field value.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t = 0) const;
  
private:
  /// Calculate the Bz and Br integrals for a given zeta (z +- L/2).
  void bzbr(G4double rho, G4double zeta,
            G4double& bz, G4double& br) const;
  
  G4double a;
  G4double halfLength;
  G4double B0;
  G4double mu0OverPi;
  G4double Iprime;
  G4double normalisation;
};

#endif
