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
#include "TRKParticleDefinition.hh"
#include "TRKPhysicsCalculations.hh"

#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParticleDefinition.hh"

#include "parser/beam.h"

#include <string>


BDSParticleDefinition* TRK::DefineParticle(const GMAD::Beam& beam)
{
  double mass = TRKParticleDefinition::Instance()->GetParticleMass((std::string)beam.particleName);
  double charge = (double)TRKParticleDefinition::Instance()->GetParticleCharge((std::string)beam.particleName);
  double energy = BDSGlobalConstants::Instance()->BeamTotalEnergy();
  double ffact  = BDSGlobalConstants::Instance()->FFact();
  
  BDSParticleDefinition* particleDefB = new BDSParticleDefinition(beam.particleName, mass, charge,
								  energy, ffact);

  return particleDefB;
}
