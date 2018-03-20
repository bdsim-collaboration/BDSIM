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
#ifndef BDSBUNCHESHELL_H
#define BDSBUNCHESHELL_H 

#include "BDSBunch.hh"

namespace CLHEP
{
  class RandFlat;
}

/**
 * @brief An uncorrelated uniform random distribution within an elliptical shell.
 * 
 * @author Stewart Boogert
 */

class BDSBunchEShell: public BDSBunch
{

public: 
  BDSBunchEShell();  
  virtual ~BDSBunchEShell(); 
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);
  virtual void CheckParameters();
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight);

protected:
  G4double shellX;
  G4double shellXp;
  G4double shellY; 
  G4double shellYp;
  G4double shellXWidth;
  G4double shellXpWidth;
  G4double shellYWidth;
  G4double shellYpWidth;

  CLHEP::RandFlat* flatGen;
};

#endif
