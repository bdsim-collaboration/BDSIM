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
#include "BDSBunchEShell.hh"
#include "BDSDebug.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Units/PhysicalConstants.h"

BDSBunchEShell::BDSBunchEShell(): 
  BDSBunch(), shellX(0.0), shellXp(0.0), shellY(0.0), shellYp(0.0),
  shellXWidth(0.0), shellXpWidth(0.0), shellYWidth(0.0), shellYpWidth(0.0) 
{
  flatGen = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine()); 
}

BDSBunchEShell::~BDSBunchEShell() 
{
  delete flatGen;
}

void BDSBunchEShell::SetOptions(const BDSParticleDefinition* beamParticle,
				const GMAD::Beam& beam,
				const BDSBunchType& distrType,
				G4Transform3D beamlineTransformIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn);
  shellX  = beam.shellX;
  shellY  = beam.shellY;
  shellXp = beam.shellXp;
  shellYp = beam.shellYp;
  shellXWidth  = beam.shellXWidth;
  shellXpWidth = beam.shellXpWidth;
  shellYWidth  = beam.shellYWidth;
  shellYpWidth = beam.shellYpWidth;
}

void BDSBunchEShell::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (shellX <= 0)
    {G4cerr << __METHOD_NAME__ << "shellX <=0 "   << G4endl; exit(1);}
  if (shellY <= 0)
    {G4cerr << __METHOD_NAME__ << "shellY <=0 "   << G4endl; exit(1);}
  if (shellXp <= 0)
    {G4cerr << __METHOD_NAME__ << "shellXp <=0 "  << G4endl; exit(1);}
  if (shellYp <= 0)
    {G4cerr << __METHOD_NAME__ << "shellYp <=0 "  << G4endl; exit(1);}

  // widths can be zero but can't be negative
  if (shellXWidth <  0)
    {G4cerr << __METHOD_NAME__ << "shellXWidth < 0 "   << G4endl; exit(1);}
  if (shellYWidth <  0)
    {G4cerr << __METHOD_NAME__ << "shellYWidth < 0 "   << G4endl; exit(1);}
  if (shellXpWidth <  0)
    {G4cerr << __METHOD_NAME__ << "shellXpWidth < 0 "  << G4endl; exit(1);}
  if (shellYpWidth <  0)
    {G4cerr << __METHOD_NAME__ << "shellYpWidth < 0 "  << G4endl; exit(1);}
}

void BDSBunchEShell::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				    G4double& xp, G4double& yp, G4double& zp,
				    G4double& t , G4double&  E, G4double& weight)
{
  G4double phi = 2 * CLHEP::pi * flatGen->shoot();
  G4double xamp  = 0.5 - flatGen->shoot();
  G4double yamp  = 0.5 - flatGen->shoot();
  G4double xpamp = 0.5 - flatGen->shoot();
  G4double ypamp = 0.5 - flatGen->shoot();
  
  x0 = (X0 +  (sin(phi) * shellX)  + xamp * shellXWidth) * CLHEP::m;
  xp = (Xp0 + (cos(phi) * shellXp) + xpamp * shellXpWidth);
  
  phi = 2 * CLHEP::pi * flatGen->shoot();
  
  y0 = (Y0 +  (sin(phi) * shellY)  + yamp * shellYWidth) * CLHEP::m;
  yp = (Yp0 + (cos(phi) * shellYp) + ypamp * shellYpWidth);
  
  z0 = Z0 * CLHEP::m;
  zp = CalculateZp(xp,yp,Zp0);

  ApplyTransform(x0,y0,z0,xp,yp,zp);
 
  t = T0 * CLHEP::s;
  E = E0 * CLHEP::GeV * (1 + sigmaE/2. * (1. -2. * flatGen->shoot()));
  weight = 1.0;

  return;
}
		    
