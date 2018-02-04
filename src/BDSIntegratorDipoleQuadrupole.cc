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
#include "BDSDebug.hh"
#include "BDSIntegratorDipole2.hh"
#include "BDSIntegratorDipoleQuadrupole.hh"
#include "BDSIntegratorQuadrupole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSIntegratorDipoleQuadrupole::BDSIntegratorDipoleQuadrupole(BDSMagnetStrength const* strengthIn,
							     G4double                 brhoIn,
							     G4Mag_EqRhs*             eqOfMIn,
							     G4double minimumRadiusOfCurvatureIn):
  BDSIntegratorMag(eqOfMIn, 6),
  dipole(new BDSIntegratorDipole2(eqOfMIn, minimumRadiusOfCurvatureIn)),
  strength(strengthIn)
{
  eq = static_cast<BDSMagUsualEqRhs*>(eqOfM);
  bPrime = std::abs(brhoIn) * (*strengthIn)["k1"];
  k1 = (*strengthIn)["k1"];
  bRho = brhoIn;
}

BDSIntegratorDipoleQuadrupole::~BDSIntegratorDipoleQuadrupole()
{
  delete dipole;
}

void BDSIntegratorDipoleQuadrupole::Stepper(const G4double yIn[],
					    const G4double dydx[],
					    const G4double h,
					    G4double       yOut[],
					    G4double       yErr[])
{
  // Protect against very small steps or neutral particles drift through.
  if (h < 1e-12 || !BDS::IsFinite(eqOfM->FCof()))
    {
      AdvanceDriftMag(yIn,h,yOut,yErr);
      SetDistChord(0);
      return;
    }

  // get beta (v/c)
  beta = eq->Beta(yIn);

  // try out a dipole step first
  dipole->Stepper(yIn, dydx, h, yOut, yErr);

  // If the particle might spiral, we return and just use the dipole only component
  // Aimed at particles of much lower momentum than the design energy.
  G4double dipoleDC          = dipole->DistChord();
  G4double radiusOfCurvature = dipole->RadiusOfHelix();
  if (dipoleDC > 0.3*radiusOfCurvature)
    {return;}

  // We assume that the major effect is the dipole component and the quadrupole
  // component is low. Therefore, we can safely set distchord from the dipole.
  SetDistChord(dipoleDC);
  
  // not going to spiral so proceed
  // convert to true curvilinear
  G4ThreeVector globalPos   = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector globalMom   = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  BDSStep       localCL     = BDSAuxiliaryNavigator::GlobalToCurvilinear(strength, globalPos, globalMom, h, false, eqOfM->FCof());
  G4ThreeVector localCLPos  = localCL.PreStepPoint();
  G4ThreeVector localCLMom  = localCL.PostStepPoint();
  G4ThreeVector localCLMomU = localCLMom.unit();

  // only proceed with thick matrix if particle is paraxial
  // judged by forward momentum > 0.9 and |transverse| < 0.1
  if (localCLMomU.z() < 0.9 || std::abs(localCLMomU.x()) > 0.1 || std::abs(localCLMomU.y()) > 0.1)
    {
      dipole->Stepper(yIn, dydx, h, yOut, yErr);
      SetDistChord(dipole->DistChord());
      return;
    }
  
  // calculate new position
  G4ThreeVector localCLPosOut;
  G4ThreeVector localCLMomOut;
  OneStep(localCLPos, localCLMom, localCLMomU, h, localCLPosOut, localCLMomOut);

  // convert to global coordinates for output
  BDSStep globalOut = BDSAuxiliaryNavigator::CurvilinearToGlobal(strength, localCLPosOut, localCLMomOut, false, eqOfM->FCof());

  G4ThreeVector globalPosOut = globalOut.PreStepPoint();
  G4ThreeVector globalMomOut = globalOut.PostStepPoint();

  // write out values and errors
  for (G4int i = 0; i < 3; i++)
    {
      yOut[i]     = globalPosOut[i];
      yOut[i + 3] = globalMomOut[i];
      yErr[i]     = 0;
      yErr[i + 3] = 0;
    }
}

void BDSIntegratorDipoleQuadrupole::OneStep(G4ThreeVector  posIn,
					    G4ThreeVector  momIn,
					    G4ThreeVector  momUIn,
					    G4double       h,
					    G4ThreeVector& posOut,
					    G4ThreeVector& momOut) const
{
  G4double momInMag = momIn.mag();

  // nominal bending radius.
  G4double magnetRho = bRho / (*strength)["field"];

  G4double c = CLHEP::c_light * CLHEP::m;
  G4double nomMomentum = std::abs(bRho * eqOfM->FCof());
  G4double energy = eq->TotalEnergy(momIn);
  G4double nomEnergy = std::sqrt(pow(nomMomentum,2) + eq->Mass());

  // deltaE/(P0*c) to match literature.
  G4double deltaEoverPc = (energy - nomEnergy) / (nomMomentum * c) ;

  // quad strength k normalised to charge and nominal momentum
  // eqOfM->FCof() gives us conversion to MeV,mm and rigidity in Tm correctly
  // as well as charge of the given particle
  G4double K1  = std::abs(eqOfM->FCof())*bPrime/nomMomentum;

  // separate focussing strengths for vertical and horizontal axes.
  // Used by matrix elements so must be derived from nominal values.
  G4double kx2 = pow(1.0 / magnetRho, 2) + K1;
  G4double kx  = sqrt(std::abs(kx2));
  G4double ky2 = -K1;
  G4double ky  = sqrt(std::abs(ky2));
  G4double kxl = kx * h;
  G4double kyl = ky * h;

  G4bool focussing = K1 >= 0; // depends on charge as well (in eqOfM->FCof())

  G4double x0  = posIn.x();
  G4double y0  = posIn.y();
  G4double s0  = posIn.z();
  G4double xp  = momUIn.x();
  G4double yp  = momUIn.y();
  G4double zp  = momUIn.z();

  G4double x1  = x0;
  G4double y1  = y0;
  G4double xp1 = xp;
  G4double yp1 = yp;
  G4double zp1 = zp;

  G4double X11,X12,X16,X21,X22,X26 = 0;
  G4double Y11,Y12,Y21,Y22 = 0;

  // matrix elements. All must derived from nominal parameters.
  if (focussing)
    {//focussing
      X11= cos(kxl);
      X12= sin(kxl)/kx;
      X21=-std::abs(kx2)*X12;
      X22= X11;
      X16 = (1.0/beta) * (-(1.0/magnetRho) / kx2) * (1 - cos(kxl));
      X26 = (1.0/beta) * -(1.0/magnetRho) * X12;

      Y11= cosh(kyl);
      Y12= sinh(kyl)/ky;
      if (!BDS::IsFinite(ky))  //otherwise y12 is nan from div by 0 in previous line
        {Y12 = h;}
      Y21= std::abs(ky2)*Y12;
      Y22= Y11;
    }
  else
    {// defocussing
      X11= cosh(kxl);
      X12= sinh(kxl)/kx;
      X21= std::abs(kx2)*X12;
      X22= X11;
      X16 = (1.0/beta) * (-(1.0/magnetRho) / kx2) * (1 - cosh(kxl));
      X26 = (1.0/beta) * -(1.0/magnetRho) * X12;
      
      Y11= cos(kyl);
      Y12= sin(kyl)/ky;
      if (!BDS::IsFinite(ky))  //otherwise y12 is nan from div by 0 in previous line
        {Y12 = h;}
      Y21= -std::abs(ky2)*Y12;
      Y22= Y11;
    }
      
  x1  = X11*x0 + X12*xp + X16*deltaEoverPc;
  xp1 = X21*x0 + X22*xp + X26*deltaEoverPc;
  
  y1  = Y11*y0 + Y12*yp;    
  yp1 = Y21*y0 + Y22*yp;

  G4double s1 = s0 + h;
  
  // relies on normalised momenta otherwise this will be nan.
  zp1 = std::sqrt(1 - xp1*xp1 - yp1*yp1);
  if (std::isnan(zp1))
    {zp1 = zp;} // ensure not nan

  G4ThreeVector momOutUnit = G4ThreeVector(xp1, yp1, zp1);
  momOut = momOutUnit * momInMag;

  posOut = G4ThreeVector(x1, y1, s1);

  /*
  G4double cosTheta  = cos(theta);
  G4double sinTheta  = sin(theta);
  G4double coshTheta = cosh(theta);
  G4double sinhTheta = sinh(theta);
  G4double m00 = cosTheta;
  G4double m01 = - sqrtKappa * sinTheta;
  G4double m10 = sinTheta / sqrtKappa;
  G4double m11 = cosTheta;
  G4double m05 = (1 - cosTheta) / sqrtKappa;
  G4double m15 = sinTheta;
  G4double m22 = coshTheta;
  G4double m23 = sinhTheta / sqrtKappa;
  G4double m32 = sqrtKappa * sinhTheta;
  G4double m33 = coshTheta;

  G4double x1  = m00*x0 + m10*xp0 + m05*delta;
  G4double xp1 = m01*x0 + m11*xp0 + m15*delta;
  G4double y1  = m22*y0 + m23*yp0;
  G4double yp1 = m32*y0 + m33*yp0;
  G4double s1  = s0 + h;

  posOut[0] = x1;
  posOut[1] = y1;
  posOut[2] = s1;
  momOut[0] = xp1;
  momOut[1] = yp1;
  momOut[2] = momIn[2];
  */
}

