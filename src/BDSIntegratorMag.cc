#include "BDSIntegratorMag.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ClassicalRK4.hh"

BDSIntegratorMag::BDSIntegratorMag(G4Mag_EqRhs* eqOfMIn,
				   G4int        nVariablesIn):
  G4MagIntegratorStepper(eqOfMIn, nVariablesIn),
  eqOfM(eqOfMIn),
  nVariables(nVariablesIn),
  distChordPrivate(0)
{
  backupStepper = new G4ClassicalRK4(eqOfMIn, nVariablesIn);
}
  
BDSIntegratorMag::~BDSIntegratorMag()
{
  delete backupStepper;
}

void BDSIntegratorMag::AdvanceChord(const G4double       h,
				    G4ThreeVector&       localPos,
				    G4ThreeVector&       localMom,
				    const G4ThreeVector& localA)
{
  // determine effective curvature
  G4double localAMag = localA.mag();
  if(BDS::IsFinite(localAMag))
    {
      // chord distance (simple quadratic approx)
      G4double h2 = h*h;
      G4double dc = h2*localAMag/8;
      SetDistChord(dc);
      
      G4double dx = localMom.x()*h + localA.x()*h2/2;
      G4double dy = localMom.y()*h + localA.y()*h2/2;

      // TBC - this can go negative for very long step queries that
      // presumably cause a very large deflection. This results in nan
      // and bad tracking from Geant4 / a crash.
      G4double dz = std::sqrt(h2*(1.-h2*localAMag*localAMag/12)-dx*dx-dy*dy);
      // check for precision problems
      G4double ScaleFac=(dx*dx+dy*dy+dz*dz)/h2;
      if(ScaleFac>1.0000001)
	{
	  ScaleFac=std::sqrt(ScaleFac);
	  dx/=ScaleFac;
	  dy/=ScaleFac;
	  dz/=ScaleFac;
	}
      
      localPos.setX(localPos.x()+dx);
      localPos.setY(localPos.y()+dy);
      localPos.setZ(localPos.z()+dz);
      
      localMom = localMom + h*localA;
    }
  else
    {localPos += h*localMom;}
}

void BDSIntegratorMag::ConvertToGlobal(const G4ThreeVector& localPos,
				       const G4ThreeVector& localMomUnit,
				       const G4double       momMag,
				       G4double             yOut[])
{
  BDSStep globalPosDir = ConvertToGlobalStep(localPos, localMomUnit, false);
  G4ThreeVector GlobalPosition = globalPosDir.PreStepPoint();
  G4ThreeVector GlobalTangent  = globalPosDir.PostStepPoint();	
  GlobalTangent*=momMag; // multiply the unit direction by magnitude to get momentum

  yOut[0] = GlobalPosition.x();
  yOut[1] = GlobalPosition.y();
  yOut[2] = GlobalPosition.z();

  yOut[3] = GlobalTangent.x();
  yOut[4] = GlobalTangent.y();
  yOut[5] = GlobalTangent.z();
}
