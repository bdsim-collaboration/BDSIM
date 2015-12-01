#include "BDSBunchComposite.hh"
#include "BDSDebug.hh"
#include "BDSBunchFactory.hh"
#include "BDSDebug.hh"

BDSBunchComposite::BDSBunchComposite()
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  xBunch = nullptr;
  yBunch = nullptr;
  zBunch = nullptr;
}

BDSBunchComposite::~BDSBunchComposite()
{
  delete xBunch;
  delete yBunch;
  delete zBunch;
}

void BDSBunchComposite::SetOptions(const GMAD::Options& opt)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSBunchInterface::SetOptions(opt);
  
  delete xBunch;
  delete yBunch;
  delete zBunch;

  if (opt.xDistribType.empty() || opt.yDistribType.empty() || opt.zDistribType.empty())
    {
      G4cout << __METHOD_NAME__ << "Compositeness distribution requires x,y,z distribution to be set" << G4endl;
      G4cout << __METHOD_NAME__ << "X: " << opt.xDistribType << G4endl;
      G4cout << __METHOD_NAME__ << "Y: " << opt.yDistribType << G4endl;
      G4cout << __METHOD_NAME__ << "Z: " << opt.zDistribType << G4endl;
      exit(1);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "X: " << opt.xDistribType << G4endl;
  G4cout << __METHOD_NAME__ << "Y: " << opt.yDistribType << G4endl;
  G4cout << __METHOD_NAME__ << "Z: " << opt.zDistribType << G4endl;
#endif

  xBunch = BDSBunchFactory::createBunch(opt.xDistribType);
  yBunch = BDSBunchFactory::createBunch(opt.yDistribType);
  zBunch = BDSBunchFactory::createBunch(opt.zDistribType);

  xBunch->SetOptions(opt);
  yBunch->SetOptions(opt);
  zBunch->SetOptions(opt);
}

void BDSBunchComposite::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
					G4double& xp, G4double& yp, G4double& zp,
					G4double& t , G4double&  E, G4double& weight)
{ 
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
