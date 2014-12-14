#include "BDSBunchPtc.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////
BDSBunchPtc::BDSBunchPtc() { 
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  // load inrays file in current directory 
  this->fileName = "./inrays.madx";
  // Set ray counter to zero
  this->iRay = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
BDSBunchPtc::~BDSBunchPtc() { 
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void BDSBunchPtc::LoadPtcFile() { 
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  // open file and read line by line and extract values
  

  // set number of available rays 
  return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void BDSBunchPtc::SetOptions(struct Options& opt) {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSBunchInterface::SetOptions(opt);
  this->SetDistribFile((G4String)opt.distribFile); 
  this->LoadPtcFile();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void BDSBunchPtc::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				  G4double& xp, G4double& yp, G4double& zp,
				  G4double& t , G4double&  E, G4double& weight) {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  x0     = 0.0;
  y0     = 0.0;
  z0     = 0.0; 
  xp     = 0.0;
  yp     = 0.0;
  zp     = CalculateZp(xp,yp,Zp0);
  t      = 0.0;
  E      = BDSGlobalConstants::Instance()->GetParticleKineticEnergy() * 1.0;
  weight = 1.0; 

  return;
}

