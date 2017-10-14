#include "BDSBunchComposite.hh"
#include "BDSDebug.hh"
#include "BDSBunchFactory.hh"
#include "BDSBunch.hh"
#include "BDSDebug.hh"

#include "parser/beam.h"

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

void BDSBunchComposite::SetOptions(const GMAD::Beam& beam,
				   G4Transform3D beamlineTransformIn)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  BDSBunch::SetOptions(beam, beamlineTransformIn);
  
  delete xBunch;
  delete yBunch;
  delete zBunch;

  BDSBunchType xType = BDS::DetermineBunchType(beam.xDistrType);
  BDSBunchType yType = BDS::DetermineBunchType(beam.yDistrType);
  BDSBunchType zType = BDS::DetermineBunchType(beam.zDistrType);

  if (xType == BDSBunchType::composite ||
      yType == BDSBunchType::composite ||
      zType == BDSBunchType::composite)
    {
      G4cerr << __METHOD_NAME__ << "x,y,z distributions cannot be 'composite'" << G4endl;
      exit(1);
    }
  
  xBunch = BDSBunchFactory::CreateBunch(xType, beam, beamlineTransformIn);
  yBunch = BDSBunchFactory::CreateBunch(yType, beam, beamlineTransformIn);
  zBunch = BDSBunchFactory::CreateBunch(zType, beam, beamlineTransformIn);
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

  ApplyTransform(x0,y0,z0,xp,yp,zp);
  
  t  = zt;
  E  = zE; 
  weight = xWeight;

  return;
}
