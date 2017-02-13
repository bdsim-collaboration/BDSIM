#include "BDSIntegratorBase.hh"

#include "BDSStep.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ClassicalRK4.hh"

BDSIntegratorBase::BDSIntegratorBase(G4Mag_EqRhs* eqOfMIn,
				     G4int        nVariablesIn):
  G4MagIntegratorStepper(eqOfMIn, nVariablesIn),
  eqOfM(eqOfMIn),
  nVariables(6),
  distChordPrivate(0)
{
  backupStepper = new G4ClassicalRK4(eqOfM, 6);
}
  
BDSIntegratorBase::~BDSIntegratorBase()
{
  delete backupStepper;
}

void BDSIntegratorBase::AdvanceDrift(const G4double yIn[],
				     const G4ThreeVector& GlobalP,
				     const G4double h,
				     G4double yOut[])
{
  G4ThreeVector InitMomDir = GlobalP.unit();
  G4ThreeVector positionMove = h * InitMomDir;
  
  yOut[0] = yIn[0] + positionMove.x();
  yOut[1] = yIn[1] + positionMove.y();
  yOut[2] = yIn[2] + positionMove.z();

  yOut[3] = GlobalP.x();
  yOut[4] = GlobalP.y();
  yOut[5] = GlobalP.z();

  SetDistChord(0);
}

void BDSIntegratorBase::AdvanceChord(const G4double h,
				     G4ThreeVector& LocalR,
				     G4ThreeVector& LocalRp,
				     const G4ThreeVector& LocalRpp)
{
  // determine effective curvature
  G4double R_1 = LocalRpp.mag();
  if(R_1>0.)
    {
      // chord distance (simple quadratic approx)
      G4double h2 = h*h;
      G4double dc = h2*R_1/8;
      SetDistChord(dc);
      
      G4double dx = LocalRp.x()*h + LocalRpp.x()*h2/2;
      G4double dy = LocalRp.y()*h + LocalRpp.y()*h2/2;
      G4double dz = sqrt(h2*(1.-h2*R_1*R_1/12)-dx*dx-dy*dy);
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
      
      LocalRp = LocalRp + h*LocalRpp;
    }
  else
    {LocalR += h*LocalRp;}
}

void BDSIntegratorBase::ConvertToGlobal(const G4ThreeVector& LocalR,
					const G4ThreeVector& LocalRp,
					const G4double InitMag,
					G4double yOut[])
{
  BDSStep globalPosDir = ConvertToGlobalStep(LocalR, LocalRp, false);
  G4ThreeVector GlobalPosition = globalPosDir.PreStepPoint();
  G4ThreeVector GlobalTangent  = globalPosDir.PostStepPoint();	
  GlobalTangent*=InitMag; // multiply the unit direction by magnitude to get momentum

  yOut[0] = GlobalPosition.x();
  yOut[1] = GlobalPosition.y();
  yOut[2] = GlobalPosition.z();

  yOut[3] = GlobalTangent.x();
  yOut[4] = GlobalTangent.y();
  yOut[5] = GlobalTangent.z();
}
