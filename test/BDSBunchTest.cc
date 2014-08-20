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

int main(int argc,char** argv) {
  BDSBunch bdsBunch;

  // default filename
  std::string filename = "./BDSBunchTestFiles/gmad";

  /// first argument is gmad file
  if (argc>1){
    filename = argv[1];
  }

  // fill options from file 
  gmad_parser(filename);

  // Print options for distrib type 
  std::cout << "BDSBunchTest> distribType : "      << options.distribType << std::endl;
  std::cout << "BDSBunchTest> particle    : "      << options.particleName << std::endl;
  std::cout << "BDSBunchTest> particle    : "      << BDSGlobalConstants::Instance()->GetParticleName() << std::endl;
  std::cout << "BDSBunchTest> numberToGenerate : " << options.numberToGenerate << std::endl;


  // Print options for reference orbit 
  std::cout << "BDSBunchTest> centre : " << options.X0  << " " << options.Y0  << " " << options.Z0 << "\n" 
	    << "BDSBunchTest> angle : " << options.Xp0 << " " << options.Xp0 << " " << options.Zp0 << "\n"
	    << "BDSBunchTest> time : " << options.T0  << "\n";

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
  std::ofstream of("./BDSBunchTestFiles/BDSBunchTestOutput.txt");    

  // Generate nparticle particles 
  double x0, y0, z0, xp, yp, zp, t, E, weight;
  for(int i=0;i<options.numberToGenerate;i++) { 
    bdsBunch.GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight);
    if(i% 1000 == 0 ) {
      std::cout << i  << " " 
		<< x0 << " " << y0 << " " << z0 << " " << xp << " "
		<< yp << " " << zp << " " << t  << " " << E << " " 
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
