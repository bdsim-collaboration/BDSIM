/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#include <limits>

#include "BDSGlobalConstants.hh" 
#include "BDSDipoleStepper.hh"
#include "G4ThreeVector.hh"
#include "G4TransportationManager.hh"

extern G4double BDSLocalRadiusOfCurvature;

BDSDipoleStepper::BDSDipoleStepper(G4Mag_EqRhs *EqRhs)
  : G4MagIntegratorStepper(EqRhs,6),  // integrate over 6 variables only !!
                                      // position & velocity
    itsLength(0.0),itsAngle(0.0),itsBGrad(0.0),itsBField(0.0),itsDist(0.0)
{
  fPtrMagEqOfMot = EqRhs;
}


void BDSDipoleStepper::AdvanceHelix( const G4double  yIn[],
				  G4ThreeVector,
				  G4double  h,
				  G4double  yOut[])
{
#ifdef BDSDEBUG
  G4double charge = (fPtrMagEqOfMot->FCof())/CLHEP::c_light;
  G4cout << "BDSDipoleStepper: step= " << h/CLHEP::m << " m" << G4endl
         << " x  = " << yIn[0]/CLHEP::m     << " m" << G4endl
         << " y  = " << yIn[1]/CLHEP::m     << " m" << G4endl
         << " z  = " << yIn[2]/CLHEP::m     << " m" << G4endl
         << " px = " << yIn[3]/CLHEP::GeV   << " GeV/c" << G4endl
         << " py = " << yIn[4]/CLHEP::GeV   << " GeV/c" << G4endl
         << " pz = " << yIn[5]/CLHEP::GeV   << " GeV/c" << G4endl
         << " q  = " << charge/CLHEP::eplus << " e" << G4endl
	 << " B  = " << itsBField/(CLHEP::tesla) << " T" << G4endl
    //         << " k= " << kappa/(1./CLHEP::m2) << "m^-2" << G4endl
         << G4endl; 
#endif

  const G4double *pIn = yIn+3;
  G4ThreeVector v0    = G4ThreeVector( pIn[0], pIn[1], pIn[2]);  

  G4ThreeVector GlobalPosition = G4ThreeVector( yIn[0], yIn[1], yIn[2]);  
  G4double      InitMag        = v0.mag();
  G4ThreeVector InitMomDir     = v0.unit();

  // in case of zero field (though what if there is a quadrupole part..)
  // or neutral particles do a linear step:
  if(itsBField==0 || fPtrMagEqOfMot->FCof()==0) {
    G4ThreeVector positionMove = h * InitMomDir;

    yOut[0] = yIn[0] + positionMove.x();
    yOut[1] = yIn[1] + positionMove.y();
    yOut[2] = yIn[2] + positionMove.z();
    
    yOut[3] = v0.x();
    yOut[4] = v0.y();
    yOut[5] = v0.z();

    // Set to infinite radius for Synchrotron Radiation calculation:
    //    BDSLocalRadiusOfCurvature=DBL_MAX;

    itsDist=0;
    return;
  }

  //G4double h2=h*h;

  G4Navigator* HelixNavigator=
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();
  
  G4AffineTransform LocalAffine=HelixNavigator-> 
    GetLocalToGlobalTransform();
  
  G4AffineTransform GlobalAffine=HelixNavigator->
    GetGlobalToLocalTransform();

  G4ThreeVector LocalR=GlobalAffine.TransformPoint(GlobalPosition); 
  G4ThreeVector LocalRp=GlobalAffine.TransformAxis(InitMomDir);

  G4ThreeVector itsInitialR=LocalR;
  G4ThreeVector itsInitialRp=LocalRp;
  
  // advance the orbit
 
  G4ThreeVector itsFinalPoint,itsFinalDir;
  G4ThreeVector yhat(0.,1.,0.);
  G4ThreeVector vhat  = LocalRp;
  G4ThreeVector vnorm = vhat.cross(yhat);
  
  // radius of curvature
  G4double R=-(InitMag/CLHEP::GeV)/(0.299792458 * itsBField/CLHEP::tesla) * CLHEP::m;

  // include the sign of the charge of the particles
  if(  fPtrMagEqOfMot->FCof()<0) R*=-1.;
  
  G4double Theta   = h/R;

  G4double CosT_ov_2, SinT_ov_2, CosT, SinT;
  CosT_ov_2=cos(Theta/2);
  SinT_ov_2=sin(Theta/2);
  
  CosT=(CosT_ov_2*CosT_ov_2)- (SinT_ov_2*SinT_ov_2);
  SinT=2*CosT_ov_2*SinT_ov_2;
  
  // Save for Synchrotron Radiation calculations:
  BDSLocalRadiusOfCurvature=R;
  
  itsDist=fabs(R)*(1.-CosT_ov_2);
  
  G4ThreeVector dPos=R*(SinT*vhat + (1-CosT)*vnorm);
  
  itsFinalPoint=LocalR+dPos;
  itsFinalDir=CosT*vhat +SinT*vnorm;
  
  G4ThreeVector GlobalTangent;

  GlobalPosition=LocalAffine.TransformPoint(itsFinalPoint); 
  GlobalTangent=LocalAffine.TransformAxis(itsFinalDir);

  GlobalTangent*=InitMag;

  yOut[0]   = GlobalPosition.x(); 
  yOut[1]   = GlobalPosition.y(); 
  yOut[2]   = GlobalPosition.z(); 
  
  yOut[3] = GlobalTangent.x();
  yOut[4] = GlobalTangent.y();
  yOut[5] = GlobalTangent.z();

  // gradient for quadrupolar field
  G4double kappa= - fPtrMagEqOfMot->FCof()* ( itsBGrad) /InitMag; // was ist das? 
  if(fabs(kappa)<std::numeric_limits<double>::epsilon()) return; // no gradient 

  G4double x1,x1p,y1,y1p,z1p;
  //G4double z1;
  
  G4double NomEnergy = BDSGlobalConstants::Instance()->GetBeamTotalEnergy();
  G4double NomR = -(NomEnergy/CLHEP::GeV)/(0.299792458 * itsBField/CLHEP::tesla) * CLHEP::m;

  G4double NominalPath = sqrt(NomR*NomR - LocalR.z()*LocalR.z()) - fabs(NomR)*cos(itsAngle/2);
  
  G4double EndNomPath = sqrt(NomR*NomR - itsFinalPoint.z()*itsFinalPoint.z()) - fabs(NomR)*cos(itsAngle/2);

  if(R<0)
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
  // else // should not happen as already returned in that case
  //   {
  //     X11 = 1;
  //     X12 = 0;
  //     X21 = 0;
  //     X22 = 1;
  //     Y11 = 1;
  //     Y12 = 0;
  //     Y21 = 0;
  //     Y22 = 1;
  //   }

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
  
  GlobalPosition=LocalAffine.TransformPoint(LocalR); 
  
  LocalRp.rotateY(-h/R);
  GlobalTangent=LocalAffine.TransformAxis(LocalRp);


  GlobalTangent*=InitMag;
  
  // gab: replace += with =
  yOut[0] = GlobalPosition.x(); 
  yOut[1] = GlobalPosition.y(); 
  yOut[2] = GlobalPosition.z(); 
  
  yOut[3] = GlobalTangent.x();
  yOut[4] = GlobalTangent.y();
  yOut[5] = GlobalTangent.z();

}


void BDSDipoleStepper::Stepper( const G4double yInput[],
			     const G4double[],
			     const G4double hstep,
			     G4double yOut[],
			     G4double yErr[]      )
{  
  const G4int nvar = 6 ;

  for(G4int i=0;i<nvar;i++) yErr[i]=0;

  AdvanceHelix(yInput,(G4ThreeVector)0,hstep,yOut);

}

G4double BDSDipoleStepper::DistChord()   const 
{
  return itsDist;
  // This is a class method that gives distance of Mid 
  // from the Chord between the Initial and Final points.
}

BDSDipoleStepper::~BDSDipoleStepper()
{}
