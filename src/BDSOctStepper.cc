/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 25.12.2003
   Copyright (c) 2003 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: BDSOctStepper.cc,v 1.6 2007/05/10 16:23:22 malton Exp $
// GEANT4 tag $Name:  $
//
#include "BDSOctStepper.hh"
#include "G4ThreeVector.hh"
#include "G4LineSection.hh"
#include "G4TransportationManager.hh"

extern G4double BDSLocalRadiusOfCurvature;

BDSOctStepper::BDSOctStepper(G4Mag_EqRhs *EqRhs)
  : G4MagIntegratorStepper(EqRhs,6),  // integrate over 6 variables only !!
                                      // position & velocity
    itsBTrpPrime(0.0), itsDist(0.0)
{
  fPtrMagEqOfMot = EqRhs;
}


void BDSOctStepper::AdvanceHelix( const G4double  yIn[],
                                   G4ThreeVector,
				   G4double  h,
				   G4double  yOct[])
{
  const G4double *pIn = yIn+3;
  G4ThreeVector v0= G4ThreeVector( pIn[0], pIn[1], pIn[2]);  
  G4ThreeVector InitMomDir=v0.unit();

  G4ThreeVector GlobalPosition= G4ThreeVector( yIn[0], yIn[1], yIn[2]);  
  G4double InitMag=v0.mag();
  G4double kappa=  -fPtrMagEqOfMot->FCof()*itsBTrpPrime/InitMag;

  // relevant momentum scale is p_z, not P_tot:
  // check that the approximations are valid, else do a linear step:
  if(fabs(kappa)<1.e-20)
    {
      G4ThreeVector positionMove  = (h/InitMag) * v0;
      
      yOct[0]   = yIn[0] + positionMove.x(); 
      yOct[1]   = yIn[1] + positionMove.y(); 
      yOct[2]   = yIn[2] + positionMove.z(); 
				
      yOct[3] = v0.x();
      yOct[4] = v0.y();
      yOct[5] = v0.z();

      itsDist=0;
    }
  else 
    {      
      G4Navigator* OctNavigator=
	G4TransportationManager::GetTransportationManager()->
	GetNavigatorForTracking();

      G4AffineTransform LocalAffine=OctNavigator->GetLocalToGlobalTransform();


      // gab_dec03>>
      // position 
      //G4ThreeVector LocalR = OctNavigator->GetCurrentLocalCoordinate();
      // position derivative r' (normalised to unity)
      //G4ThreeVector LocalRp= (OctNavigator->ComputeLocalAxis(v0)).unit();

      G4AffineTransform GlobalAffine=OctNavigator->GetGlobalToLocalTransform();
      G4ThreeVector LocalR=GlobalAffine.TransformPoint(GlobalPosition); 
      G4ThreeVector LocalRp=GlobalAffine.TransformAxis(InitMomDir);
      // gab_dec03<<



      G4double x0=LocalR.x(); 
      G4double y0=LocalR.y();
      G4double z0=LocalR.z();

      //G4double x02My02=(x0*x0-y0*y0);

      G4double xp=LocalRp.x();
      G4double yp=LocalRp.y();
      G4double zp=LocalRp.z();

      G4double y3fac=y0*(y0*y0-3*x0*x0);
      G4double x3fac=x0*(x0*x0-3*y0*y0);
      
      // local r'' (for curvature)
      G4ThreeVector LocalRpp;
      // extra minus signs were because x,y_machine = - x_,-y_geant_world
      // New CVS version of BDSIM uses +x, +y in geant world
      /*
      LocalRpp.setX(zp*x3fac);
      LocalRpp.setY(zp*y3fac);
      LocalRpp.setZ(- xp*x3fac - yp*y3fac);
      */
      LocalRpp.setX(-zp*x3fac);
      LocalRpp.setY(-zp*y3fac);
      LocalRpp.setZ( xp*x3fac + yp*y3fac);

      LocalRpp*=kappa/6; // 6 is actually a 3! factor.;

      // determine effective curvature
      G4double R_1 = LocalRpp.mag();
      if(R_1>0.)
	{
	  // Save for Synchrotron Radiation calculations:
	  BDSLocalRadiusOfCurvature=1/R_1;

	  // chord distance (simple quadratic approx)
          G4double h2=h*h;
	  itsDist= h2*R_1/8;

	  G4double dx=LocalRp.x()*h + LocalRpp.x()*h2/2; 
	  G4double dy=LocalRp.y()*h + LocalRpp.y()*h2/2; 

	  G4double dz=sqrt(h2*(1.-h2*R_1*R_1/12)-dx*dx-dy*dy);
	  // check for precision problems
	  G4double ScaleFac=(dx*dx+dy*dy+dz*dz)/h2;
	  if(ScaleFac>1.0000001)
	    {
	      ScaleFac=sqrt(ScaleFac);
	      dx/=ScaleFac;
	      dy/=ScaleFac;
	      dz/=ScaleFac;
	    }

	  LocalR.setX(x0+dx);
	  LocalR.setY(y0+dy);
	  LocalR.setZ(z0+dz);

	  LocalRp = LocalRp+ h*LocalRpp;
	}
      else
	{LocalR += h*LocalRp;}
       
      GlobalPosition=LocalAffine.TransformPoint(LocalR); 
      G4ThreeVector GlobalTangent=LocalAffine.TransformAxis(LocalRp)*InitMag;
      
      yOct[0]   = GlobalPosition.x(); 
      yOct[1]   = GlobalPosition.y(); 
      yOct[2]   = GlobalPosition.z(); 
				
      yOct[3] = GlobalTangent.x();
      yOct[4] = GlobalTangent.y();
      yOct[5] = GlobalTangent.z();
    }
}

void BDSOctStepper::Stepper( const G4double yInput[],
			    const G4double[],
			    const G4double hstep,
			    G4double yOut[],
			    G4double yErr[]      )
{  
  G4int i;
  const G4int nvar = 6 ;
  
  //const G4double *pIn = yInput+3;
  //G4ThreeVector v0= G4ThreeVector( pIn[0], pIn[1], pIn[2]);  
  //G4double InitMag=v0.mag();
  //G4double kappa=  -fPtrMagEqOfMot->FCof()*itsBTrpPrime/InitMag;
  
  G4double yTemp[7], yIn[7];
  
  //  Saving yInput because yInput and yOut can be aliases for same array
  
  for(i=0;i<nvar;i++) yIn[i]=yInput[i];
  
  G4double h = hstep * 0.5; 
  
  // Do two half steps
  AdvanceHelix(yIn,   (G4ThreeVector)0,  h, yTemp);
  AdvanceHelix(yTemp, (G4ThreeVector)0, h, yOut); 
  
  // Do a full Step
  h = hstep ;
  AdvanceHelix(yIn, (G4ThreeVector)0, h, yTemp); 
  
  for(i=0;i<nvar;i++) yErr[i] = yOut[i] - yTemp[i] ;

  return ;
}

G4double BDSOctStepper::DistChord()   const 
{
  return itsDist;
}

BDSOctStepper::~BDSOctStepper()
{}
