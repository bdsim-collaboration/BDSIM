#include <string>
#include <iostream>
#include <fstream>

#include "G4ParticleTable.hh"
#include "G4Electron.hh"
#include "G4Proton.hh"

// GMAD parser
#include "parser/gmad.h"  
#include "parser/options.h"

#include "BDSBunch.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"

namespace GMAD {
  extern Options options;
}

using GMAD::options;

int main(int argc,char** argv) {
  BDSBunch bdsBunch;

  // default filename
  std::string filename = "./BDSBunchTestFiles/gmad";

  const BDSExecOptions* execOptions = BDSExecOptions::Instance(argc,argv);

  G4cout << __FUNCTION__ << "> Using input file : "<< execOptions->GetInputFilename()<<G4endl;
  
  GMAD::gmad_parser(execOptions->GetInputFilename());

  BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();

  // Print options for distrib type 
  std::cout << "BDSBunchTest> distribFile : "      << options.distribFile << std::endl;
  std::cout << "BDSBunchTest> distribType : "      << options.distribType << std::endl;
  std::cout << "BDSBunchTest> particle    : "      << options.particleName << std::endl;
  std::cout << "BDSBunchTest> particle    : "      << globalConstants->GetParticleName() << std::endl;
  std::cout << "BDSBunchTest> numberToGenerate : " << options.numberToGenerate << std::endl;


  // Print options for reference orbit 
  std::cout << "BDSBunchTest> centre : " << options.X0  << " " << options.Y0  << " " << options.Z0 << "\n" 
	    << "BDSBunchTest> angle : " << options.Xp0 << " " << options.Xp0 << " " << options.Zp0 << "\n"
	    << "BDSBunchTest> time : " << options.T0  << "\n";

  // From BDSPhysicsList.cc
  G4Electron::ElectronDefinition();
  G4Proton::ProtonDefinition();

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();  
  globalConstants->SetParticleDefinition(particleTable->FindParticle(globalConstants->GetParticleName()));  
  globalConstants->SetBeamMomentum(sqrt(pow(globalConstants->GetBeamTotalEnergy(),2)-pow(globalConstants->GetParticleDefinition()->GetPDGMass(),2)));  
  globalConstants->SetBeamKineticEnergy(globalConstants->GetBeamTotalEnergy()-globalConstants->GetParticleDefinition()->GetPDGMass());

  // Set options for bunch
  bdsBunch.SetOptions(options);

  // open output file 
  std::ofstream of("./BDSBunchTestFiles/BDSBunchTestOutput.txt");    

  // Generate nparticle particles 
  double x0, y0, z0, xp, yp, zp, t, E, weight;
  for(int i=0;i<options.numberToGenerate;i++) { 
    bdsBunch.GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight);
    if(i% 1 == 0 ) {
      std::cout << "i = " << i  << " x0 = " 
		<< x0 << " y0 = " << y0 << ", z0 " << z0 << ", xp = " << xp << ", yp = "
		<< yp << ", zp = " << zp << ", t = " << t  << ", E = " << E << ", weight = " 
		<< weight << std::endl;
    }
    of << i  << " " 
       << x0 << " " << y0 << " " << z0 << " " << xp << " "
       << yp << " " << zp << " " << t  << " " << E << " " 
       << weight << std::endl;    
  }

  // close output file
  of.close();
  return 0;
}
