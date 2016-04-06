#include "BDSBunchTwiss.hh"
#include "BDSDebug.hh"

BDSBunchTwiss::BDSBunchTwiss():
  betaX(0.0), betaY(0.0),
  alphaX(0.0), alphaY(0.0),
  emitX(0.0), emitY(0.0),
  gammaX(0.0), gammaY(0.0),
  dispX(0.0), dispY(0.0),
  dispXP(0.0), dispYP(0.0)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  GaussMultiGen = nullptr;   
}

BDSBunchTwiss::~BDSBunchTwiss()
{
  delete GaussMultiGen;
}

void BDSBunchTwiss::SetOptions(const GMAD::Options& opt)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSBunchInterface::SetOptions(opt);
  SetBetaX(opt.betx);
  SetBetaY(opt.bety);
  SetAlphaX(opt.alfx);
  SetAlphaY(opt.alfy);
  SetEmitX(opt.emitx);
  SetEmitY(opt.emity);
  SetDispX(opt.dispx);
  SetDispY(opt.dispy);
  SetDispXP(opt.dispxp);
  SetDispYP(opt.dispyp);
  gammaX = (1.0+alphaX*alphaX)/betaX;
  gammaY = (1.0+alphaY*alphaY)/betaY;


  CommonConstruction();

  return;
}

void BDSBunchTwiss::CommonConstruction()
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  meansGM = CLHEP::HepVector(6);

  // Fill means 
  meansGM[0] = X0;
  meansGM[1] = Xp0;
  meansGM[2] = Y0;
  meansGM[3] = Yp0;
  meansGM[4] = T0;
  meansGM[5] = 1;

  sigmaGM = CLHEP::HepSymMatrix(6);

  // Fill sigmas 
  sigmaGM[0][0] =  emitX*betaX;  
  sigmaGM[0][1] = -emitX*alphaX;
  sigmaGM[1][0] = -emitX*alphaX;
  sigmaGM[1][1] =  emitX*gammaX;
  sigmaGM[2][2] =  emitY*betaY;
  sigmaGM[2][3] = -emitY*alphaY;
  sigmaGM[3][2] = -emitY*alphaY;
  sigmaGM[3][3] =  emitY*gammaY; 
  sigmaGM[4][4] =  pow(sigmaT,2); 
  sigmaGM[5][5] =  pow(sigmaE,2);
  sigmaGM[0][5] = dispX*pow(sigmaE,2);
  sigmaGM[5][0] = dispX*pow(sigmaE,2);
  sigmaGM[1][5] = dispXP*pow(sigmaE,2);
  sigmaGM[5][1] = dispXP*pow(sigmaE,2);
  sigmaGM[2][5] = dispY*pow(sigmaE,2);
  sigmaGM[5][2] = dispY*pow(sigmaE,2);
  sigmaGM[3][5] = dispYP*pow(sigmaE,2);
  sigmaGM[5][3] = dispYP*pow(sigmaE,2);
  

  delete GaussMultiGen;
  GaussMultiGen = CreateMultiGauss(*CLHEP::HepRandom::getTheEngine(),meansGM,sigmaGM);

  return;
}

void BDSBunchTwiss::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				    G4double& xp, G4double& yp, G4double& zp,
				    G4double& t , G4double&  E, G4double& weight)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  CLHEP::HepVector v = GaussMultiGen->fire();
  x0 = v[0] * CLHEP::m;
  xp = v[1] * CLHEP::rad;
  y0 = v[2] * CLHEP::m;
  yp = v[3] * CLHEP::rad;
  t  = v[4] * CLHEP::s;
  zp = 0.0  * CLHEP::rad;
  z0 = Z0*CLHEP::m + t*CLHEP::c_light;
  E  = BDSGlobalConstants::Instance()->ParticleTotalEnergy() * v[5];
  
  zp = CalculateZp(xp,yp,Zp0);
  
  weight = 1.0;
  return;
}
