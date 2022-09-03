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
#include "BDSModulatorSinT.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSModulatorSinT::BDSModulatorSinT(G4double frequencyIn,
				 G4double phaseIn,
				 G4double amplitudeOffsetIn,
				 G4double amplitudeScaleIn):
  angularFrequency(CLHEP::twopi * frequencyIn),
  phase(phaseIn),
  offset(amplitudeOffsetIn),
  scale(amplitudeScaleIn)
{;}

G4double BDSModulatorSinT::Factor(const G4ThreeVector& /*xyz*/,
				 G4double T) const
{
  G4double factor = offset + scale*std::sin(angularFrequency*T + phase);
  return factor;
}
