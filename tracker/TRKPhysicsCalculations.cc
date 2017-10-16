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
