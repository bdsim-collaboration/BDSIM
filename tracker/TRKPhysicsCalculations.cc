#include "TRKParticleDefinition.hh"
#include "TRKPhysicsCalculations.hh"

#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"
#include "parser/options.h"

#include <cmath>
#include <string>
#include <iostream>

void TRK::CalculateKineticEnergy(Options& options)
{
#ifdef TRKDEBUG
  std::cout << __METHOD_NAME__ << std::endl;
#endif
  double particlemass = TRKParticleDefinition::Instance()->GetParticleMass((std::string)options.particleName);
  
  // set kinetic beam parameters other than total energy
  BDSGlobalConstants::Instance()->SetBeamMomentum( sqrt(pow(BDSGlobalConstants::Instance()->GetBeamTotalEnergy(),2)-
							pow(particlemass,2)) );
  
  BDSGlobalConstants::Instance()->SetBeamKineticEnergy(BDSGlobalConstants::Instance()->GetBeamTotalEnergy() - 
						       particlemass );


  BDSGlobalConstants::Instance()->SetParticleMomentum (sqrt(pow(BDSGlobalConstants::Instance()->GetParticleTotalEnergy(),2)-
							    pow(particlemass,2)) );
  
  BDSGlobalConstants::Instance()->SetParticleKineticEnergy(BDSGlobalConstants::Instance()->GetParticleTotalEnergy() - 
							   particlemass );

}
