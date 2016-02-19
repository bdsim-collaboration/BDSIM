#include "BDSGlobalConstants.hh"
#include "BDSFieldObjects.hh"

#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4MagneticField.hh"

#include <vector>

class BDSFieldInfo;

class G4EquationOfMotion;
class G4MagIntegratorStepper;

BDSFieldObjects::BDSFieldObjects(BDSFieldInfo*           infoIn,
				 G4Field*                fieldIn,
				 G4EquationOfMotion*     equationOfMotionIn,
				 G4MagIntegratorStepper* magIntegratorStepperIn,
				 G4ChordFinder*          chordFinderIn,
				 G4FieldManager*         fieldManagerIn):
  info(infoIn),
  field(fieldIn),
  equationOfMotion(equationOfMotionIn),
  magIntegratorStepper(magIntegratorStepperIn),
  chordFinder(chordFinderIn),
  fieldManager(fieldManagerIn)
{;}

BDSFieldObjects::BDSFieldObjects(BDSFieldInfo*           infoIn,
				 G4MagneticField*        fieldIn,
				 G4EquationOfMotion*     equationOfMotionIn,
				 G4MagIntegratorStepper* magIntegratorStepperIn):
  info(infoIn),
  field(fieldIn),
  equationOfMotion(equationOfMotionIn),
  magIntegratorStepper(magIntegratorStepperIn)
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  
  chordFinder = new G4ChordFinder(fieldIn, // note using higher pointer in inheritance
				  globals->GetChordStepMinimum(),
				  magIntegratorStepper);

  fieldManager = new G4FieldManager(field, chordFinder);
  fieldManager->SetDeltaIntersection(globals->GetDeltaIntersection());
  fieldManager->SetMinimumEpsilonStep(globals->GetMinimumEpsilonStep());
  fieldManager->SetMaximumEpsilonStep(globals->GetMaximumEpsilonStep());
  fieldManager->SetDeltaOneStep(globals->GetDeltaOneStep());
}

BDSFieldObjects::~BDSFieldObjects()
{
  delete field;
  delete fieldManager;
  delete chordFinder;
  delete magIntegratorStepper;
  delete equationOfMotion;
}
