#include "BDSIntegratorEulerOld.hh"

#include "globals.hh"
#include "G4AffineTransform.hh"
#include "G4ThreeVector.hh"

#include <cmath>

BDSIntegratorEulerOld::BDSIntegratorEulerOld(G4Mag_EqRhs* eqOfMIn):
  BDSIntegratorMag(eqOfMIn, 6)
{;}

void BDSIntegratorEulerOld::Stepper(const G4double yIn[],
				    const G4double dydx[],
				    const G4double h,
				    G4double       yOut[],
				    G4double       yErr[])
{
  G4double yTemp[7];

  G4ThreeVector pos     = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector mom     = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4ThreeVector momUnit = mom.unit();

  auxNavigator->LocateGlobalPointAndSetup(pos);
  G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform();
  G4ThreeVector     localMomUnit = GlobalAffine.TransformAxis(momUnit);

  if (localMomUnit.z() < 0.9 || mom.mag() < 40.0 )
    {
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
      SetDistChord(backupStepper->DistChord());
      return;
    }
  
  // Do two half steps
  AdvanceHelix(yIn,   0.5*h, yTemp);
  AdvanceHelix(yTemp, 0.5*h, yOut); 
  
  // Do a full Step
  AdvanceHelix(yIn, h, yTemp);

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
  // TBC - we have not set DistChord here!
}
