#include <string>
#include <iostream>
#include <fstream>

#include "G4ParticleTable.hh"
#include "G4Electron.hh"
#include "G4Proton.hh"

// GMAD parser
#include "parser/gmad.h"  
#include "parser/options.h"

#include "BDSGlobalConstants.hh"
#include "BDSBunch.hh"

extern Options options;

int main(void) {
  BDSBunch bdsBunch;

  // fill options from file 
  gmad_parser("./BDSBunchTestFiles/gmad");

  BDSGlobalConstants::Instance();

  // Print options for distrib type 
  std::cout << "BDSBunchTest> distribType : "      << options.distribType << std::endl;
  std::cout << "BDSBunchTest> particle    : "      << options.particleName << std::endl;
  std::cout << "BDSBunchTest> particle    : "      << BDSGlobalConstants::Instance()->GetParticleName() << std::endl;
  std::cout << "BDSBunchTest> numberToGenerate : " << options.numberToGenerate << std::endl;


  // Print options for reference orbit 
  std::cout << "BDSBunchTest> centre : " << options.X0  << " " << options.Y0  << " " << options.Z0 << "\n" 
	    << "BDSBunchTest> centre : " << options.Xp0 << " " << options.Xp0 << " " << options.Zp0 << "\n"
	    << "BDSBunchTest> centre : " << options.T0  << "\n";

  // From BDSPhysicsList.cc
  G4Electron::ElectronDefinition();
  G4Proton::ProtonDefinition();

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();  
  BDSGlobalConstants::Instance()->SetParticleDefinition(particleTable->FindParticle(BDSGlobalConstants::Instance()->GetParticleName()));  
  BDSGlobalConstants::Instance()->SetBeamMomentum(sqrt(pow(BDSGlobalConstants::Instance()->GetBeamTotalEnergy(),2)-pow(BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass(),2)));  
  BDSGlobalConstants::Instance()->SetBeamKineticEnergy(BDSGlobalConstants::Instance()->GetBeamTotalEnergy()-BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass());

  // Set options for bunch
  bdsBunch.SetOptions(options);

  // open output file 
  std::ofstream of("BDSBunchTestOutput.txt");    

  // Generate nparticle particles 
  double x0, y0, z0, xp, yp, zp, t, E, weight;
  for(int i=0;i<options.numberToGenerate;i++) { 
    bdsBunch.GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight);
    std::cout << i  << " " 
	      << x0 << " " << y0 << " " << z0 << " " << xp << " "
              << yp << " " << zp << " " << t  << " " << E << " " 
	      << weight << std::endl;
    of << i  << " " 
       << x0 << " " << y0 << " " << z0 << " " << xp << " "
       << yp << " " << zp << " " << t  << " " << E << " " 
       << weight << std::endl;    
  }    

  // close output file
  of.close();

}
