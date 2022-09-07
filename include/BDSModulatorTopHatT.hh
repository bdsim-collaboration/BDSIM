/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#ifndef BDSMODULATORTOPHATT_H
#define BDSMODULATORTOPHATT_H

#include "BDSModulator.hh"

#include "G4Types.hh"

/**
 * @brief Top-hat modulator as a function of T
 * 
 * factor = amplitudeOffset + amplitudeScale, T0 <= T <= T1 ; else 0
 * 
 * @author Laurie Nevay
 */

class BDSModulatorTopHatT: public BDSModulator
{
public:
  BDSModulatorTopHatT() = delete;
  explicit BDSModulatorTopHatT(G4double T0,
			       G4double T1,
			       G4double amplitudeOffsetIn  = 0,
			       G4double amplitudeScaleIn   = 1.0);
  virtual ~BDSModulatorTopHatT() = default;

  /// Returns value of equation in brief at the top of this file.
  virtual G4double Factor(const G4ThreeVector& xyz,
                          G4double T) const;

private:
  G4double T0;
  G4double T1;
  G4double offset;
  G4double scale;
};

#endif
