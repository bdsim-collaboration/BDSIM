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
#include "BDSTrajectoryPointLocal.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Allocator.hh"

//G4Allocator<BDSHitEnergyDepositionExtra> BDSAllocatorEnergyDepositionExtra;

BDSTrajectoryPointLocal::BDSTrajectoryPointLocal(
							 G4double xIn,                      // local x
							 G4double yIn,                      // local y
							 G4double zIn,                		 // local z
							 G4double pxIn,                     // local px
							 G4double pyIn,					 // local py
							 G4double pzIn):				 // local pz
  x(xIn),
  y(yIn),
  z(zIn),
  px(pxIn),
  py(pyIn),
  pz(pzIn)
{;}

BDSTrajectoryPointLocal::~BDSTrajectoryPointLocal()
{;}
