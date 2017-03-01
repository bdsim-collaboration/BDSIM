#include "BDSDebug.hh"
#include "BDSIntegratorDecapole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AffineTransform.hh"
#include "G4Mag_EqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>


BDSIntegratorDecapole::BDSIntegratorDecapole(BDSMagnetStrength const* strength,
					     G4double                 brho,
					     G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorMag(eqOfMIn, 6),
  yInitial(0), yMidPoint(0), yFinal(0)
{
  // B'''' = d^4By/dx^4 = Brho * (1/Brho d^4By/dx^4) = Brho * k4
  bQuadruplePrime = brho * (*strength)["k4"] / (CLHEP::m3*CLHEP::m2);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B'''' = " << bQuadruplePrime << G4endl;
#endif
}

void BDSIntegratorDecapole::AdvanceHelix(const G4double  yIn[],
					 G4double        h,
					 G4double        yOut[])
{
  const G4double *pIn = yIn+3;
  G4ThreeVector v0 = G4ThreeVector( pIn[0], pIn[1], pIn[2]);  

  G4ThreeVector GlobalPosition = G4ThreeVector(yIn[0], yIn[1], yIn[2]);  
  G4double InitMag = v0.mag();
  G4double kappa   = -eqOfM->FCof()*bQuadruplePrime/InitMag;

  // relevant momentum scale is p_z, not P_tot:
  // check that the approximations are valid, else do a linear step:
  if(fabs(kappa)<1.e-20)
    {
      AdvanceDriftMag(yIn, h, yOut);
      SetDistChord(0);
    }
  else 
    {
      // global to local
      BDSStep   localPosMom = ConvertToLocal(GlobalPosition, v0, h, false);
      G4ThreeVector LocalR  = localPosMom.PreStepPoint();
      G4ThreeVector Localv0 = localPosMom.PostStepPoint();
      G4ThreeVector LocalRp = Localv0.unit();

      G4double x0=LocalR.x(); 
      G4double y0=LocalR.y();
    
      G4double x02My02=(x0*x0-y0*y0);

      G4double xp=LocalRp.x();
      G4double yp=LocalRp.y();
      G4double zp=LocalRp.z();

      G4double Bx=4.0*x0*y0*(x02My02);
      G4double By=std::pow(x0,4)-6.0*x0*x0*y0*y0+std::pow(y0,4);

      // local r'' (for curvature)
      G4ThreeVector LocalRpp;
      // extra minus signs because x,y_machine = - x_,-y_geant_world
      LocalRpp.setX( zp*By);
      LocalRpp.setY(-zp*Bx);
      LocalRpp.setZ( xp*By - yp*Bx);
      
      LocalRpp*=kappa/24; // 24 is actually a 4! factor.;

      AdvanceChord(h,LocalR,LocalRp,LocalRpp);

      ConvertToGlobal(LocalR,LocalRp,InitMag,yOut);
    }
}

void BDSIntegratorDecapole::Stepper(const G4double yIn[],
				    const G4double[] /*dydx*/,
				    const G4double h,
				    G4double       yOut[],
				    G4double       yErr[])
{    
  for(G4int i = 0; i < nVariables; i++)
    {yErr[i]=0;} // this is clearly wrong!
  AdvanceHelix(yIn, h, yOut);
  
  // TBC - we have not set DistChord here!

  // TBC - no use of backup stepper here
}
