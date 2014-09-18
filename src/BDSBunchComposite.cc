#include "BDSBunchComposite.hh"
#include "BDSBunchFactory.hh"

BDSBunchComposite::BDSBunchComposite() {
  xBunch = NULL;
  yBunch = NULL;
}

BDSBunchComposite::~BDSBunchComposite() {
  delete xBunch;
  delete yBunch;
}

void BDSBunchComposite::SetOptions(struct Options& opt) {
  BDSBunchInterface::SetOptions(opt);
  
  delete xBunch;
  delete yBunch;

  xBunch = BDSBunchFactory::createBunch(opt.xDistribType);
  yBunch = BDSBunchFactory::createBunch(opt.yDistribType);

  xBunch->SetOptions(opt);
  yBunch->SetOptions(opt);
}

void BDSBunchComposite::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
					G4double& xp, G4double& yp, G4double& zp,
					G4double& t , G4double&  E, G4double& weight) { 
  G4double xx0, xy0, xz0, xxp, xyp, xzp, xt, xE, xWeight;
  G4double yx0, yy0, yz0, yxp, yyp, yzp, yt, yE, yWeight;
  
  xBunch->GetNextParticle(xx0, xy0, xz0, xxp, xyp, xzp, xt, xE, xWeight);
  yBunch->GetNextParticle(yx0, yy0, yz0, yxp, yyp, yzp, yt, yE, yWeight);

  x0 = xx0;
  xp = xxp;
  y0 = yy0;
  yp = yyp;
  z0 = xz0;
  zp = xzp; // Is this correct?
  t  = xt;
  E  = xE; 
  weight = xWeight;

  return;
}
