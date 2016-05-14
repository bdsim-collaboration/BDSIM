#include "BDSDebug.hh"
#include "BDSIntegratorQuadrupole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

#include <cmath>

using std::max;

BDSIntegratorQuadrupole::BDSIntegratorQuadrupole(BDSMagnetStrength const* strength,
						 G4double                 brho,
						 G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorBase(eqOfMIn, 6),
  yInitial(0), yMidPoint(0), yFinal(0)
{
  // B' = dBy/dx = Brho * (1/Brho dBy/dx) = Brho * k1
  bPrime = brho * (*strength)["k1"];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B' = " << bPrime << G4endl;
#endif
}

void BDSIntegratorQuadrupole::AdvanceHelix(const G4double yIn[],
					   const G4double dydx[],
					   const G4double h,
					   G4double       yOut[],
					   G4double       yErr[])
{
  const G4double *pIn      = yIn+3;
  G4ThreeVector GlobalR    = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector GlobalP    = G4ThreeVector(pIn[0], pIn[1], pIn[2]);
  G4ThreeVector InitMomDir = GlobalP.unit();
  G4double      InitPMag   = GlobalP.mag();
  // quad strength k normalised to charge and momentum of this particle
  G4double kappa = - eqOfM->FCof()*bPrime/InitPMag;

  /*
#ifdef BDSDEBUG
  G4double charge = (eqOfM->FCof())/CLHEP::c_light;
  G4cout << "BDSIntegratorQuadrupole: step = " << h/CLHEP::m << " m" << G4endl
         << " x  = " << yIn[0]/CLHEP::m     << " m"     << G4endl
         << " y  = " << yIn[1]/CLHEP::m     << " m"     << G4endl
         << " z  = " << yIn[2]/CLHEP::m     << " m"     << G4endl
         << " px = " << yIn[3]/CLHEP::GeV   << " GeV/c" << G4endl
         << " py = " << yIn[4]/CLHEP::GeV   << " GeV/c" << G4endl
         << " pz = " << yIn[5]/CLHEP::GeV   << " GeV/c" << G4endl
         << " q  = " << charge/CLHEP::eplus << " e"     << G4endl
         << " dBy/dx = " << bPrime/(CLHEP::tesla/CLHEP::m) << " T/m" << G4endl
         << " k = " << kappa/(1./CLHEP::m2) << " m^-2" << G4endl
         << G4endl; 
#endif
  */
  // relevant momentum scale is p_z, not P_tot:
  // check that the approximations are valid, else do a linear step:
  if(fabs(kappa)<1.e-12)
    {
      G4ThreeVector positionMove = h * InitMomDir;

      yOut[0] = yIn[0] + positionMove.x();
      yOut[1] = yIn[1] + positionMove.y();
      yOut[2] = yIn[2] + positionMove.z();
				
      yOut[3] = GlobalP.x();
      yOut[4] = GlobalP.y();
      yOut[5] = GlobalP.z();

      distChord=0;

      for(G4int i = 0; i < nVariables; i++)
	{yErr[i] = 0;}
      return;
    }

  G4double      h2      = pow(h,2);
  G4ThreeVector LocalR  = ConvertToLocal(GlobalR);
  G4ThreeVector LocalRp = ConvertAxisToLocal(GlobalR, GlobalP);
  
  G4double x0  = LocalR.x();
  G4double y0  = LocalR.y();
  G4double z0  = LocalR.z();
  G4double xp  = LocalRp.x();
  G4double yp  = LocalRp.y();
  G4double zp  = LocalRp.z();
  G4double x1  = x0;
  G4double y1  = y0;
  G4double z1  = z0;
  G4double xp1 = xp;
  G4double yp1 = yp;
  G4double zp1 = zp;

  // local r'' (for curvature)
  G4ThreeVector LocalRpp;
  LocalRpp.setX(-zp*x0); // can this be replaced by a cross produce?
  LocalRpp.setY( zp*y0); // G4ThreeVector as cross method
  LocalRpp.setZ( x0*xp - y0*yp);
  LocalRpp *= kappa;
  // determine effective curvature 
  G4double R_1 = LocalRpp.mag();
  
  if (!BDS::IsFinite(R_1))
    {
      // use a classical Runge Kutta stepper here
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
      return;
    }
  
  // Don't need 'else' (and associated indentation) as returns above
  G4double R=1./R_1;
      
  // chord distance (simple quadratic approx)
  distChord= h2/(8*R);

  // check for paraxial approximation:
  if(std::abs(zp)>0.9)
    {
#ifdef BDSDEBUG
      G4cout << "paraxial approximation being used" << G4endl;
#endif
      // G4double rootK  = sqrt(fabs(kappa*zp)); // direction independent
      G4double rootK  = sqrt(fabs(kappa)); // direction independent
      G4double rootKh = rootK*h; //*zp
      G4double X11,X12,X21,X22 = 0;
      G4double Y11,Y12,Y21,Y22 = 0;
      
      if (kappa>0)
	{
	  X11= cos(rootKh);
	  X12= sin(rootKh)/rootK;
	  X21=-fabs(kappa)*X12;
	  X22= X11;
	  
	  G4double sinhk = sinh(rootKh);
	  Y11= cosh(rootKh);
	  Y12= sinhk/rootK;
	  Y21= sqrt(std::abs(kappa))*Y12;
	  Y22= Y11;
	}
      else //if (kappa<0)
	{
	  X11= cosh(rootKh);
	  X12= sinh(rootKh)/rootK;
	  X21= fabs(kappa)*X12;
	  X22= X11;
	  
	  Y11= cos(rootKh);
	  Y12= sin(rootKh)/rootK;
	  Y21= -fabs(kappa)*Y12;
	  Y22= Y11;
	}
      
      x1  = X11*x0 + X12*xp;    
      xp1 = X21*x0 + X22*xp;
      
      y1  = Y11*y0 + Y12*yp;    
      yp1 = Y21*y0 + Y22*yp;
      
      zp1 = sqrt(1 - xp1*xp1 - yp1*yp1);
      
      G4double dx = x1-x0;
      G4double dy = y1-y0;
      
      // Linear chord length
      G4double dR2=dx*dx+dy*dy;
      G4double dz=sqrt(h2*(1.-h2/(12*R*R))-dR2);
      
      // check for precision problems - enforce conservation
      G4double ScaleFac = (dx*dx+dy*dy+dz*dz)/h2;
      if(ScaleFac>1.0000001)
	{
	  ScaleFac=sqrt(ScaleFac);
	  dx/=ScaleFac;
	  dy/=ScaleFac;
	  dz/=ScaleFac;
	  x1=x0+dx;
	  y1=y0+dy;
	}
      z1 = z0 + dz;
    }
		  /*
      Else
	// perform local helical steps (paraxial approx not safe)
	{
#ifdef BDSDEBUG
	  G4cout << "local helical steps" << G4endl;
#endif

	  // simple quadratic approx:	      
	  G4double quadX= - kappa*x0*zp;
	  G4double quadY=   kappa*y0*zp;
	  G4double quadZ=   kappa*(x0*xp - y0*yp);
	      
	  // determine maximum curvature:
	  G4double maxCurv=max(fabs(quadX),fabs(quadY));
	  maxCurv=max(maxCurv,fabs(quadZ));
	      
	  x1 = x0 + h*xp + quadX*h2/2;
	  y1 = y0 + h*yp + quadY*h2/2; 
	  z1 = z0 + h*zp + quadZ*h2/2;
	      
	  xp1 = xp + quadX*h;
	  yp1 = yp + quadY*h;
	  zp1 = zp + quadZ*h;
	      
	  // estimate parameters at end of step:
	  G4double quadX_end= - kappa*x1*zp1;
	  G4double quadY_end=   kappa*y1*zp1;
	  G4double quadZ_end=   kappa*(x1*xp1 - y1*yp1);
	      
	  // determine maximum curvature:
	  maxCurv=max(maxCurv,fabs(quadX_end));
	  maxCurv=max(maxCurv,fabs(quadY_end));
	  maxCurv=max(maxCurv,fabs(quadZ_end));

	  distChord=maxCurv*h2/4.;
	      
	  // use the average:
	  G4double quadX_av=(quadX+quadX_end)/2;
	  G4double quadY_av=(quadY+quadY_end)/2;
	  G4double quadZ_av=(quadZ+quadZ_end)/2;
	      
	  G4double x_prime_av=(xp + xp1)/2;
	  G4double y_prime_av=(yp + yp1)/2;
	  G4double z_prime_av=(zp + zp1)/2;
	      
	  x1 = x0 + h*x_prime_av + quadX_av * h2/2;
	  y1 = y0 + h*y_prime_av + quadY_av * h2/2; 
	  z1 = z0 + h*z_prime_av + quadZ_av * h2/2;
	      
	  xp1 = xp + quadX_av*h;
	  yp1 = yp + quadY_av*h;
	  zp1 = zp + quadZ_av*h;
	      
	  G4double dx = (x1-x0);
	  G4double dy = (y1-y0);
	  G4double dz = (z1-z0);
	  G4double chord2 = dx*dx + dy*dy + dz*dz;
	  if(chord2>h2)
	    {
	      G4double hnew=h*sqrt(h2/chord2);
	      x1=x0 + hnew*x_prime_av + quadX_av * hnew*hnew/2;
	      y1=y0 + hnew*y_prime_av + quadY_av * hnew*hnew/2; 
	      z1=z0 + hnew*z_prime_av + quadZ_av * hnew*hnew/2;

	      xp1=xp + quadX_av*hnew;
	      yp1=yp + quadY_av*hnew;
	      zp1=zp + quadZ_av*hnew;
	    }
	}

      LocalR.setX(x1);
      LocalR.setY(y1);
      LocalR.setZ(z1);
	  
      LocalRp.setX(xp1);
      LocalRp.setY(yp1);
      LocalRp.setZ(zp1);

    }
  else //if curvature = 0 ...
    {
      LocalR += h*LocalRp;
      distChord=0.;
    }
*/
    LocalR.setX(x1);
    LocalR.setY(y1);
    LocalR.setZ(z1);

    LocalRp.setX(xp1);
    LocalRp.setY(yp1);
    LocalRp.setZ(zp1);

  GlobalR = ConvertToGlobal(LocalR);
  GlobalP = ConvertAxisToGlobal(LocalRp);

  yOut[0] = GlobalR.x();
  yOut[1] = GlobalR.y();
  yOut[2] = GlobalR.z();

  yOut[3] = GlobalP.x();
  yOut[4] = GlobalP.y();
  yOut[5] = GlobalP.z();
}

void BDSIntegratorQuadrupole::Stepper(const G4double yInput[],
				      const G4double dydx[],
				      const G4double h,
				      G4double       yOut[],
				      G4double       yErr[])
{
  const G4double *pIn   = yInput+3;
  G4ThreeVector GlobalP = G4ThreeVector(pIn[0], pIn[1], pIn[2]);
  G4double InitPMag     = GlobalP.mag();
  G4double kappa        = - eqOfM->FCof()*bPrime/InitPMag;
  
  if(std::abs(kappa) < 1e-9) //kappa is small - no error needed for paraxial treatment
    {AdvanceHelix(yInput,dydx,h,yOut,yErr);}
  else
    {
      // Compute errors by making two half steps
      G4double yTemp[7], yIn[7];
      
      // Saving yInput because yInput and yOut can be aliases for same array
      for(G4int i = 0; i < nVariables; i++)
	{yIn[i] = yInput[i];}
      
      // Do two half steps
      G4double hstep = h * 0.5; 
      AdvanceHelix(yIn,   dydx, hstep, yTemp, yErr);
      AdvanceHelix(yTemp, dydx, hstep, yOut,  yErr);
      
      // Do a full Step
      AdvanceHelix(yIn, dydx, h, yTemp, yErr);
      
      for(G4int i = 0; i < nVariables; i++)
	{
	  yErr[i] = yOut[i] - yTemp[i];
	  // if error small, set error to 0
	  // this is done to prevent Geant4 going to smaller and smaller steps
	  // ideally use some of the global constants instead of hardcoding here
	  // could look at step size as well instead.
	  if (std::abs(yErr[i]) < 1e-7)
	    {yErr[i] = 0;}
	}
    }
}
