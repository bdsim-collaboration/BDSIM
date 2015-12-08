#include "BDSCavityRF.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSCavity.hh"

#include "globals.hh"
#include "G4Material.hh"
#include "BDSPillBoxField.hh"
#include "G4FieldManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4ElectroMagneticField.hh"
#include "G4EqMagElectricField.hh"
#include "G4ClassicalRK4.hh"

BDSCavityRF::BDSCavityRF(G4String       name,
			 G4double       length,
			 G4double       fieldAmplitude,
			 BDSCavityInfo* cavityInfo):
  BDSCavity(name, length, fieldAmplitude, cavityInfo)
{;}

 void BDSCavityRF::BuildField()
{
  itsField = new BDSPillBoxField(fieldAmplitude,
				 cavityRadius,
				 cavityInfo->frequency,
				 cavityInfo->phase);
}

void BDSCavityRF::AttachField()
{
  G4EqMagElectricField* itsEquation = new G4EqMagElectricField(itsField); //Create field
  itsStepper = new G4ClassicalRK4(itsEquation, nvar); //Create stepper

  itsIntgrDriver = new G4MagInt_Driver(BDSGlobalConstants::Instance()->GetChordStepMinimum(), //minimum step
				       itsStepper, //Stepper
				       itsStepper->GetNumberOfVariables() ); //nvar

  itsChordFinder = new G4ChordFinder(itsIntgrDriver);  //EM fields need to use integrator driver constructor.
  itsFieldManager = new G4FieldManager();                      //Create field manager

  
  itsFieldManager->SetDetectorField(itsField);               //Set field in field mnager
  itsFieldManager ->SetChordFinder(itsChordFinder);       //set chord finder in manager
  
  //Defining the chord finder parameters.
  itsFieldManager ->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  itsFieldManager ->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  itsFieldManager ->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  itsFieldManager ->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
  
  G4bool allLocal = true ; //using "true" makes it push the field to all existing daugher volumes (and their daughters and so on) -- unless a daughter has its own field manager.

  vacuumLV->SetFieldManager(itsFieldManager,allLocal);

  // std::cout << "field attached" << std::endl;
  
}


