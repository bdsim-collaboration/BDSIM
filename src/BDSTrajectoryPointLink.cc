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
#include "BDSTrajectoryPointLink.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Allocator.hh"

//G4Allocator<BDSHitEnergyDepositionExtra> BDSAllocatorEnergyDepositionExtra;

BDSTrajectoryPointLink::BDSTrajectoryPointLink(
							 G4int    chargeIn,					 // charge
							 G4double kineticEnergyIn,		     // kinetic energy
							 G4int    turnsTakenIn,             // turns taken if circular
							 G4double massIn,					 // mass
							 G4double rigidityIn):				 // rigidity
  charge(chargeIn),
  kineticEnergy(kineticEnergyIn),
  turnsTaken(turnsTakenIn),
  mass(massIn),
  rigidity(rigidityIn)

{;}

BDSTrajectoryPointLink::~BDSTrajectoryPointLink()
{;}
