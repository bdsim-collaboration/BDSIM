#include "BDSDebug.hh"
#include "BDSIntegratorDipole.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetStrength.hh"

#include <utility>
#include "globals.hh" // geant4 types / globals
#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"


BDSIntegratorDipole::BDSIntegratorDipole(BDSMagnetStrength const*  strength,
					 G4double                  brho,
					 G4Mag_EqRhs*              eqOfMIn):
  BDSIntegratorBase(eqOfMIn, 6),
  angle((*strength)["angle"]),
  length((*strength)["length"]),
  bField((*strength)["field"])
{
  bPrime = brho * (*strength)["k1"];
  nominalEnergy = BDSGlobalConstants::Instance()->BeamTotalEnergy();

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B Field " << bField << G4endl;
  G4cout << __METHOD_NAME__ << "B'      " << bPrime << G4endl;
#endif
}

void BDSIntegratorDipole::AdvanceHelix(const G4double  yIn[],
				       const G4double dydx[],
				       G4ThreeVector  /*bField*/,
				       G4double  h,
				       G4double yOut[],
				       G4double yErr[])
{
  // UNCOMMENT TO USE ONLY the g4 stepper for testing
  // use a classical Runge Kutta stepper here
  //backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
  //return;
  G4ThreeVector GlobalPosition = G4ThreeVector(yIn[0], yIn[1], yIn[2]);  
      
  G4double charge = (eqOfM->FCof())/CLHEP::c_light;
#ifdef BDSDEBUG
  G4cout << "BDSIntegratorDipole: step= " << h/CLHEP::m << " m" << G4endl
         << " x  = " << yIn[0]/CLHEP::m     << " m" << G4endl
         << " y  = " << yIn[1]/CLHEP::m     << " m" << G4endl
         << " z  = " << yIn[2]/CLHEP::m     << " m" << G4endl
         << " px = " << yIn[3]/CLHEP::GeV   << " GeV/c" << G4endl
         << " py = " << yIn[4]/CLHEP::GeV   << " GeV/c" << G4endl
         << " pz = " << yIn[5]/CLHEP::GeV   << " GeV/c" << G4endl
         << " q  = " << charge/CLHEP::eplus << " e" << G4endl
	 << " B  = " << bField/(CLHEP::tesla) << " T" << G4endl
    //         << " k= " << kappa/(1./CLHEP::m2) << "m^-2" << G4endl
         << G4endl; 
#endif

  const G4double *pIn = yIn+3;
  G4ThreeVector v0    = G4ThreeVector(pIn[0], pIn[1], pIn[2]);
  G4double      InitMag        = v0.mag();
  G4ThreeVector InitMomDir     = v0.unit();
  G4double rho = InitMag/CLHEP::GeV/(0.299792458 * bField/CLHEP::tesla * charge) * CLHEP::m;

  // in case of zero field (though what if there is a quadrupole part..)
  // or neutral particles do a linear step:
  if(bField==0 || eqOfM->FCof()==0)
    {
      G4ThreeVector positionMove = h * InitMomDir;
      
      yOut[0] = yIn[0] + positionMove.x();
      yOut[1] = yIn[1] + positionMove.y();
      yOut[2] = yIn[2] + positionMove.z();
      
      yOut[3] = v0.x();
      yOut[4] = v0.y();
      yOut[5] = v0.z();
      
      distChord = 0;
      return;
    }

  G4ThreeVector globalPositionIn(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector LocalR  = ConvertToLocal(globalPositionIn);
  G4ThreeVector LocalRp = ConvertAxisToLocal(globalPositionIn, InitMomDir);
  
  //Copy of LocalR and LocalRP for use with quadrupolar component code
  G4ThreeVector itsInitialR      = LocalR;
  G4ThreeVector itsInitialRp     = LocalRp;
  
  // advance the orbit
  std::pair<G4ThreeVector,G4ThreeVector> RandRp = updatePandR(rho,h,LocalR,LocalRp);
  G4ThreeVector itsFinalPoint = RandRp.first;
  G4ThreeVector itsFinalDir = RandRp.second;

  G4double CosT_ov_2=cos(h/rho/2.0);
  distChord = fabs(rho)*(1.-CosT_ov_2);

  // check for paraxial approximation:
  if(LocalRp.z() > 0.9)
  {
    // gradient for quadrupolar field
    G4double kappa = - eqOfM->FCof() * bPrime / InitMag; // was ist das?
    // ignore quadrupolar component for now as this needs fixing
    if(true || fabs(kappa)<1.e-12)
      {
      GlobalPosition = ConvertToGlobal(itsFinalPoint);
      G4ThreeVector GlobalTangent = ConvertAxisToGlobal(GlobalPosition, itsFinalDir);

      GlobalTangent*=InitMag;
      
      yOut[0] = GlobalPosition.x(); 
      yOut[1] = GlobalPosition.y(); 
      yOut[2] = GlobalPosition.z(); 
      
      yOut[3] = GlobalTangent.x();
      yOut[4] = GlobalTangent.y();
      yOut[5] = GlobalTangent.z();
      return;
      }

      G4double x1,x1p,y1,y1p,z1p;
      //G4double z1;
      
      G4double NomR = nominalEnergy/CLHEP::GeV/(0.299792458 * bField/CLHEP::tesla) * CLHEP::m;
      
      G4double NominalPath = sqrt(NomR*NomR - LocalR.z()*LocalR.z()) - fabs(NomR)*cos(angle/2);
      
      G4double EndNomPath = sqrt(NomR*NomR - itsFinalPoint.z()*itsFinalPoint.z()) - fabs(NomR)*cos(angle/2);

      if(rho<0)
	{
	  NominalPath*=-1;
	  EndNomPath*=-1;
	}

      G4double x0=LocalR.x() - NominalPath;
      G4double y0=LocalR.y();
      G4double z0=LocalR.z();

      G4double theta_in = asin(z0/NomR);
  
      LocalRp.rotateY(-theta_in);

      G4double xp=LocalRp.x();
      G4double yp=LocalRp.y();
      G4double zp=LocalRp.z();
      
      G4double rootK=sqrt(fabs(kappa*zp));
      G4double rootKh=rootK*h*zp;
      G4double X11,X12,X21,X22;
      G4double Y11,Y12,Y21,Y22;
      
      if (kappa>0)
	{
	  X11= cos(rootKh);
	  X12= sin(rootKh)/rootK;
	  X21=-fabs(kappa)*X12;
	  X22= X11;
	  
	  Y11= cosh(rootKh);
	  Y12= sinh(rootKh)/rootK;
	  Y21= fabs(kappa)*Y12;
	  Y22= Y11;
	}
      else // if (kappa<0)
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
      x1p = X21*x0 + X22*xp;
      
      y1  = Y11*y0 + Y12*yp;    
      y1p = Y21*y0 + Y22*yp;
      
      z1p = sqrt(1 - x1p*x1p -y1p*y1p);

      /* 
	 x1 -=(kappa/ (24*R) ) * h2*h2;
	 x1p-=(kappa/ (6*R) ) * h*h2;
      */
      G4double dx=x1-x0;
      G4double dy=y1-y0;
      // Linear chord length
      
      LocalR.setX(dx +itsInitialR.x() + EndNomPath - NominalPath);
      LocalR.setY(dy + itsInitialR.y());
      LocalR.setZ(itsFinalPoint.z());
      

      LocalRp.setX(x1p);
      LocalRp.setY(y1p);
      LocalRp.setZ(z1p);
      LocalRp.rotateY(theta_in);

      GlobalPosition = ConvertToGlobal(LocalR);
      
      LocalRp.rotateY(-h/rho);
      G4ThreeVector GlobalTangent = ConvertAxisToGlobal(LocalRp);
      
      GlobalTangent*=InitMag;
  
      // gab: replace += with =
      yOut[0] = GlobalPosition.x(); 
      yOut[1] = GlobalPosition.y(); 
      yOut[2] = GlobalPosition.z(); 
      
      yOut[3] = GlobalTangent.x();
      yOut[4] = GlobalTangent.y();
      yOut[5] = GlobalTangent.z();
      
    }
  else
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " local helical steps - using G4ClassicalRK4" << G4endl;
#endif
      // use a classical Runge Kutta stepper here
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
    }
}

void BDSIntegratorDipole::Stepper(const G4double yInput[],
				  const G4double dydx[],
				  const G4double hstep,
				  G4double yOut[],
				  G4double yErr[])
{
  //G4double err = 1e-10 * hstep; // very small linear increase with distance
  for(G4int i=0; i<nVariables; i++)
    {yErr[i] = 0;}

  AdvanceHelix(yInput,dydx,(G4ThreeVector)0,hstep,yOut,yErr);
}

std::pair<G4ThreeVector,G4ThreeVector> BDSIntegratorDipole::updatePandR(G4double rho,
                                            G4double h,
                                            G4ThreeVector LocalR,
                                            G4ThreeVector LocalRp)
{
  G4ThreeVector yhat(0.,1.,0.);
  G4ThreeVector vhat  = LocalRp;
  G4ThreeVector vnorm = vhat.cross(yhat);
    
  G4double Theta   = h/rho;

  G4double CosT_ov_2, SinT_ov_2, CosT, SinT;
  CosT_ov_2=cos(Theta/2);
  SinT_ov_2=sin(Theta/2);
  
  CosT=(CosT_ov_2*CosT_ov_2)- (SinT_ov_2*SinT_ov_2);
  SinT=2*CosT_ov_2*SinT_ov_2;

  G4ThreeVector dPos = rho*(SinT*vhat + (1-CosT)*vnorm);
  G4ThreeVector itsFinalPoint = LocalR+dPos;
  G4ThreeVector itsFinalDir   = CosT*vhat +SinT*vnorm;
  
  return std::make_pair(itsFinalPoint,itsFinalDir);

}
