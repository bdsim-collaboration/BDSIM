/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 18.10.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#include "BDSGlobalConstants.hh" 
//
#include "BDSHelixStepper.hh"
#include "G4ThreeVector.hh"
#include "G4LineSection.hh"
#include "G4TransportationManager.hh"

extern G4double BDSLocalRadiusOfCurvature;

BDSHelixStepper::BDSHelixStepper(G4Mag_EqRhs *EqRhs)
  : G4MagIntegratorStepper(EqRhs,6),  // integrate over 6 variables only !!
    itsBField(0.0), itsDist(0.0)
{
  its_EqRhs = EqRhs;
}


void BDSHelixStepper::AdvanceHelix( const G4double  yIn[],
				  G4double  h,
				  G4double  yHelix[])
{
  G4ThreeVector positionMove, endTangent;

  const G4double *pIn = yIn+3;
 
  G4ThreeVector v0= G4ThreeVector( pIn[0], pIn[1], pIn[2]);  
  
  G4ThreeVector GlobalPosition= G4ThreeVector( yIn[0], yIn[1], yIn[2]);  

  G4double InitMag=v0.mag();

  G4ThreeVector LocalR,LocalRp ;

  //G4Navigator* HelixNavigator=
  //  G4TransportationManager::GetTransportationManager()->
  //  GetNavigatorForTracking();


  LocalR=GlobalPosition;
  LocalRp=v0.unit();

  G4ThreeVector itsFinalPoint,itsFinalDir;
  
  G4ThreeVector yhat(0.,1.,0.);

  G4ThreeVector vhat=LocalRp;
  
  G4ThreeVector vnorm=vhat.cross(yhat);
   
  G4double R;

  if(BDSGlobalConstants::Instance()->GetSynchRescale())
    {
      G4double B[3];
      its_EqRhs->GetFieldValue(yIn, B);
      R=-(InitMag/CLHEP::GeV)/(0.299792458 * B[1]/CLHEP::tesla) *CLHEP::m;
    }
  else
    {
      R=-(InitMag/CLHEP::GeV)/(0.299792458 * itsBField/CLHEP::tesla) * CLHEP::m;
    }

 // include the sign of the charge of the particles

  if( its_EqRhs->FCof()<0) R*=-1.;
  else if (its_EqRhs->FCof()==0) R=DBL_MAX;

  // check that the approximations are valid, else do a linear step:
  if(fabs(R)<DBL_MAX) 
    { 
  
      G4double Theta   = h/R;

      G4double CosT_ov_2, SinT_ov_2, CosT, SinT;
      CosT_ov_2=cos(Theta/2);
      SinT_ov_2=sin(Theta/2);

      CosT=(CosT_ov_2*CosT_ov_2)- (SinT_ov_2*SinT_ov_2);
      SinT=2*CosT_ov_2*SinT_ov_2;

      BDSLocalRadiusOfCurvature=R;

      itsDist=fabs(R)*(1.-CosT_ov_2);

      G4ThreeVector dPos=R*(SinT*vhat + (1-CosT)*vnorm);
 	
      itsFinalPoint=LocalR+dPos;
      itsFinalDir=CosT*vhat +SinT*vnorm;


    }
  else
    {
      itsFinalPoint=LocalR + h * LocalRp;
      itsFinalDir=LocalRp;
      itsDist=0.;
    }


  G4ThreeVector GlobalTangent;
  GlobalPosition=itsFinalPoint;
  GlobalTangent=itsFinalDir;
  GlobalTangent*=InitMag;

  yHelix[0] = GlobalPosition.x(); 
  yHelix[1] = GlobalPosition.y(); 
  yHelix[2] = GlobalPosition.z(); 
				
  yHelix[3] = GlobalTangent.x();
  yHelix[4] = GlobalTangent.y();
  yHelix[5] = GlobalTangent.z();
}    


void BDSHelixStepper::Stepper( const G4double yInput[],
			     const G4double[],
			     const G4double hstep,
			     G4double yOut[],
			     G4double yErr[]      )
{  
  const G4int nvar = 6 ;

  for(G4int i=0;i<nvar;i++) yErr[i]=0;
  AdvanceHelix(yInput,hstep,yOut);
}

G4double BDSHelixStepper::DistChord()   const 
{

  return itsDist;
  // This is a class method that gives distance of Mid 
  //  from the Chord between the Initial and Final points.
}

BDSHelixStepper::~BDSHelixStepper()
{}
