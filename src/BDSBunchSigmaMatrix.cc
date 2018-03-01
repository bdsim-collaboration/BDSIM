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
#include "BDSBunchSigmaMatrix.hh"
#include "BDSDebug.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"

BDSBunchSigmaMatrix::BDSBunchSigmaMatrix(): 
  BDSBunchGaussian()
{;}

void BDSBunchSigmaMatrix::SetOptions(const GMAD::Beam& beam,
				     G4Transform3D beamlineTransformIn)
{
  // Fill means and class BDSBunch::SetOptions
  BDSBunchGaussian::SetOptions(beam, beamlineTransformIn);

  if(strcmp(beam.distrType.data(),"gaussmatrix") == 0)
    {
      sigmaGM[0][0] = beam.sigma11; 
      sigmaGM[0][1] = beam.sigma12;
      sigmaGM[0][2] = beam.sigma13;
      sigmaGM[0][3] = beam.sigma14;
      sigmaGM[0][4] = beam.sigma15;
      sigmaGM[0][5] = beam.sigma16;  
      sigmaGM[1][1] = beam.sigma22;
      sigmaGM[1][2] = beam.sigma23;
      sigmaGM[1][3] = beam.sigma24;
      sigmaGM[1][4] = beam.sigma25;
      sigmaGM[1][5] = beam.sigma26;  
      sigmaGM[2][2] = beam.sigma33;
      sigmaGM[2][3] = beam.sigma34;
      sigmaGM[2][4] = beam.sigma35;
      sigmaGM[2][5] = beam.sigma36;  
      sigmaGM[3][3] = beam.sigma44;
      sigmaGM[3][4] = beam.sigma45;
      sigmaGM[3][5] = beam.sigma46;  
      sigmaGM[4][4] = beam.sigma55;
      sigmaGM[4][5] = beam.sigma56;  
      sigmaGM[5][5] = beam.sigma66;
    }
  else if (strcmp(beam.distrType.data(),"gauss") == 0) 
    {
      sigmaGM[0][0] = std::pow(beam.sigmaX, 2);
      sigmaGM[1][1] = std::pow(beam.sigmaXp,2);
      sigmaGM[2][2] = std::pow(beam.sigmaY, 2);
      sigmaGM[3][3] = std::pow(beam.sigmaYp,2);
      sigmaGM[4][4] = std::pow(beam.sigmaT, 2);
      sigmaGM[5][5] = std::pow(beam.sigmaE, 2);
    }
#ifdef BDSDEBUG
  G4cout << "sigmaGM" << sigmaGM << G4endl;
#endif
  delete gaussMultiGen;
  gaussMultiGen = CreateMultiGauss(*CLHEP::HepRandom::getTheEngine(),meansGM,sigmaGM);
}

void BDSBunchSigmaMatrix::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
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
#ifdef BDSDEBUG 
      G4cout << __METHOD_NAME__ << "HEPVECTOR " << v << G4endl;
#endif
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
