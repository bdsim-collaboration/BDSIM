#include "BDSGlobalConstants.hh"
#include "BDSFieldObjects.hh"

#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4MagneticField.hh"

class G4EquationOfMotion;
class G4MagIntegratorStepper;

BDSFieldObjects::BDSFieldObjects(G4MagneticField*        magneticFieldIn,
                                    G4EquationOfMotion*     equationOfMotionIn,
                                    G4MagIntegratorStepper* magIntegratorStepperIn,
                                    G4ChordFinder*          chordFinderIn,
                                    G4FieldManager*         fieldManagerIn):
  magneticField(magneticFieldIn),
  equationOfMotion(equationOfMotionIn),
  magIntegratorStepper(magIntegratorStepperIn),
  chordFinder(chordFinderIn),
  fieldManager(fieldManagerIn)
{;}

BDSFieldObjects::BDSFieldObjects(G4MagneticField*        magneticFieldIn,
                                    G4EquationOfMotion*     equationOfMotionIn,
                                    G4MagIntegratorStepper* magIntegratorStepperIn):
  magneticField(magneticFieldIn),
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

BDSFieldObjects::~BDSFieldObjects()
{
  // this doesn't own magneticField
  delete fieldManager;
  delete chordFinder;
  delete magIntegratorStepper;
  delete equationOfMotion;
}
