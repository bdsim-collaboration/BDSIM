/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#ifndef BDSBUNCHCIRCLE_H
#define BDSBUNCHCIRCLE_H 

#include "BDSBunch.hh"

namespace CLHEP
{
  class RandFlat;
}

/**
 * @brief An uncorrelated uniform random distribution within a circle in each dimension.
 * 
 * @author Stewart Boogert
 */

class BDSBunchCircle: public BDSBunch
{
protected : 
  G4double envelopeR; 
  G4double envelopeRp; 
  G4double envelopeT;
  G4double envelopeE; 
  
  CLHEP::RandFlat  *FlatGen;    

public: 
  BDSBunchCircle(); 
  virtual ~BDSBunchCircle(); 
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);  
  
protected:
  void SetEnvelopeR(G4double envelopeRIn)   {envelopeR  = envelopeRIn;}
  void SetEnvelopeRp(G4double envelopeRpIn) {envelopeRp = envelopeRpIn;}
  void SetEnvelopeT(G4double envelopeTIn)   {envelopeT  = envelopeTIn;}
  void SetEnvelopeE(G4double envelopeEIn)   {envelopeE  = envelopeEIn;}

};

#endif
