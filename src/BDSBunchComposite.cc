/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSBunchComposite.hh"
#include "BDSDebug.hh"
#include "BDSBunchFactory.hh"
#include "BDSBunch.hh"
#include "BDSDebug.hh"

#include "parser/beam.h"

BDSBunchComposite::BDSBunchComposite():
  xBunch(nullptr),
  yBunch(nullptr),
  zBunch(nullptr)
{;}

BDSBunchComposite::~BDSBunchComposite()
{
  delete xBunch;
  delete yBunch;
  delete zBunch;
}

void BDSBunchComposite::SetOptions(const GMAD::Beam& beam,
				   G4Transform3D beamlineTransformIn)
{
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
