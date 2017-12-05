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
  bPrime = std::abs(brhoIn) * (*strengthIn)["k1"] / CLHEP::m2;
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
  
  // try out a dipole step first
  dipole->Stepper(yIn, dydx, h, yOut, yErr);

  // if no quadrupole component, simply return
  if (!BDS::IsFinite(bPrime))
    {return;}

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
  BDSStep       localCL     = GlobalToCurvilinear(strength, globalPos, globalMom, h, false);
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
  BDSStep globalOut = CurvilinearToGlobal(localCLPosOut, localCLMomOut, false);
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
  G4double delta    = eqOfM->FCof() / momInMag;
  
  // quad strength k normalised to charge and momentum of this particle
  // note bPrime was calculated w.r.t. the nominal rigidity.
  // eqOfM->FCof() gives us conversion to MeV,mm and rigidity in Tm correctly
  // as well as charge of the given particle
  G4double Kappa    = eqOfM->FCof()*bPrime/momInMag;

  // K = k + k_0^2
  //G4double Kappa     = 0;
  G4bool   focussing = Kappa >= 0; // depends on charge as well (in eqOfM->FCof())
  // In -ve K case we would take sqrt(|K|), so valid in either case
  G4double sqrtKappa = std::sqrt(std::abs(Kappa));

  G4double theta = sqrtKappa * h;

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
  
  G4double rootK  = std::sqrt(std::abs(Kappa*zp)); // direction independent
  if (std::isnan(rootK))
    {rootK = 0;}
  G4double rootKh = rootK*h*zp;
  G4double X11,X12,X21,X22 = 0;
  G4double Y11,Y12,Y21,Y22 = 0;

  if (focussing)
    {//focussing
      X11= cos(rootKh);
      X12= sin(rootKh)/rootK;
      X21=-std::abs(Kappa)*X12;
      X22= X11;
      
      Y11= cosh(rootKh);
      Y12= sinh(rootKh)/rootK;
      Y21= std::abs(Kappa)*Y12;
      Y22= Y11;
    }
  else
    {// defocussing
      X11= cosh(rootKh);
      X12= sinh(rootKh)/rootK;
      X21= std::abs(Kappa)*X12;
      X22= X11;
      
      Y11= cos(rootKh);
      Y12= sin(rootKh)/rootK;
      Y21= -std::abs(Kappa)*Y12;
      Y22= Y11;
    }
      
  x1  = X11*x0 + X12*xp;    
  xp1 = X21*x0 + X22*xp;
  
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

