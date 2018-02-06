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
#ifndef BDSBUNCHGAUSSIAN_H
#define BDSBUNCHGAUSSIAN_H 

#include "BDSBunch.hh"
#include "CLHEP/Matrix/Vector.h" 
#include "CLHEP/Matrix/SymMatrix.h"

namespace CLHEP {
  class RandMultiGauss;
}

/**
 * @brief A 6D Gaussian distribution.
 * 
 * @author Stewart Boogert
 */

class BDSBunchGaussian: public BDSBunch
{ 
protected: 
  G4double sigmaX;
  G4double sigmaY;
  G4double sigmaXp;
  G4double sigmaYp;

  CLHEP::HepVector    meansGM;
  CLHEP::HepSymMatrix sigmaGM;

  // Multidimensional Gaussian random number generator
  CLHEP::RandMultiGauss* GaussMultiGen;

public:
  BDSBunchGaussian();
  virtual ~BDSBunchGaussian();
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity); 
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);  
  
protected:
  void SetSigmaX(G4double sigmaXIn) {sigmaX = sigmaXIn;}
  void SetSigmaY(G4double sigmaYIn) {sigmaY = sigmaYIn;}
  void SetSigmaXp(G4double sigmaXpIn) {sigmaXp = sigmaXpIn;}
  void SetSigmaYp(G4double sigmaYpIn) {sigmaYp = sigmaYpIn;}
  void SetSigma(G4int i, G4int j, G4double sigmaIn) {sigmaGM[i][j] = sigmaIn;}   
};

#endif
