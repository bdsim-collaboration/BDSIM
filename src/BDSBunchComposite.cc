#include "BDSBunchComposite.hh"
#include "BDSBunchFactory.hh"

BDSBunchComposite::BDSBunchComposite() {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  xBunch = NULL;
  yBunch = NULL;
  zBunch = NULL;
}

BDSBunchComposite::~BDSBunchComposite() {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  delete xBunch;
  delete yBunch;
  delete zBunch;
}

void BDSBunchComposite::SetOptions(struct Options& opt) {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSBunchInterface::SetOptions(opt);
  
  delete xBunch;
  delete yBunch;
  delete zBunch;

  xBunch = BDSBunchFactory::createBunch(opt.xDistribType);
  yBunch = BDSBunchFactory::createBunch(opt.yDistribType);
  zBunch = BDSBunchFactory::createBunch(opt.zDistribType);

  xBunch->SetOptions(opt);
  yBunch->SetOptions(opt);
  zBunch->SetOptions(opt);
}

void BDSBunchComposite::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
					G4double& xp, G4double& yp, G4double& zp,
					G4double& t , G4double&  E, G4double& weight) { 
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  G4double xx0, xy0, xz0, xxp, xyp, xzp, xt, xE, xWeight;
  G4double yx0, yy0, yz0, yxp, yyp, yzp, yt, yE, yWeight;
  G4double zx0, zy0, zz0, zxp, zyp, zzp, zt, zE, zWeight;
  
  xBunch->GetNextParticle(xx0, xy0, xz0, xxp, xyp, xzp, xt, xE, xWeight);
  yBunch->GetNextParticle(yx0, yy0, yz0, yxp, yyp, yzp, yt, yE, yWeight);
  zBunch->GetNextParticle(zx0, zy0, zz0, zxp, zyp, zzp, zt, zE, zWeight);

  x0 = xx0;
  xp = xxp;
  y0 = yy0;
  yp = yyp;
  z0 = zz0;
  zp = zzp; 
  t  = zt;
  E  = zE; 
  weight = xWeight;

  return;
}
