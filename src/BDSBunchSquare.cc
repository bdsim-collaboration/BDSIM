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
#include "BDSBunchSquare.hh"
#include "BDSDebug.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Units/PhysicalConstants.h"

BDSBunchSquare::BDSBunchSquare() :
  BDSBunch(), envelopeX(0.0), envelopeY(0.0),
  envelopeXp(0.0), envelopeYp(0.0), envelopeT(0.0), envelopeE(0.0)
{
  flatGen = new CLHEP::RandFlat(*CLHEP::HepRandom::getTheEngine());
}

BDSBunchSquare::~BDSBunchSquare()
{
  delete flatGen;
}

void BDSBunchSquare::SetOptions(const BDSParticleDefinition* beamParticle,
				const GMAD::Beam& beam,
				const BDSBunchType& distrType,
				G4Transform3D beamlineTransformIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn);
  envelopeX  = beam.envelopeX; 
  envelopeY  = beam.envelopeY;
  envelopeXp = beam.envelopeXp;
  envelopeYp = beam.envelopeYp;
  envelopeT  = beam.envelopeT;
  envelopeE  = beam.envelopeE; 
}

void BDSBunchSquare::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (envelopeX <  0)
    {G4cerr << __METHOD_NAME__ << "envelopeX < 0 "  << G4endl; exit(1);}
  if (envelopeXp < 0)
    {G4cerr << __METHOD_NAME__ << "envelopeXp < 0 " << G4endl; exit(1);}
  if (envelopeY <  0)
    {G4cerr << __METHOD_NAME__ << "envelopeY < 0 "  << G4endl; exit(1);}
  if (envelopeYp < 0)
    {G4cerr << __METHOD_NAME__ << "envelopeYp < 0 " << G4endl; exit(1);}
  if (envelopeT < 0)
    {G4cerr << __METHOD_NAME__ << "envelopeT < 0 "  << G4endl; exit(1);}
  if (envelopeE < 0)
    {G4cerr << __METHOD_NAME__ << "envelopeE < 0 "  << G4endl; exit(1);}
}

void BDSBunchSquare::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		     G4double& xp, G4double& yp, G4double& zp,
		     G4double& t , G4double&  E, G4double& weight)
{
  x0 = X0  * CLHEP::m;
  y0 = Y0  * CLHEP::m;
  z0 = Z0  * CLHEP::m;
  xp = Xp0 * CLHEP::rad;
  yp = Yp0 * CLHEP::rad;
  z0 = Z0  * CLHEP::m + (T0 - envelopeT * (1.-2.*flatGen->shoot())) * CLHEP::c_light * CLHEP::s;
  
  if(envelopeX !=0) x0  += envelopeX  * (1-2*flatGen->shoot()) * CLHEP::m;
  if(envelopeY !=0) y0  += envelopeY  * (1-2*flatGen->shoot()) * CLHEP::m;
  if(envelopeXp !=0) xp += envelopeXp * (1-2*flatGen->shoot()) * CLHEP::rad;
  if(envelopeYp !=0) yp += envelopeYp * (1-2*flatGen->shoot()) * CLHEP::rad;
  
  zp = CalculateZp(xp,yp,Zp0);

  ApplyTransform(x0,y0,z0,xp,yp,zp);
  
  t = T0 * CLHEP::s;
  E = E0 * CLHEP::GeV * (1 + envelopeE * (1-2*flatGen->shoot()));

  weight = 1.0;
  return; 
}
