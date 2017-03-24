#include "BDSIntegratorMag.hh"
#include "BDSStep.hh"

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
