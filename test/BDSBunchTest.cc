#include <string>
#include <iostream>

#include "G4ParticleTable.hh"
#include "G4Electron.hh"

#include "parser/options.h"
#include "BDSGlobalConstants.hh"
#include "BDSBunch.hh"

extern Options options;

int main(void) {
  BDSBunch bdsBunch;
  // Number of particles to generate for test 
  int nparticle = 5;

  // fill options from file 
  gmad_parser();

  options.beampipeRadius   = 10;
  options.componentBoxSize = 11;
  options.tunnelRadius     = 12;

  // Set options for reference orbit 
  options.X0  = 0.0;
  options.Y0  = 0.0;
  options.Z0  = 0.0; 
  options.Xp0 = 0.0;
  options.Yp0 = 0.0;
  options.T0  = 0.0;    
  options.beamEnergy = 1.0;
  options.particleName = std::string("e-");
  options.distribType  = std::string("reference");

  // From BDSPhysicsList.cc
  G4Electron::ElectronDefinition();
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();  
  BDSGlobalConstants::Instance()->SetParticleDefinition(particleTable->FindParticle(BDSGlobalConstants::Instance()->GetParticleName()));  
  BDSGlobalConstants::Instance()->SetBeamMomentum(sqrt(pow(BDSGlobalConstants::Instance()->GetBeamTotalEnergy(),2)-pow(BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass(),2)));  
  BDSGlobalConstants::Instance()->SetBeamKineticEnergy(BDSGlobalConstants::Instance()->GetBeamTotalEnergy()-BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass());
  bdsBunch.SetOptions(options);

  // Generate nparticle particles 
  double x0, y0, z0, xp, yp, zp, t, E, weight;
  for(int i=0;i<nparticle;i++) { 
    bdsBunch.GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight);
    std::cout << i  << " " 
	      << x0 << " " << y0 << " " << z0 << " " << xp << " "
              << yp << " " << zp << " " << t  << " " << E << " " 
	      << weight << std::endl;
  }    
}
