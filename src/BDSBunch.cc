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
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBunch.hh"
#include "BDSDebug.hh"
#include "BDSParticleDefinition.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Geometry/Point3D.h"


BDSBunch::BDSBunch():
  X0(0.0), Y0(0.0), Z0(0.0), S0(0.0), T0(0.0), 
  Xp0(0.0), Yp0(0.0), Zp0(0.0), E0(0.0), P0(0.0),
  sigmaT(0.0), sigmaP(0.0), sigmaE(0.0),
  useCurvilinear(false),
  particleCanBeDifferent(false),
  particleDefinition(nullptr),
  finiteSigmaE(true),
  finiteSigmaT(true),
  generatePrimariesOnly(false),
  beamlineTransform(G4Transform3D()),
  nonZeroTransform(false),
  mass2(0.0),
  beamline(nullptr)
{;}

G4double BDSBunch::EFromP(const G4double &pIn) const
{//E2 = p2 + m2
  return std::sqrt(std::pow(pIn,2) + mass2);
}

void BDSBunch::SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& /*distrType*/,
			  G4Transform3D beamlineTransformIn)
{
  particleDefinition = beamParticle;
  beamlineTransform  = beamlineTransformIn;
  nonZeroTransform   = beamlineTransform != G4Transform3D::Identity;
  
  X0     = beam.X0;
  Y0     = beam.Y0;
  Z0     = beam.Z0;
  S0     = beam.S0;
  T0     = beam.T0;
  Xp0    = beam.Xp0;
  Yp0    = beam.Yp0;
  E0     = beam.E0;
  sigmaE = beam.sigmaE;
  sigmaT = beam.sigmaT;

  finiteSigmaE = BDS::IsFinite(sigmaE);
  finiteSigmaT = BDS::IsFinite(sigmaT);
  
  // calculate momentum - used by some generators
  G4double mass = beamParticle->Mass()/CLHEP::GeV;
  mass2 = std::pow(mass,2);
  if (E0 <= mass)
    {
      G4cerr << __METHOD_NAME__ << "E0 (central total energy) " << E0
	     << " GeV lower than mass of particle! " << mass << " GeV" << G4endl;
      exit(1);
    }
  P0 = std::sqrt(std::pow(E0,2) - mass2); // E^2 = p^2 + m^2
  sigmaP = (1./std::pow(beamParticle->Beta(),2)) * sigmaE; // dE/E = 1/(beta^2) dP/P
  if (finiteSigmaE)
    {G4cout << __METHOD_NAME__ << "sigmaE = " << sigmaE << " -> sigmaP = " << sigmaP << G4endl;}

  Zp0 = CalculateZp(Xp0,Yp0,beam.Zp0);

  if (S0 > 0)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "using curvilinear transform" << G4endl;
#endif
      useCurvilinear = true;
    } 
}

void BDSBunch::CheckParameters()
{
  if (sigmaE < 0)
    {G4cerr << __METHOD_NAME__ << "sigmaE " << sigmaE << " < 0!" << G4endl; exit(1);}
  if (sigmaT < 0)
    {G4cerr << __METHOD_NAME__ << "sigmaT " << sigmaT << " < 0!" << G4endl; exit(1);}
}

void BDSBunch::GetNextParticle(G4double& x0, G4double& y0, G4double& z0,
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double& E,  G4double& weight)
{
  x0 = X0  * CLHEP::m;
  y0 = Y0  * CLHEP::m;
  z0 = Z0  * CLHEP::m;
  xp = Xp0 * CLHEP::rad;
  yp = Yp0 * CLHEP::rad;
  zp = CalculateZp(xp,yp,Zp0);

  ApplyTransform(x0,y0,z0,xp,yp,zp);
  
  t = T0 * CLHEP::s;
  E = E0 * CLHEP::GeV;
  weight = 1.0;
}

void BDSBunch::BeginOfRunAction(const G4int& /*numberOfEvents*/)
{;}

void BDSBunch::EndOfRunAction()
{;}

void BDSBunch::SetGeneratePrimariesOnly(const G4bool& generatePrimariesOnlyIn)
{generatePrimariesOnly = generatePrimariesOnlyIn;}

void BDSBunch::ApplyTransform(G4double& x0, G4double& y0, G4double& z0,
			      G4double& xp, G4double& yp, G4double& zp) const
{
  
  if (useCurvilinear)
    {ApplyCurvilinearTransform(x0,y0,z0,xp,yp,zp);}
  else if (nonZeroTransform)
    {
      G4ThreeVector originalPos = G4ThreeVector(x0,y0,z0);
      G4ThreeVector newPos = beamlineTransform * (HepGeom::Point3D<G4double>)originalPos;
      x0 = newPos.x();
      y0 = newPos.y();
      z0 = newPos.z();
      
      G4ThreeVector originalMom = G4ThreeVector(xp,yp,zp);
      G4ThreeVector newMom      = beamlineTransform * (HepGeom::Vector3D<G4double>)originalMom;
      xp = newMom.x();
      yp = newMom.y();
      zp = newMom.z();
    }
}

void BDSBunch::ApplyCurvilinearTransform(G4double& x0, G4double& y0, G4double& z0,
					 G4double& xp, G4double& yp, G4double& zp) const
{
  if (generatePrimariesOnly)
    {return;} // no beam line built so no possible transform
  if (!beamline)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "initialising beam line reference" << G4endl;
#endif
      beamline = BDSAcceleratorModel::Instance()->BeamlineMain();
      if (!beamline)
	{
	  G4cout << __METHOD_NAME__ << "ERROR no beamline constructed! " << G4endl;
	  exit(1);
	}
    }

  // 'c' for curvilinear
  // z0 is treated as the intended s coordinate on input
  G4Transform3D cTrans = beamline->GetGlobalEuclideanTransform(S0*CLHEP::m + z0,x0,y0);
  G4ThreeVector cPrime = G4ThreeVector(xp,yp,zp).transform(cTrans.getRotation()); // rotate the momentum vector
  G4ThreeVector cPos   = cTrans.getTranslation(); // translation contains displacement from origin already
  x0 = cPos.x();
  y0 = cPos.y();
  z0 = cPos.z(); // z0 now treated as global z0 rather than s (as is required)
  xp = cPrime.x();
  yp = cPrime.y();
  zp = cPrime.z();
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << "x0: " << x0 << " y0: " << y0 << " z0: " << z0 << G4endl;
  G4cout << "xp: " << xp << " yp: " << yp << " zp: " << zp << G4endl;
#endif
}

G4double BDSBunch::CalculateZp(G4double xp, G4double yp, G4double Zp0In)const
{
  G4double zp;
  G4double transMom = std::pow(xp, 2) + std::pow(yp, 2);

  if (transMom > 1) {
    G4cout << __METHOD_NAME__ << "ERROR xp, yp too large, xp: " << xp << " yp: " << yp << G4endl;
    exit(1);
  }
  if (Zp0In < 0)
    {zp = -std::sqrt(1.0 - transMom);}
  else
    {zp = std::sqrt(1.0 - transMom);}

  return zp;
}
