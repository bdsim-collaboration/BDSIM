#include "BDSDebug.hh"
#include "BDSIntegratorSolenoid.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"

#include "globals.hh" // geant4 types / globals.hh
#include "G4ClassicalRK4.hh"
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"


BDSIntegratorSolenoid::BDSIntegratorSolenoid(BDSMagnetStrength const* strength,
					     G4double                 brho,
					     G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorBase(eqOfMIn, 6)
{
  bField = brho * (*strength)["ks"];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B (local) = " << bField << G4endl;
#endif
}

void BDSIntegratorSolenoid::AdvanceHelix(const G4double yIn[],
					 const G4double dydx[],
					 G4ThreeVector  /*bField*/,
					 G4double       h,
					 G4double       yOut[],
					 G4double       yErr[])
{
  const G4double *pIn      = yIn+3;
  G4ThreeVector GlobalR    = G4ThreeVector( yIn[0], yIn[1], yIn[2]);
  G4ThreeVector GlobalP    = G4ThreeVector( pIn[0], pIn[1], pIn[2]);
  G4ThreeVector InitMomDir = GlobalP.unit();
  G4double      InitPMag   = GlobalP.mag();
  G4double      kappa      = - 0.5*eqOfM->FCof()*bField/InitPMag;
  G4double      h2         = h*h;
  
#ifdef BDSDEBUG
  G4double charge = (eqOfM->FCof())/CLHEP::c_light;
  G4cout << "BDSIntegratorSolenoid: step = " << h/CLHEP::m << " m" << G4endl
         << " x  = " << yIn[0]/CLHEP::m   << " m"     << G4endl
         << " y  = " << yIn[1]/CLHEP::m   << " m"     << G4endl
         << " z  = " << yIn[2]/CLHEP::m   << " m"     << G4endl
         << " px = " << yIn[3]/CLHEP::GeV << " GeV/c" << G4endl
         << " py = " << yIn[4]/CLHEP::GeV << " GeV/c" << G4endl
         << " pz = " << yIn[5]/CLHEP::GeV << " GeV/c" << G4endl
         << " q  = " << charge/CLHEP::eplus << "e" << G4endl
         << " Bz = " << bField/(CLHEP::tesla/CLHEP::m) << " T" << G4endl
         << " k  = " << kappa/(1./CLHEP::m2) << " m^-2" << G4endl
         << G4endl; 
#endif
  BDSStep   localPosMom = ConvertToLocal(GlobalR, GlobalP, h, false);
  G4ThreeVector LocalR  = localPosMom.PreStepPoint();
  G4ThreeVector Localv0 = localPosMom.PostStepPoint();
  G4ThreeVector LocalRp = Localv0.unit();
  
  G4double x1,xp1,y1,yp1,z1,zp1; //output coordinates to be
  
  if (fabs(kappa)<1e-12)
    {
      // very low strength - treat as a drift
      G4ThreeVector positionMove = h * InitMomDir;

      yOut[0] = yIn[0] + positionMove.x(); 
      yOut[1] = yIn[1] + positionMove.y(); 
      yOut[2] = yIn[2] + positionMove.z(); 
				
      yOut[3] = GlobalP.x();
      yOut[4] = GlobalP.y();
      yOut[5] = GlobalP.z();
      
      yErr[0] = 0; // 0 error as a drift
      yErr[1] = 0; // must set here as we return after this
      yErr[2] = 0;
      yErr[3] = 0;
      yErr[4] = 0;
      yErr[5] = 0;

      distChord=0;
      return;
    }
  
  // finite strength - treat as a solenoid
  G4double x0  = LocalR.x();
  G4double y0  = LocalR.y();
  G4double z0  = LocalR.z();
  G4double xp0 = LocalRp.x();
  G4double yp0 = LocalRp.y();
  G4double zp0 = LocalRp.z();
  
  // local r'' (for curvature)
  G4ThreeVector LocalRpp;
  LocalRpp.setX(-zp0*x0);
  LocalRpp.setY( zp0*y0);
  LocalRpp.setZ( x0*xp0 - y0*yp0);
  LocalRpp *= kappa;
  
  // determine effective curvature 
  G4double R_1 = LocalRpp.mag();
#ifdef BDSDEBUG 
  G4cout << " curvature= " << R_1*CLHEP::m << "m^-1" << G4endl;
#endif

  if (R_1<1e-15)
    {
      // very large radius of curvature - treat as drift
      G4ThreeVector positionMove = h * InitMomDir;
      
      yOut[0] = yIn[0] + positionMove.x(); 
      yOut[1] = yIn[1] + positionMove.y(); 
      yOut[2] = yIn[2] + positionMove.z(); 
      
      yOut[3] = GlobalP.x();
      yOut[4] = GlobalP.y();
      yOut[5] = GlobalP.z();

      yErr[0] = 0; // 0 error as a drift
      yErr[1] = 0; // must set here as we return after this
      yErr[2] = 0;
      yErr[3] = 0;
      yErr[4] = 0;
      yErr[5] = 0;

      distChord=0;
      return;
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " using thick lens matrix " << G4endl;
#endif
  
  // Save for Synchrotron Radiation calculations
  G4double R=1./R_1;
  
  // chord distance (simple quadratic approx)
  distChord= h2/(8*R);

  // check for paraxial approximation:
  if(fabs(zp0)>0.9)
    {
      // RMatrix - from Andy Wolszki's Linear Dynamics lectures (#5, slide 43)
      // http://pcwww.liv.ac.uk/~awolski/main_teaching_Cockcroft_LinearDynamics.htm
      // note this is actually for one step through the whole solenoid as focussing
      // comes from edge effects - may need to reconsider this in the future...
      // maximum step size is set to full length in BDSSolenoid.cc
      // ( cos^2 (wL)     , (1/2w)sin(2wL)  , (1/2)sin(2wL)  , (1/w)sin^2(wL) ) (x )
      // ( (w/2)sin(2wL)  , cos^2(wL)       ,  -w sin^2(wL)  , (1/2)sin(2wL)  ) (x')
      // ( -(1/2)sin(2wL) , (-1/w)sin^2(wL) , cos^2(wL)      , (1/2w)sin(2wL) ) (y )
      // ( w sin^2(wL)    , (-1/2)sin(2wL)  , (-w/2)sin(2wL) , cos^2(wL)      ) (y')
      
      G4double w       = kappa;
      //need the length along curvilinear s -> project h on z
      G4ThreeVector positionMove = h * InitMomDir; 
      G4double dz      = positionMove.z();
      G4double wL      = kappa*dz; 
      G4double cosOL   = cos(wL); // w is really omega - so use 'O' to describe - OL = omega*L
      G4double cosSqOL = cosOL*cosOL;
      G4double sinOL   = sin(wL);
      G4double sin2OL  = sin(2.0*wL);
      G4double sinSqOL = sinOL*sinOL;
      
      // calculate thick lens transfer matrix
      x1  = x0*cosSqOL + (0.5*xp0/w)*sin2OL + (0.5*y0)*sin2OL + (yp0/w)*sinSqOL;
      xp1 = (0.5*x0*w)*sin2OL + xp0*cosSqOL - (w*y0)*sinSqOL + (0.5*yp0)*sin2OL;
      y1  = (-0.5*x0)*sin2OL - (xp0/w)*sinSqOL + y0*cosSqOL + (0.5*yp0/w)*sin2OL;
      yp1 = x0*w*sinSqOL - (0.5*xp0)*sin2OL - (0.5*w*y0)*sin2OL + yp0*cosSqOL;  
      
      // ensure normalisation for vector
      zp1 = sqrt(1 - xp1*xp1 -yp1*yp1);
      
      // calculate deltas to existing coords
      G4double dx = x1-x0;
      G4double dy = y1-y0;
      
      // check for precision problems
      G4double ScaleFac = (dx*dx+dy*dy+dz*dz)/h2;
#ifdef BDSDEBUG
      G4cout << "Ratio of calculated to proposed step length: " << ScaleFac << G4endl;
#endif
      if(ScaleFac>1.0000001)
	{
#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << " normalising to conserve step length" << G4endl;
#endif
	  ScaleFac = sqrt(ScaleFac);
	  dx /= ScaleFac;
	  dy /= ScaleFac;
	  dz /= ScaleFac;
	  x1 =  x0+dx;
	  y1 =  y0+dy;
	}
      z1 = z0+dz;

      //write the final coordinates
      LocalR.setX(x1);
      LocalR.setY(y1);
      LocalR.setZ(z1);
      LocalRp.setX(xp1);
      LocalRp.setY(yp1);
      LocalRp.setZ(zp1);
      
#ifdef BDSDEBUG 
      G4cout << "BDSIntegratorSolenoid: final point in local coordinates:" << G4endl
	     << " x  = " << LocalR[0]/CLHEP::m << " m" << G4endl
	     << " y  = " << LocalR[1]/CLHEP::m << " m" << G4endl
	     << " z  = " << LocalR[2]/CLHEP::m << " m" << G4endl
	     << " x' = " << LocalRp[0]         << G4endl
	     << " y' = " << LocalRp[1]         << G4endl
	     << " z' = " << LocalRp[2]         << G4endl
	     << G4endl; 
#endif
      
      BDSStep globalPosDir = ConvertToGlobalStep(LocalR, LocalRp, false);
      GlobalR = globalPosDir.PreStepPoint();
      GlobalP = globalPosDir.PostStepPoint();	
      GlobalP*=InitPMag; // multiply the unit direction by magnitude to get momentum
            
      yOut[0] = GlobalR.x(); 
      yOut[1] = GlobalR.y(); 
      yOut[2] = GlobalR.z(); 
      
      yOut[3] = GlobalP.x();
      yOut[4] = GlobalP.y();
      yOut[5] = GlobalP.z();

      for(G4int i = 0; i < nVariables; i++)
	{yErr[i]=0;} //set error to be zero - not strictly correct
    }
  else
    // perform local helical steps (paraxial approx not safe)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " local helical steps - using G4ClassicalRK4" << G4endl;
#endif
      // use a classical Runge Kutta stepper here
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
    }  
}


void BDSIntegratorSolenoid::Stepper(const G4double yInput[],
				    const G4double dydx[],
				    const G4double hstep,
				    G4double yOut[],
				    G4double yErr[])
{
  //simply perform one step here
  AdvanceHelix(yInput,dydx,(G4ThreeVector)0,hstep,yOut,yErr);
}
