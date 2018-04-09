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
#include "BDSBunchGaussian.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <vector>

/// helper method
namespace {
  /// check if matrix is positive definite
  bool isPositiveDefinite(CLHEP::HepSymMatrix& S)
  {
    CLHEP::HepSymMatrix temp (S); // Since diagonalize does not take a const s
                                  // we have to copy S.
    CLHEP::HepMatrix U = diagonalize ( &temp ); // S = U Sdiag U.T()
    CLHEP::HepSymMatrix D = S.similarityT(U);   // D = U.T() S U = Sdiag
    for (G4int i = 1; i <= S.num_row(); i++)
      {
	G4double s2 = D(i,i);
	if ( s2 <= 0 )
	  {return false;}
      }
    return true;
  }
}

BDSBunchGaussian::BDSBunchGaussian(): 
  BDSBunch(),
  meansGM(CLHEP::HepVector(6)),
  sigmaGM(CLHEP::HepSymMatrix(6)),
  gaussMultiGen(nullptr)
{
  coordinates = {&x0_v, &xp_v, &y0_v, &yp_v, &z0_v, &zp_v, &E_v, &t_v, &weight_v};
}

BDSBunchGaussian::~BDSBunchGaussian()
{
  delete gaussMultiGen;
}

void BDSBunchGaussian::SetOptions(const BDSParticleDefinition* beamParticle,
				  const GMAD::Beam& beam,
				  G4Transform3D beamlineTransformIn)
{
  BDSBunch::SetOptions(beamParticle, beam, beamlineTransformIn);

  // require sigmaE to be finite for sigma matrix to be positive definite
  // set to very small value if 0
  if (!finiteSigmaE) // set in BDSBunch::SetOptions
    {sigmaE = 1e-50;}
  if (!finiteSigmaT)
    {sigmaT = 1e-50;}

  offsetSampleMean  = beam.offsetSampleMean;
  iPartIteration    = 0;
  
  meansGM[0] = X0;
  meansGM[1] = Xp0;
  meansGM[2] = Y0;
  meansGM[3] = Yp0;
  meansGM[4] = T0;
  meansGM[5] = 1;
}

void BDSBunchGaussian::BeginOfRunAction(const G4int& numberOfEvents)
{
  if (!offsetSampleMean)
    {return;}
  /// clear previous means
  for (auto& vec : coordinates)
    {vec->clear();}
  
  PreGenerateEvents(numberOfEvents);
}

void BDSBunchGaussian::EndOfRunAction()
{
  if (!offsetSampleMean)
    {return;}
  /// clear previous means
  for (auto& vec : coordinates)
    {vec->clear();}
  iPartIteration = 0;
}

CLHEP::RandMultiGauss* BDSBunchGaussian::CreateMultiGauss(CLHEP::HepRandomEngine& anEngine,
							  const CLHEP::HepVector& mu,
							  CLHEP::HepSymMatrix& sigma)
{
  /// check if sigma matrix is positive definite
  /// if not add small offset and cout warning
  
  if (!isPositiveDefinite(sigma))
    {
      G4cout << __METHOD_NAME__ << "WARNING bunch generator sigma matrix is not positive definite" << G4endl;
      G4cout << sigma << G4endl;
      G4cout << __METHOD_NAME__ << "adding a small error to zero diagonal elements" << G4endl;
      
      // very small number especially for time, since the sigma goes with the square
      G4double small_error = 1e-50;
      
      for (G4int i=0; i<6; i++)
	{
	  if (sigma[i][i]==0)
	    {sigma[i][i] += small_error;}
	}
      
      if (!isPositiveDefinite(sigma))
	{
	  G4cout << __METHOD_NAME__ << "WARNING bunch generator sigma matrix is still not positive definite" << G4endl;
	  G4cout << sigma << G4endl;
	  G4cout << __METHOD_NAME__ << "adding a small error to all elements" << G4endl;
	  for (G4int i=0; i<6; i++)
	    {
	      for (G4int j=0; j<6; j++)
		{
		  if (sigma[i][j]==0)
		    {sigma[i][j] += small_error;}
		}
	    }
	  if (!isPositiveDefinite(sigma))
	    {
	      G4cout << __METHOD_NAME__ << "ERROR bunch generator sigma matrix is still not positive definite, giving up" << G4endl;
	      G4cout << sigma << G4endl;
	      exit(1);
	    }
	}
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "mean "  << G4endl
	 << mu    << G4endl
	 << __METHOD_NAME__ << "sigma " << G4endl
	 << sigma << G4endl;
#endif
  
  return new CLHEP::RandMultiGauss(anEngine,mu,sigma); 
}

void BDSBunchGaussian::PreGenerateEvents(const G4int& nGenerate)
{
  // generate all required primaries first
  G4double x0,xp,y0,yp,z0,zp,E,t,weight;
  G4double x0_a = 0.0, xp_a = 0.0, y0_a = 0.0, yp_a = 0.0;
  G4double z0_a = 0.0, zp_a = 0.0, E_a  = 0.0, t_a  = 0.0;

  for (G4int iParticle = 0; iParticle < nGenerate; ++iParticle)
    {
      GetNextParticleCoords(x0, y0, z0, xp, yp, zp, t, E, weight);

      G4double nT = (G4double)iParticle + 1;
      G4double d = 0;
      d     = x0 - x0_a;
      x0_a  = x0_a + (d/nT);
      d     = xp - xp_a;
      xp_a  = xp_a + (d/nT);
      d     = y0 - y0_a;
      y0_a  = y0_a + (d/nT);
      d     = yp - yp_a;
      yp_a  = yp_a + (d/nT);
      d     = z0 - z0_a;
      z0_a  = z0_a + (d/nT);
      d     = zp - zp_a;
      zp_a  = zp_a + (d/nT);
      d     = y0 - y0_a;
      d     = E - E_a;
      E_a   = E_a + (d/nT);
      d     = t - t_a;
      t_a   = t_a + (d/nT);
      
      x0_v.push_back(x0);
      xp_v.push_back(xp);
      y0_v.push_back(y0);
      yp_v.push_back(yp);
      z0_v.push_back(z0);
      zp_v.push_back(zp);
      E_v.push_back(E);
      t_v.push_back(t);
      weight_v.push_back(weight);
    }

  // Compute difference between sample mean and specified means
  x0_a = x0_a - X0*CLHEP::m;
  xp_a = xp_a - Xp0*CLHEP::rad;
  y0_a = y0_a - Y0*CLHEP::m;
  yp_a = yp_a - Yp0*CLHEP::rad;
  z0_a = z0_a - Z0*CLHEP::m;
  zp_a = zp_a - Zp0*CLHEP::rad;
  E_a  = E_a  - E0*CLHEP::GeV;
  t_a  = t_a  - T0*CLHEP::s;

  // Offset with different w.r.t. central value
  for(G4int iParticle = 0; iParticle < nGenerate; ++iParticle)
    {
      x0_v[iParticle] -= x0_a;
      xp_v[iParticle] -= xp_a;
      y0_v[iParticle] -= y0_a;
      yp_v[iParticle] -= yp_a;
      z0_v[iParticle] -= z0_a;
      zp_v[iParticle] -= zp_a;
      E_v[iParticle]  -= E_a;
      t_v[iParticle]  -= t_a;
    }
}
  
void BDSBunchGaussian::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				       G4double& xp, G4double& yp, G4double& zp,
				       G4double& t , G4double&  E, G4double& weight)
{
  if (offsetSampleMean)
    {
      // iPartIteration should never exceed the size of each vector.
      // the units are already correct in the vector of stored coordinates
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
    {GetNextParticleCoords(x0, y0, z0, xp, yp, zp, t, E, weight);}
}


void BDSBunchGaussian::GetNextParticleCoords(G4double& x0, G4double& y0, G4double& z0, 
					     G4double& xp, G4double& yp, G4double& zp,
					     G4double& t , G4double&  E, G4double& weight)
{
  CLHEP::HepVector v = gaussMultiGen->fire();
  x0 = v[0] * CLHEP::m;
  xp = v[1] * CLHEP::rad;
  y0 = v[2] * CLHEP::m;
  yp = v[3] * CLHEP::rad;
  if (finiteSigmaT)
    {t = v[4] * CLHEP::s;}
  else
    {t = T0 * CLHEP::s;}
  zp = 0.0  * CLHEP::rad;
  z0 = Z0 * CLHEP::m;
  if (finiteSigmaT)
    {z0 += t * CLHEP::c_light;}
  
  E  = E0 * CLHEP::GeV;
  if (finiteSigmaE)
    {E *= v[5];} // only if there's a finite energy spread
  
  zp = CalculateZp(xp,yp,Zp0);
  
  ApplyTransform(x0,y0,z0,xp,yp,zp);
  
  weight = 1.0;
}
