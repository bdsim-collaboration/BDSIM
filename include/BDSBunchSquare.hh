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
#ifndef BDSBUNCHSQUARE_H
#define BDSBUNCHSQUARE_H 

#include "BDSBunch.hh"

namespace CLHEP {
  class RandFlat;
}

/**
 * @brief A bunch distribution that produces an uncorrelated uniform
 * random distribution within a square in phase space.
 * 
 * @author Stewart Boogert
 */

class BDSBunchSquare: public BDSBunch
{ 
public: 
  BDSBunchSquare(); 
  virtual ~BDSBunchSquare(); 
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& distrType,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);
  virtual void CheckParameters();
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight);  
protected:
  G4double envelopeX;
  G4double envelopeY;
  G4double envelopeXp;
  G4double envelopeYp;
  G4double envelopeT;
  G4double envelopeE;
  
  CLHEP::RandFlat* flatGen;
};

#endif
