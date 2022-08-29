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
#ifndef BDSMODULATORSIN_H
#define BDSMODULATORSIN_H

#include "BDSModulator.hh"

#include "G4Types.hh"

/**
 * @brief Sinusoidal modulator.
 * 
 * factor = offset + scale * sin(2pi*frequency + phase)
 * 
 * @author Laurie Nevay
 */

class BDSModulatorSin: public BDSModulator
{
public:
  BDSModulatorSin() = delete;
  explicit BDSModulatorSin(G4double frequencyIn,
			   G4double phaseIn   = 0,
			   G4double amplitudeOffsetIn  = 0,
			   G4double amplitudeScaleIn   = 1.0);
  virtual ~BDSModulatorSin() = default;

  /// Returns value of equation in brief at the top of this file.
  virtual G4double Factor(const G4ThreeVector& xyz,
                          G4double T) const;

private:
  G4double angularFrequency;
  G4double phase;
  G4double offset;
  G4double scale;
};

#endif
