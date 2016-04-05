#include "BDSGlobalConstants.hh"
#include "BDSFieldObjects.hh"

#include "G4ChordFinder.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4MagneticField.hh"
#include "G4MagIntegratorDriver.hh" // for G4MagInt_Driver

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
  fieldManager(fieldManagerIn),
  magIntDriver(nullptr)
{;}

BDSFieldObjects::BDSFieldObjects(BDSFieldInfo*           infoIn,
				 G4ElectroMagneticField* fieldIn,
				 G4EquationOfMotion*     equationOfMotionIn,
				 G4MagIntegratorStepper* magIntegratorStepperIn):
  info(infoIn),
  field(fieldIn),
  equationOfMotion(equationOfMotionIn),
  magIntegratorStepper(magIntegratorStepperIn)
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  magIntDriver = new G4MagInt_Driver(globals->ChordStepMinimum(),
				     magIntegratorStepper,
				     magIntegratorStepper->GetNumberOfVariables());

  chordFinder  = new G4ChordFinder(magIntDriver);
  fieldManager = new G4FieldManager(field, chordFinder);
  fieldManager->SetDeltaIntersection(globals->DeltaIntersection());
  fieldManager->SetMinimumEpsilonStep(globals->MinimumEpsilonStep());
  fieldManager->SetMaximumEpsilonStep(globals->MaximumEpsilonStep());
  fieldManager->SetDeltaOneStep(globals->DeltaOneStep());
}

BDSFieldObjects::BDSFieldObjects(BDSFieldInfo*           infoIn,
				 G4MagneticField*        fieldIn,
				 G4EquationOfMotion*     equationOfMotionIn,
				 G4MagIntegratorStepper* magIntegratorStepperIn):
  info(infoIn),
  field(fieldIn),
  equationOfMotion(equationOfMotionIn),
  magIntegratorStepper(magIntegratorStepperIn),
  magIntDriver(nullptr)
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  
  chordFinder = new G4ChordFinder(fieldIn, // note using higher pointer in inheritance
				  globals->ChordStepMinimum(),
				  magIntegratorStepper);

  fieldManager = new G4FieldManager(field, chordFinder);
  fieldManager->SetDeltaIntersection(globals->DeltaIntersection());
  fieldManager->SetMinimumEpsilonStep(globals->MinimumEpsilonStep());
  fieldManager->SetMaximumEpsilonStep(globals->MaximumEpsilonStep());
  fieldManager->SetDeltaOneStep(globals->DeltaOneStep());
}

BDSFieldObjects::~BDSFieldObjects()
{
  delete field;
  delete fieldManager;
  delete chordFinder;
  delete magIntegratorStepper;
  delete equationOfMotion;
  delete magIntDriver;
}

void BDSFieldObjects::AttachToVolume(G4LogicalVolume* volume,
				     G4bool penetrateToDaughterVolumes)
{
  volume->SetFieldManager(fieldManager, penetrateToDaughterVolumes);
}

void BDSFieldObjects::AttachToVolume(std::vector<G4LogicalVolume*> volumes,
				     G4bool penetrateToDaughterVolumes)
{
  for (auto volume : volumes)
    {AttachToVolume(volume, penetrateToDaughterVolumes);}
}
