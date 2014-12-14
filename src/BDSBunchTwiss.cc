#include "BDSBunchTwiss.hh"

BDSBunchTwiss::BDSBunchTwiss() :
  BDSBunchInterface(), betaX(0.0), betaY(0.0), alphaX(0.0), alphaY(0.0), emitX(0.0), emitY(0.0), gammaX(0.0), gammaY(0.0)
{
  GaussMultiGen = NULL;   
}

BDSBunchTwiss::BDSBunchTwiss(G4double betaXIn,  G4double betaYIn, 
			     G4double alphaXIn, G4double alphaYIn,
			     G4double emitXIn,  G4double emitYIn,
			     G4double X0In,     G4double Y0In,       G4double Z0In,   G4double T0In, 
			     G4double Xp0In,    G4double Yp0In,      G4double Zp0In,			     
			     G4double sigmaTIn, G4double sigmaEIn) : 
  BDSBunchInterface(X0In,Y0In,Z0In,T0In,Xp0In,Yp0In,Zp0In,sigmaTIn,sigmaEIn), betaX(betaXIn), betaY(betaYIn), alphaX(alphaXIn), alphaY(alphaYIn), emitX(emitXIn), emitY(emitYIn)
{
  GaussMultiGen = NULL;

  sigmaT = sigmaTIn; 
  sigmaE = sigmaEIn;
  gammaX = (1.0+alphaX*alphaX)/betaX;
  gammaY = (1.0+alphaY*alphaY)/betaY;

  CommonConstruction();
}

BDSBunchTwiss::~BDSBunchTwiss() {
  delete GaussMultiGen;
}

void BDSBunchTwiss::SetOptions(struct Options& opt) {
  BDSBunchInterface::SetOptions(opt);
  SetBetaX(opt.betx);
  SetBetaY(opt.bety);
  SetAlphaX(opt.alfx);
  SetAlphaY(opt.alfy);
  SetEmitX(opt.emitx);
  SetEmitY(opt.emity);  
  gammaX = (1.0+alphaX*alphaX)/betaX;
  gammaY = (1.0+alphaY*alphaY)/betaY;


  CommonConstruction();

  return;
}

void BDSBunchTwiss::CommonConstruction() {

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
  sigmaGM[1][1] =  emitX*gammaX;`
  sigmaGM[2][2] =  emitY*betaY;
  sigmaGM[2][3] = -emitY*alphaY;
  sigmaGM[3][2] = -emitY*alphaY;
  sigmaGM[3][3] =  emitY*gammaY; 
  sigmaGM[4][4] =  pow(sigmaT,2); 
  sigmaGM[5][5] =  pow(sigmaE,2);

  delete GaussMultiGen;
  GaussMultiGen = CreateMultiGauss(*CLHEP::HepRandom::getTheEngine(),meansGM,sigmaGM);

  return;
}

void BDSBunchTwiss::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				    G4double& xp, G4double& yp, G4double& zp,
				    G4double& t , G4double&  E, G4double& weight) {
  /*
  G4double phiX = CLHEP::twopi * G4UniformRand();
  G4double phiY = CLHEP::twopi * G4UniformRand();
  G4double ex   = std::abs(GaussGen->shoot()*emitX);
  G4double ey   = std::abs(GaussGen->shoot()*emitY);
  x0 = sqrt(2*ex*betaX)*sin(phiX)*CLHEP::m;
  xp = sqrt(2*ex/betaX)*(cos(phiX)-alphaX*sin(phiX))*CLHEP::rad;

  y0 = sqrt(2*ey*betaY)*sin(phiY)*CLHEP::m;
  yp = sqrt(2*ey/betaY)*(cos(phiY)-alphaY*sin(phiY))*CLHEP::rad;

  z0 = Z0 * CLHEP::m + (T0 - sigmaT * (1.-2.*GaussGen->shoot())) * CLHEP::c_light * CLHEP::s;

  zp = CalculateZp(xp,yp,Zp0);
  t = 0; // (T0 - sigmaT * (1.-2.*GaussGen->shoot())) * s;
  E = BDSGlobalConstants::Instance()->GetParticleKineticEnergy() * (1 + sigmaE * GaussGen->shoot());
  weight = 1.0;
  */

  CLHEP::HepVector v = GaussMultiGen->fire();
  x0 = v[0] * CLHEP::m;
  xp = v[1] * CLHEP::rad;
  y0 = v[2] * CLHEP::m;
  yp = v[3] * CLHEP::rad;
  t  = v[4] * CLHEP::s;
  zp = 0.0  * CLHEP::rad;
  z0 = Z0*CLHEP::m + t*CLHEP::c_light;
  E  = BDSGlobalConstants::Instance()->GetParticleKineticEnergy() * v[5];
  
  zp = CalculateZp(xp,yp,Zp0);

  weight = 1.0;
  return;
}
