#include "BDSCavityRF.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSCavity.hh"
#include "BDSCavityField.hh"
#include "BDSCavityInfo.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh"
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
  itsField = new BDSCavityField(fieldAmplitude,
				 cavityRadius,
				 cavityInfo->frequency,
				 cavityInfo->phase);
}

void BDSCavityRF::AttachField()
{
  G4EqMagElectricField* itsEquation = new G4EqMagElectricField(itsField); //Create field
  itsStepper = new G4ClassicalRK4(itsEquation, nvar); //Create stepper

  itsIntgrDriver = new G4MagInt_Driver(BDSGlobalConstants::Instance()->ChordStepMinimum(), //minimum step
				       itsStepper, //Stepper
				       itsStepper->GetNumberOfVariables() ); //nvar

  itsChordFinder = new G4ChordFinder(itsIntgrDriver);  //EM fields need to use integrator driver constructor.
  itsFieldManager = new G4FieldManager();                      //Create field manager

  
  itsFieldManager->SetDetectorField(itsField);               //Set field in field mnager
  itsFieldManager ->SetChordFinder(itsChordFinder);       //set chord finder in manager
  
  //Defining the chord finder parameters.
  itsFieldManager ->SetDeltaIntersection(BDSGlobalConstants::Instance()->DeltaIntersection());
  itsFieldManager ->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->MinimumEpsilonStep());
  itsFieldManager ->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->MaximumEpsilonStep());
  itsFieldManager ->SetDeltaOneStep(BDSGlobalConstants::Instance()->DeltaOneStep());
  
  G4bool allLocal = true ; //using "true" makes it push the field to all existing daugher volumes (and their daughters and so on) -- unless a daughter has its own field manager.

  vacuumLV->SetFieldManager(itsFieldManager,allLocal);

  // std::cout << "field attached" << std::endl;
  
}


