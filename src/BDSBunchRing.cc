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
#include "BDSBunchRing.hh"
#include "BDSDebug.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Units/PhysicalConstants.h"

BDSBunchRing::BDSBunchRing(): 
  rMin(0),
  rMax(0)
{
  flatGen = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());  
}

BDSBunchRing::~BDSBunchRing()
{
  delete flatGen;
}

void BDSBunchRing::SetOptions(const BDSParticleDefinition* beamParticle,
			      const GMAD::Beam& beam,
			      const BDSBunchType& distrType,
			      G4Transform3D beamlineTransformIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn);
  rMin = beam.Rmin;  
  rMax = beam.Rmax;  
}

void BDSBunchRing::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (rMin < 0)
    {G4cerr << __METHOD_NAME__ << "rMin: " << rMin << " < 0" << G4endl; exit(1);}
  if (rMax < 0)
    {G4cerr << __METHOD_NAME__ << "rMax: " << rMin << " < 0" << G4endl; exit(1);}
  if (rMax <= rMin)
    {G4cerr << __METHOD_NAME__ << "rMax: " << rMax << " < rMin: " << rMin << G4endl; exit(1);}
}

void BDSBunchRing::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				   G4double& xp, G4double& yp, G4double& zp,
				   G4double& t , G4double&  E, G4double& weight)
{
  double r = ( rMin + (rMax - rMin) *  rand() / RAND_MAX );
  double phi = 2 * CLHEP::pi * rand() / RAND_MAX;
     
  x0 = ( X0 + r * sin(phi) ) * CLHEP::m;
  y0 = ( Y0 + r * cos(phi) ) * CLHEP::m;
  z0 = Z0  * CLHEP::m;
  xp = Xp0 * CLHEP::rad;
  yp = Yp0 * CLHEP::rad;
  zp = CalculateZp(xp,yp,Zp0);

  ApplyTransform(x0,y0,z0,xp,yp,zp);
  
  t  = T0 * CLHEP::s;
  E  = E0 * CLHEP::GeV * (1 + sigmaE/2. * (1. -2. * flatGen->shoot()));
  weight = 1.0;
}

