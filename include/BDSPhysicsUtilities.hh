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
#ifndef BDSPHYSICSUTILITIES_H
#define BDSPHYSICSUTILITIES_H

#include "globals.hh"

#include "parser/fastlist.h"
#include "parser/physicsbiasing.h"

class BDSParticleDefinition;
class G4VModularPhysicsList;

namespace BDS
{
  G4VModularPhysicsList* BuildPhysics(const G4String& physicsList);
  
  BDSParticleDefinition* ConstructBeamParticle(G4String particleName,
					       G4double totalEnergy,
					       G4double ffact = 1);


  void PrintPrimaryParticleProcesses(const G4VModularPhysicsList* physics);
  void PrintDefinedParticles(const G4VModularPhysicsList* physics);
  void BuildAndAttachBiasWrapper(G4VModularPhysicsList* physics,
				 const GMAD::FastList<GMAD::PhysicsBiasing>& biases);

};

#endif
