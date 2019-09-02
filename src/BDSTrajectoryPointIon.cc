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
#include "BDSTrajectoryPointIon.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Allocator.hh"

//G4Allocator<BDSHitEnergyDepositionExtra> BDSAllocatorEnergyDepositionExtra;

BDSTrajectoryPointIon::BDSTrajectoryPointIon(G4bool   isIonIn,					 // particle type = ion
							 G4int    ionAIn, 					 // ion atomic mass number (Z+N)
							 G4int    ionZIn, 					 // ion atomic / proton number
							 G4int	  nElectronsIn): 			 // number of electrons on ion
  isIon(isIonIn),
  ionA(ionAIn),
  ionZ(ionZIn),
  nElectrons(nElectronsIn)
{;}

BDSTrajectoryPointIon::~BDSTrajectoryPointIon()
{;}
