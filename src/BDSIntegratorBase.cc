#include "BDSIntegratorBase.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ClassicalRK4.hh"

BDSIntegratorBase::BDSIntegratorBase(G4Mag_EqRhs* eqOfMIn,
				     G4int        nVariablesIn):
  G4MagIntegratorStepper(eqOfMIn, nVariablesIn),
  eqOfM(eqOfMIn),
  nVariables(6),
  distChord(0)
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

  distChord=0;
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
