#include "BDSBunchTwiss.hh"

BDSBunchTwiss::BDSBunchTwiss(G4double betaXIn,  G4double betaYIn, 
			     G4double alphaXIn, G4double alphaYIn,
			     G4double emitXIn,  G4double emitYIn,
			     G4double sigmaTIn,   G4double sigmaEIn) : 
  BDSBunchInterface(), betaX(betaXIn), betaY(betaYIn), alphaX(alphaXIn), alphaY(alphaYIn), emitX(emitXIn), emitY(emitYIn)
{
  GaussGen = new CLHEP::RandGauss(*CLHEP::HepRandom::getTheEngine());
  FlatGen  = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());  
  sigmaT = sigmaTIn; 
  sigmaE = sigmaEIn;
}

BDSBunchTwiss::~BDSBunchTwiss() {
  delete GaussGen;
  delete FlatGen;
}

void BDSBunchTwiss::SetOptions(struct Options& opt) {
  BDSBunchInterface::SetOptions(opt);
  SetBetaX(opt.betx);
  SetBetaY(opt.bety);
  SetAlphaX(opt.alfx);
  SetAlphaY(opt.alfy);
  SetEmitX(opt.emitx);
  SetEmitY(opt.emity);  
  return;
}

void BDSBunchTwiss::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				    G4double& xp, G4double& yp, G4double& zp,
				    G4double& t , G4double&  E, G4double& weight) {
  
  G4double phiX = CLHEP::twopi * G4UniformRand();
  G4double phiY = CLHEP::twopi * G4UniformRand();
  G4double ex   = std::abs(GaussGen->shoot()*emitX);
  G4double ey   = std::abs(GaussGen->shoot()*emitY);
  x0 = sqrt(2*ex*betaX)*sin(phiX)*CLHEP::m;
  xp = sqrt(2*ex/betaX)*(cos(phiX)-alphaX*sin(phiX))*CLHEP::rad;
  y0 = sqrt(2*ey*betaY)*sin(phiY)*CLHEP::m;
  yp = sqrt(2*ey/betaY)*(cos(phiY)-alphaY*sin(phiY))*CLHEP::rad;
  z0 = Z0 * CLHEP::m + (T0 - sigmaT * (1.-2.*GaussGen->shoot())) * CLHEP::c_light * CLHEP::s;

  if (Zp0<0)
    zp = -sqrt(1.-xp*xp -yp*yp);
  else
    zp = sqrt(1.-xp*xp -yp*yp);
  t = 0; // (T0 - sigmaT * (1.-2.*GaussGen->shoot())) * s;
  E = BDSGlobalConstants::Instance()->GetBeamKineticEnergy() * (1 + sigmaE * GaussGen->shoot());
  weight = 1.0;

  return;
}
