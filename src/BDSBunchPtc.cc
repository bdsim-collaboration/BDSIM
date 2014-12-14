#include "BDSBunchPtc.hh"

BDSBunchPtc::BDSBunchPtc() { 
  // load inrays file in current directory 
  this->fileName = "./inrays.madx";
}

BDSBunchPtc::~BDSBunchPtc() { 
}

void BDSBunchPtc::LoadPtcFile() { 
  // open file and read line by line and extract values
  

  // set number of available rays 
  return;
}

void BDSBunchPtc::SetOptions(struct Options& opt) {
  BDSBunchInterface::SetOptions(opt);
  this->SetDistribFile((G4String)opt.distribFile); 
  this->LoadPtcFile();
}

void BDSBunchPtc::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				  G4double& xp, G4double& yp, G4double& zp,
				  G4double& t , G4double&  E, G4double& weight) {
  x0     = X0;
  y0     = Y0;
  z0     = 0.0; 
  xp     = Xp0;
  yp     = Yp0;
  zp     = 0.0; 
  t      = T0;
  E      = BDSGlobalConstants::Instance()->GetParticleKineticEnergy() * 1.0;
  weight = 1.0; 
}

