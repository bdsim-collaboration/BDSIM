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
#include "TRKParticleDefinition.hh"
#include "TRKPhysicsCalculations.hh"

#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParticleDefinition.hh"

#include "parser/beam.h"

#include <string>

void TRK::ConstructDesignAndBeamParticle(const GMAD::BeamBase& beamDefinition,
					 double ffact,
					 BDSParticleDefinition*& designParticle,
					 BDSParticleDefinition*& beamParticle,
					 bool& beamDifferentFromDesignParticle)
{
  std::string designParticleName = beamDefinition.particle;
  G4double designTotalEnergy     = beamDefinition.beamEnergy * CLHEP::GeV;
  designParticle = TRK::ConstructParticleDefinition(designParticleName, designTotalEnergy, ffact);
  if ((beamDefinition.particle == beamDefinition.beamParticleName) &&
      (beamDefinition.beamEnergy == beamDefinition.E0))
    {// copy definition
      beamParticle = new BDSParticleDefinition(*designParticle);
      beamDifferentFromDesignParticle = false;
    }
  else
    {
      G4String beamParticleName = beamDefinition.beamParticleName;
      G4double beamTotalEnergy  = beamDefinition.E0 * CLHEP::GeV;
      beamParticle = TRK::ConstructParticleDefinition(beamParticleName,
						      beamTotalEnergy,
						      ffact);
      beamDifferentFromDesignParticle = true;
    }
}

BDSParticleDefinition* TRK::ConstructParticleDefinition(std::string particleNameIn,
							double totalEnergy,
							double ffact)
{
  double mass   = TRKParticleDefinition::Instance()->GetParticleMass(particleNameIn);
  double charge = (double)TRKParticleDefinition::Instance()->GetParticleCharge(particleNameIn);
  
  BDSParticleDefinition* particleDefB = new BDSParticleDefinition(particleNameIn,
								  mass, charge,
								  totalEnergy, ffact);

  return particleDefB;
}
