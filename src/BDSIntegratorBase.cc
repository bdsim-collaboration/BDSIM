#include "BDSIntegratorBase.hh"

#include "globals.hh" // geant4 types / globals

BDSIntegratorBase::BDSIntegratorBase(const G4Mag_EqRhs* eqRHSIn,
				     const G4double     nVariablesIn):
  G4MagIntegratorStepper(eqRHS, nVariablesIn),
  eqRHS(eqRHSIn),
  nVariables(6)
{
  backupStepper = new G4ClassicalRK4(eqRHS, 6);
}
  
BDSIntegratorBase::~BDSIntegratorBase()
{
  delete backupStepper;
}
