#include "BDSIntegratorBase.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ClassicalRK4.hh"

BDSIntegratorBase::BDSIntegratorBase(G4Mag_EqRhs * const eqRHSIn,
				     const G4double     nVariablesIn):
  G4MagIntegratorStepper(eqRHSIn, nVariablesIn),
  eqRHS(eqRHSIn),
  nVariables(6),
  distChord(0)
{
  backupStepper = new G4ClassicalRK4(eqRHS, 6);
}
  
BDSIntegratorBase::~BDSIntegratorBase()
{
  delete backupStepper;
}
