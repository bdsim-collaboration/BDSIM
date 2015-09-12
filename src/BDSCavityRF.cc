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


BDSCavityRF::BDSCavityRF(G4String name, 
			 G4String type,  //??
			 G4double length,
			 G4Material* cavityMaterialIn,
			 G4Material* vacuumMaterialIn,
			 G4double equatorRadiusIn, //lrgst radial distance from z axs
			 G4double irisRadiusIn, //radius at ends (assuming equal..)
			 G4double thicknessIn,
			 G4double frequencyIn,
			 G4double phaseIn
			 ):
    BDSCavity(name, type, length, cavityMaterialIn, vacuumMaterialIn, equatorRadiusIn, irisRadiusIn, thicknessIn)

{
  frequency = frequencyIn;
  phase = phaseIn;
    }

 void BDSCavityRF::BuildField()
{
  G4double eFieldMax = 16 * CLHEP::megavolt / CLHEP::m;
  itsField = new BDSPillBoxField(eFieldMax,
				 equatorRadius,
				 frequency,
				 phase
				 );
  std::cout << "field build" << std::endl;
}

void BDSCavityRF::AttachField()
{
  G4EqMagElectricField* itsEquation = new G4EqMagElectricField(itsField); //Create field
  itsStepper = new G4ClassicalRK4(itsEquation, nvar);                 //Create stepper
  itsIntgrDriver = new G4MagInt_Driver(BDSGlobalConstants::Instance()->GetChordStepMinimum(), //minimum step
				       itsStepper, //Stepper
				       itsStepper->GetNumberOfVariables() ); //nvar
  itsChordFinder = new G4ChordFinder(itsIntgrDriver); //EM fields need to use integrator driver constructor.
  itsFieldManager = new G4FieldManager();                      //Create field manager
  itsFieldManager->SetDetectorField(itsField);               //Set field in field mnager

  //Defining parameters?
  itsFieldManager ->SetDeltaIntersection(BDSGlobalConstants::Instance()->GetDeltaIntersection());
  itsFieldManager ->SetMinimumEpsilonStep(BDSGlobalConstants::Instance()->GetMinimumEpsilonStep());
  itsFieldManager ->SetMaximumEpsilonStep(BDSGlobalConstants::Instance()->GetMaximumEpsilonStep());
  itsFieldManager ->SetDeltaOneStep(BDSGlobalConstants::Instance()->GetDeltaOneStep());
  itsFieldManager->SetChordFinder(itsChordFinder);       //set chord finder in manager
  G4bool allLocal = true ; //using "true" makes it push the field to all existing daugher volumes (and their daughters and so on) -- unless a daughter has its own field manager.

  vacuumLV->SetFieldManager(itsFieldManager,allLocal);

  // std::cout << "field attached" << std::endl;
  
}

void BDSCavityRF::Build()
{
  BuildEllipticalCavityGeometry();
  //BuildPillBoxCavityGeometry();
  BDSAcceleratorComponent::Build();
  BuildField();
  AttachField();
  PlaceComponents();
}
