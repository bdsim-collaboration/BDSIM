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
#include "BDSSDFilterKineticEnergy.hh"

#include "G4Step.hh"

BDSSDFilterKineticEnergy::BDSSDFilterKineticEnergy(const G4String& name,
						   G4double minimumKineticEnergyIn):
  G4VSDFilter(name),
  minimumKineticEnergy(minimumKineticEnergyIn)
{;}

G4bool BDSSDFilterKineticEnergy::Accept(const G4Step* aStep) const
{
  G4double kineticEnergy = aStep->GetTrack()->GetKineticEnergy();
  return kineticEnergy >= minimumKineticEnergy;
}
