#include "BDSDebug.hh"
#include "BDSIntegratorSextupole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

#include "BDSIntegratorVelocityVerlet.hh"

BDSIntegratorSextupole::BDSIntegratorSextupole(BDSMagnetStrength const* strength,
					       G4double                 brho,
					       G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorBase(eqOfMIn, 6)
{
  // B'' = d^2By/dx^2 = Brho * (1/Brho d^2By/dx^2) = Brho * k2
  bDoublePrime     = brho * (*strength)["k2"] / CLHEP::m3;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B'' = " << bDoublePrime << G4endl;
#endif

  newstepper = new BDSIntegratorVelocityVerlet(eqOfMIn);
}

void BDSIntegratorSextupole::AdvanceHelix(const G4double  yIn[],
					  G4double        h,
					  G4double        ySext[],
					  G4bool          flag)
{
  const G4double *pIn = yIn+3;
  G4ThreeVector v0= G4ThreeVector( pIn[0], pIn[1], pIn[2]);  

  G4ThreeVector GlobalPosition= G4ThreeVector( yIn[0], yIn[1], yIn[2]);
  G4double InitMag=v0.mag();
  G4double kappa=  (-eqOfM->FCof()*bDoublePrime) /InitMag;

  if(fabs(kappa)<1.e-12)
    {
      // very low strength - treat as a drift
      AdvanceDrift(yIn,v0,h,ySext);
    }
  else 
    {
      // global to local
      BDSStep        localPosMom = ConvertToLocal(GlobalPosition, v0, h, false);
      G4ThreeVector      LocalR  = localPosMom.PreStepPoint();
      G4ThreeVector      Localv0 = localPosMom.PostStepPoint();
      G4ThreeVector      LocalRp = Localv0.unit();
      
      G4double x0=LocalR.x(); 
      G4double y0=LocalR.y();
      
      // Evaluate field at the approximate midpoint of the step.
      x0=x0+LocalRp.x()*h/2;
      y0=y0+LocalRp.y()*h/2;
      
      G4double x02My02=(x0*x0-y0*y0);
      
      G4double xp=LocalRp.x();
      G4double yp=LocalRp.y();
      G4double zp=LocalRp.z();
      
      // local r'' (for curvature)
      G4ThreeVector LocalRpp;
      LocalRpp.setX(zp*x02My02);
      LocalRpp.setY(-2*zp*x0*y0);
      LocalRpp.setZ(xp*x02My02-2*yp*x0*y0);

      if (flag)
	{G4cout << "LocalRpp before kappa/2 " << LocalRpp << G4endl;}
      
      LocalRpp*=kappa/2; // 2 is actually a 2! factor.

      if (flag)
	{G4cout << "LocalRpp " << LocalRpp << G4endl;}
      
      AdvanceChord(h,LocalR,LocalRp,LocalRpp);

      ConvertToGlobal(LocalR,LocalRp,InitMag,ySext);
    }
}

void BDSIntegratorSextupole::Stepper(const G4double yInput[],
				     const G4double dydx[],
				     const G4double hstep,
				     G4double yOut[],
				     G4double yErr[])
{
  G4double yTemp[7], yIn[7];
  
  //  Saving yInput because yInput and yOut can be aliases for same array 
  for(G4int i = 0; i < nVariables; i++)
    {yIn[i]=yInput[i];}

  //G4cout << "hstep " << hstep << G4endl;
  G4ThreeVector dxdyv0 = G4ThreeVector(dydx[0], dydx[1], dydx[2]);
  G4ThreeVector dxdyv1 = G4ThreeVector(dydx[3], dydx[4], dydx[5]);
  //G4cout << "dydx " << dxdyv0 << " " << dxdyv1 << G4endl;

  const G4double *pIn = yInput+3;
  G4ThreeVector GlobalR = G4ThreeVector(yInput[0], yInput[1], yInput[2]);
  G4ThreeVector GlobalP = G4ThreeVector(pIn[0],    pIn[1],    pIn[2]);

  G4ThreeVector GlobalPDir = GlobalP.unit();

  auxNavigator->LocateGlobalPointAndSetup(GlobalR);
  G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform();
  G4ThreeVector     localPDir= GlobalAffine.TransformAxis(GlobalPDir);

  if (localPDir.z() < 0.9 || GlobalP.mag() < 40.0 )
    {
      backupStepper->Stepper(yIn, dydx, hstep, yOut, yErr);
      SetDistChord(backupStepper->DistChord());
      return;
    }
  
  G4double h = hstep * 0.5;
  
  // Do two half steps
  AdvanceHelix(yIn, h, yTemp);
  AdvanceHelix(yTemp, h, yOut); 
  
  // Do a full Step
  h = hstep ;
  AdvanceHelix(yIn, h, yTemp, false);

  /*
  G4ThreeVector v1div = dxdyv1/1.5;
  G4ThreeVector dpos  = GlobalP*h + dxdyv1*(h*h / 2);
  G4ThreeVector dpos2 = v1div*h + v1div*(h*h / 2);
  G4ThreeVector newpos2 = GlobalR + dpos2;


  G4ThreeVector newpos = GlobalR + GlobalP*h + dxdyv1*(h*h / 2);

  G4cout << "global r " << GlobalR << G4endl;
  G4cout << "bdsim   ";
  for (G4int i = 0; i < 3; i++)
    {G4cout << yOut[i] << " ";}
  G4cout << G4endl;

  G4double yOutG4[7], yErrG4[7];
  backupStepper->Stepper(yIn, dydx, hstep, yOutG4, yErrG4);
  //G4cout << "g4 ";
  //for (G4int i = 0; i < 3; i++)
  //  {G4cout << yOutG4[i] << " ";}
  //G4cout << G4endl;

  //G4cout << "new bdism " << newpos << G4endl;
  //G4cout << "new bdism2 " << newpos2 << G4endl;

  G4double ynew2[7];
  G4double ynew2Err[7];
  newstepper->Stepper(yIn, dydx, h, ynew2, ynew2Err);
  G4cout << "bdsimvv ";
  for (G4int j = 0; j < 3; j++)
    {G4cout << "(" << ynew2[j] << ", " << ynew2Err[j] << ")";}
  G4cout << G4endl;

  G4int i;
  G4double yTemp2[7];
  G4double yOut3[7];
  G4double dydxTemp[7];
  h = hstep;
  for( i = 0; i < nVariables; i++ ) 
  {
    yTemp2[i] = yIn[i] + h*dydx[i] ;          
  }
  
  RightHandSide(yTemp2,dydxTemp);
  
  for( i = 0; i < nVariables; i++ ) 
  {
    yOut3[i] = yIn[i] + 0.5 * h * ( dydx[i] + dydxTemp[i] );
  }
  //G4cout << "euler ";
  //for (i = 0; i < nVariables; i++)
  //  {G4cout << yOut3[i] << " ";}
  //G4cout << G4endl;
  */
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

  //G4cout << G4endl;

  // TBC - we have not set DistChord here!
}
