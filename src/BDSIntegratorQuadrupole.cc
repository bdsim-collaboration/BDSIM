#include "BDSDebug.hh"
#include "BDSIntegratorQuadrupole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"

#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSIntegratorQuadrupole::BDSIntegratorQuadrupole(BDSMagnetStrength const* strength,
						 G4double                 brho,
						 G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorMag(eqOfMIn, 6),
  yInitial(0), yMidPoint(0), yFinal(0)
{
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  bPrime = brho * (*strength)["k1"] / CLHEP::m2;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B' = " << bPrime << G4endl;
#endif
}

void BDSIntegratorQuadrupole::Stepper(const G4double yIn[],
				      const G4double dydx[],
				      const G4double h,
				      G4double       yOut[],
				      G4double       yErr[])
{
  G4ThreeVector GlobalP    = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4double      InitPMag   = GlobalP.mag();

  // quad strength k normalised to charge and momentum of this particle
  // note bPrime was calculated w.r.t. the nominal rigidity.
  G4double kappa = eqOfM->FCof()*bPrime/InitPMag;
  // eqOfM->FCof() gives us conversion to MeV,mm and rigidity in Tm correctly
  
  // Check this will have a perceptible effect and if not do a linear step.
  if(std::abs(kappa) < 1e-20)
    {
      AdvanceDriftMag(yIn, h, yOut, yErr);
      SetDistChord(0);
      return;
    }

  G4ThreeVector GlobalR     = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector InitMomDir  = GlobalP.unit();
  BDSStep       localPosMom = ConvertToLocal(GlobalR, InitMomDir, h, false);
  G4ThreeVector LocalR      = localPosMom.PreStepPoint();
  G4ThreeVector LocalRp     = localPosMom.PostStepPoint();

  if (LocalRp.z() < 0.9) // not forwards - can't use our paraxial stepper - use backup one
    {
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
      SetDistChord(backupStepper->DistChord());
      return;
    }
  
  G4double h2  = h*h; // safer than pow
  G4double x0  = LocalR.x();
  G4double y0  = LocalR.y();
  G4double z0  = LocalR.z();
  G4double xp  = LocalRp.x();
  G4double yp  = LocalRp.y();
  G4double zp  = LocalRp.z();
    
  // initialise output varibles with input position as default
  G4double x1  = x0;
  G4double y1  = y0;
  G4double z1  = z0 + h; // new z position will be along z by step length h
  G4double xp1 = xp;
  G4double yp1 = yp;
  G4double zp1 = zp;

  // local r'' (for curvature)
  G4ThreeVector LocalRpp;
  LocalRpp.setX(-zp*x0); // can this be replaced by a cross produce?
  LocalRpp.setY( zp*y0); // G4ThreeVector has a cross method
  LocalRpp.setZ( x0*xp - y0*yp);
  LocalRpp *= kappa;
  // determine effective curvature 
  G4double R_1 = LocalRpp.mag();
  
  // Don't need 'else' (and associated indentation) as returns above
  G4double R=1./R_1;
      
  // chord distance (simple quadratic approx)
  G4double dc = h2/(8*R);
  SetDistChord(dc);
  
  G4double rootK  = std::sqrt(std::abs(kappa*zp)); // direction independent
  G4double rootKh = rootK*h*zp;
  G4double X11,X12,X21,X22 = 0;
  G4double Y11,Y12,Y21,Y22 = 0;
  
  if (kappa > 0)
    {//focussing
      X11= cos(rootKh);
      X12= sin(rootKh)/rootK;
      X21=-std::abs(kappa)*X12;
      X22= X11;
      
      Y11= cosh(rootKh);
      Y12= sinh(rootKh)/rootK;
      Y21= std::abs(kappa)*Y12;
      Y22= Y11;
    }
  else
    {// defocussing
      X11= cosh(rootKh);
      X12= sinh(rootKh)/rootK;
      X21= std::abs(kappa)*X12;
      X22= X11;
      
      Y11= cos(rootKh);
      Y12= sin(rootKh)/rootK;
      Y21= -std::abs(kappa)*Y12;
      Y22= Y11;
    }
      
  x1  = X11*x0 + X12*xp;    
  xp1 = X21*x0 + X22*xp;
  
  y1  = Y11*y0 + Y12*yp;    
  yp1 = Y21*y0 + Y22*yp;
  
  // relies on normalised momenta otherwise this will be nan.
  zp1 = std::sqrt(1 - xp1*xp1 - yp1*yp1);
  
  G4double dx = x1 - x0;
  G4double dy = y1 - y0;
  
  // Linear chord length
  G4double dR2 = dx*dx + dy*dy;
  G4double dz = std::sqrt(h2 * (1. - h2 / (12 * R * R)) - dR2);
  
  z1 = z0 + dz;
  
  LocalR.setX(x1);
  LocalR.setY(y1);
  LocalR.setZ(z1);
  LocalRp.setX(xp1);
  LocalRp.setY(yp1);
  LocalRp.setZ(zp1);
  
  ConvertToGlobal(LocalR,LocalRp,InitPMag,yOut);
  for (G4int i = 0; i < nVariables; i++)
    {yErr[i] = 0;}
}
