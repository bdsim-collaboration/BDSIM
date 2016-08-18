#include "BDSIntegratorBase.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ClassicalRK4.hh"

BDSIntegratorBase::BDSIntegratorBase(G4Mag_EqRhs* eqOfMIn,
				     G4int        nVariablesIn,
				     G4bool       cacheTransforms):
  G4MagIntegratorStepper(eqOfMIn, nVariablesIn),
  BDSAuxiliaryNavigator(cacheTransforms),
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
