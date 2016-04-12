#include "TRKParticleDefinition.hh"
#include "TRKPhysicsCalculations.hh"

#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"
#include "parser/options.h"

#include <cmath>
#include <string>
#include <iostream>

void TRK::CalculateKineticEnergy(const GMAD::Options& options)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << std::endl;
#endif
  double particlemass = TRKParticleDefinition::Instance()->GetParticleMass((std::string)options.particleName);

  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  
  // set kinetic beam parameters other than total energy
  globals->SetBeamMomentum( sqrt(pow(globals->BeamTotalEnergy(),2) - pow(particlemass,2)) );
  
  globals->SetBeamKineticEnergy(globals->BeamTotalEnergy() - particlemass );
  
  globals->SetParticleMomentum(sqrt(pow(globals->ParticleTotalEnergy(),2) - pow(particlemass,2)) );
  
  globals->SetParticleKineticEnergy(globals->ParticleTotalEnergy() - particlemass);

}
