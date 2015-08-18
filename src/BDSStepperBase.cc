#include "BDSDebug.hh"
#include "BDSStepperBase.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Navigator.hh"

G4Navigator* BDSStepperBase::auxNavigator = new G4Navigator;

BDSStepperBase::BDSStepperBase(G4Mag_EqRhs* eqRHS,
			       G4int        nVariables):
  G4MagIntegratorStepper(eqRHS, nVariables)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
}

void BDSStepperBase::AttachWorldVolumeToStepper(G4VPhysicalVolume* worldPV)
{
  auxNavigator->SetWorldVolume(worldPV);
}
