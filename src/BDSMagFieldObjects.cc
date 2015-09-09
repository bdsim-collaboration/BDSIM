#include "BDSGlobalConstants.hh"
#include "BDSMagFieldObjects.hh"

#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"

class G4EquationOfMotion;
class G4MagIntegratorStepper;
class G4MagneticField;

BDSMagFieldObjects::BDSMagFieldObjects(G4MagneticField*        magneticField,
				       G4EquationOfMotion*     equationOfMotionIn,
				       G4MagIntegratorStepper* magIntegratorStepperIn,
				       G4ChordFinder*          chordFinderIn,
				       G4FieldManager*         fieldManagerIn):
  equationOfMotion(equationOfMotionIn),
  magIntegratorStepper(magIntegratorStepperIn),
  chordManager(chordManagerIn),
  fieldManager(fieldManagerIn)
{;}

BDSMagFieldObjects::BDSMagFieldObjects(G4MagneticField*        magneticField,
				       G4EquationOfMotion*     equationOfMotionIn,
				       G4MagIntegratorStepper* magIntegratorStepperIn):
  equationOfMotion(equationOfMotionIn),
  magIntegratorStepper(magIntegratorStepperIn)
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  
  chordFinder = new G4ChordFinder(magneticField,
				  globals->GetChordStepMinimum(),
				  magIntegratorStepper);

  fieldManager = new G4FieldManager(magneticField, chordFinder);
  fieldManager->SetDeltaIntersection(globals->GetDeltaIntersection());
  fieldManager->SetMinimumEpsilonStep(globals->GetMinimumEpsilonStep());
  fieldManager->SetMaximumEpsilonStep(globals->GetMaximumEpsilonStep());
  fieldManager->SetDeltaOneStep(globals->GetDeltaOneStep());
}

BDSMagFieldObjects::~BDSMagFieldObjects()
{
  delete fieldManager;
  delete chordFinder;
  delete magIntegratorStepper;
  delete equationOfMotion;
}
