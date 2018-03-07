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
#include "BDSBunchTwiss.hh"
#include "BDSDebug.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <vector>

BDSBunchTwiss::BDSBunchTwiss():
  betaX(0.0), betaY(0.0),
  alphaX(0.0), alphaY(0.0),
  emitX(0.0), emitY(0.0),
  gammaX(0.0), gammaY(0.0),
  dispX(0.0), dispY(0.0),
  dispXP(0.0), dispYP(0.0)
{;}

void BDSBunchTwiss::SetOptions(const GMAD::Beam& beam,
			       G4Transform3D beamlineTransformIn)
{
  // Fill means and class BDSBunch::SetOptions
  BDSBunchGaussian::SetOptions(beam, beamlineTransformIn);

  betaX  = beam.betx;
  betaY  = beam.bety;
  alphaX = beam.alfx;
  alphaY = beam.alfy;
  emitX  = beam.emitx;
  emitY  = beam.emity;
  dispX  = beam.dispx;
  dispY  = beam.dispy;
  dispXP = beam.dispxp;
  dispYP = beam.dispyp;
  gammaX = (1.0+alphaX*alphaX)/betaX;
  gammaY = (1.0+alphaY*alphaY)/betaY;

  // Fill sigmas
  //2x2 block in horizontal
  sigmaGM[0][0] =  emitX*betaX + std::pow(dispX*sigmaE,2); 
  sigmaGM[0][1] = -emitX*alphaX + dispX*dispXP*std::pow(sigmaE,2);
  sigmaGM[1][0] = -emitX*alphaX + dispX*dispXP*std::pow(sigmaE,2);
  sigmaGM[1][1] =  emitX*gammaX + std::pow(dispXP*sigmaE,2);

  //2x2 block in vertical
  sigmaGM[2][2] =  emitY*betaY + std::pow(dispY*sigmaE,2);
  sigmaGM[2][3] = -emitY*alphaY + dispY*dispYP*std::pow(sigmaE,2);
  sigmaGM[3][2] = -emitY*alphaY + dispY*dispYP*std::pow(sigmaE,2);;
  sigmaGM[3][3] =  emitY*gammaY + std::pow(dispYP*sigmaE,2);

  //2 2x2 blocks for horizontal-vertical coupling
  sigmaGM[2][0] =  dispX*dispY*std::pow(sigmaE,2);
  sigmaGM[0][2] =  dispX*dispY*std::pow(sigmaE,2);
  sigmaGM[3][0] =  dispX*dispYP*std::pow(sigmaE,2);
  sigmaGM[0][3] =  dispX*dispYP*std::pow(sigmaE,2);
  sigmaGM[2][1] =  dispXP*dispY*std::pow(sigmaE,2);
  sigmaGM[1][2] =  dispXP*dispY*std::pow(sigmaE,2);
  sigmaGM[3][1] =  dispXP*dispYP*std::pow(sigmaE,2);
  sigmaGM[1][3] =  dispXP*dispYP*std::pow(sigmaE,2);

  //2x2 block in longitudinal
  sigmaGM[4][4] =  std::pow(sigmaT,2); 
  sigmaGM[5][5] =  std::pow(sigmaE,2);

  //4 2x2 blocks for longitudinal-transverse coupling
  sigmaGM[0][5] = dispX*std::pow(sigmaE,2);
  sigmaGM[5][0] = dispX*std::pow(sigmaE,2);
  sigmaGM[1][5] = dispXP*std::pow(sigmaE,2);
  sigmaGM[5][1] = dispXP*std::pow(sigmaE,2);
  sigmaGM[2][5] = dispY*std::pow(sigmaE,2);
  sigmaGM[5][2] = dispY*std::pow(sigmaE,2);
  sigmaGM[3][5] = dispYP*std::pow(sigmaE,2);
  sigmaGM[5][3] = dispYP*std::pow(sigmaE,2);
  
  delete gaussMultiGen;
  gaussMultiGen = CreateMultiGauss(*CLHEP::HepRandom::getTheEngine(),meansGM,sigmaGM);
}

void BDSBunchTwiss::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				    G4double& xp, G4double& yp, G4double& zp,
				    G4double& t , G4double&  E, G4double& weight)
{
  if (offsetSampleMean)
    {
      // iPartIteration should never exceed the size of each vector.
      x0     = x0_v[iPartIteration];
      xp     = xp_v[iPartIteration];
      y0     = y0_v[iPartIteration];
      yp     = yp_v[iPartIteration];
      z0     = z0_v[iPartIteration];
      zp     = zp_v[iPartIteration];
      t      = t_v[iPartIteration];
      E      = E_v[iPartIteration];
      weight = weight_v[iPartIteration];
      
      iPartIteration++;
    }
  else
    {
      CLHEP::HepVector v = gaussMultiGen->fire();
      x0 = v[0] * CLHEP::m;
      xp = v[1] * CLHEP::rad;
      y0 = v[2] * CLHEP::m;
      yp = v[3] * CLHEP::rad;
      t  = v[4] * CLHEP::s;
      zp = 0.0  * CLHEP::rad;
      z0 = Z0 * CLHEP::m + t * CLHEP::c_light;

      E  = E0 * CLHEP::GeV;
      if (finiteSigmaE)
	{E *= v[5];} // only if there's a finite energy spread
      
      zp = CalculateZp(xp,yp,Zp0);
      
      ApplyTransform(x0,y0,z0,xp,yp,zp);
      
      weight = 1.0;
    }
}
