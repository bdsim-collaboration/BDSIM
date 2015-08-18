#include "BDSDebug.hh"
#include "BDSSextStepper.hh"
#include "BDSStepperBase.hh"

#include "G4ThreeVector.hh"

extern G4double BDSLocalRadiusOfCurvature;

BDSSextStepper::BDSSextStepper(G4Mag_EqRhs* eqRHS):
  BDSStepperBase(eqRHS, 6),
  fPtrMagEqOfMot(eqRHS),
  itsBDblPrime(0.0),
  itsDist(0.0)
{;}

void BDSSextStepper::AdvanceHelix( const G4double  yIn[],
                                   G4ThreeVector,
				   G4double  h,
				   G4double  ySext[])
{

  const G4double *pIn = yIn+3;
  G4ThreeVector v0= G4ThreeVector( pIn[0], pIn[1], pIn[2]);  
  G4ThreeVector InitMomDir=v0.unit();

  G4ThreeVector GlobalPosition= G4ThreeVector( yIn[0], yIn[1], yIn[2]);  
  G4double InitMag=v0.mag();
  G4double kappa=  (-fPtrMagEqOfMot->FCof()*itsBDblPrime) /InitMag;

#ifdef BDSDEBUG  
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__ << "kappa                 : " << kappa << G4endl;
  G4cout << __METHOD_NAME__ << "InitMag               : " << InitMag << G4endl;
  G4cout << __METHOD_NAME__ << "fPtrMagEqOfMot::FCof(): " << fPtrMagEqOfMot->FCof() << G4endl;
  G4cout << __METHOD_NAME__ << "g''                   : " << itsBDblPrime<< G4endl;
  G4cout << __METHOD_NAME__ << "fPtrMagEqOfMot->FCof(): " << fPtrMagEqOfMot->FCof() << G4endl;
  G4cout << __METHOD_NAME__ << "h                     : " << h << G4endl;

  G4double charge = (fPtrMagEqOfMot->FCof())/CLHEP::c_light;

  G4String VolName = auxNavigator->LocateGlobalPointAndSetup(GlobalPosition)->GetName();

  G4cout << "BDSSextStepper: " << VolName << G4endl
	 << " step= " << h/CLHEP::m << " m" << G4endl
         << " x= " << yIn[0]/CLHEP::m << "m" << G4endl
         << " y= " << yIn[1]/CLHEP::m << "m" << G4endl
         << " z= " << yIn[2]/CLHEP::m << "m" << G4endl
         << " px= " << yIn[3]/CLHEP::GeV << "GeV/c" << G4endl
         << " py= " << yIn[4]/CLHEP::GeV << "GeV/c" << G4endl
         << " pz= " << yIn[5]/CLHEP::GeV << "GeV/c" << G4endl
         << " q= " << charge/CLHEP::eplus << "e" << G4endl
         << " dBy/dx= " << itsBDblPrime/(CLHEP::tesla/CLHEP::m/CLHEP::m) << "T/m^2" << G4endl
         << " k= " << kappa/(1./CLHEP::m2) << "m^-2" << G4endl
         << G4endl 
         << G4endl;
#endif 

   if(fabs(kappa)<1.e-12)
     {
       G4ThreeVector positionMove  = (h/InitMag) * v0;
       
       ySext[0]   = yIn[0] + positionMove.x(); 
       ySext[1]   = yIn[1] + positionMove.y(); 
       ySext[2]   = yIn[2] + positionMove.z(); 
				
       ySext[3] = v0.x();
       ySext[4] = v0.y();
       ySext[5] = v0.z();

       itsDist=0;
     }
   else 
     {       
       G4AffineTransform LocalAffine  = auxNavigator->GetLocalToGlobalTransform();
       G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform();
       
       G4ThreeVector LocalR=GlobalAffine.TransformPoint(GlobalPosition); 
       G4ThreeVector LocalRp=GlobalAffine.TransformAxis(InitMomDir);
       
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
       LocalRpp.setX(- zp*x02My02);
       LocalRpp.setY(2*zp*x0*y0);
       LocalRpp.setZ(xp*x02My02-2*yp*x0*y0);

       //G4cout << "LocalRpp: " <<LocalRpp<< G4endl;

       LocalRpp*=kappa/2; // 2 is actually a 2! factor.
       // determine effective curvature
       G4double R_1 = LocalRpp.mag();


       if(R_1>0.)
	 {    
	   G4double h2=h*h;
	   // chord distance (simple quadratic approx)
	   itsDist= h2*R_1/8;

	   // Save for Synchrotron Radiation calculations:
	   BDSLocalRadiusOfCurvature=1./R_1;
	   
           G4double dx=LocalRp.x()*h + LocalRpp.x()*h2 /2.; 
	   G4double dy=LocalRp.y()*h + LocalRpp.y()*h2 /2.;

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
	   
           LocalR.setX(LocalR.x()+dx);
           LocalR.setY(LocalR.y()+dy);
           LocalR.setZ(LocalR.z()+dz);
	  
	   LocalRp = LocalRp+ h*LocalRpp;

	 }
       else
	 {LocalR += h*LocalRp;}
       
       GlobalPosition=LocalAffine.TransformPoint(LocalR); 
       G4ThreeVector GlobalTangent=LocalAffine.TransformAxis(LocalRp)*InitMag;
       
       ySext[0]   = GlobalPosition.x(); 
       ySext[1]   = GlobalPosition.y(); 
       ySext[2]   = GlobalPosition.z(); 
				
       ySext[3] = GlobalTangent.x();
       ySext[4] = GlobalTangent.y();
       ySext[5] = GlobalTangent.z();
     }
}

void BDSSextStepper::Stepper( const G4double yInput[],
			     const G4double[],
			     const G4double hstep,
			     G4double yOut[],
			     G4double yErr[]      )
{  
  const G4int nvar = 6 ;
  G4int i;
 
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
  
  for(i=0;i<nvar;i++) {
    yErr[i] = yOut[i] - yTemp[i] ;
    // if error small, set error to 0
    // this is done to prevent Geant4 going to smaller and smaller steps
    // ideally use some of the global constants instead of hardcoding here
    // could look at step size as well instead.
    if (std::abs(yErr[i]) < 1e-7) yErr[i] = 0;
  }
}

G4double BDSSextStepper::DistChord()   const 
{
  return itsDist;
}

BDSSextStepper::~BDSSextStepper()
{}
