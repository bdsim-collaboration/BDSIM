#include "BDSIntegratorCurvilinear.hh"

#include "BDSStep.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ClassicalRK4.hh"

BDSIntegratorCurvilinear::BDSIntegratorCurvilinear(G4Mag_EqRhs* eqOfMIn,
						   G4int        nVariablesIn):
  BDSIntegrator(eqOfMIn, nVariablesIn)
{;}

void BDSIntegratorCurvilinear::ConvertToGlobal(const G4ThreeVector& LocalR,
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
